#include "program/data.h"
#include "sys/error_exception.h"

template<>
ErrorCode Data::__read__<Data_f::GRIB_v1>(const fs::path& fn){
    using namespace serialization;
    std::ifstream file(fn,std::ios::binary);
    if(!file.is_open())
        return ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,fn.c_str());
    auto& ds = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    deserialize_from_file(ds,file);
    file.close();
    return ErrorCode::NONE;
}

template<>
ErrorCode Data::__write__<Data_f::GRIB_v1>(const fs::path& dir){
    using namespace serialization;
    if(!fs::create_directories(dir) && !fs::is_directory(dir))
        return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    fs::path save_file = dir/filename_by_format(Data_f::GRIB_v1);
    std::cout<<"Saved data file: "<<save_file<<std::endl;
    std::ofstream file(save_file,std::ios::binary);
    SerializationEC err = serialize_to_file(data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>(),file);
    if(err==SerializationEC::NONE)
        files_[Data_f::GRIB_v1]=save_file;
    else return ErrorPrint::print_error(ErrorCode::SERIALIZATION_ERROR,"grib data",AT_ERROR_ACTION::CONTINUE);
    file.close();
    return ErrorCode::NONE;
}
ErrorCode Data::read(const fs::path& filename) noexcept{
    ErrorCode err = ErrorCode::NONE;
    if(fs::exists(filename)){
        if(auto fmts = utility_token(filename.extension().string());!fmts.has_value())
            return ErrorPrint::print_error(ErrorCode::UNKNOWN_X1_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
        else{
            switch (fmts.value())
            {
            case Data_f::GRIB_v1:{
                return __read__<Data_f::GRIB_v1>(filename);err==ErrorCode::NONE;
                break;
            }
            default:
                return ErrorPrint::print_error(ErrorCode::UNKNOWN_X1_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
                break;
            }
        }
    }
    else return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
}
ErrorCode Data::write(const fs::path& filename) noexcept{
    if(!filename.has_extension())
        return ErrorPrint::print_error(ErrorCode::UNKNOWN_X1_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE,filename.extension().c_str());
    std::vector<Data_f> fmts;
    if(auto fmt_tmp = extension_to_tokens(filename.extension().c_str());!fmt_tmp.has_value())
        return ErrorPrint::print_error(ErrorCode::UNKNOWN_X1_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE,filename.extension().c_str());
    else fmts = fmt_tmp.value();
    std::ofstream file;
    if(!fs::exists(filename)){
        if(!fs::exists(filename.parent_path()))
            if(!fs::create_directories(filename.parent_path())){
                return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,filename.parent_path().c_str());
            }
    }
    file.open(filename,std::ios::trunc|std::ios::out);
    if(!file.is_open())
        return ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
    for(Data_f fmt:fmts){
        switch (fmt)
        {
        case FORMAT::GRIB_v1:{
            __write__<Data_f::GRIB_v1>(filename);
            unsaved_.erase(fmt);
            return ErrorCode::NONE;
            break;
        }
        default:
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Invalid file input",AT_ERROR_ACTION::CONTINUE);
            break;
        }
    }
    return ErrorCode::NONE;
}

#include <format>
#include "definitions/def.h"

template <Data_f I>
void Data::__write_all__(){
    if (static_cast<int>(I) < number_of_types()) {
        if(unsaved_.contains(I)){
            __write__<I>(data_directory_);
            unsaved_.erase(I);
        }
    }
    else assert(false);
}

void Data::save(){
    auto save_proxy = [this]<size_t... Is>(std::index_sequence<Is...>){
        (__write_all__<static_cast<Data_f>(Is)>(),...);
    };
    save_proxy(std::make_index_sequence<static_cast<size_t>(Data_f::GRIB_v1)+1>());
}