#pragma once
#include "byte_order.h"
#include <unordered_map>
#include <optional>
#include <limits>
#include "code_tables/table_4.h"
#include "code_tables/table_0.h"
#include <variant>
#include <memory>
#include <bitset>
#include "data/def.h"
#include "code_tables/table_2.h"
#include "sections/product/levels.h"
#include "code_tables/table_5.h"
#include "sections/product/time_forecast.h"
template<Data_t TYPE,Data_f FORMAT>
struct CommonDataProperties;

using Grib1CommonDataProperties = CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

template<>
struct CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    enum SEARCH_MODE{
        BASIC,
        FORECAST,
        LEVEL
    };
    std::optional<Organization> center_;
    std::optional<uint8_t> table_version_;
    std::optional<uint8_t> parameter_;

    CommonDataProperties(   std::optional<Organization> center,
                            std::optional<uint8_t> table_version,
                            std::optional<uint8_t> parameter):
    center_(center),table_version_(table_version),parameter_(parameter){}
    CommonDataProperties() = default;
    bool operator==(const CommonDataProperties& other) const{
        return  center_==other.center_ && 
                table_version_==other.table_version_ &&
                parameter_==other.parameter_;
    }

static size_t hash(  const std::optional<Organization>& center,
                                        const std::optional<uint8_t>& table_version,
                                        const std::optional<uint8_t>& parameter)
    {
        size_t hash = 0;
        if(center.has_value())
            hash|=static_cast<uint64_t>(center.value())<<56;
        else hash|=static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<Organization>>::max())<<56;
        if(table_version.has_value())
            hash|=(static_cast<uint64_t>(table_version.value())<<48);
        else hash|=(static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<48);
        if(parameter.has_value())
            hash|=(static_cast<uint64_t>(parameter.value())<<40);
        else hash|=(static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<40);
        return hash;
    }

    size_t hash() const
    {
        return  hash(center_,table_version_,parameter_);
    }

    bool operator<(const std::shared_ptr<CommonDataProperties>& other) const{
        if(other)
            return this->hash()<other->hash();
        else return true;
    }
    bool operator>(const std::shared_ptr<CommonDataProperties>& other) const{
        if(other)
            return this->hash()>other->hash();
        else return false;
    }

    bool operator<(const CommonDataProperties& other) const{
        return this->hash()<other.hash();
    }
    bool operator>(const CommonDataProperties& other) const{
        return this->hash()>other.hash();
    }
};

using VariantCommonDataProperties = std::variant<Grib1CommonDataProperties>;

template<Data_t TYPE,Data_f FORMAT>
struct std::less<std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>>{
    using is_transparent = std::true_type;
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs,const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs<(*rhs);
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs,const CommonDataProperties<TYPE,FORMAT>& rhs) const{
        return rhs>(*lhs);
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs,const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs)
            return (*lhs)<(*rhs);
        else
            return true;
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::hash<CommonDataProperties<TYPE,FORMAT>>{
    size_t operator()(const CommonDataProperties<TYPE,FORMAT>& cs) const{
        return cs.hash();
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::hash<std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& node_ptr) const{
        return std::hash<CommonDataProperties<TYPE,FORMAT>>{}(*node_ptr);
    }
    size_t operator()(const CommonDataProperties<TYPE,FORMAT>& node_ptr) const{
        return std::hash<CommonDataProperties<TYPE,FORMAT>>{}(node_ptr);
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::hash<std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& node_ptr) const{
        return std::hash<CommonDataProperties<TYPE,FORMAT>>{}(*node_ptr.lock());
    }
    size_t operator()(const CommonDataProperties<TYPE,FORMAT>& node_ptr) const{
        return std::hash<CommonDataProperties<TYPE,FORMAT>>{}(node_ptr);
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        return *lhs==*rhs;
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs==*rhs;
}
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<CommonDataProperties<TYPE,FORMAT>>
{
    using is_transparent = std::true_type;
    
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs==*rhs;
    }
    bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
        return lhs==rhs;
    }
};

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Min_serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };

    template<>
    struct Max_serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };
}

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props);

template<>
std::expected<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& value);

