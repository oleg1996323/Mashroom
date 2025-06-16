#pragma once
#include <optional>
#include <vector>
#include <bit>
#include <functional/float_conv.h>
#include <functional/def.h>
#include <chrono>
#include <cstring>
#include <expected>
#include <concepts>
#include <tuple>
#include <utility>

namespace ch = std::chrono;

struct Coord;
struct TimeOffset;
struct SearchParamTableVersion;

namespace serialization{
    
    enum class SerializationEC{
        NONE,
        BUFFER_SIZE_LESSER,
        UNMATCHED_TYPE,
        BUFFER_OVERFLOW
    };

    template<typename T>
    concept numeric_types_concept = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>;

    template<typename T>
    concept common_types_concept = numeric_types_concept<T> || std::is_class_v<T>;

    template<bool NETWORK_ORDER = true,typename T=void>
    bool serialize(const T& val,std::vector<char>& buf) noexcept;

    template<bool NETWORK_ORDER = true,typename T=void>
    auto deserialize(std::span<const char> buf) noexcept ->std::expected<T,SerializationEC>;

    template<typename T>
    constexpr size_t serial_size(const T& val) noexcept;

    template<typename T>
    constexpr size_t serial_size() noexcept{
        return serial_size(T{});
    }

    template<typename T>
    constexpr size_t min_serial_size(const T& val) noexcept;

    template<typename T>
    constexpr size_t min_serial_size() noexcept{
        return min_serial_size(T{});
    }

    template<typename T>
    constexpr size_t max_serial_size(const T& val) noexcept;

    template<typename T>
    constexpr size_t max_serial_size() noexcept{
        return max_serial_size(T{});
    }

    template<bool NETWORK_ORDER = true,typename... ARGS>
    requires ((!std::is_void_v<ARGS> && ...) && sizeof...(ARGS)>1)
    bool serialize(std::vector<char>& buf,ARGS&&... args) noexcept{
        return (serialize<NETWORK_ORDER>(args, buf) && ...);
    }   

    template<bool NETWORK_ORDER = true,typename T,typename... ARGS>
    auto deserialize(std::span<const char> buf,ARGS&&... args) noexcept ->std::expected<T,SerializationEC>{
        T result;
        size_t offset = 0;

        if (buf.size() < (min_serial_size(args)+...)) {
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        }
        else if(buf)

        auto deserialize_field = [&](auto& field) -> std::expected<void, SerializationEC> {
            if (offset >= buf.size()) {
                return std::unexpected(SerializationEC::BUFFER_OVERFLOW);
            }
            auto subspan = buf.subspan(offset);
            auto deserial_res = deserialize<NETWORK_ORDER, std::remove_reference_t<decltype(field)>>(subspan);
            
            if (!deserial_res) {
                return std::unexpected(deserial_res.error());
            }

            field = *deserial_res;
            offset += serial_size(field);
            return {};
        };
        auto success = (deserialize_field(args).has_value() && ...);
        if (!success) {
            return std::unexpected(SerializationEC::UNMATCHED_TYPE);
        }
        return result;
    }

    template<typename T>
    concept serialize_concept = 
    requires(const T& val, const std::vector<char>& buf){
        { serialize(val, buf) } -> std::same_as<bool>;
    };

    template<typename T>
    concept deserialize_concept = 
    requires(std::span<const char> buf){
        { deserialize<T>(buf) } -> std::same_as<std::expected<T,SerializationEC>>;
    };

    template<typename T>
    auto serialize_native(const T& val,std::vector<char>& buf) noexcept{
        return serialize<false,T>(val,buf);
    }
    template<typename T>
    auto serialize_network(const T& val,std::vector<char>& buf) noexcept{
        return serialize<true,T>(val,buf);
    }
    template<typename T>
    auto deserialize_native(std::span<const char> buf) noexcept{
        return deserialize<false,T>(buf);
    }
    template<typename T>
    auto deserialize_network(std::span<const char> buf) noexcept{
        return deserialize<true,T>(buf);
    }

