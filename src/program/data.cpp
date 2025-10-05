#include "program/data.h"

template<>
void Data::__read__<Data::FORMAT::GRIB>(const fs::path& fn){
    using namespace serialization;
    std::ifstream file(fn,std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,fn.c_str());
    auto& ds = data_struct<Data_t::METEO,Data_f::GRIB>();
    deserialize_from_file(ds.sublimed_,file);
    for(auto& [filename,file_data]:ds.sublimed_.data()){
        for(auto& [common,grib_data]:file_data){
            // std::cout<<"Added to \"by common\":"<<*grib_.by_common_data_[common].insert(filename).first<<std::endl;
            // std::cout<<"Parameter: "<<parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name<<std::endl;
            // std::cout<<"Center name: "<<center_to_abbr(common->center_.value())<<std::endl;
            // std::cout<<"Table version: "<<(int)common->table_version_.value()<<std::endl;
            ds.by_common_data_[common].insert(filename);
            for(const auto& sub_data:grib_data){
                ds.by_date_[sub_data.sequence_time_].insert(filename);
                ds.by_grid_[sub_data.grid_data_].insert(filename);
            }
        }
    }
    file.close();
}

template<>
void Data::__write__<Data::FORMAT::GRIB>(const fs::path& dir){
    using namespace serialization;
    if(!fs::create_directories(dir) && !fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    fs::path save_file = dir/filename_by_type(Data::FORMAT::GRIB);
    std::cout<<"Saved data file: "<<save_file<<std::endl;
    std::ofstream file(save_file,std::ios::binary);
    SerializationEC err = serialize_to_file(data_struct<Data_t::METEO,Data_f::GRIB>().sublimed_,file);
    if(err==SerializationEC::NONE)
        files_[Data::FORMAT::GRIB]=save_file;
    else ErrorPrint::print_error(ErrorCode::SERIALIZATION_ERROR,"grib data",AT_ERROR_ACTION::CONTINUE);
    file.close();
}
void Data::read(const fs::path& filename){
    if(fs::exists(filename)){
        __Data__::FORMAT fmt = extension_to_type(filename.extension().c_str());
        if(fmt==__Data__::FORMAT::UNDEF){
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Invalid file input",AT_ERROR_ACTION::CONTINUE);
            return;
        }
        switch ((FORMAT)fmt)
        {
        case Data::FORMAT::GRIB:{
            __read__<Data::FORMAT::GRIB>(filename);
            break;
        }
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Invalid file input",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
}
bool Data::write(const fs::path& filename){
    if(!filename.has_extension()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE);
        return false;
    }
    __Data__::FORMAT fmt = extension_to_type(filename.extension().c_str());
    if(fmt==__Data__::FORMAT::UNDEF){
        ErrorPrint::print_error(ErrorCode::UNKNOWN_X1_FORMAT_FILE,"",AT_ERROR_ACTION::CONTINUE,filename.extension().c_str());
        return false;
    }
    std::ofstream file;
    if(!fs::exists(filename)){
        if(!fs::exists(filename.parent_path()))
            if(!fs::create_directories(filename.parent_path())){
                ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,filename.parent_path().c_str());
                return false;
            }
    }
    file.open(filename,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
        return false;
    }
    switch ((FORMAT)fmt)
    {
    case FORMAT::GRIB:{
        __write__<FORMAT::GRIB>(filename);
        unsaved_.erase((FORMAT)fmt);
        return true;
        break;
    }
    default:
        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Invalid file input",AT_ERROR_ACTION::CONTINUE);
        return false;
        break;
    }
}

#include <format>
#include "definitions/def.h"

template <size_t I=0>
void Data::__write_all__(){
    if constexpr (I < data_types.size()) {
        if(unsaved_.contains((FORMAT)(I+1))){
            __write__<data_types[I]>(data_directory_);
            unsaved_.erase((FORMAT)(I+1));
        }
        __write_all__<I + 1>();
    }
}

void Data::save(){
    __write_all__<0>();
}