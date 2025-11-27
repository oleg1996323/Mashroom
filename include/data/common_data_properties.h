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
template<Data_t TYPE,Data_f FORMAT>
struct CommonDataProperties;

using Grib1CommonDataProperties = CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

template<>
struct CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    std::optional<TimeFrame> fcst_unit_;
    std::optional<Organization> center_;
    std::optional<uint8_t> table_version_;
    std::optional<uint8_t> parameter_;

    CommonDataProperties(std::optional<Organization> center,std::optional<uint8_t> table_version,std::optional<TimeFrame> fcst_unit,std::optional<uint8_t> parameter):
    center_(center),table_version_(table_version),fcst_unit_(fcst_unit),parameter_(parameter){}
    CommonDataProperties() = default;
    bool operator==(const CommonDataProperties& other) const{
        return center_==other.center_ && table_version_==other.table_version_ && fcst_unit_==other.fcst_unit_ && parameter_==other.parameter_;
    }

static size_t hash(  const std::optional<Organization>& center,
                                        const std::optional<TimeFrame>& fcst_unit,
                                        const std::optional<uint8_t>& table_version,
                                        const std::optional<uint8_t>& parameter)
    {
        size_t hash = 0;
        if(center.has_value())
            hash|=static_cast<uint64_t>(center.value())<<24;
        else hash|=static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<Organization>>::max())<<24;
        if(fcst_unit.has_value())
            hash|=static_cast<uint64_t>(fcst_unit.value())<<16;
        else hash|=(static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<TimeFrame>>::max())<<16);
        if(table_version.has_value())
            hash|=(static_cast<uint64_t>(table_version.value())<<8);
        else hash|=(static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<8);
        if(parameter.has_value())
            hash|=parameter.value();
        else hash|=std::numeric_limits<uint8_t>::max();
        return hash;
    }

    //@brief Designated for search by hash the upper-bound in sorted range
    static size_t max_definitive_hash(  const std::optional<Organization>& center,
                                        const std::optional<TimeFrame>& fcst_unit,
                                        const std::optional<uint8_t>& table_version,
                                        const std::optional<uint8_t>& parameter)
    {
        uint64_t hash = 0;
        if(center.has_value())
            hash|=static_cast<uint64_t>(center.value())<<24;
        else hash|=static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<Organization>>::max())<<24;
        if(fcst_unit.has_value())
            hash|=static_cast<uint64_t>(fcst_unit.value())<<16;
        else hash|=static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<TimeFrame>>::max())<<16;
        if(fcst_unit.has_value())
            hash|=static_cast<uint64_t>(table_version.value())<<8;
        else hash|=static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<8;
        if(fcst_unit.has_value())
            hash|=static_cast<uint64_t>(parameter.value());
        else hash|=static_cast<uint64_t>(std::numeric_limits<uint8_t>::max());
        return hash;
    }

    size_t max_definitive_hash() const
    {
        return  max_definitive_hash(center_,fcst_unit_,table_version_,parameter_);
    }

    size_t hash() const
    {
        return  hash(center_,fcst_unit_,table_version_,parameter_);
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

template<>
struct std::less<std::shared_ptr<Grib1CommonDataProperties>>{
    using is_transparent = std::true_type;
    bool operator()(const Grib1CommonDataProperties& lhs,const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        return lhs<(*rhs);
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs,const Grib1CommonDataProperties& rhs) const{
        return rhs>(*lhs);
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs,const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        if(lhs)
            return (*lhs)<(*rhs);
        else
            return true;
    }
};

template<>
struct std::hash<Grib1CommonDataProperties>{
    size_t operator()(const Grib1CommonDataProperties& cs) const{
        return cs.hash();
    }
};
#include <memory>
template<>
struct std::hash<std::shared_ptr<Grib1CommonDataProperties>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::shared_ptr<Grib1CommonDataProperties>& node_ptr) const{
        return std::hash<Grib1CommonDataProperties>{}(*node_ptr);
    }
    size_t operator()(const Grib1CommonDataProperties& node_ptr) const{
        return std::hash<Grib1CommonDataProperties>{}(node_ptr);
    }
};

template<>
struct std::hash<std::weak_ptr<Grib1CommonDataProperties>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::weak_ptr<Grib1CommonDataProperties>& node_ptr) const{
        return std::hash<Grib1CommonDataProperties>{}(*node_ptr.lock());
    }
    size_t operator()(const Grib1CommonDataProperties& node_ptr) const{
        return std::hash<Grib1CommonDataProperties>{}(node_ptr);
    }
};

template<>
struct std::equal_to<std::shared_ptr<Grib1CommonDataProperties>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs, const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        return *lhs==*rhs;
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const Grib1CommonDataProperties& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const Grib1CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs, const Grib1CommonDataProperties& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const Grib1CommonDataProperties& lhs, const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        return lhs==*rhs;
}
};

template<>
struct std::equal_to<std::weak_ptr<Grib1CommonDataProperties>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const Grib1CommonDataProperties& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const Grib1CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
};

template<>
struct std::equal_to<Grib1CommonDataProperties>
{
    using is_transparent = std::true_type;
    
    bool operator()(const Grib1CommonDataProperties& lhs, const std::weak_ptr<Grib1CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<Grib1CommonDataProperties>& lhs, const Grib1CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<Grib1CommonDataProperties>& lhs, const Grib1CommonDataProperties& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const Grib1CommonDataProperties& lhs, const std::shared_ptr<Grib1CommonDataProperties>& rhs) const{
        return lhs==*rhs;
    }
    bool operator()(const Grib1CommonDataProperties& lhs, const Grib1CommonDataProperties& rhs) const{
        return lhs==rhs;
    }
};

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Min_serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::fcst_unit_),decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };

    template<>
    struct Max_serial_size<Grib1CommonDataProperties>{
        using type = Grib1CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::fcst_unit_),decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };
}