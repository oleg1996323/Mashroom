#include "application.h"
#include "extract_cpp.h"
#ifdef __cplusplus
    extern "C"{
        #include <stdio.h>
        #include <stdlib.h>
        #include <stdbool.h>
        #include <float.h>
        #include <assert.h>
        #include <string.h>
        #include <math.h>
        #include "extract.h"
        #include "PDS.h"
        #include "BDS.h"
        #include "GDS.h"
        #include "write.h"
        #include "ensemble.h"
        #include "func.h"
        #include "print.h"
        #include "ecmwf_ext.h"
        #include "error_handle.h"
        #include "read.h"
        #include "seek_grib.h"
        #include "levels.h"
        #include "Parm_Table.h"
        #include "def.h"
        #include <sys/types.h>
        #include <dirent.h>
        #include <unistd.h>
        #include <limits.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <errno.h>
        #include <decode_aux.h>
        #include <png.h>
        #include <zip.h>
    }
#endif
#include <set>
#include "compressor.h"
#include "error_code.h"
#include "err_msg.h"
#include "format.h"

namespace fs = std::filesystem;

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
        DATA_OUT fmt_out)
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
            if(!fs::exists(root_path/"format.bin")){
                log().record_log(ErrorCodeLog::BIN_FMT_FILE_MISS_IN_DIR_X1,"",root_path.c_str());
                return ErrorCode::INTERNAL_ERROR;
            }
            FormatData format = format::read(root_path/"format.bin");
            std::string fmt = functions::capitalize::get_txt_order(format.order);
            std::string str_reg(root_path.string());
            for(auto ch:fmt){
                switch(ch){
                    case 'Y':
                    str_reg = str_reg+R"(/(19[0-9]{2}|20[0-9]{2}))";
                    break;
                    case 'M':
                    str_reg = str_reg+R"(/(0?[1-9]|1[0-2]))";
                    break;
                    case 'D':
                    str_reg = str_reg+R"(/([1-2][0-9]|3[0-1]|0?[1-9]))";
                    break;
                    case 'H':
                    str_reg = str_reg+R"(/(0[1-9]|[12][0-9]|3[01]))";
                    break;
                    case 'L':
                    str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?)))";
                    break;
                    case 'O':
                    str_reg = str_reg+R"(/(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                    break;
                    case 'C':
                    str_reg = str_reg+R"(/(?:lat(-?90(?:\.[0]+)?|[0-8]?[0-9](?:\.[0-9]+)?))_(?:lon(-?180(?:\.[0]+)?|1[0-7]?[0-9](?:\.[0-9]+)?|0?[0-9]?[0-9](?:\.[0-9]+)?)))";
                    break;
                    default:
                        break;
                }
            }
            switch (format.order.fmt)
            {
                case DATA_FORMAT::GRIB:
                    f_format.insert(".grib");
                    f_format.insert(".grb");
                    break;
                case DATA_FORMAT::BINARY:
                    f_format.insert(".omdb");
                    break;
                default:
                    LogError::message(ErrorCodeLog::FORMAT_FILE_CORRUPTED,"");
                    return ErrorCode::INTERNAL_ERROR;
                    break;
            }
            str_reg.append("$");
        }
        time_point<system_clock> beg = system_clock::now();
        std::map<std::string,StampVal*> tags;
        {
            bool skip_increment = false;
            auto iterator=fs::recursive_directory_iterator(root_path);
            auto end=fs::end(iterator);
            while(iterator!=end){
                std::smatch m;
                TaggedValues result;
                const std::string& p = iterator->path().string();
                std::cout<<p<<std::endl;
                if(!std::regex_match(p,m,reg)){
                    //std::cout<<p<<" : Not matched."<<std::endl;
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
                                result = extract_val_by_coord_grib(from,to,coord,iterator->path().c_str(),0,0);
                                for(int i=0;i<result.sz;++i)
                                    tags[result.vals[i].tag]=result.vals[i].values;
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
                            for(auto& [tag,values]:tags)
                                out<<std::left<<std::setw(10)<<tag<<'\t';
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
        DATA_OUT out_fmt){return ErrorCode::INTERNAL_ERROR;} //unused method
}