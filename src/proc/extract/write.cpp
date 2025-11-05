#include "proc/extract/write.h"
#include <variant>
#include <vector>
#include "compressor.h"
#include "proc/common/gen.h"
#include "API/grib1/include/sections/grid/grid.h"
#include "proc/common/fs.h"

bool extraction_empty(const ExtractedData& data) noexcept{
    auto is_empty = [](auto&& arg){
        return arg.empty();
    };
    return std::visit(is_empty,data);
}

ErrorCode write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path,
                        const std::string& dirname_format,
                        const std::string& filename_format,
                        OutputDataFileFormats output_format,
                        std::unordered_set<fs::path>& maked_files) noexcept{
    if(extraction_empty(result))
        return ErrorCode::NONE;
    utc_tp current_time = utc_tp::max();
    size_t max_length = 0;
    auto col_vals_ = get_columns(result);
    for(auto& [cmn_data,values]:get_result(result)){
        std::sort(values.begin(),values.end(),std::less());
        if(!values.empty()){
            current_time = std::min(values.front().time_date,current_time);
            col_vals_.push_back(&values);
        }
        max_length = std::max(max_length,values.size());
    }

    std::vector<int> rows;
    rows.resize(col_vals_.size());
    utc_tp file_end_time = t_off.get_next_tp(current_time);
    std::ofstream out;
    fs::path out_f_name;
    for(int row=0;row<max_length;++row){
        if(stop_token.stop_requested())
            return ErrorCode::INTERRUPTED;
        current_time = utc_tp::max();
        for(int col=0;col<col_vals_.size();++col)
            if(rows[col]<col_vals_[col]->size())
                current_time = std::min((*col_vals_.at(col))[rows.at(col)].time_date,current_time);
        if(current_time>=file_end_time || row==0){
            out.close();
            out_f_name/=generate_directory(out_path,dirname_format,current_time);
            out_f_name/=generate_filename(output_format,
                filename_format,
                center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,current_time);
            {
                ErrorCode err = make_file(out,out_f_name,result);
                if(err!=ErrorCode::NONE){
                    maked_files.clear();
                    return err;
                }
                maked_files.insert(out_f_name);
                out<<get_header(result,props,"");//@todo add header format text
            }
            file_end_time = t_off.get_next_tp(current_time);
        }
        out<<std::format("{:%Y/%m/%d %H:%M:%S}",time_point_cast<std::chrono::seconds>(current_time))<<'\t';
        for(int col=0;col<col_vals_.size();++col){
            if(rows[col]<col_vals_[col]->size()){
                if((*col_vals_[col])[rows[col]].time_date==current_time){
                    out<<std::left<<std::setw(10)<<(*col_vals_[col])[rows[col]].value<<'\t';
                    ++rows[col];
                }
                else{
                    out<<std::left<<std::setw(10)<<"NaN"<<'\t';
                }
            }
            else out<<std::left<<std::setw(10)<<"NaN"<<'\t';
        }
        out<<std::endl;
    }
    if(out.is_open()){
        out.close();
    }
    return ErrorCode::NONE;
}