    template<typename T>
    concept serial_size_concept = 
    requires(const T& val){
        { serial_size(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    concept min_serial_size_concept = 
    requires(const T& val){
        { min_serial_size(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    concept max_serial_size_concept = 
    requires(const T& val){
        { max_serial_size(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    requires numeric_types_concept<T>
    constexpr size_t serial_size(const T& val) noexcept{
        return sizeof(T);
    }
    template<template<typename...> class CLASS,typename... ARGS>
    requires (serial_size_concept<ARGS> && ...)
    constexpr size_t serial_size(const CLASS<ARGS...>& val) noexcept;

    template<typename T>
    requires serial_size_concept<T>
    constexpr size_t serial_size(const std::optional<T>& val) noexcept{
        constexpr size_t flag_size = 1;
        return flag_size + (val ? serial_size(val.value()) : 0);
    }

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t serial_size(const T& val) noexcept{
        return sizeof(val.time_since_epoch().count());
    }

    template<template<typename> class CLASS,typename... ARGS>
    requires (min_serial_size_concept<ARGS> && ...)
    constexpr size_t min_serial_size(const CLASS<ARGS...>& val) noexcept;

    template<typename T>
    requires numeric_types_concept<T>
    constexpr size_t min_serial_size(const T& val) noexcept{
        return serial_size(val);
    }

    template<typename T>
    requires common_types_concept<T> && min_serial_size_concept<T>
    constexpr size_t min_serial_size(const std::optional<T>& val) noexcept{
        return sizeof(val.has_value());
    }

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t min_serial_size(const T& val) noexcept{
        return serial_size(val.time_since_epoch().count());
    }

    template<typename T>
    requires numeric_types_concept<T>
    constexpr size_t max_serial_size(const T& val) noexcept{
        return serial_size(val);
    }

    template<typename T>
    requires common_types_concept<T> && max_serial_size_concept<T>
    constexpr size_t max_serial_size(const std::optional<T>& val) noexcept{
        return min_serial_size(val)+max_serial_size(T{});
    }

    template<template<typename> class CLASS,typename... ARGS>
    requires (max_serial_size_concept<ARGS> && ...)
    constexpr size_t max_serial_size(const CLASS<ARGS...>& val) noexcept;

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t max_serial_size(const T& val) noexcept{
        return serial_size(val.time_since_epoch().count());
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires numeric_types_concept<T>
    bool serialize(const T& val,std::vector<char>& buf) noexcept{
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>){
            using RawType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
            RawType raw_val = static_cast<RawType>(val);
            if constexpr(sizeof(T)==1)
                buf.push_back(static_cast<char>(raw_val));
            else{
                if constexpr(NETWORK_ORDER && is_little_endian())
                    raw_val = std::byteswap(raw_val);
                const auto* begin = reinterpret_cast<const char*>(&raw_val);
                buf.insert(buf.end(),begin,begin+sizeof(raw_val));
            }
        }
        else{
            if constexpr(NETWORK_ORDER && is_little_endian()){
                    auto int_val = to_integer(val);
                    int_val = std::byteswap(int_val);
                    val = to_float(int_val);
            }
            const auto* begin = reinterpret_cast<const char*>(&val);
            buf.insert(buf.end(),begin,begin+sizeof(val));
        }
        return true;
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    bool serialize(const T& time, std::vector<char>& buf) noexcept{
        using Rep = decltype(time.time_since_epoch().count());
        Rep time_count = time.time_since_epoch().count();
        if constexpr (NETWORK_ORDER && sizeof(Rep) > 1 && is_little_endian())
            time_count = std::byteswap(time_count);
        const auto* begin = reinterpret_cast<const char*>(&time_count);
        buf.insert(buf.end(), begin, begin + sizeof(time_count));
        return true;
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires common_types_concept<T> && serialize_concept<T>
    bool serialize(const std::optional<T>& val,std::vector<char>& buf){
        const bool has_val = val.has_value();
        buf.push_back(static_cast<char>(has_val));
        if(!has_val)
            return true;
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>){
            using RawType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
            RawType raw_val = static_cast<RawType>(val.value());
            if constexpr (NETWORK_ORDER && sizeof(RawType) > 1 && is_little_endian()) 
                raw_val = std::byteswap(raw_val);
            const auto* begin = reinterpret_cast<const char*>(&raw_val);
                buf.insert(buf.end(),
                begin,
                begin + sizeof(raw_val));
        }
        else if constexpr(std::is_floating_point_v<T>){
            T val_n = val.value();
            if constexpr(NETWORK_ORDER && is_little_endian()){
                auto int_val = to_integer(val_n);
                int_val = std::byteswap(int_val);
                val_n = to_float(int_val);
            }
            const auto* begin = reinterpret_cast<const char*>(&val_n);
            buf.insert(buf.end(),begin,begin + sizeof(val_n));
        }
        else
            serialize(val.value(),buf);
        return true;
    }

    /// @brief Deserialize data from buffer to specified type
    /// @tparam T Supported types: integral, floating-point, enum (including scoped enum)
    /// @tparam NETWORK_ORDER If true, converts from network (big-endian) byte order
    /// @param buf Input data buffer (read-only)
    /// @return std::expected<T, SerializationEC> - value or error code
    /// @note Supports both runtime and constexpr contexts
    /// @warning Buffer must be properly aligned for type T
    template<bool NETWORK_ORDER = true,typename T=void>
    requires numeric_types_concept<T>
    auto deserialize(std::span<const char> buf) noexcept -> std::expected<T,SerializationEC>{
        static_assert(std::is_trivially_copyable_v<T>, 
                 "Type T must be trivially copyable");
        static_assert(std::is_same_v<decltype(deserialize<int>({})), 
                std::expected<int, SerializationEC>>,
                "Return type validation failed");
        if(buf.size()<sizeof(T))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>){
            using RawType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
            RawType value;
            if constexpr (sizeof(RawType) == 1) 
                return static_cast<T>(*buf.data());
            if (std::is_constant_evaluated())
                std::copy_n(buf.data(), sizeof(RawType), reinterpret_cast<char*>(&value));
            else
                std::memcpy(&value,buf.data(),sizeof(RawType));
            if constexpr(NETWORK_ORDER && sizeof(RawType)>1 && is_little_endian())
                value = std::byteswap(value);
            return static_cast<T>(value);
        }
        else{
            using IntType = decltype(::detail::to_integer_type<sizeof(T)>());
            IntType int_val;
            if (std::is_constant_evaluated())
                std::copy_n(buf.data(), sizeof(IntType), reinterpret_cast<char*>(&int_val));
            else
                std::memcpy(&int_val,buf.data(),sizeof(IntType));
            if constexpr(NETWORK_ORDER && is_little_endian())
                int_val = std::byteswap(int_val);
            return to_float(int_val);
        }
    }


    /// @brief For duration
    /// @tparam T duration class
    /// @tparam NETWORK_ORDER 
    /// @param buf 
    /// @return std::chrono duration
    template<bool NETWORK_ORDER = true,typename T=void>
    requires requires (const T& time){
        time.count();
    }
    auto deserialize(std::span<const char> buf) noexcept->std::expected<T,SerializationEC>{
        T result;
        if(buf.size()<min_serial_size(result))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        
        decltype(T::count()) IntVal = 0;
        std::memcpy(&IntVal,buf.data(),serial_size(result));
        if constexpr (NETWORK_ORDER && is_little_endian())
            result = T(std::byteswap(IntVal));
        else result = T(IntVal);
        return result;
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    auto deserialize(std::span<const char> buf) noexcept->std::expected<T,SerializationEC>{
        T result;
        if(buf.size()<min_serial_size(result))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        decltype(result.time_since_epoch().count()) IntVal = 0;
        std::memcpy(&IntVal,buf.data(),serial_size(result));
        result = std::chrono::duration_cast<decltype(result)::duration>(std::chrono::nanoseconds(IntVal));
        return result;
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires deserialize_concept<T>
    auto deserialize(std::span<const char> buf)->std::expected<std::optional<T>,SerializationEC>{
        std::optional<T> result;
        if(buf.size()<min_serial_size(result))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        auto flag_result = [&]() -> std::expected<bool, SerializationEC> 
        {
            if constexpr (NETWORK_ORDER) {
                return deserialize_network<bool>(buf.first(min_serial_size(result)));
            } else {
                return deserialize_native<bool>(buf.first(min_serial_size(result)));
            }
        }();
        if(!flag_result)
            return std::unexpected(flag_result.error());
        if(!*flag_result)
            return result;
        if(buf.size()<max_serial_size(result))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        auto value_result = [&]() -> std::expected<T, SerializationEC> 
        {
            if constexpr (NETWORK_ORDER) {
                return deserialize_network<T>(buf.subspan(min_serial_size(result)));
            } else {
                return deserialize_native<T>(buf.subspan(min_serial_size(result)));
            }
        }();
        if(!value_result)
            return std::unexpected(value_result.error());
        return std::optional<T>{std::move(*value_result)};
    }
}
