#pragma once
#include "datastruct/grib1.h"
#include <variant>

struct DataStructVariation:
std::variant<std::monostate,
    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>
{   
    using variant::variant;
    using variant::operator=;
    template<Data_t T,Data_f F>
    void add_data(const DataStruct<T,F>& other,std::error_code& err){
        auto add = [&err,&other](const auto& val){
            using type = std::decay_t<decltype(val)>;
            if constexpr(std::is_same_v<type,std::monostate>)
                err=std::make_error_code(std::errc::invalid_argument);
            else{
                auto loc_add = [&err,&other]<Data_t TYPE,Data_f FORMAT>(DataStruct<TYPE,FORMAT>& this_data){
                    if constexpr(TYPE==T && F==FORMAT){
                        this_data.add_data(other);
                        err.clear();
                    }
                    else err=std::make_error_code(std::errc::invalid_argument);
                };
                loc_add(val);
            }
        };
        std::visit(add,*this);
    }
    template<Data_t T,Data_f F>
    void add_data(const path::Storage<false>& file,const std::vector<FileMsg<T,F>>& other,std::error_code& err){
        add_data(file,other,err);
    }
};

template<>
struct std::hash<DataStructVariation>{
    using is_transparent = std::true_type;

    size_t operator()(const DataStructVariation& val) const{
        auto hash_compute = [](const auto& val){
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T,std::monostate>)
                return static_cast<size_t>(0);
            else{
                auto deep_inside = []<Data_t TYPE,Data_f FORMAT>(const DataStruct<TYPE,FORMAT>& v){
                    return static_cast<size_t>(FORMAT)<<4+static_cast<size_t>(TYPE);
                };
                return deep_inside(val);
            }
        };
        return std::visit(hash_compute,val);
    }

    size_t operator()(std::pair<Data_t, Data_f> tags) const{
        return static_cast<size_t>(tags.second)<<4+static_cast<size_t>(tags.first);
    }
    size_t operator()(std::pair<Data_f, Data_t> tags) const{
        return static_cast<size_t>(tags.first)<<4+static_cast<size_t>(tags.second);
    }
    size_t operator()(Data_t type, Data_f format) const{
        return static_cast<size_t>(format)<<4+static_cast<size_t>(type);
    }
    size_t operator()(Data_f format,Data_t type) const{
        return static_cast<size_t>(format)<<4+static_cast<size_t>(type);
    }
};

template<>
struct std::equal_to<DataStructVariation>{
    using is_transparent = std::true_type;
    bool operator()(const DataStructVariation& lhs,const DataStructVariation& rhs) const{
        return lhs.index() == rhs.index();
    }
    bool operator()(Data_t type, Data_f format, const DataStructVariation& val) const{
        auto is_equal = [type,format](const auto& val){
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T,std::monostate>)
                return false;
            else{
                auto deep_inside = [type,format]<Data_t TYPE,Data_f FORMAT>(const DataStruct<TYPE,FORMAT>& v){
                    return FORMAT==format && TYPE==type;
                };
                return deep_inside(val);
            }
        };
        return std::visit(is_equal,val);
    }
    bool operator()(const DataStructVariation& val,Data_t type, Data_f format) const{
        return this->operator()(type,format,val);
    }
    bool operator()(const std::pair<Data_t,Data_f>& tags, const DataStructVariation& val) const{
        return operator()(tags.first,tags.second,val);
    }
    bool operator()(const std::pair<Data_f,Data_t>& tags, const DataStructVariation& val) const{
        return operator()(tags.second,tags.first,val);
    }
    bool operator()(const DataStructVariation& val,const std::pair<Data_t,Data_f>& tags) const{
        return operator()(tags.first,tags.second,val);
    }
    bool operator()(const DataStructVariation& val,const std::pair<Data_f,Data_t>& tags) const{
        return operator()(tags.second,tags.first,val);
    }
};