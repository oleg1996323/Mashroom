#include "program/data.h"
#include "sys/error.h"

template<>
osterlib::ContextedError Data::__read__<Data_f::GRIB_v1>(const fs::path& fn){
    using namespace serialization;
    std::ifstream file(fn,std::ios::binary);
    if(!fs::exists(fn))
        return osterlib::ContextedError(
                mashroom::errc::no_exists_path)
                .with_field("path",fn.c_str());
    if(!file.is_open())
        return osterlib::ContextedError(
                mashroom::errc::file_permission_denied)
                .with_field("file",fn.c_str());
    auto& ds = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    auto ser_res = deserialize_from_file(ds,file);
    if(ser_res!=SerializationEC::NONE)
        return osterlib::ContextedError(
                mashroom::errc::deserialization_error)
                .with_field("path",fn.c_str());
    file.close();
    return {};
}

template<>
osterlib::ContextedError Data::__write__<Data_f::GRIB_v1>(const fs::path& dir){
    using namespace serialization;
    if(!fs::create_directories(dir) && !fs::is_directory(dir)){
        return osterlib::ContextedError(
                mashroom::errc::not_directory)
                .with_field("path",dir.c_str());
    }
    fs::path save_file = dir/filename_by_format(Data_f::GRIB_v1);
    std::cout<<"Saved data file: "<<save_file<<std::endl;
    std::ofstream file(save_file,std::ios::binary);
    SerializationEC err = serialize_to_file(data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>(),file);
    if(err==SerializationEC::NONE)
        files_[Data_f::GRIB_v1]=save_file;
    else return osterlib::ContextedError(
                mashroom::errc::serialization_error,"grib data")
                .with_field("path",dir.c_str());
    file.close();
    return {};
}
osterlib::ContextedError Data::read(const fs::path& filename) noexcept{
    if(fs::exists(filename)){
        if(auto fmts = utility_token(filename.extension().string());!fmts.has_value())
            return osterlib::ContextedError(
                mashroom::errc::file_corrupted,"unknown format")
                .with_field("file",filename.c_str());
        else{
            switch (fmts.value())
            {
            case Data_f::GRIB_v1:{
                return __read__<Data_f::GRIB_v1>(filename);
                break;
            }
            default:
                return osterlib::ContextedError(
                mashroom::errc::file_corrupted,"unknown format")
                .with_field("file",filename.c_str());
                break;
            }
        }
    }
    else return osterlib::ContextedError(
                mashroom::errc::no_exists_path)
                .with_field("path",filename.c_str());
}
osterlib::ContextedError Data::write(const fs::path& filename) noexcept{
    if(!filename.has_extension())
        return osterlib::ContextedError(
                mashroom::errc::file_corrupted,"unknown format")
                .with_field("file",filename.c_str())
                .with_field("extension",filename.extension().c_str());
    std::vector<Data_f> fmts;
    if(auto fmt_tmp = extension_to_tokens(filename.extension().c_str());!fmt_tmp.has_value())
        return osterlib::ContextedError(
                mashroom::errc::file_corrupted,"unknown format")
                .with_field("file",filename.c_str())
                .with_field("extension",filename.extension().c_str());
    else fmts = fmt_tmp.value();
    std::ofstream file;
    if(!fs::exists(filename)){
        if(!fs::exists(filename.parent_path()))
            if(!fs::create_directories(filename.parent_path()))
                return osterlib::ContextedError(
                mashroom::errc::create_directory_denied)
                .with_field("dir",filename.parent_path().c_str());
    }
    file.open(filename,std::ios::trunc|std::ios::out);
    if(!file.is_open())
        return osterlib::ContextedError(
                mashroom::errc::file_permission_denied)
                .with_field("file",filename.c_str());
    for(Data_f fmt:fmts){
        switch (fmt)
        {
        case FORMAT::GRIB_v1:{
            __write__<Data_f::GRIB_v1>(filename);
            unsaved_.erase(fmt);
            return {};
            break;
        }
        default:
            return osterlib::ContextedError(
                mashroom::errc::internal_error,"invalid file input")
                .with_field("file",filename.c_str());
            break;
        }
    }
    return {};
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
        auto nothing = [](){return;};
        ((Is!=0?__write_all__<static_cast<Data_f>(Is)>():nothing()),...);
    };
    save_proxy(std::make_index_sequence<static_cast<size_t>(Data_f::GRIB_v1)>());
}