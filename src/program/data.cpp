#include "program/data.h"

template<>
inline bool Data::write<Data::TYPE::METEO,Data::FORMAT::GRIB>
(                   std::vector<char>& buf,
                    Organization center,
                    std::optional<TimeFrame> time_fcst,
                    const std::unordered_set<SearchParamTableVersion>& parameters,
                    TimeInterval time_interval,
                    RepresentationType rep_t,
                    Coord pos)
{
    using namespace serialization;
    for(auto& [path,matched]:match(center,time_fcst,parameters,time_interval,rep_t,pos))
        if(path.type_==path::TYPE::FILE)
            serialize_native(matched,buf);
    for(const SearchParamTableVersion& param:parameters){
        ///@todo
    }
    assert(false);
}

template<>
void Data::__read__<Data::FORMAT::GRIB>(const fs::path& fn){
    using namespace serialization;
    std::ifstream file(fn,std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,fn.c_str());
    deserialize_from_file(grib_.grib_data_,file);
    for(auto& [filename,file_data]:grib_.grib_data_.data()){
        for(auto& [common,grib_data]:file_data){
            // std::cout<<"Added to \"by common\":"<<*grib_.by_common_data_[common].insert(filename).first<<std::endl;
            // std::cout<<"Parameter: "<<parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name<<std::endl;
            // std::cout<<"Center name: "<<center_to_abbr(common->center_.value())<<std::endl;
            // std::cout<<"Table version: "<<(int)common->table_version_.value()<<std::endl;
            grib_.by_common_data_[common].insert(filename);
            for(const auto& sub_data:grib_data){
                grib_.by_date_[TimeInterval{sub_data.from_,sub_data.to_}].insert(filename);
                grib_.by_grid_[sub_data.grid_data_].insert(filename);
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
    SerializationEC err = serialize_to_file(grib_.grib_data_,file);
    if(err==SerializationEC::NONE)
        files_.insert(save_file);
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
std::unordered_map<path::Storage<true>,SublimedDataInfo> Data::match(
    Organization center,
    uint8_t table_version,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::unordered_map<path::Storage<true>,SublimedDataInfo> result;
    for(const auto& [cmn,seq_fn]:grib_.by_common_data_){
        if(!cmn.expired() && cmn.lock()->center_==center && cmn.lock()->table_version_==table_version){
            for(auto fn:seq_fn)
                result[fn];
        }
    }
    for(const auto& [date_interval,seq_fn]:grib_.by_date_)
        if(!intervals_intersect(time_interval,date_interval))
            for(auto fn:seq_fn)
                result.erase(fn);
    for(const auto& [grid,seq_fn]:grib_.by_grid_){
        if(grid.has_value())
            for(auto fn:seq_fn)
                result.erase(fn);
        if(!pos_in_grid(pos,grid.value()))
            for(auto fn:seq_fn)
                result.erase(fn);
    }
    for(auto& [fn,ptrs]:result){
        for(const auto& [common,data_seq]:grib_.grib_data_.data().at(fn))
            if(common->center_==center)
                for(const auto& d:data_seq){
                    if(d.grid_data_.has_value() &&
                    d.grid_data_.value().rep_type==rep_t &&
                    intervals_intersect(d.from_,d.to_,time_interval.from_,time_interval.to_) &&
                    pos_in_grid(pos,d.grid_data_.value())){
                        auto beg_end = interval_intersection_pos(time_interval,TimeInterval{d.from_,d.to_},d.discret_);
                        ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end.first)|std::views::take(beg_end.second-beg_end.first));
                    }
                }
    }
    return result;
}
#include <format>
#include "definitions/def.h"
std::unordered_map<path::Storage<true>,SublimedDataInfo> Data::match(
    Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::unordered_map<path::Storage<true>,SublimedDataInfo> result;
    std::vector<CommonDataProperties> param_variations;
    if(!time_fcst.has_value()){
        if(parameters.empty())
            param_variations.resize(time_frame_number*256);
        else
            param_variations.resize(time_frame_number*parameters.size());
    }
    else{
        if(parameters.empty())
            param_variations.resize(256);
        else
            param_variations.resize(parameters.size());
    }
    if(!time_fcst.has_value()){
        int time_counter = 0;
        for(int time=0;time<256;++time)
            if(is_time[time]){
                if(parameters.empty()){
                    for(int param=0;param<256;++param){
                        for(int table_version=0;table_version<256;++table_version)
                            param_variations.at(time_counter*256+param)=CommonDataProperties(center,table_version,(TimeFrame)time,param);
                    }
                }
                else{
                    int param_counter = 0;
                    for(const auto& [param,table_version]:parameters)
                        param_variations.at(time_counter*parameters.size()+param_counter++)=CommonDataProperties(center,table_version,(TimeFrame)time,param);
                }
                ++time_counter;
            }
            else continue;
    }
    else{
            if(is_time[time_fcst.value()] && parameters.empty()){
                for(uint8_t param=0;param<256;++param)
                    for(int table_version=0;table_version<256;++table_version)
                        param_variations.at(param) = CommonDataProperties(center,table_version,time_fcst.value(),param);
            }
            else{
                uint8_t param_counter = 0;
                for(const auto& [param,table_version]:parameters)
                    param_variations.at(param_counter++)=CommonDataProperties(center,table_version,time_fcst.value(),param);
            }
    }
    for(const auto& common:param_variations){
        auto found = grib_.by_common_data_.find(common);
        if(found!=grib_.by_common_data_.end() && !found->first.expired()){
            for(auto fn:found->second)
                result[fn];
        }
    }
    for(const auto& [date_interval,seq_fn]:grib_.by_date_){
        if(!intervals_intersect(time_interval,date_interval))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(const auto& [grid,seq_fn]:grib_.by_grid_){
        if(!grid.has_value())
            for(auto fn:seq_fn){
                result.erase(fn);
            }
        if(!pos_in_grid(pos,grid.value()))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(auto& [fn,ptrs]:result){
        for(const auto& [common,data]:grib_.grib_data_.data().find(fn)->second)
            for(const auto& d:data){
                if(d.grid_data_.has_value() && 
                    d.grid_data_.value().rep_type==rep_t && 
                    intervals_intersect(d.from_,d.to_,time_interval.from_,time_interval.to_) && 
                    pos_in_grid(pos,d.grid_data_.value())){
                    auto beg_end = interval_intersection_pos(time_interval,TimeInterval{d.from_,d.to_},d.discret_);
                    ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end.first)|std::views::take(beg_end.second-beg_end.first));
                    ptrs.from_ = d.from_;
                    ptrs.to_ = d.to_;
                    ptrs.discret_ = d.discret_;
                    ptrs.grid_data_ = d.grid_data_;
                }
            }
    }
    return result;
}

std::unordered_set<SublimedDataInfo> Data::match(
    path::Storage<true> path,
    Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::vector<ptrdiff_t> result;
    std::vector<CommonDataProperties> param_variations;
    if(!time_fcst.has_value()){
        if(parameters.empty())
            param_variations.resize(time_frame_number*256);
        else
            param_variations.resize(time_frame_number*parameters.size());
    }
    else{
        if(parameters.empty())
            param_variations.resize(256);
        else
            param_variations.resize(parameters.size());
    }
    if(!time_fcst.has_value()){
        int time_counter = 0;
        for(int time=0;time<256;++time)
            if(is_time[time]){
                if(parameters.empty()){
                    for(int param=0;param<256;++param){
                        for(int table_version=0;table_version<256;++table_version)
                            param_variations.at(time_counter*256+param)=CommonDataProperties(center,table_version,(TimeFrame)time,param);
                    }
                }
                else{
                    int param_counter = 0;
                    for(const auto& [param,table_version]:parameters)
                        param_variations.at(time_counter*parameters.size()+param_counter++)=CommonDataProperties(center,table_version,(TimeFrame)time,param);
                }
                ++time_counter;
            }
            else continue;
    }
    else{
            if(is_time[time_fcst.value()] && parameters.empty()){
                for(uint8_t param=0;param<256;++param)
                    for(int table_version=0;table_version<256;++table_version)
                        param_variations.at(param) = CommonDataProperties(center,table_version,time_fcst.value(),param);
            }
            else{
                uint8_t param_counter = 0;
                for(const auto& [param,table_version]:parameters)
                    param_variations.at(param_counter++)=CommonDataProperties(center,table_version,time_fcst.value(),param);
            }
    }
    for(const auto& common:param_variations){
        auto found = grib_.by_common_data_.find(common);
        if(found!=grib_.by_common_data_.end() && !found->first.expired()){
            for(auto fn:found->second)
                result[fn];
        }
    }
    for(const auto& [date_interval,seq_fn]:grib_.by_date_){
        if(!intervals_intersect(time_interval,date_interval))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(const auto& [grid,seq_fn]:grib_.by_grid_){
        if(!grid.has_value())
            for(auto fn:seq_fn){
                result.erase(fn);
            }
        if(!pos_in_grid(pos,grid.value()))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(auto& [fn,ptrs]:result){
        for(const auto& [common,data]:grib_.grib_data_.data().find(fn)->second)
            for(const auto& d:data){
                if(d.grid_data_.has_value() && 
                    d.grid_data_.value().rep_type==rep_t && 
                    intervals_intersect(d.from_,d.to_,time_interval.from_,time_interval.to_) && 
                    pos_in_grid(pos,d.grid_data_.value())){
                    auto beg_end = interval_intersection_pos(time_interval,TimeInterval{d.from_,d.to_},d.discret_);
                    ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end.first)|std::views::take(beg_end.second-beg_end.first));
                    ptrs.from_ = d.from_;
                    ptrs.to_ = d.to_;
                    ptrs.discret_ = d.discret_;
                    ptrs.grid_data_ = d.grid_data_;
                }
            }
    }
    return result;
}

void Data::add_data(SublimedGribDataInfo& grib_data){
    grib_.grib_data_.add_data(grib_data);
    for(auto& [filename,file_data]:grib_data.data()){
        const auto& tmp_view = grib_.grib_data_.data().find(filename)->first;
        for(auto& [common,grib_data]:grib_.grib_data_.data().find(filename)->second){
            grib_.by_common_data_[common].insert(tmp_view);
            for(const auto& sub_data:grib_data){
                grib_.by_date_[TimeInterval{sub_data.from_,sub_data.to_}].insert(tmp_view);
                grib_.by_grid_[sub_data.grid_data_].insert(tmp_view);
            }
        }
    }
    unsaved_.insert(FORMAT::GRIB);
    std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
}

void Data::add_data(SublimedGribDataInfo&& grib_data){
    add_data(grib_data);
}

void Data::add_data(GribDataInfo& grib_data){
    add_data(grib_data.sublime());
}

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