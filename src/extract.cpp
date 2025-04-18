#include <cassert>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <set>
#include <map>
#include <regex>
#include "extract.h"
#include "compressor.h"
#include <iostream>
#include "extract.h"
#include "def.h"
#include "message.h"
#include "aux_code/int_pow.h"
#include "types/data_info.h"
#include "types/data_binary.h"
#include "generated/code_tables/eccodes_tables.h"

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs|(int)(rhs));
}
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs&(int)(rhs));
}
Extract::ExtractFormat& operator|=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs){
    return lhs=lhs|rhs;
}
Extract::ExtractFormat& operator&=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs){
    return lhs=lhs&rhs;
}
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs^(int)(rhs));
}

namespace fs = std::filesystem;
using namespace std::chrono;
ExtractedData Extract::__extract__(const fs::path& file){
    ExtractedData result;
    HGrib1 grib;
    try{
        grib.open_grib(file);
    }
    catch(const std::runtime_error& err){
        std::cerr<<err.what()<<std::endl;
        exit(0);
    }
    
    if(grib.file_size()==0)
        return result;
    do{
        const auto& msg = grib.message();
        if(!msg.has_value())
            throw std::runtime_error("Message undefined");
        if(msg.value().get().message_length()==0){
            grib.next_message();
        }

		//ReturnVal result_date;
        GribMsgDataInfo msg_info(   msg.value().get().section_2_.define_grid(),
                                    msg.value().get().section_1_.date(),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg.value().get().section_1_.IndicatorOfParameter(),
                                    msg.value().get().section_1_.unit_time_range(),
                                    msg.value().get().section_1_.center(),
                                    msg.value().get().section_1_.table_version());
        if(props_.common_.has_value()){
            const CommonDataProperties& common_tmp = props_.common_.value();
            if(msg_info.center!=common_tmp.center_)
                continue;
            if(msg_info.table_version!=common_tmp.table_version_)
                continue;
            if(common_tmp.parameter_!=common_tmp.parameter_)
                continue;
            if(common_tmp.fcst_unit_.has_value() && msg_info.t_unit!=common_tmp.fcst_unit_.value())
                continue;
        }
        if(props_.grid_type_.has_value()){
            if(!msg_info.grid_data.has_value())
                continue;
            if(props_.grid_type_.value()!=msg_info.grid_data.value().rep_type)
                continue;                  
            if(msg_info.date>props_.to_date_ || msg_info.date<props_.from_date_)
                continue;
            if(!pos_in_grid(props_.position_.value(),msg_info.grid_data.value()))
                continue;
        }
        /* decode numeric data */
        if(msg_info.grid_data.has_value())
            result[CommonDataProperties(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)].push_back(
                ExtractedValue{msg_info.date,msg.value().get().extract_value(value_by_raw(props_.position_.value(),msg_info.grid_data.value()))});
        else continue; //TODO still not accessible getting data without position
    }
    while(grib.next_message());
    return result;
}

using namespace std::string_literals;

bool operator<(const Date& lhs,const Date& rhs){
    if (lhs.year != rhs.year) return lhs.year < rhs.year;
    if (lhs.month != rhs.month) return lhs.month < rhs.month;
    if (lhs.day != rhs.day) return lhs.day < rhs.day;
    return lhs.hour < rhs.hour;
}

std::ostream& operator<<(std::ostream& stream,Date date){
    std::ostringstream oss;
    oss << date.year << '/' 
        << std::setw(2) << std::setfill('0') << date.month << '/' 
        << std::setw(2) << std::setfill('0') << date.day << " "
        << std::setw(2) << std::setfill('0') << date.hour << ":00";
    
    stream << std::setw(18) << std::left << oss.str();
    return stream;
}

#include <format>
#include <chrono>