// template<Data_f FORMAT,Data_t TYPE,Grib1CommonDataProperties::SEARCH_MODE MODE = Grib1CommonDataProperties::SEARCH_MODE::BASIC>
// struct HashOption{
//     size_t operator()(const CommonDataProperties<TYPE,FORMAT>& cs) const{
//         if constexpr(FORMAT==Data_f::GRIB_v1 && TYPE==Data_t::TIME_SERIES){
//             if constexpr(MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::SEARCH_MODE::FORECAST){
//                 if(cs.fcst_unit_.has_value())
//                     return std::hash<TimeForecast>()(cs.fcst_unit_.value());
//                 else return std::numeric_limits<size_t>::max();
//             }
//             else if constexpr (MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::SEARCH_MODE::LEVEL){
//                 if(cs.level_.has_value())
//                     return std::hash<Level>()(cs.level_.value());
//                 else return std::numeric_limits<size_t>::max();
//             }
//             else
//                 return cs.hash();
//         }
//         else static_assert(false,"Not implemented HashOption");
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs.expired())
//             return std::numeric_limits<size_t>::max();
//         else{
//             return operator()(*rhs.lock());
//         }
//     }
//     bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(!rhs)
//             return std::numeric_limits<size_t>::max();
//         else return operator()(*rhs);
//     }
// };

// #include "concepts.h"

// template<Data_f FORMAT,Data_t TYPE,Grib1CommonDataProperties::SEARCH_MODE MODE = Grib1CommonDataProperties::SEARCH_MODE::BASIC>
// struct EqualOption{
//     using is_transparent = std::true_type;

//     size_t operator()(const CommonDataProperties<TYPE,FORMAT>& lhs,const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         if constexpr(FORMAT==Data_f::GRIB_v1 && TYPE==Data_t::TIME_SERIES){
//             if constexpr(MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::FORECAST){
//                 return lhs.fcst_unit_==rhs.fcst_unit_;
//             }
//             else if constexpr (MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::LEVEL){
//                 return lhs.level_==rhs.level_;
//             }
//             else
//                 return lhs == rhs;
//         }
//         else static_assert(false,"Not implemented EqualOption");
//     }

//     bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs.expired())
//             return false;
//         else{
//             return operator()(lhs,*rhs.lock());
//         }
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs)
//             return operator()(lhs,*rhs);
//         else return false;
//     }
//     bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(lhs.expired() || rhs)
//             return false;
//         else
//             return operator()(*lhs.lock(),*rhs);
//     }
//     bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs.expired() || lhs.expired())
//             return false;
//         else
//             return operator()(*lhs.lock(),*rhs.lock());
//     }
// };

// template<Data_f FORMAT,Data_t TYPE,Grib1CommonDataProperties::SEARCH_MODE MODE = Grib1CommonDataProperties::SEARCH_MODE::BASIC>
// struct LessOption{
//     using is_transparent = std::true_type;

//     size_t operator()(const CommonDataProperties<TYPE,FORMAT>& lhs,const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         if constexpr(FORMAT==Data_f::GRIB_v1 && TYPE==Data_t::TIME_SERIES){
//             if constexpr(MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::FORECAST){
//                 return std::hash<TimeForecast>()(lhs.fcst_unit_)<std::hash<TimeForecast>()(rhs.fcst_unit_);
//             }
//             else if constexpr (MODE==CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::LEVEL){
//                 return std::hash<Level>()(lhs.level_)<std::hash<Level>()(rhs.level_);
//             }
//             else
//                 return std::hash<CommonDataProperties<TYPE,FORMAT>>()(lhs) < std::hash<CommonDataProperties<TYPE,FORMAT>>()(rhs);
//         }
//         else static_assert(false,"Not implemented EqualOption");
//     }

//     bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs.expired())
//             return false;
//         else{
//             return operator()(lhs,*rhs.lock());
//         }
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const CommonDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs)
//             return operator()(lhs,*rhs);
//         else return false;
//     }
//     bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const CommonDataProperties<TYPE,FORMAT>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(lhs.expired() || rhs)
//             return false;
//         else
//             return operator()(*lhs.lock(),*rhs);
//     }
//     bool operator()(const std::shared_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         return operator()(rhs,lhs);
//     }
//     bool operator()(const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<CommonDataProperties<TYPE,FORMAT>>& rhs) const{
//         if(rhs.expired() || lhs.expired())
//             return false;
//         else
//             return operator()(*lhs.lock(),*rhs.lock());
//     }
// };