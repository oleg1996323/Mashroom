#pragma once
#include "byte_order.h"
#include <unordered_map>
#include <optional>
#include "code_tables/table_4.h"
#include "code_tables/table_0.h"

struct CommonDataProperties
{
    std::optional<TimeFrame> fcst_unit_ ={};
    std::optional<Organization> center_ = Organization::WMO;
    std::optional<uint8_t> table_version_ = 0;
    std::optional<uint8_t> parameter_ = 0;

    CommonDataProperties(std::optional<Organization> center,std::optional<uint8_t> table_version,std::optional<TimeFrame> fcst_unit,std::optional<uint8_t> parameter):
    center_(center),table_version_(table_version),fcst_unit_(fcst_unit),parameter_(parameter){}
    CommonDataProperties() = default;
    bool operator==(const CommonDataProperties& other) const{
        return center_==other.center_ && table_version_==other.table_version_ && fcst_unit_==other.fcst_unit_ && parameter_==other.parameter_;
    }

    static size_t hash(  const std::optional<Organization>& center,
                                        const std::optional<TimeFrame>& fcst_unit = {},
                                        const std::optional<uint8_t>& table_version = {},
                                        const std::optional<uint8_t>& parameter = {})
    {
        return  center.has_value()?std::underlying_type_t<Organization>(center.value())<<24:std::numeric_limits<std::underlying_type_t<Organization>>::min()<<24+
                fcst_unit.has_value()?std::underlying_type_t<TimeFrame>(fcst_unit.value())<<16:std::numeric_limits<std::underlying_type_t<TimeFrame>>::min()<<16+
                table_version.has_value()?table_version.value()<<8:std::numeric_limits<uint8_t>::min()<<8+
                parameter.has_value()?parameter.value():std::numeric_limits<uint8_t>::min();
    }

    static size_t max_definitive_hash(  const std::optional<Organization>& center,
                                        const std::optional<TimeFrame>& fcst_unit = {},
                                        const std::optional<uint8_t>& table_version = {},
                                        const std::optional<uint8_t>& parameter = {})
    {
        return  center.has_value()?std::underlying_type_t<Organization>(center.value())<<24:std::numeric_limits<std::underlying_type_t<Organization>>::max()<<24+
                fcst_unit.has_value()?std::underlying_type_t<TimeFrame>(fcst_unit.value())<<16:std::numeric_limits<std::underlying_type_t<TimeFrame>>::max()<<16+
                table_version.has_value()?table_version.value()<<8:std::numeric_limits<uint8_t>::max()<<8+
                parameter.has_value()?parameter.value():std::numeric_limits<uint8_t>::max();
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
        else return false;
    }
    bool operator>(const std::shared_ptr<CommonDataProperties>& other) const{
        if(other)
            return this->hash()>other->hash();
        else return true;
    }

    bool operator<(const CommonDataProperties& other) const{
        return this->hash()<other.hash();
    }
    bool operator>(const CommonDataProperties& other) const{
        return this->hash()>other.hash();
    }
};

template<>
struct std::less<std::shared_ptr<CommonDataProperties>>{
    using is_transparent = std::true_type;
    bool operator()(const CommonDataProperties& lhs,const std::shared_ptr<CommonDataProperties>& rhs) const{
        return lhs<rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs,const CommonDataProperties& rhs) const{
        return rhs>lhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs,const std::shared_ptr<CommonDataProperties>& rhs) const{
        return lhs?(rhs?lhs->hash()<rhs->hash():false):true;
    }
};

template<>
struct std::hash<CommonDataProperties>{
    size_t operator()(const CommonDataProperties& cs) const{
        size_t result = std::hash<size_t>{}(static_cast<size_t>(cs.center_.has_value()?cs.center_.value():Organization::Undefined)<<24);
        result+=std::hash<size_t>{}(static_cast<size_t>(cs.table_version_.has_value()?cs.table_version_.value():0)<<16);
        result+=std::hash<size_t>{}(static_cast<size_t>(cs.fcst_unit_.has_value()?cs.fcst_unit_.value():0)<<8);
        result+=std::hash<size_t>{}(cs.parameter_.has_value()?cs.parameter_.value():0);
        return result;
        return std::hash<size_t>{}((static_cast<size_t>(cs.center_.has_value()?cs.center_.value():Organization::Undefined)<<24)+
        (static_cast<size_t>(cs.table_version_.has_value()?cs.table_version_.value():0)<<16)+
        (static_cast<size_t>(cs.fcst_unit_.has_value()?cs.fcst_unit_.value():0)<<8)+
        cs.parameter_.has_value()?cs.parameter_.value():0);
    }
};
#include <memory>
template<>
struct std::hash<std::shared_ptr<CommonDataProperties>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::shared_ptr<CommonDataProperties>& node_ptr) const{
        return std::hash<CommonDataProperties>{}(*node_ptr);
    }
    size_t operator()(const CommonDataProperties& node_ptr) const{
        return std::hash<CommonDataProperties>{}(node_ptr);
    }
};

template<>
struct std::hash<std::weak_ptr<CommonDataProperties>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::weak_ptr<CommonDataProperties>& node_ptr) const{
        return std::hash<CommonDataProperties>{}(*node_ptr.lock());
    }
    size_t operator()(const CommonDataProperties& node_ptr) const{
        return std::hash<CommonDataProperties>{}(node_ptr);
    }
};

template<>
struct std::equal_to<std::shared_ptr<CommonDataProperties>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs, const std::shared_ptr<CommonDataProperties>& rhs) const{
        return *lhs==*rhs;
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const std::shared_ptr<CommonDataProperties>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const CommonDataProperties& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs, const CommonDataProperties& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const CommonDataProperties& lhs, const std::shared_ptr<CommonDataProperties>& rhs) const{
        return lhs==*rhs;
}
};

template<>
struct std::equal_to<std::weak_ptr<CommonDataProperties>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const std::shared_ptr<CommonDataProperties>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const CommonDataProperties& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
};

template<>
struct std::equal_to<CommonDataProperties>
{
    using is_transparent = std::true_type;
    
    bool operator()(const CommonDataProperties& lhs, const std::weak_ptr<CommonDataProperties>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<CommonDataProperties>& lhs, const CommonDataProperties& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<CommonDataProperties>& lhs, const CommonDataProperties& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const CommonDataProperties& lhs, const std::shared_ptr<CommonDataProperties>& rhs) const{
        return lhs==*rhs;
    }
    bool operator()(const CommonDataProperties& lhs, const CommonDataProperties& rhs) const{
        return lhs==rhs;
    }
};

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,CommonDataProperties>{
        using type = CommonDataProperties;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,CommonDataProperties>{
        using type = CommonDataProperties;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Serial_size<CommonDataProperties>{
        using type = CommonDataProperties;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.fcst_unit_,msg.center_,msg.table_version_,msg.parameter_);
        }
    };

    template<>
    struct Min_serial_size<CommonDataProperties>{
        using type = CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::fcst_unit_),decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };

    template<>
    struct Max_serial_size<CommonDataProperties>{
        using type = CommonDataProperties;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::fcst_unit_),decltype(type::center_),decltype(type::table_version_),decltype(type::parameter_)>();
        }();
    };
}