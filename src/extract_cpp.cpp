#include <cassert>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <decode_aux.h>
#include <set>
#include "application.h"
#include "extract_cpp.h"
#include "extract.h"
#include "format.h"
#include "compressor.h"

namespace fs = std::filesystem;
using namespace std::string_literals;
namespace cpp{

    DATA_OUT operator|(DATA_OUT lhs,DATA_OUT rhs){
        return (DATA_OUT)((int)lhs|(int)(rhs));
    }
    DATA_OUT operator&(DATA_OUT lhs,DATA_OUT rhs){
        return (DATA_OUT)((int)lhs&(int)(rhs));
    }
    DATA_OUT& operator|=(DATA_OUT& lhs,DATA_OUT rhs){
        return lhs=lhs|rhs;
    }
    DATA_OUT& operator&=(DATA_OUT& lhs,DATA_OUT rhs){
        return lhs=lhs&rhs;
    }
    DATA_OUT operator^(DATA_OUT lhs,DATA_OUT rhs){
        return (DATA_OUT)((int)lhs^(int)(rhs));
    }

    bool operator<(const Date& lhs,const Date& rhs){
        if (lhs.year != rhs.year) return lhs.year < rhs.year;
        if (lhs.month != rhs.month) return lhs.month < rhs.month;
        if (lhs.day != rhs.day) return lhs.day < rhs.day;
        return lhs.hour < rhs.hour;
    }
    
