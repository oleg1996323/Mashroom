#pragma once

#include "data/common_data_properties.h"
#include "extracted_value.h"
#include <memory>

namespace procedures::extract::details{

template<Data_t TYPE,Data_f FORMAT>
struct AdditionalExtractDataProperties:std::monostate{
    size_t hash() const{return 0;}
};

template<>
struct AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    TimeForecast fcst_;
    Level level_;
};

template<Data_t TYPE,Data_f FORMAT>
struct ExtractDataProperties{
    AdditionalExtractDataProperties<TYPE,FORMAT> add_;
    CommonDataProperties<TYPE,FORMAT> cmn_;

    ExtractDataProperties(const CommonDataProperties<TYPE,FORMAT>& cmn):cmn_(cmn){
        static_assert(std::is_base_of_v<std::monostate,AdditionalExtractDataProperties<TYPE,FORMAT>>);
    }
    ExtractDataProperties(const CommonDataProperties<TYPE,FORMAT>& cmn, const AdditionalExtractDataProperties<TYPE,FORMAT>& add):cmn_(cmn),add_(add){
        static_assert(!std::is_base_of_v<std::monostate,AdditionalExtractDataProperties<TYPE,FORMAT>>);
    }
    ExtractDataProperties(const ExtractDataProperties& other):cmn_(other.cmn_),add_(other.add_){}

    ExtractDataProperties() = default;
    bool operator==(const ExtractDataProperties& other) const{
        if constexpr(std::is_base_of_v<std::monostate,AdditionalExtractDataProperties<TYPE,FORMAT>>)
            return  cmn_ == other.cmn_;
        else return cmn_ == other.cmn_ && add_ == other.add_;
    }

    bool operator!=(const ExtractDataProperties& other) const{
        return !(*this==other);
    }

    static size_t hash(const ExtractDataProperties<TYPE,FORMAT>& data)
    {
        return data.cmn_.hash()|data.add_.hash();
    }

    size_t hash() const
    {
        return  hash(cmn_,add_);
    }

    bool operator<(const std::shared_ptr<ExtractDataProperties>& other) const{
        if(other)
            return this->hash()<other->hash();
        else return true;
    }
    bool operator>(const std::shared_ptr<ExtractDataProperties>& other) const{
        if(other)
            return this->hash()>other->hash();
        else return false;
    }

    bool operator<(const ExtractDataProperties& other) const{
        return this->hash()<other.hash();
    }
    bool operator>(const ExtractDataProperties& other) const{
        return this->hash()>other.hash();
    }
};

}

using VariantExtractDataProperties = std::variant<procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;

template<Data_t TYPE,Data_f FORMAT>
struct std::less<std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>>{
    using is_transparent = std::true_type;
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs,const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs<(*rhs);
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs,const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
        return rhs>(*lhs);
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs,const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs)
            return (*lhs)<(*rhs);
        else
            return true;
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::hash<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
    size_t operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& cs) const{
        return cs.hash();
    }
};
#include <memory>
template<Data_t TYPE,Data_f FORMAT>
struct std::hash<std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& node_ptr) const{
        return std::hash<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{}(*node_ptr);
    }
    size_t operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& node_ptr) const{
        return std::hash<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{}(node_ptr);
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::hash<std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    size_t operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& node_ptr) const{
        return std::hash<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{}(*node_ptr.lock());
    }
    size_t operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& node_ptr) const{
        return std::hash<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{}(node_ptr);
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        return *lhs==*rhs;
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs==*rhs;
}
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>>
{
    using is_transparent = std::true_type;
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs;
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return *lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired() || lhs.expired())
            return false;
        else
            return *lhs.lock()==*rhs.lock();
    }
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
};

template<Data_t TYPE,Data_f FORMAT>
struct std::equal_to<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>
{
    using is_transparent = std::true_type;
    
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        if(rhs.expired())
            return false;
        else
            return lhs==*rhs.lock();
    }
    bool operator()(const std::weak_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
        if(lhs.expired())
            return false;
        else
            return *lhs.lock()==rhs;
    }
    bool operator()(const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
            return *lhs==rhs;
    }
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const std::shared_ptr<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>& rhs) const{
        return lhs==*rhs;
    }
    bool operator()(const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& lhs, const procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>& rhs) const{
        return lhs==rhs;
    }
};

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.fcst_,msg.level_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.fcst_,msg.level_);
        }
    };

    template<>
    struct Serial_size<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.fcst_,msg.level_);
        }
    };

    template<>
    struct Min_serial_size<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::fcst_),decltype(type::level_)>();
        }();
    };

    template<>
    struct Max_serial_size<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::fcst_),decltype(type::level_)>();
        }();
    };

    template<bool NETWORK_ORDER,Data_t TYPE,Data_f FORMAT>
    struct Serialize<NETWORK_ORDER,procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
        using type = procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<bool NETWORK_ORDER,Data_t TYPE,Data_f FORMAT>
    struct Deserialize<NETWORK_ORDER,procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
        using type = procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<Data_t TYPE,Data_f FORMAT>
    struct Serial_size<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
        using type = procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.add_);
        }
    };

    template<Data_t TYPE,Data_f FORMAT>
    struct Min_serial_size<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
        using type = procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };

    template<Data_t TYPE,Data_f FORMAT>
    struct Max_serial_size<procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>>{
        using type = procedures::extract::details::ExtractDataProperties<TYPE,FORMAT>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };
}

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props);

template<>
std::expected<procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<
        procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& value);

template<Data_t T,Data_f F>
using ExtractedValues = std::map<procedures::extract::details::ExtractDataProperties<T,F>, std::vector<ExtractedValue<T,F>>>;

using VariantExtractedValue = std::variant<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
using VariantExtractedData = std::variant<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
using ExtractedData = VariantExtractedData;

static_assert(std::is_default_constructible_v<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>);
static_assert(std::is_default_constructible_v<VariantExtractedData>);

template<>
boost::json::value to_json(const ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>& vals);

template<>
std::expected<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& vals);

template<>
boost::json::value to_json(const ExtractedData& vals);

template<>
boost::json::value to_json(const procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& add);

template<>
std::expected<ExtractedData,std::exception> from_json<ExtractedData>(const boost::json::value& vals);

template<>
std::expected<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> 
    from_json<procedures::extract::details::AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& vals);