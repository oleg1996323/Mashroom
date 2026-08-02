#include "proc/extract/read.h"
#include "sys/error.h"
#include <fstream>
#include "data/def.h"
#include "OsterLib/types/coord.h"
#include <expected>
#include "proc/extract/extracted_data.h"
#include "grib1/sections.h"
#include "grib1/paramtableversion.h"
#include "boost/algorithm/string.hpp"
#include "OsterLib/serialization.h"
#include "OsterLib/boost_functional/json.h"
#include "OsterLib/contexted_error.h"


using namespace osterlib;

class TimeForecastProxy:public TimeForecast{
    template<typename... STRUCT_TOKENS, typename... ARGS>
    friend 
    std::ostream& get_txt_structure(std::istream& stream, std::tuple<STRUCT_TOKENS...> struct_tokens,const char* token, ARGS&... args, const fs::path& filename);
    using TimeForecast::TimeForecast;
};

template<RangeOfStrings RESULT,String INPUT, String SEP>
RESULT split_ignore(INPUT&& input_string,SEP&& separator){
    size_t beg = 0;
    size_t end = 0;
    std::string_view view_str(input_string);
    RESULT result;
    for(;;){
        beg = std::distance(view_str.begin(),std::find_if(view_str.begin()+end,view_str.end(),
                [&separator](char ch){return !separator.contains(ch);}));
        if(beg!=std::string::npos){
            end = std::distance(view_str.begin(),std::find_if(view_str.begin()+beg,view_str.end(),
                    [&separator](char ch){return separator.contains(ch);}));
            if(beg==end)
                break;
            result.insert(result.end(),input_string.substr(beg,end-beg));
            if(end==std::string::npos)
                break;
        }
        else break;
    }
    return result;
}
template<String INPUT, String SEP>
std::size_t find_first_no_of(INPUT&& input_string,SEP&& separator){
    std::string_view view_str(input_string);
    std::string_view sep_view(separator);
    return std::distance(view_str.begin(),std::find_if(view_str.begin(),view_str.end(),
        [&sep_view](char ch){return !sep_view.contains(ch);}));
}

namespace procedures::extract{
namespace fs = std::filesystem;

namespace txt::details{    