ErrorCode Extract::execute(){
    std::map<char,int> fmt_pos;
    std::regex reg;
    std::set<std::string> f_format;
    {
        if(!fs::exists(dest_directory_/format_filename)){
            log().record_log(ErrorCodeLog::BIN_FMT_FILE_MISS_IN_DIR_X1,"",dest_directory_.c_str());
            return ErrorCode::INTERNAL_ERROR;
        }
        BinaryGribInfo::sublimed_data_t sublimed_data = BinaryGribInfo::read(dest_directory_/format_filename).sublime();
        std::vector<std::pair<std::string_view,ptrdiff_t>> file_pos;
        {
            if(!is_correct_interval(props_.from_date_,props_.to_date_)){
                ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::CONTINUE);
                return ErrorCode::INCORRECT_DATE;
            }
            if(!props_.position_.has_value()){
                ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"Not defined",AT_ERROR_ACTION::CONTINUE);
                return ErrorCode::INCORRECT_COORD;
            }
            else{
                if(!is_correct_pos(&props_.position_.value())){
                    ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE);
                    return ErrorCode::INCORRECT_COORD;
                }
            }
            bool found = false;
            if(props_.common_.has_value()){
                const CommonDataProperties& common_tmp = props_.common_.value();
                for(const auto& [filename,file_data]:sublimed_data){
                    if(common_tmp.fcst_unit_.value()!=d.first.fcst_unit_)
                        continue;
                    if(common_tmp.center_!=d.first.center_)
                        continue;
                    if(common_tmp.table_version_!=d.first.table_version_)
                        continue;
                    for(const auto& spec_data:d.second){
                        if(spec_data.from>props_.to_date_ || spec_data.to<props_.from_date_)
                            continue;
                            //TODO without position definition
                        else if(!spec_data.grid_data.has_value() || !pos_in_grid(props_.position_.value(),spec_data.grid_data.value()))
                            continue;
                    }
                }
                if(!found)
                    return ErrorCode::NONE;
            }
        }
            
        std::string str_reg("^"s+root_dir_.string());
        for(int i = 0;i<format.order().size();++i){
            switch(format.order()[i]){
                case 'y':
                case 'Y':
                    str_reg = str_reg+R"(/(19[0-9]{2}|20[0-9]{2}))";
                    fmt_pos['Y']=i+1;
                    break;
                case 'm':
                case 'M':
                    str_reg = str_reg+R"(/(0?[1-9]|1[0-2]))";
                    fmt_pos['M']=i+1;
                    break;
                case 'd':
                case 'D':
                    str_reg = str_reg+R"(/([1-2][0-9]|3[0-1]|0?[1-9]))";
                    fmt_pos['D']=i+1;
                    break;
                case 'h':
                case 'H':
                    str_reg = str_reg+R"(/(0[1-9]|[12][0-9]|3[01]))";
                    fmt_pos['H']=i+1;
                    break;
                case 'l':
                case 'L':
                    str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?)))";
                    fmt_pos['L']=i+1;
                    break;
                case 'o':
                case 'O':
                    str_reg = str_reg+R"(/(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                    fmt_pos['O']=i+1;
                    break;
                case 'c':
                case 'C':
                    str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?))_(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                    fmt_pos['C']=i+1;
                    break;
                default:
                    break;
            }
        }
        str_reg = str_reg+R"((/([^/\\]+).grib))";
        f_format.insert(".grib");
        f_format.insert(".grb");
        // switch (format.order())
        // {
        //     case DataFormat::GRIB:
        //         str_reg = str_reg+R"((/([^/\\]+).grib))";
        //         f_format.insert(".grib");
        //         f_format.insert(".grb");
        //         break;
        //     case DataFormat::BINARY:
        //         str_reg = str_reg+R"(/*.omdb)";
        //         f_format.insert(".omdb");
        //         break;
        //     default:
        //         LogError::message(ErrorCodeLog::FORMAT_FILE_CORRUPTED,"");
        //         return ErrorCode::INTERNAL_ERROR;
        //         break;
        // }
        str_reg.append("$");
        std::cout<<str_reg<<std::endl;
        reg = str_reg;
    }
    year_month_day ymd_from(floor<days>(props_.from_date_));
    year_month_day ymd_to(floor<days>(props_.to_date_));
    hh_mm_ss hms_from(props_.to_date_- floor<days>(props_.from_date_));
    hh_mm_ss hms_to(props_.to_date_- floor<days>(props_.to_date_));
    std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>> summary_result;
    {
        bool skip_increment = false;
        auto iterator=fs::recursive_directory_iterator(root_dir_);
        auto end=fs::end(iterator);
        while(iterator!=end){
            std::smatch m;
            std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>> result;
            const std::string& p = iterator->path().string();
            std::cout<<p<<std::endl;
            if(!std::regex_match(p,m,reg)){
                std::cout<<p<<" : Not matched."<<std::endl;
                ++iterator;
                continue;
            }
            else{
                int year_;
                int month_;
                int day_ = 0;
                int hour_ = 0;
                assert(fmt_pos.contains('Y'));
                if(fmt_pos.contains('Y')){
                    std::ssub_match year_m = m[fmt_pos.at('Y')];
                    year_ = std::stoi(year_m);
                    if((int)ymd_from.year()>year_ ||
                        (int)ymd_to.year()<year_){
                        ++iterator;
                        continue;
                    }
                }
                if(fmt_pos.contains('M')){
                    std::ssub_match month_m = m[fmt_pos.at('M')];
                    month_ = std::stoi(month_m);
                    if((unsigned)ymd_from.month()>month_ ||
                        (unsigned)ymd_to.month()<month_){
                        ++iterator;
                        continue;
                    }
                }
                if(fmt_pos.contains('D')){
                    std::ssub_match day_m = m[fmt_pos.at('D')];
                    day_ = std::stoi(day_m);
                    if((unsigned)ymd_from.day()>day_ ||
                        (unsigned)ymd_to.day()<day_){
                        ++iterator;
                        continue;
                    }
                }
                if(fmt_pos.contains('L')){
                    std::ssub_match lat_m = m[fmt_pos.at('L')];
                }
                if(fmt_pos.contains('O')){
                    std::ssub_match lon_m = m[fmt_pos.at('O')];
                }
                if(fmt_pos.contains('C'))
                    std::ssub_match lon_m = m[fmt_pos.at('C')];

                ++iterator;
                
                if(iterator.depth()==fmt_pos.size()){
                    while(iterator!=fs::end(iterator) && iterator->is_regular_file()){
                        skip_increment=true;
                        if(f_format.contains(iterator->path().extension().string())){
                            std::cout<<"Extracting from "<<iterator->path()<<std::endl;
                            result = __extract__(iterator->path());
                            for(const auto& [cmn_data,values]:result)
                                summary_result[cmn_data].append_range(values);
                        }
                        ++iterator;
                    }
                    if(static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
                    static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::TXT_F) || 
                    static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
                    static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::DEFAULT)){
                        fs::path out_f_name = (dest_directory_/std::to_string(year_)/std::to_string(month_)).string()+std::string(".txt");
                        if(!fs::exists(out_f_name.parent_path()))
                            if(!fs::create_directories(out_f_name.parent_path())){
                                log().record_log(ErrorCodeLog::CANNOT_ACCESS_PATH_X1,"",out_f_name.relative_path().c_str());
                                return ErrorCode::INTERNAL_ERROR;
                            }
                        std::ofstream out(out_f_name,std::ios::trunc);
                        if(!out.is_open()){
                            if(fs::exists(out_f_name) && fs::is_regular_file(out_f_name) && fs::status(out_f_name).permissions()>fs::perms::none){
                                log().record_log(ErrorCodeLog::FILE_X1_PERM_DENIED,"",out_f_name.c_str());
                                return ErrorCode::INTERNAL_ERROR;
                            }   
                        }
                        else
                            std::cout<<"Openned "<<out_f_name<<std::endl;

                        out<<"Mashroom extractor v=0.01\nData formats: ECMWF\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n";
                        //print header
                        //print data to file
                        out<<std::left<<std::setw(18)<<"Time"<<"\t";
                        std::vector<decltype(summary_result)::node_type::mapped_type*> col_vals_;
                        size_t max_length = 0;
                        for(auto& [cmn_data,values]:summary_result){
                            std::sort(values.begin(),values.end());
                            max_length = std::max(values.size(),max_length);
                            out<<std::left<<std::setw(10)<<parameter_table(cmn_data.center_,cmn_data.table_version_,cmn_data.parameter_)->name<<'\t';
                            col_vals_.push_back(&values);
                        }
                        out<<std::endl;
                        int row=0;
                        while(row<max_length){
                            std::chrono::system_clock::time_point min_time = std::chrono::system_clock::time_point::max();
                            for(const auto item:col_vals_)
                                min_time = std::min((*item)[row].time_date,min_time);
                            out<<std::format("{:%Y/%b/%d %H}",min_time)<<'\t';
                            for(auto val:col_vals_){
                                if((*val)[row].time_date==min_time)
                                    out<<std::left<<std::setw(10)<<(*val)[row].value<<'\t';
                                else out<<std::left<<std::setw(10)<<"NaN"<<'\t';
                            }
                            out<<std::endl;
                            ++row;
                        }
                        summary_result.clear();
                        out.close();
                    }
                    // else if(fmt_out&Extract::ExtractFormat::BIN_F){
                    //     std::string out_f_name = (destination/(std::to_string(year)+'_'+std::to_string(month)+".omdb")).string();
                    //     std::ofstream out(out_f_name,std::ios::trunc|std::ios::binary);
                    //     if(!out.is_open()){
                    //         log().record_log(ErrorCodeLog::CANNOT_OPEN_FILE_X1,"",out_f_name);
                    //         return ErrorCode::INTERNAL_ERROR;
                    //     }
                    //     else
                    //         std::cout<<"Openned "<<out_f_name<<std::endl;
                        
                    //     out<<"Mashroom extractor v=0.01\nData formats: ECMWF\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n";
                    //     //print header
                    //     //print data to file
                        
                    //     for(auto& [tag,values]:tags){
                    //         out.write((char*)tag.size(),sizeof(tag.size()));
                    //         out.write(tag.c_str(),tag.size());
                    //     }
                    //     out<<std::endl;
                    //     int row=0;
                    //     while(row<result.vals->sz){
                    //         out<<result.vals->values[row].time<<'\t';
                    //         for(auto& [tag,vals]:tags){
                    //             out.write((char*)&vals[row].val,sizeof(vals[row].val));
                    //         }
                    //         out<<std::endl;
                    //         ++row;
                    //     }
                    //     delete_values(result);
                    //     tags.clear();
                    //     out.close();
                    // }
                    
                }
                //output<<result.time.day<<"/"<<result.time.month<<"/"<<result.time.year<<" "<<result.time.hour<<":"<<result.val<<std::endl;
            }
            skip_increment?iterator:++iterator,skip_increment=false;
        }
        if(static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
            static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::ARCHIVED)){
            cpp::zip_ns::Compressor cmprs(dest_directory_,"any.zip"); //TODO: change name from typevals (ERA5,Merra2) and time interval
            for(fs::directory_entry entry:fs::recursive_directory_iterator(dest_directory_)){
                if(!entry.is_regular_file())
                    continue;
                cmprs.add_file(dest_directory_,entry.path());
            }                    
        }
    }
    return ErrorCode::NONE;
}