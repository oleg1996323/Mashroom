#include "proc/extract/write.h"
#include <variant>
#include <vector>
#include "compressor.h"
#include "proc/extract/gen.h"
#include "API/grib1/include/sections/grid/grid.h"
#include "proc/common/fs.h"
#include "sys/error_exception.h"
#include "concepts.h"
#include "API/grib1/include/sections/product/time_forecast.h"
#include "API/grib1/include/sections/product/levels.h"

using namespace procedures::extract;
using namespace std::string_literals;
using namespace std::string_view_literals;

bool extraction_empty(const ExtractedData& data) noexcept{
    auto is_empty = [](auto&& arg){
        return arg.empty();
    };
    return std::visit(is_empty,data);
}


template<size_t indent=0>
std::ostream& write_parameter(std::ostream& stream,const char* token){
    for(size_t i=0;i<indent;++i)
        stream<<"\t";
    stream<<token<<":";
    return stream;
}

template<size_t indent=0>
std::ostream& write_parameter(std::ostream& stream,String auto&& token,auto&& value){
    using value_t = std::decay_t<decltype(value)>;
    for(size_t i=0;i<indent;++i)
        stream<<"\t";
    stream<<token<<":";
    if constexpr(std::is_integral_v<value_t> && sizeof(value_t)==1)
        stream<<static_cast<int>(value);
    else if constexpr (std::is_enum_v<value_t>){
        if constexpr (std::is_integral_v<std::underlying_type_t<value_t>> && sizeof(value_t)==1)
            stream<<static_cast<int>(value);
        else stream<<static_cast<std::underlying_type_t<value_t>>(value);
    }
    else stream<<value;
    return stream;
}

template<size_t indent_tabs,String... STRUCT_TOKENS,typename... ARGS>
std::ostream& write_structure(std::tuple<STRUCT_TOKENS...> struct_tokens,std::ostream& stream,String auto&& token,ARGS&&... args){
    static_assert(sizeof...(STRUCT_TOKENS)>0 && sizeof...(STRUCT_TOKENS)==sizeof...(args));
    stream<<token<<":\n";
    auto write_tokens_proxy = [&stream]<size_t... Is>(const std::tuple<STRUCT_TOKENS...>& tokens,std::index_sequence<Is...>,auto&&... args_loc){
        auto write_token = [&stream](String auto&& token,auto&& value){
            for(size_t i=1;i<=indent_tabs;++i)
                stream<<'\t';
            write_parameter(stream,token,value)<<"\n";
        };
        (write_token(std::get<Is>(tokens),args_loc),...);
    };
    
    write_tokens_proxy(struct_tokens,std::make_index_sequence<sizeof...(STRUCT_TOKENS)>{},args...);
    return stream;
}

// template<int indent_tabs,typename... ARGS>
// std::ostream& write_structure(auto&&... struct_tokens,std::ostream& stream,String auto&& token,ARGS&&... args){
//     static_assert(sizeof...(struct_tokens)>0 && sizeof...(struct_tokens)==sizeof...(args));
//     stream<<token<<":\n";
//     auto write_token = [&stream](String auto&& token,auto&& value){
//         for(int i=1;i<=indent_tabs;++i)
//             stream<<'\t';
//         write_parameter(stream,token,value)<<"\n";
//     };
//     (write_token(struct_tokens,args),...);
//     return stream;
// }