    template<typename T>
    T get_txt_token(
                const String auto& buffer,
                const char* token,
                const fs::path& filename){
        using namespace std::string_literals;
        if constexpr (!std::is_same_v<void,T> && !std::is_void_v<T>){
            auto not_indent = find_first_no_of(buffer,"\t ");
            if(not_indent==std::string::npos){
                ContextedError ctx_err(mashroom::errc::file_corrupted);
                ctx_err
                .with_field("procedure","extract")
                .with_field("at","get_txt_token")
                .with_field("file",filename.c_str())
                .with_field("token",token);
                throw ctx_err;
            }
                
            T result;
            if(!std::string_view(buffer).substr(not_indent).starts_with(token+":"s)){
                ContextedError ctx_err(mashroom::errc::file_corrupted);
                ctx_err
                .with_field("procedure","extract")
                .with_field("at","get_txt_token")
                .with_field("file",filename.c_str())
                .with_field("token",token);
                throw std::unexpected(ctx_err);
            }
            auto value_str = std::string_view(buffer).substr(not_indent+std::strlen(token)+1);
            if constexpr (IsTimePoint<T>){
                std::istringstream stream(value_str.data());
                stream>>std::chrono::parse("{:%Y/%m/%d %H:%M:%S}",result);
                if(stream.fail()){
                    ContextedError ctx_err(mashroom::errc::file_corrupted);
                    ctx_err
                    .with_field("procedure","extract")
                    .with_field("at","get_txt_token")
                    .with_field("file",filename.c_str())
                    .with_field("token",token)
                    .with_field("type","timepoint");
                    throw std::unexpected(ctx_err);
                }
                else return result;
            }
            else if constexpr(IsDuration<T>){
                std::string back_sign;
                std::istringstream stream(value_str.data());
                stream>>result>>back_sign;
                if(stream.fail()){
                    ContextedError ctx_err(mashroom::errc::file_corrupted);
                    ctx_err
                    .with_field("procedure","extract")
                    .with_field("at","get_txt_token")
                    .with_field("file",filename.c_str())
                    .with_field("token",token)
                    .with_field("type","duration");
                    throw ctx_err;
                }
                if(back_sign.size()==1){
                    switch (*back_sign.c_str())
                    {
                    case 's':
                        result = std::chrono::seconds(result);
                        break;
                    case 'm':
                        result = std::chrono::minutes(result);
                        break;
                    case 'h':
                        result = std::chrono::hours(result);
                        break;
                    case 'd':
                        result = std::chrono::days(result);
                        break;
                    default:{
                        ContextedError ctx_err(mashroom::errc::file_corrupted);
                        ctx_err
                        .with_field("procedure","extract")
                        .with_field("at","get_txt_token")
                        .with_field("file",filename.c_str())
                        .with_field("token",token)
                        .with_field("type","duration");
                        throw ctx_err;
                    }
                    }
                }
                else {
                    ContextedError ctx_err(mashroom::errc::file_corrupted);
                    ctx_err
                    .with_field("procedure","extract")
                    .with_field("at","get_txt_token")
                    .with_field("file",filename.c_str())
                    .with_field("token",token)
                    .with_field("type","duration");
                    throw ctx_err;
                }
            }
            else if constexpr(std::is_integral_v<T>){
                if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),result).ec == std::error_code{})
                    return result;
                else {
                    ContextedError ctx_err(mashroom::errc::file_corrupted);
                    ctx_err
                    .with_field("procedure","extract")
                    .with_field("at","get_txt_token")
                    .with_field("file",filename.c_str())
                    .with_field("token",token)
                    .with_field("type","integer");
                     throw ctx_err;
                }
            }
            else if constexpr(std::is_floating_point_v<T>){
                if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),result).ec == std::error_code{})
                    return result;
                else {
                    ContextedError ctx_err(mashroom::errc::file_corrupted);
                    ctx_err
                    .with_field("procedure","extract")
                    .with_field("at","get_txt_token")
                    .with_field("file",filename.c_str())
                    .with_field("token",token)
                    .with_field("type","float");
                    throw ctx_err;
                }
            }
            else if constexpr(std::is_enum_v<T>){
                result = static_cast<T>(get_txt_token<std::underlying_type_t<T>>(buffer,token,filename));
                return result;
            }
            else if constexpr (String<T>){
                return T(value_str);
            }
            else static_assert(false,"Not implemented");
        }
        else if constexpr (std::is_same_v<void,T> || std::is_void_v<T>)
            return;
        else static_assert(false,"Not implemented");
    }

    template<typename T>
    T get_txt_token(std::istream& stream,const char* token,const fs::path& filename){
        std::string buffer;
        if(!std::getline(stream,buffer)){
            ContextedError ctx_err(mashroom::errc::file_reading_error);
            ctx_err
            .with_field("procedure","extract")
            .with_field("at","get_txt_token");
            throw ctx_err;
        }
        else return get_txt_token<T>(buffer,token,filename);
    }

    template<typename... STRUCT_TOKENS, typename... ARGS>
    std::istream& get_txt_structure(
            std::istream& stream,
            std::tuple<STRUCT_TOKENS...> struct_tokens,
            const char* token,
            ARGS&... args,
            const fs::path& filename)
    {
        static_assert(sizeof...(STRUCT_TOKENS)>0 && sizeof...(STRUCT_TOKENS)==sizeof...(args));
        std::string buffer;
        get_txt_token<void>(stream,token,filename);
        auto read_tokens_proxy = [&stream,&buffer,&filename]<size_t... Is>(const std::tuple<STRUCT_TOKENS...>& tokens,std::index_sequence<Is...>,auto&... args_loc){
            auto get_token = [&stream,&buffer,&filename](String auto&& token,auto& value){
                using type = std::decay_t<decltype(value)>;
                value = get_txt_token<type>(stream,token,filename);
            };
            (get_token(std::get<Is>(tokens),args_loc),...);
        };
        
        read_tokens_proxy(struct_tokens,std::make_index_sequence<sizeof...(STRUCT_TOKENS)>{},args...);
        return stream;
    }

    template<typename T>
    T get_txt_structure(std::istream& stream,const fs::path& filename);

    template<>
    TimeForecast get_txt_structure<TimeForecast>(std::istream& stream,const fs::path& filename){
        TimeForecast result;
        get_txt_token<void>(stream,"forecast data",filename);
        result.set_time_frame(get_txt_token<TimeFrame>(stream,"time frame",filename));
        result.set_time_range_indicator(get_txt_token<TimeRangeIndicator>(stream,"time range indicator",filename));
        if(result.is_intervaled()){
            result.set_N(get_txt_token<uint16_t>(stream,"N avg/acc",filename));
            result.set_N_missed(get_txt_token<uint8_t>(stream,"N missed",filename));
        }
        if(result.octet_doubled()){
            uint16_t tmp = get_txt_token<uint16_t>(stream,"P1",filename);
            result.set_P1(TimeForecast::period_t{static_cast<uint8_t>((tmp>>8)&0b1111111)});
            result.set_P1(TimeForecast::period_t{static_cast<uint8_t>(tmp&0b1111111)});
        }
        else if(result.is_unique_value())
            result.set_P1(TimeForecast::period_t{get_txt_token<uint8_t>(stream,"P1",filename)});
        else {
            result.set_P1(TimeForecast::period_t{get_txt_token<uint8_t>(stream,"P1",filename)});
            result.set_P2(TimeForecast::period_t{get_txt_token<uint8_t>(stream,"P2",filename)});
        }
        return result;
    }

    template<>
    Level get_txt_structure<Level>(std::istream& stream,const fs::path& filename){
        Level result;
        get_txt_token<void>(stream,"level",filename);
        result.set_type(get_txt_token<LevelsTags>(stream,"tag",filename));
        result.set_first_octet(get_txt_token<uint8_t>(stream,"o11",filename));
        result.set_second_octet(get_txt_token<uint8_t>(stream,"o12",filename));
        if(result.is_bounded()){
            get_txt_token<void>(stream,"top bound",filename);
            get_txt_token<void>(stream,"bottom bound",filename);
        }
        else{
            get_txt_token<void>(stream,"height",filename);
        }
        return result;
    }

    template<Data_f FORMAT, Data_t TYPE>
    struct HeaderInfo;

    template<>
    struct HeaderInfo<Data_f::GRIB_v1,Data_t::TIME_SERIES>{
        constexpr Data_f format() const{
            return Data_f::GRIB_v1;
        }

        constexpr Data_t type() const{
            return Data_t::TIME_SERIES;
        }
        std::vector<procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> param_info_;
        int version_ = 0;
    };

    //@todo may return invalid parameter and then conflict with function "read_data()"
    template<Data_f FORMAT,Data_t TYPE>
    CommonDataProperties<TYPE,FORMAT> get_common_data_properties(std::istream& file,const fs::path& filename){
        CommonDataProperties<TYPE,FORMAT> result;
        std::string param_name = get_txt_token<std::string>(file,"name",filename);
        result.parameter_ = get_txt_token<uint8_t>(file,"indicator",filename);
        result.table_version_ = get_txt_token<uint8_t>(file,"table version",filename);
        result.center_ = get_txt_token<Organization>(file,"center",filename);
        return result;
    }

    template<Data_f FORMAT,Data_t TYPE>
    procedures::extract::details::AdditionalExtractDataProperties<TYPE,FORMAT> 
            get_additional_data_properties(std::istream& file,const fs::path& filename){
        procedures::extract::details::AdditionalExtractDataProperties<TYPE,FORMAT> result;
        result.pos_ = Coord{.lat_ = get_txt_token<Lat>(file,"latitude",filename),
                            .lon_=get_txt_token<Lon>(file,"longitude",filename)};
        result.grid_ = get_txt_token<RepresentationType>(file,"grid",filename);
        result.fcst_ = get_txt_structure<TimeForecast>(file,filename);
        result.level_ = get_txt_structure<Level>(file,filename);
        return result;
    }
    
    template<Data_f FORMAT, Data_t TYPE>
    HeaderInfo<FORMAT,TYPE> read_header(std::istream& file,const fs::path& filename){
        using namespace procedures::extract::details;
        using ExtractProps = ExtractDataProperties<TYPE,FORMAT>;
        HeaderInfo<FORMAT,TYPE> result;
        if constexpr(TYPE == Data_t::TIME_SERIES){
            if constexpr(FORMAT==Data_f::GRIB_v1){
                result.version_ = get_txt_token<int>(file,"version",filename);
                size_t number_of_parameters = get_txt_token<size_t>(file,"parameters",filename);
                uint32_t current = 0;
                std::string buffer;
                for(size_t i = 0;i<number_of_parameters;++i){
                    if(!std::getline(file,buffer)){ 
                        ContextedError ctx_err(mashroom::errc::file_reading_error);
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read header")
                            .with_field("file",filename.c_str());
                        throw ctx_err;
                    }
                    else{
                        if(!buffer.starts_with("#") ||
                        !buffer.ends_with(":")){
                            ContextedError ctx_err(mashroom::errc::file_corrupted);
                                ctx_err.with_field("procedure","extract")
                                .with_field("at","read header")
                                .with_field("file",filename.c_str())
                                .with_field("token","header")
                                .with_field("type","parameter numeration");
                            throw ctx_err;
                        }
                        std::string_view sub(buffer);
                        sub = sub.substr(1,sub.size()-2);
                        if(std::from_chars(sub.begin(),sub.end(),current).ec!=std::errc() ||
                            current!=i+1){
                            ContextedError ctx_err(mashroom::errc::file_corrupted);
                                ctx_err.with_field("procedure","extract")
                                .with_field("at","read header")
                                .with_field("file",filename.c_str())
                                .with_field("token","header")
                                .with_field("type","parameter numeration");
                            throw ctx_err;
                        }
                    }
                    auto cmn = get_common_data_properties<FORMAT,TYPE>(file,filename);
                    auto add = get_additional_data_properties<FORMAT,TYPE>(file,filename);
                    result.param_info_.emplace_back(ExtractProps(
                        std::move(cmn),std::move(add)));
                    }
                return result;
            }
            else static_assert(false,"Not implemented");
        }
        else static_assert(false,"Not implemented");
    }

    //read the data after header
    template<Data_f FORMAT, Data_t TYPE>
    ExtractedValues<TYPE,FORMAT> read_data(std::ifstream& stream,const HeaderInfo<FORMAT,TYPE>& header_info,const fs::path& filename,const std::string& separator){
        std::string buffer;
        using namespace std::string_literals;
        if constexpr (FORMAT==Data_f::GRIB_v1){
            if constexpr(TYPE==Data_t::TIME_SERIES){
                ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
                if(header_info.version_==1){
                    if(!std::getline(stream,buffer) && buffer!="\\data"){ 
                        ContextedError ctx_err(mashroom::errc::file_reading_error);
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read data")
                            .with_field("file",filename.c_str());
                        throw ctx_err;
                    }
                    if(!std::getline(stream,buffer)){ 
                        ContextedError ctx_err(mashroom::errc::file_reading_error);
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read data")
                            .with_field("file",filename.c_str());
                        throw ctx_err;
                    }
                    std::vector<std::string> data_head_names = split_ignore<std::vector<std::string>>(buffer,separator);
                    //check if all parameters names are correctly defined and ordered
                    if(header_info.param_info_.size()!=data_head_names.size()-1) //because data_head_names includes "Time"
                    { 
                        ContextedError ctx_err(
                            mashroom::errc::file_reading_error,
                            "invalid parameters number");
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read data")
                            .with_field("file",filename.c_str());
                        throw ctx_err;
                    }
                    for(size_t i=0;i<header_info.param_info_.size();++i){
                        const CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props = header_info.param_info_.at(i).cmn_;
                        if(auto p_ptr = parameter_table(props.center_.value(),props.table_version_.value(),props.parameter_.value());p_ptr==nullptr)
                            continue;
                            // throw ErrorException(mashroom::errc::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,
                            //         "undefined parameter table (center="s+std::to_string(static_cast<std::underlying_type_t<Organization>>(*props.center_))+
                            //         ";table version="s+std::to_string(*props.table_version_)+";parameter="+
                            //         std::to_string(*props.parameter_)+")",filename.c_str());
                        else{
                            if(data_head_names[i+1]!=std::string_view(p_ptr->name)){ 
                                ContextedError ctx_err(
                                    mashroom::errc::file_reading_error,
                                    "incorrect order of parameters in data section");
                                    ctx_err.with_field("procedure","extract")
                                    .with_field("at","read data")
                                    .with_field("file",filename.c_str());
                                throw ctx_err;
                            }
                        }
                    }
                    for(;;){
                        typename ExtractedValue<Data_t::TIME_SERIES,FORMAT>::time_type time;
                        stream>>std::chrono::parse("%Y/%m/%d %H:%M:%S",time);
                        if(stream.fail()){
                            if(stream.eof())
                                return result;
                            else{ 
                                ContextedError ctx_err(
                                    mashroom::errc::file_reading_error,
                                    "incorrect time format");
                                    ctx_err.with_field("procedure","extract")
                                    .with_field("at","read data")
                                    .with_field("file",filename.c_str());
                                throw ctx_err;
                            }
                        }
                        if(getline(stream,buffer).fail()){ 
                            ContextedError ctx_err(
                                mashroom::errc::file_reading_error,
                                "at reading data values");
                                ctx_err.with_field("procedure","extract")
                                .with_field("at","read data")
                                .with_field("file",filename.c_str());
                            throw ctx_err;
                        }
                        std::vector<std::string> values_str = split_ignore<std::vector<std::string>>(buffer,separator);
                        if(values_str.size()<header_info.param_info_.size()){ 
                            ContextedError ctx_err(
                                mashroom::errc::file_reading_error,
                                "incorrect number of values in data section's string");
                                ctx_err.with_field("procedure","extract")
                                .with_field("at","read data")
                                .with_field("file",filename.c_str());
                            throw ctx_err;
                        }
                        size_t number_vals = data_head_names.size()-1;
                        for(int i=0;i<number_vals;++i){
                            if(values_str.at(i)=="NaN")
                                continue;
                            else {
                                typename ExtractedValue<Data_t::TIME_SERIES,FORMAT>::value_t value_parse;
                                if(std::from_chars(values_str.at(i).data(),values_str.at(i).data()+values_str.at(i).size(),value_parse).ec!=std::errc()){ 
                                    ContextedError ctx_err(
                                        mashroom::errc::file_reading_error,
                                        "invalid reading value as floating point");
                                        ctx_err.with_field("procedure","extract")
                                        .with_field("at","read data")
                                        .with_field("file",filename.c_str());
                                    throw ctx_err;
                                }
                                result[header_info.param_info_.at(i)].push_back(ExtractedValue<Data_t::TIME_SERIES,FORMAT>(time,value_parse));
                            }
                        }
                    }
                }
                else { 
                    ContextedError ctx_err(
                        mashroom::errc::file_reading_error,
                        "incorrect time format");
                        ctx_err.with_field("procedure","extract")
                        .with_field("at","read data")
                        .with_field("file",filename.c_str());
                    throw ctx_err;
                }
            }
            else static_assert(false,"Not implemented");
        }
        else static_assert(false,"Not implemented");
    }

}
ExtractedData read_txt_file(const std::stop_token& stop_token,const fs::path& filename){
    Data_f data_format;
    Data_t data_type;
    if(fs::exists(filename)){
        if(!fs::is_regular_file(filename)){ 
            ContextedError ctx_err(
                mashroom::errc::not_file);
                ctx_err.with_field("procedure","extract")
                .with_field("at","read txt file")
                .with_field("file",filename.c_str());
            throw ctx_err;
        }
        std::ifstream file(filename,std::ifstream::in);
        if(file.is_open()){
            ExtractedData data;
            std::string buffer;
            if((!std::getline(file,buffer) || buffer!="Mashroom") && 
                (!std::getline(file,buffer) || buffer!="\\header")){ 
                    ContextedError ctx_err(
                        mashroom::errc::file_corrupted,
                        "header not defined");
                        ctx_err.with_field("procedure","extract")
                        .with_field("at","read txt file")
                        .with_field("file",filename.c_str());
                    throw ctx_err;
                }
            while(std::getline(file,buffer) && !buffer.starts_with("type:")){}

            switch (procedures::extract::txt::details::get_txt_token<Data_t>(buffer,"type",filename))
            {
            case Data_t::TIME_SERIES:{
                switch (procedures::extract::txt::details::get_txt_token<Data_f>(file,"format",filename))
                {
                    case Data_f::GRIB_v1:{
                        return txt::details::read_data<Data_f::GRIB_v1,Data_t::TIME_SERIES>(file,
                        txt::details::read_header<Data_f::GRIB_v1,Data_t::TIME_SERIES>(file,buffer),filename," \t");
                        break;
                    }
                    //case ...@todo
                    default:
                    { 
                        ContextedError ctx_err(
                            mashroom::errc::file_corrupted,
                            "header not defined");
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read txt file")
                            .with_field("file",filename.c_str());
                        throw ctx_err;
                    }
                    break;
                }
                
                break;
            }
            case Data_t::POLYGONE:
                break;
            case Data_t::ISOLINES:
                break;
            case Data_t::GRID:
                break;
            default:
                { 
                    ContextedError ctx_err(
                        mashroom::errc::file_corrupted,
                        "header not defined");
                        ctx_err.with_field("procedure","extract")
                        .with_field("at","read txt file")
                        .with_field("file",filename.c_str());
                    throw ctx_err;
                }
                break;
            }
            
            //visitor
            return data;
        }
        else{ 
            ContextedError ctx_err(
                mashroom::errc::file_permission_denied);
                ctx_err.with_field("procedure","extract")
                .with_field("at","read txt file")
                .with_field("file",filename.c_str());
            throw ctx_err;
        }
    }
    else{
        ContextedError ctx_err(
            mashroom::errc::no_exists_path);
            ctx_err.with_field("procedure","extract")
            .with_field("at","read txt file")
            .with_field("file",filename.c_str());
        throw ctx_err;
    }
}

