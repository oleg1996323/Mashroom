#pragma once
#include <cinttypes>
#include "serialization.h"

namespace network{
struct ProgressBase{
    enum struct Process:uint8_t{
        NOTHING,
        SEARCH,
        EXTRACT,
        PREPARE,
        SENDING
    };
    float value_=0;
    Process server_process_ = Process::NOTHING;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ProgressBase>{
        using type = ProgressBase;
        SerializationEC operator()(const type& progress, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(progress,buf,progress.value_,progress.server_process_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ProgressBase>{
        using type = ProgressBase;
        SerializationEC operator()(type& progress, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(progress,buf,progress.value_,progress.server_process_);
        }
    };

    template<>
    struct Serial_size<ProgressBase>{
        using type = ProgressBase;
        size_t operator()(const type& progress) const noexcept{
            return serial_size(progress.value_,progress.server_process_);
        }
    };

    template<>
    struct Min_serial_size<ProgressBase>{
        using type = ProgressBase;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::value_),decltype(type::server_process_)>();
        }();
    };

    template<>
    struct Max_serial_size<ProgressBase>{
        using type = ProgressBase;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::value_),decltype(type::server_process_)>();
        }();
    };
}