//"Data formats: "s+center_to_abbr(props.center_.value())+"\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n"
std::string get_file_header(const ExtractedData& result, const SearchProperties& props, std::string_view user_hdr_format){
    std::ostringstream stream;
    stream<<"Mashroom\n\\header\n";//@todo add version
    if(!user_hdr_format.empty())
        write_parameter(stream,"description",user_hdr_format)<<"\n";
    auto write_head = [&stream,&props,&result]<Data_t dtype,Data_f dformat>(const ExtractedValues<dtype,dformat>& value){
        write_parameter(stream,"type",dtype)<<"\n";
        write_parameter(stream,"format",dformat)<<"\n";
        write_parameter(stream,"version",VERSION)<<"\n";
        if constexpr(std::is_same_v<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::decay_t<decltype(value)>>){
            write_parameter(stream,"latitude",props.position_->lat_)<<"\n";
            write_parameter(stream,"longitude",props.position_->lon_)<<"\n";
            write_parameter(stream,"grid",props.grid_type_.value())<<"\n";
            write_parameter(stream,"parameters",get_result(result).size())<<"\n";
            for(auto& [common,value]:get_result(result)){
                if(auto param_ptr = parameter_table(*common.center_,*common.table_version_,*common.parameter_);param_ptr==nullptr)
                    throw ErrorException(ErrorCode::INTERNAL_ERROR,
                        "undefined parameter table (center="s+std::to_string(static_cast<std::underlying_type_t<Organization>>(*common.center_))+
                        ";table version="s+std::to_string(*common.table_version_)+";parameter="+
                        std::to_string(*common.parameter_)+")");
                else
                    write_parameter(stream,"name",param_ptr->name)<<"\n";
                write_parameter(stream,"indicator",*common.parameter_)<<"\n";
                write_parameter(stream,"table version",*common.table_version_)<<"\n";
                write_parameter(stream,"center",props.center_.value())<<"\n";
                write_parameter(stream,"forecast data")<<"\n";
                write_parameter<1>(stream,"time frame",common.fcst_unit_->get_time_frame())<<"\n";
                write_parameter<1>(stream,"time range indicator",common.fcst_unit_->get_time_range_indicator())<<"\n";
                if(common.fcst_unit_->is_intervaled()){
                    write_parameter<1>(stream,"N avg/acc",common.fcst_unit_->get_n())<<"\n";
                    write_parameter<1>(stream,"N missed",common.fcst_unit_->get_avg_acc_miss_N_vals())<<"\n";
                }
                if(common.fcst_unit_->octet_doubled())
                    write_parameter<1>(stream,"P1",static_cast<uint16_t>((static_cast<uint16_t>(common.fcst_unit_->get_P1().val)<<8)|common.fcst_unit_->get_P2().val))<<"\n";
                else if(common.fcst_unit_->is_unique_value())
                    write_parameter<1>(stream,"P1",common.fcst_unit_->get_P1().val)<<"\n";
                else {
                    write_parameter<1>(stream,"P1",common.fcst_unit_->get_P1().val)<<"\n";
                    write_parameter<1>(stream,"P2",common.fcst_unit_->get_P2().val)<<"\n";
                }
                if(common.level_->is_bounded()){
                    write_structure<1>(std::make_tuple("tag"s,"o11"s,"o12"s,"top bound"s,"bottom bound"s),stream,
                        "level",
                        common.level_->type(),
                        common.level_->get_first_octet(),
                        common.level_->get_second_octet(),
                        common.level_->get_top_bound(),
                        common.level_->get_bottom_bound());
                }
                else {
                    write_structure<1>(std::make_tuple("tag"s,"o11"s,"o12"s,"height"s),stream,
                        "level",
                        common.level_->type(),
                        common.level_->get_first_octet(),
                        common.level_->get_second_octet(),
                        common.level_->get_level_height());
                }
            }
        }
        else static_assert(false,"Undefined text format header");
    };
    std::visit(write_head,result);
    return stream.str();
}

std::string get_data_header(const ExtractedData& result, const SearchProperties& props){
    std::ostringstream stream;
    stream<<"\\data\n";
    auto write_columns = [&stream](auto&& value){
        if constexpr(std::is_same_v<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::decay_t<decltype(value)>>){
            stream<<std::left<<std::setw(18)<<"Time"<<"\t";
            for(auto& [cmn_data,values]:value){
                stream<<std::left<<std::setw(10)<<parameter_table(
                        cmn_data.center_.has_value()?
                        cmn_data.center_.value():
                        Organization::Missing,
                        cmn_data.table_version_.has_value()?
                        cmn_data.table_version_.value():
                        0,cmn_data.parameter_.has_value()?
                        cmn_data.parameter_.value():0)->name<<'\t';
            }
            stream<<"\n";
        }
        else static_assert(false,"Undefined text format header");
    };
    std::visit(write_columns,result);
    return stream.str();
}