template<typename T>
T get_json_token(boost::json::value json,const char* token,const fs::path& filename){
    boost::json::error_code err;
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    //ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value_type
    if(auto ptr = json.find_pointer(token,err);ptr==nullptr){
        ContextedError ctx_err(
            mashroom::errc::file_reading_error,
            "json file doesn't contains token");
            ctx_err.with_field("procedure","extract")
            .with_field("at","get json token")
            .with_field("file",filename.c_str())
            .with_field("format","json")
            .with_field("token",token);
        throw ctx_err;
    }
    else {
        if(auto data_from_json = from_json<T>(*ptr);!data_from_json.has_value()){
            ContextedError ctx_err(
                mashroom::errc::file_reading_error,
                "invalid value in json file");
                ctx_err.with_field("procedure","extract")
                .with_field("at","get json token")
                .with_field("file",filename.c_str())
                .with_field("format","json")
                .with_field("token",token);
            throw ctx_err;
        }
        else return data_from_json.value();
    }
}

ExtractedData read_json_file(const std::stop_token& stop_token,const fs::path& filename){

    std::ifstream file(filename,std::ifstream::in);
    using namespace std::string_view_literals;
    boost::json::value json;
    if(auto json_tmp = parse_json_from_file(filename);  !json_tmp.has_value() ||
                                                    !json_tmp->is_object() || 
                                                    !json_tmp->as_object().contains("type") || 
                                                    !json_tmp->as_object().contains("format")){
        ContextedError ctx_err(
            mashroom::errc::file_reading_error,
            "missing json tokens");
            ctx_err.with_field("procedure","extract")
            .with_field("at","read json file")
            .with_field("file",filename.c_str())
            .with_field("token","type")
            .with_field("token","format");
        throw ctx_err;
    }
    else json = json_tmp.value();
    
    Data_f fmt = get_json_token<Data_f>(json,"/format",filename);
    Data_t type = get_json_token<Data_t>(json,"/type",filename);
    switch(fmt){
        case Data_f::GRIB_v1:{
            switch(type){
                case Data_t::TIME_SERIES:{
                    ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> values = get_json_token<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(json,"/data",filename);
                    return values;
                }
                default:{
                    ContextedError ctx_err(
                        mashroom::errc::file_corrupted,
                        "invalid data type");
                        ctx_err.with_field("procedure","extract")
                        .with_field("at","read json file")
                        .with_field("file",filename.c_str())
                        .with_field("value",type);
                    throw ctx_err;
                }
            }
        }
        default:{
            ContextedError ctx_err(
                mashroom::errc::file_corrupted,
                "invalid data format");
                ctx_err.with_field("procedure","extract")
                .with_field("at","read json file")
                .with_field("file",filename.c_str())
                .with_field("value",fmt);
            throw ctx_err;
        }
    }
}

