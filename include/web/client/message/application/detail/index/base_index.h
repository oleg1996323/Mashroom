#pragma once
#include "serialization.h"
#include "gribv1.h"

namespace network{
    struct BaseIndexRequest{
        std::vector<IndexParameters_t> parameters_;
        utc_tp last_update_;
        using data_t = decltype(parameters_);
        BaseIndexRequest() = default;
        BaseIndexRequest(const utc_tp& last_update){
            last_update_=last_update;
        }
        BaseIndexRequest(const BaseIndexRequest& other):
        parameters_(other.parameters_),
        last_update_(other.last_update_){}
        BaseIndexRequest(BaseIndexRequest&& other):parameters_(std::move(other.parameters_)){}
        BaseIndexRequest& operator=(const BaseIndexRequest& other){
            if(this!=&other){
                parameters_ = other.parameters_;
                last_update_ = other.last_update_;
            }
            return *this;
        }
        BaseIndexRequest& operator=(BaseIndexRequest&& other) noexcept{
            if(this!=&other){
                parameters_ = std::move(other.parameters_);
                last_update_ = std::move(other.last_update_);
            }
            return *this;
        }
        template<Data_t T,Data_f F>
        IndexParameters<T,F>& add_index(IndexParameters<T,F> index){
            return parameters_.emplace_back().
                emplace<IndexParameters<T,F>>(
                    std::forward<IndexParameters<T,F>>(index));
        }
        template<Data_t T,Data_f F>
        IndexParameters<T,F>& add_index(){
            return parameters_.emplace_back().
                emplace<IndexParameters<T,F>>(
                    IndexParameters<T,F>());
        }
        const std::vector<IndexParameters_t>& parameters() const noexcept{
            return parameters_;
        }
        void parameters(std::vector<IndexParameters_t> p) noexcept{
            parameters_ = std::move(p);
        }
        utc_tp last_update() const noexcept{
            return last_update_;
        }
        void last_update(utc_tp last_upd) noexcept{
            last_update_ = last_upd;
        }
    };
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::BaseIndexRequest>{
        using type = network::BaseIndexRequest;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,
                msg.parameters_,msg.last_update_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::BaseIndexRequest>{
        using type = network::BaseIndexRequest;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,
                msg.parameters_,msg.last_update_);
        }
    };

    template<>
    struct Serial_size<network::BaseIndexRequest>{
        using type = network::BaseIndexRequest;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.parameters_,msg.last_update_);
        }
    };

    template<>
    struct Min_serial_size<network::BaseIndexRequest>{
        using type = network::BaseIndexRequest;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                decltype(type::parameters_),
                decltype(type::last_update_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::BaseIndexRequest>{
        using type = network::BaseIndexRequest;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                decltype(type::parameters_),
                decltype(type::last_update_)>();
        }();
    };
}