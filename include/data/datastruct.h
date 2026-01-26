#pragma once
#include "datastruct/grib1.h"
#include <variant>

using DataStructVariation = 
std::variant<std::monostate,
    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;

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

// template<>
// struct std::equal_to<std::unique_ptr<AbstractDataStruct>>{
//     using is_transparent = std::true_type;

//     bool operator()(const AbstractDataStruct& lhs,const AbstractDataStruct& rhs) const{
//         return lhs.format_type()==rhs.format_type() && lhs.data_type()==rhs.data_type();
//     }
//     bool operator()(const std::unique_ptr<AbstractDataStruct>& lhs,const std::unique_ptr<AbstractDataStruct>& rhs) const{
//         return (lhs && rhs)?(lhs->format_type()==rhs->format_type() && lhs->data_type()==rhs->data_type()):(!lhs && !rhs);
//     }
//     bool operator()(Data_t type, Data_f format, const AbstractDataStruct& val) const{
//         return val.format_type()==format && val.data_type()==type;
//     }
//     bool operator()(const AbstractDataStruct& val,Data_t type, Data_f format) const{
//         return this->operator()(type,format,val);
//     }
//     bool operator()(const std::pair<Data_t,Data_f>& tags, const std::unique_ptr<AbstractDataStruct>& val) const{
//         return val && val->format_type()==tags.second && val->data_type()==tags.first;
//     }
//     bool operator()(const std::pair<Data_f,Data_t>& tags, const std::unique_ptr<AbstractDataStruct>& val) const{
//         return val && val->format_type()==tags.first && val->data_type()==tags.second;
//     }
//     bool operator()(const std::unique_ptr<AbstractDataStruct>& val,const std::pair<Data_t,Data_f>& tags) const{
//         return this->operator()(tags,val);
//     }
//     bool operator()(const std::unique_ptr<AbstractDataStruct>& val,const std::pair<Data_f,Data_t>& tags) const{
//         return this->operator()(tags,val);
//     }
// };