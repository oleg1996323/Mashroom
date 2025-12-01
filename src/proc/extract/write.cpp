#include "proc/extract/write.h"
#include <variant>
#include <vector>
#include "compressor.h"
#include "proc/extract/gen.h"
#include "API/grib1/include/sections/grid/grid.h"
#include "proc/common/fs.h"
#include "sys/error_exception.h"

using namespace procedures::extract;

bool extraction_empty(const ExtractedData& data) noexcept{
    auto is_empty = [](auto&& arg){
        return arg.empty();
    };
    return std::visit(is_empty,data);
}

std::unordered_set<fs::path> procedures::extract::write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path,
                        const std::string& dirname_format,
                        const std::string& filename_format){
    std::unordered_set<fs::path> paths;
    if(extraction_empty(result))
        return paths;
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
            throw ErrorException(ErrorCode::INTERRUPTED,std::string_view("writting txt files"));
        current_time = utc_tp::max();
        for(int col=0;col<col_vals_.size();++col)
            if(rows[col]<col_vals_[col]->size())
                current_time = std::min((*col_vals_.at(col))[rows.at(col)].time_date,current_time);
        if(current_time>=file_end_time || row==0){
            out.close();
            out_f_name/=generate_directory(out_path,dirname_format,current_time);
            out_f_name/=generate_filename(OutputDataFileFormats::TXT_F,
                filename_format,
                center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,current_time);
            {
                make_file(out,out_f_name);
                paths.insert(out_f_name);
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
    return paths;
}

std::unordered_set<fs::path> procedures::extract::write_json_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path,
                        const std::string& dirname_format,
                        const std::string& filename_format){

}

std::unordered_set<fs::path> procedures::extract::write_bin_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path,
                        const std::string& dirname_format,
                        const std::string& filename_format){
    using namespace serialization;
    using namespace std::string_view_literals;
    std::unordered_set<fs::path> paths;
    if(extraction_empty(result))
        return paths;
    utc_tp min_time = utc_tp::max();
    utc_tp max_time = utc_tp::min();
    for(auto& [cmn_data,values]:get_result(result)){
        std::sort(values.begin(),values.end(),std::less());
        if(!values.empty()){
            min_time = std::min(values.front().time_date,min_time);
            max_time = std::max(values.front().time_date,max_time);
        }
    }
    utc_tp lower_bound_time = min_time;
    utc_tp upper_bound_time = t_off.get_next_tp(min_time);
    std::ofstream out;
    fs::path out_f_name;
    while(true){
        if(lower_bound_time>max_time)
            break;
        out.close();
        out_f_name.clear();
        out_f_name/=generate_directory(out_path,dirname_format,lower_bound_time);
        out_f_name/=generate_filename(OutputDataFileFormats::BIN_F,
            filename_format,
            center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,lower_bound_time);
        make_file(out,out_f_name);
        out.open(out_f_name,std::ofstream::out);
        try{
            for(const auto& [cmn_data,values]:get_result(result)){
                if(auto ser_err = serialize_to_file(Data_t::TIME_SERIES,out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(Data_f::GRIB_v1,out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(props.grid_type_,out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(props.position_,out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(get_result(result).size(),out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(cmn_data,out);ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
                if(auto ser_err = serialize_to_file(std::span(  std::lower_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(lower_bound_time,0.f)),
                                                                std::upper_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(upper_bound_time,0.f))),
                                                                out);
                                                                ser_err!=SerializationEC::NONE)
                    throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            }
            paths.insert(out_f_name);
            out.close();
        }
        catch(const ErrorException& exc_err){
            out.close();
            fs::remove(out_f_name);
        }
    }
    return paths;
}