ExtractedData read_bin_file(const std::stop_token& stop_token,const fs::path& filename){
    using namespace serialization;
    using namespace std::string_view_literals;
    std::ifstream file(filename,std::ifstream::in);
    if(!file.is_open()){
        if(!fs::exists(filename)){
            ContextedError ctx_err(
                mashroom::errc::no_exists_path);
                ctx_err.with_field("procedure","extract")
                .with_field("at","read bin file")
                .with_field("file",filename.c_str());
            throw ctx_err;
        }
        else if(!fs::is_regular_file(filename)){
            ContextedError ctx_err(
                mashroom::errc::not_file);
                ctx_err.with_field("procedure","extract")
                .with_field("at","read bin file")
                .with_field("file",filename.c_str());
            throw ctx_err;
        }
        else {
            ContextedError ctx_err(
                mashroom::errc::file_permission_denied);
                ctx_err.with_field("procedure","extract")
                .with_field("at","read bin file")
                .with_field("file",filename.c_str());
            throw ctx_err;
        }
    }
    Data_f fmt;
    Data_t type;
    if(auto ser_err = deserialize_from_file(file,type,fmt);ser_err!=SerializationEC::NONE){
        ContextedError ctx_err(
            mashroom::errc::deserialization_error);
            ctx_err.with_field("procedure","extract")
            .with_field("at","read bin file")
            .with_field("file",filename.c_str())
            .with_field("token","type")
            .with_field("token","format");
        throw ctx_err;
    }
    switch(fmt){
        case Data_f::GRIB_v1:{
            switch(type){
                case Data_t::TIME_SERIES:{
                    ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> values;
                    if(auto ser_err = deserialize_from_file(values,file);ser_err!=SerializationEC::NONE){
                        ContextedError ctx_err(
                            mashroom::errc::deserialization_error);
                            ctx_err.with_field("procedure","extract")
                            .with_field("at","read bin file")
                            .with_field("file",filename.c_str())
                            .with_field("token","extracted values");
                        throw ctx_err;
                    }
                    else return values;
                }
                default:{
                    ContextedError ctx_err(
                        mashroom::errc::file_corrupted,
                        "invalid data type");
                        ctx_err.with_field("procedure","extract")
                        .with_field("at","read bin file")
                        .with_field("file",filename.c_str())
                        .with_field("value",type);
                    throw ctx_err;
                }
            }
        }
        default:{
            ContextedError ctx_err(
                mashroom::errc::file_corrupted,
                "invalid data format");
                ctx_err.with_field("procedure","extract")
                .with_field("at","read bin file")
                .with_field("file",filename.c_str())
                .with_field("value",fmt);
            throw ctx_err;
        }
    }
    // deserialize_from_file()
}
}