    bool operator<(const StampVal& lhs,const StampVal& rhs){
        if(lhs.time.year==-1?0:lhs.time.year<rhs.time.year==-1?0:rhs.time.year)
            return true;
        else if(lhs.time.year==-1?0:lhs.time.year>rhs.time.year==-1?0:rhs.time.year)
            return true;
        
        if(lhs.time.month==-1?0:lhs.time.month<rhs.time.month==-1?0:rhs.time.month)
            return true;
        else if(lhs.time.month==-1?0:lhs.time.month>rhs.time.month==-1?0:rhs.time.month)
            return false;
    
        if(lhs.time.day==-1?0:lhs.time.day<rhs.time.day==-1?0:rhs.time.day)
            return true;
        else if(lhs.time.day==-1?0:lhs.time.day>rhs.time.day==-1?0:rhs.time.day)
            return false;
    
        if(lhs.time.hour==-1?0:lhs.time.hour<rhs.time.hour==-1?0:rhs.time.hour)
            return true;
        else if(lhs.time.hour==-1?0:lhs.time.hour>rhs.time.hour==-1?0:rhs.time.hour)
            return false;
        return false;
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

    ErrorCode extract_cpp_pos(const std::filesystem::path& root_path,
        const std::filesystem::path& destination,
        Date from, 
        Date to,
        Coord coord,
        DATA_OUT fmt_out,
        float* progress)
    {
        std::map<char,int> fmt_pos;
        std::regex reg;
        std::set<std::string> f_format;
        {
            if(!fs::exists(destination)){
                if(!fs::create_directories(destination))
                    log().record_log(ErrorCodeLog::CREATE_DIR_X1_DENIED,"",destination.c_str());
                    return ErrorCode::INTERNAL_ERROR;
            }
            if(!fs::exists(root_path/format_filename)){
                log().record_log(ErrorCodeLog::BIN_FMT_FILE_MISS_IN_DIR_X1,"",root_path.c_str());
                return ErrorCode::INTERNAL_ERROR;
            }
            FormatBinData format = format::read(root_path/format_filename);
            if(correct_date_interval(&from,&to) && (get_epoch_time(&format.data_.from)>get_epoch_time(&to) || get_epoch_time(&format.data_.to)<get_epoch_time(&from))){
                ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::CONTINUE);
                return ErrorCode::INCORRECT_DATE;
            }
            else if(!point_in_grid(&format.data_.grid_data,coord)){
                ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE);
                return ErrorCode::INCORRECT_COORD;
            }
                
            std::string fmt = functions::capitalize::get_txt_order(format.order_);
            std::string str_reg("^"s+root_path.string());
            for(int i = 0;i<fmt.size();++i){
                switch(fmt[i]){
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
            switch (format.order_.fmt)
            {
                case DataFormat::GRIB:
                    str_reg = str_reg+R"((/([^/\\]+).grib))";
                    f_format.insert(".grib");
                    f_format.insert(".grb");
                    break;
                case DataFormat::BINARY:
                    str_reg = str_reg+R"(/*.omdb)";
                    f_format.insert(".omdb");
                    break;
                default:
                    LogError::message(ErrorCodeLog::FORMAT_FILE_CORRUPTED,"");
                    return ErrorCode::INTERNAL_ERROR;
                    break;
            }
            str_reg.append("$");
            std::cout<<str_reg<<std::endl;
            reg = str_reg;
        }
        time_point<system_clock> beg = system_clock::now();
        std::unordered_map<CommonDataProperties,std::vector<ExtractedValue>> summary_result;
        {
            bool skip_increment = false;
            auto iterator=fs::recursive_directory_iterator(root_path);
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
                    int year;
                    int month;
                    int day = 0;
                    int hour = 0;
                    assert(fmt_pos.contains('Y'));
                    if(fmt_pos.contains('Y')){
                        std::ssub_match year_m = m[fmt_pos.at('Y')];
                        year = std::stoi(year_m);
                        if(from.year>year ||
                            to.year<year){
                            ++iterator;
                            continue;
                        }
                    }
                    if(fmt_pos.contains('M')){
                        std::ssub_match month_m = m[fmt_pos.at('M')];
                        month = std::stoi(month_m);
                        if(from.month>month ||
                            to.month<month){
                            ++iterator;
                            continue;
                        }
                    }
                    if(fmt_pos.contains('D')){
                        std::ssub_match day_m = m[fmt_pos.at('D')];
                        day = std::stoi(day_m);
                        if(from.day>day ||
                            to.day<day){
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
                                result = extract(from,to,coord,iterator->path());
                                for(const auto& [cmn_data,values]:result)
                                    summary_result[cmn_data].append_range(values);
                            }
                            ++iterator;
                        }
                        if(fmt_out&DATA_OUT::TXT_F || fmt_out&DATA_OUT::DEFAULT){
                            fs::path out_f_name = (destination/std::to_string(year)/std::to_string(month)).string()+std::string(".txt");
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
                            for(auto& [cmn_data,values]:summary_result)
                                //TODO:sort values
                                out<<std::left<<std::setw(10)<<parameter_table(cmn_data.center_,cmn_data.subcenter_,cmn_data.parameter_)->name<<'\t';
                            out<<std::endl;
                            int row=0;
                            while(row<result.vals->sz){
                                out<<result.vals->values[row].time<<'\t';
                                for(auto& [tag,vals]:tags){
                                    out<<std::left<<std::setw(10)<<vals[row].val<<'\t';
                                }
                                out<<std::endl;
                                ++row;
                            }
                            delete_values(result);
                            tags.clear();
                            out.close();
                        }
                        else if(fmt_out&DATA_OUT::BIN_F){
                            std::string out_f_name = (destination/(std::to_string(year)+'_'+std::to_string(month)+".omdb")).string();
                            std::ofstream out(out_f_name,std::ios::trunc|std::ios::binary);
                            if(!out.is_open()){
                                log().record_log(ErrorCodeLog::CANNOT_OPEN_FILE_X1,"",out_f_name);
                                return ErrorCode::INTERNAL_ERROR;
                            }
                            else
                                std::cout<<"Openned "<<out_f_name<<std::endl;
                            
                            out<<"Mashroom extractor v=0.01\nData formats: ECMWF\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n";
                            //print header
                            //print data to file
                            
                            for(auto& [tag,values]:tags){
                                out.write((char*)tag.size(),sizeof(tag.size()));
                                out.write(tag.c_str(),tag.size());
                            }
                            out<<std::endl;
                            int row=0;
                            while(row<result.vals->sz){
                                out<<result.vals->values[row].time<<'\t';
                                for(auto& [tag,vals]:tags){
                                    out.write((char*)&vals[row].val,sizeof(vals[row].val));
                                }
                                out<<std::endl;
                                ++row;
                            }
                            delete_values(result);
                            tags.clear();
                            out.close();
                        }
                        
                    }
                    //output<<result.time.day<<"/"<<result.time.month<<"/"<<result.time.year<<" "<<result.time.hour<<":"<<result.val<<std::endl;
                }
                skip_increment?iterator:++iterator,skip_increment=false;
            }
            if(fmt_out&DATA_OUT::ARCHIVED){
                cpp::zip_ns::Compressor cmprs(destination,"any.zip"); //TODO: change name from typevals (ERA5,Merra2) and time interval
                for(fs::directory_entry entry:fs::recursive_directory_iterator(destination)){
                    if(!entry.is_regular_file())
                        continue;
                    cmprs.add_file(destination,entry.path());
                }                    
            }
        }
        return ErrorCode::NONE;
    }

    ErrorCode extract_cpp_rect(const std::filesystem::path& root_path,
        Date from, 
        Date to,
        Rect rect,
        DATA_OUT out_fmt,
        float* progress){return ErrorCode::INTERNAL_ERROR;} //unused method
}