std::unordered_set<fs::path> procedures::extract::write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path){
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
    using printable_values_t = ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value_t;
    for(int row=0;row<max_length;++row){
        if(stop_token.stop_requested())
            throw ErrorException(ErrorCode::INTERRUPTED,std::string_view("writting txt files"));
        current_time = utc_tp::max();
        for(int col=0;col<col_vals_.size();++col)
            if(rows[col]<col_vals_[col]->size())
                current_time = std::min((*col_vals_.at(col))[rows.at(col)].time_date,current_time);
        if(current_time>=file_end_time || row==0){
            out.close();
            out_f_name/=generate_directory(out_path,round_by_time_period(t_off,current_time));
            out_f_name/=generate_filename(OutputDataFileFormats::TXT_F,
                center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,round_by_time_period(t_off,current_time));
            {
                make_and_open_file(out,out_f_name);
                paths.insert(out_f_name);
                out<<get_file_header(result,props,"");
                out<<get_data_header(result,props);//@todo add header format text
            }
            file_end_time = t_off.get_next_tp(current_time);
        }
        out<<std::format("{:%Y/%m/%d %H:%M:%S}",time_point_cast<std::chrono::seconds>(current_time))<<'\t';
        for(int col=0;col<col_vals_.size();++col){
            if(rows[col]<col_vals_[col]->size()){
                if((*col_vals_[col])[rows[col]].time_date==current_time){
                    out<<std::left<<std::setw(std::numeric_limits<printable_values_t>::max_digits10)<<
                    std::setprecision(std::numeric_limits<printable_values_t>::max_digits10)<<
                    (*col_vals_[col])[rows[col]].value<<'\t';
                    ++rows[col];
                }
                else{
                    out<<std::left<<std::setw(std::numeric_limits<printable_values_t>::max_digits10)<<"NaN"<<'\t';
                }
            }
            else out<<std::left<<std::setw(std::numeric_limits<printable_values_t>::max_digits10)<<"NaN"<<'\t';
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
                        const fs::path& out_path){
    std::unordered_set<fs::path> paths;
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
        out_f_name/=generate_directory(out_path,round_by_time_period(t_off,lower_bound_time));
        out_f_name/=generate_filename(OutputDataFileFormats::JSON_F,
            center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,round_by_time_period(t_off,lower_bound_time));
        make_and_open_file(out,out_f_name);
        if(!out.is_open())
            throw ErrorException(ErrorCode::CANNOT_OPEN_FILE_X1,std::string_view(),out_f_name.c_str());
        try{
            boost::json::object json;
            json["type"] = to_json(Data_t::TIME_SERIES);
            json["format"] = to_json(Data_f::GRIB_v1);
            json["grid"] = to_json(props.grid_type_);
            json["position"] = to_json(props.position_);
            json["data"] = boost::json::array();
            boost::json::array& json_data = json["data"].as_array();
            for(const auto& [cmn_data,values]:get_result(result)){
                boost::json::object& current_data = json_data.emplace_back(boost::json::object()).as_object();
                current_data["info"]=to_json(cmn_data);
                current_data["values"]=to_json(std::span(  std::lower_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(lower_bound_time,0.f)),
                                                std::upper_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(upper_bound_time,0.f))));
            }
            out<<json<<std::endl;
            if(out.fail() && !out.eof()){
                if(out.is_open())
                    out.close();
                else throw ErrorException(ErrorCode::INTERNAL_ERROR,std::string_view(),out_f_name.c_str());
            }
            else{
                paths.insert(out_f_name);
                out.close();
            }
        }
        catch(const ErrorException& exc_err){
            out.close();
            fs::remove(out_f_name);
        }
        lower_bound_time = upper_bound_time;
        upper_bound_time = t_off.get_next_tp(upper_bound_time);
    }
    return paths;
}

std::unordered_set<fs::path> procedures::extract::write_bin_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path){
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
        out_f_name/=generate_directory(out_path,round_by_time_period(t_off,lower_bound_time));
        out_f_name/=generate_filename(OutputDataFileFormats::BIN_F,
            center_to_abbr(props.center_.value()),grid_to_abbr(props.grid_type_.value()),props.position_.value().lat_,props.position_.value().lon_,round_by_time_period(t_off,lower_bound_time));
        make_and_open_file(out,out_f_name);
        if(!out.is_open())
            throw ErrorException(ErrorCode::CANNOT_OPEN_FILE_X1,std::string_view(),out_f_name.c_str());
        try{
            if(auto ser_err = serialize_to_file(Data_t::TIME_SERIES,out);ser_err!=SerializationEC::NONE)
                throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            if(auto ser_err = serialize_to_file(Data_f::GRIB_v1,out);ser_err!=SerializationEC::NONE)
                throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            if(auto ser_err = serialize_to_file(props.grid_type_,out);ser_err!=SerializationEC::NONE)
                throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            if(auto ser_err = serialize_to_file(props.position_,out);ser_err!=SerializationEC::NONE)
                throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            
            using result_t = std::decay_t<decltype(get_result(result))>;
            std::vector<std::pair<std::reference_wrapper<const result_t::key_type>,std::span<const result_t::mapped_type::value_type>>> result_separated;
            for(const auto& [cmn_data,values]:get_result(result)){
                auto values_view = std::span(  std::lower_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(lower_bound_time,0.f)),
                        std::upper_bound(values.begin(),values.end(),ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>(upper_bound_time,0.f)));
                if(values_view.empty())
                    continue;
                else
                    result_separated.push_back(std::make_pair(std::cref(cmn_data),values_view));
            }
            if(auto ser_err = serialize_to_file(result_separated,out);ser_err!=SerializationEC::NONE)
                throw ErrorException(ErrorCode::SERIALIZATION_ERROR,""sv);
            paths.insert(out_f_name);
            out.close();
        }
        catch(const ErrorException& exc_err){
            out.close();
            fs::remove(out_f_name);
        }
        lower_bound_time = upper_bound_time;
        upper_bound_time = t_off.get_next_tp(upper_bound_time);
    }
    return paths;
}