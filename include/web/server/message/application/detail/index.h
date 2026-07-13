#pragma once
#include "web/common/msgdef.h"
#include <variant>
#include "data/datastruct.h"
#include "data/def.h"
#include "boost_functional/json.h"

namespace network{

template<Data_t T, Data_f F>
struct BaseIndexResult{
    using data_t = DataStruct<T,F>::find_all_t;
    data_t data_;
    BaseIndexResult() = default;
    BaseIndexResult(const BaseIndexResult::data_t& other_data){
        data_ = other_data;
    }
    BaseIndexResult(BaseIndexResult::data_t&& other_data){
        data_ = std::move(other_data);
    }
    BaseIndexResult(const BaseIndexResult& other):data_(other.data_){}
    BaseIndexResult(BaseIndexResult&& other):data_(std::move(other.data_)){}
    BaseIndexResult& operator=(const BaseIndexResult& other){
        if(this!=&other){
            data_ = other.data_;
        }
        return *this;
    }
    BaseIndexResult& operator=(BaseIndexResult&& other) noexcept{
        if(this!=&other){
            data_ = std::move(other.data_);
        }
        return *this;
    }
};

using IndexResult = std::variant<std::monostate,
        DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t>;
}

template<>
std::expected<network::IndexResult,std::exception> from_json<network::IndexResult>(const boost::json::value& val){

}

template<>
boost::json::value to_json(const network::IndexResult& val){
    
}

namespace serialization{
    template<bool NETWORK_ORDER,Data_t T, Data_f F>
    struct Serialize<NETWORK_ORDER,network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.data_);
        }
    };

    template<bool NETWORK_ORDER,Data_t T, Data_f F>
    struct Deserialize<NETWORK_ORDER,network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.data_);
        }
    };

    template<Data_t T, Data_f F>
    struct Serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.data_);
        }
    };

    template<Data_t T, Data_f F>
    struct Min_serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::data_)>();
        }();
    };

    template<Data_t T, Data_f F>
    struct Max_serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::data_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::deserialize_concept<false,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::serialize_concept<true,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::serialize_concept<false,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);