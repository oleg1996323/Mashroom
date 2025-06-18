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

    template<bool NETWORK_ORDER,typename T,typename R>
    struct _SerializationBaseTypeDef_{
        using return_t = R;
        using type = T;
        constexpr decltype(NETWORK_ORDER) byte_order = NETWORK_ORDER;
    };

    template<typename T>
    concept serialize_concept = 
    requires(const T& val, const std::vector<char>& buf){
        { serialize(val, buf) } -> std::same_as<SerializationEC>;
        requires noexcept (serialize(val, buf));
    };

    template<typename T>
    concept deserialize_concept = 
    requires(const T& val,std::span<const char> buf){
        { deserialize(val,buf) } -> std::same_as<SerializationEC>;
        requires noexcept (deserialize(val,buf));
    };

    template<typename T>
    concept serial_size_concept = 
    requires(const T& val){
        { serial_size(val) } -> std::same_as<size_t>;
        requires noexcept (serial_size(val));
    };

    template<typename T>
    concept min_serial_size_concept = 
    requires(const T& val){
        { min_serial_size(val) } -> std::same_as<size_t>;
        requires noexcept (min_serial_size(val));
    };

    template<typename T>
    concept max_serial_size_concept = 
    requires(const T& val){
        { max_serial_size(val) } -> std::same_as<size_t>;
        requires noexcept (max_serial_size(val));
    };

    template<bool NETWORK_ORDER,typename T>
    requires (!serialize_concept<std::decay_t<T>> && !numeric_types_concept<std::decay_t<T>>)
    SerializationEC serialize(const std::decay_t<T>& val,std::vector<char>& buf) noexcept = delete;

    template<bool NETWORK_ORDER = true,typename T=void>
    requires (!deserialize_concept<std::decay_t<T>> && !numeric_types_concept<std::decay_t<T>>)
    SerializationEC deserialize(std::decay_t<T>& val,std::span<const char> buf) noexcept = delete;

    template<typename T>
    requires (!serial_size_concept<T> && !numeric_types_concept<T>)
    size_t serial_size(const T& val) noexcept = delete;

    template<typename T>
    requires requires { { serial_size(std::declval<const T&>()) } -> std::same_as<size_t>; }
    size_t serial_size() noexcept{
        return serial_size(T{});
    }

    template<typename T>
    requires (!min_serial_size_concept<T> && !numeric_types_concept<T>)
    constexpr size_t min_serial_size(const T& val) noexcept = delete;

    template<typename T>
    requires requires { { min_serial_size(std::declval<const T&>()) } -> std::same_as<size_t>; }
    constexpr size_t min_serial_size() noexcept{
        return min_serial_size(T{});
    }

    template<typename T>
    requires (!max_serial_size_concept<T> && !numeric_types_concept<T>)
    constexpr size_t max_serial_size(const T& val) noexcept = delete;

    template<typename T>
    requires requires { { max_serial_size(std::declval<const T&>()) } -> std::same_as<size_t>; }
    constexpr size_t max_serial_size() noexcept {
        return max_serial_size(T{});
    }

    template<typename T>
    constexpr size_t max_serial_size() noexcept;

    template<typename... ARGS>
    requires (serial_size_concept<ARGS> && ...)
    size_t serial_size(ARGS&&... val) noexcept{
        return (serial_size(val) + ...);
    }

    template<typename... ARGS>
    size_t serial_size(ARGS&&... val) noexcept;

    template<typename... ARGS>
    requires (min_serial_size_concept<ARGS> && ...)
    constexpr size_t min_serial_size(ARGS&&... val) noexcept{
        return (min_serial_size(val) + ...);
    }

    template<typename... ARGS>
    constexpr size_t min_serial_size(ARGS&&... val) noexcept;

    template<typename... ARGS>
    requires (max_serial_size_concept<ARGS> && ...)
    constexpr size_t max_serial_size(ARGS&&... val) noexcept{
        return (max_serial_size(val) + ...);
    }

    template<typename... ARGS>
    constexpr size_t max_serial_size(ARGS&&... val) noexcept;


    template<bool NETWORK_ORDER>
    SerializationEC serialize(const auto& to_serialize,const std::vector<char>& buf,const auto... args) noexcept{
        static_assert(min_serial_size(to_serialize)==min_serial_size(args...),
        "Minimal serial size of serialized struct must be equal to the minimal serial size of all its members to be serialized");
        static_assert(max_serial_size(to_serialize)==max_serial_size(args...),
        "Maximal serial size of serialized struct must be equal to the maximal serial size of all its members to be serialized");
        return (serialize<NETWORK_ORDER>(args, buf) && ...);
    }   
    
    template<bool NETWORK_ORDER,typename T,typename... ARGS>
    SerializationEC deserialize(const std::decay_t<T>& to_deserialize,std::span<const char> buf,std::decay_t<ARGS>&... args) noexcept{
        static_assert((!std::is_void_v<ARGS> && ...),"Cannot be void");
        static_assert(sizeof...(ARGS...)>1,"Cannot be less than 2 argument");
        static_assert(min_serial_size_concept<decltype(to_deserialize)>,"Expected specification of min_serial_size function");
        static_assert(max_serial_size_concept<decltype(to_deserialize)>,"Expected specification of max_serial_size function");
        static_assert(min_serial_size(to_deserialize)==min_serial_size(args...),"Expected equal minimal serial size of object and its fields' summary minimal serial size");
        static_assert(min_serial_size(to_deserialize)==min_serial_size(args...),"Expected equal maximal serial size of object and its fields' summary maximal serial size");
        size_t offset = 0;
        if (buf.size() < min_serial_size(to_deserialize))
            return SerializationEC::BUFFER_SIZE_LESSER;

        SerializationEC deserialize_field = [&](auto field) {
            if (offset >= buf.size()) {
                return SerializationEC::BUFFER_OVERFLOW;
            }
            auto subspan = buf.subspan(offset);
            SerializationEC code = deserialize<NETWORK_ORDER>(field,subspan);
            
            if (code!=SerializationEC::NONE) {
                return code;
            }

            offset += serial_size(field);
            return SerializationEC::NONE;
        };

        for(auto& field:{std::ref(args)...})
            if (auto code = deserialize_field(field); code != SerializationEC::NONE) {
                return code;
            }
        return SerializationEC::NONE;
    }

    template<typename T>
    auto serialize_native(const T& val,std::vector<char>& buf) noexcept{
        return serialize<false,T>(val,buf);
    }
    template<typename T>
    auto serialize_network(const T& val,std::vector<char>& buf) noexcept{
        return serialize<true,T>(val,buf);
    }
    template<typename T>
    auto deserialize_native(const T& to_deserialize,std::span<const char> buf) noexcept{
        return deserialize<false,T>(to_deserialize,buf);
    }
    template<typename T>
    auto deserialize_network(const T& to_deserialize,std::span<const char> buf) noexcept{
        return deserialize<true,T>(to_deserialize,buf);
    }

    template<typename T,typename... ARGS>
    auto serialize_native(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept{
        return serialize<false,T>(val,buf,args...);
    }
    template<typename T,typename... ARGS>
    auto serialize_network(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept{
        return serialize<true,T>(val,buf,args...);
    }
    template<typename T,typename... ARGS>
    auto deserialize_native(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept{
        return deserialize<false>(to_deserialize,buf,args...);
    }
    template<typename T,typename... ARGS>
    auto deserialize_network(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept{
        return deserialize<true>(to_deserialize,buf,args...);
    }

/// Some basic definitions

    //simple numeric objects

    template<bool NETWORK_ORDER,typename T>
    requires numeric_types_concept<T>
    SerializationEC serialize(const T& val,std::vector<char>& buf) noexcept{
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
        return SerializationEC::NONE;
    }

    /// @brief Deserialize data from buffer to specified type
    /// @tparam T Supported types: integral, floating-point, enum (including scoped enum)
    /// @tparam NETWORK_ORDER If true, converts from network (big-endian) byte order
    /// @param buf Input data buffer (read-only)
    /// @return std::expected<T, SerializationEC> - value or error code
    /// @note Supports both runtime and constexpr contexts
    /// @warning Buffer must be properly aligned for type T
    template<bool NETWORK_ORDER,typename T>
    requires numeric_types_concept<T>
    SerializationEC deserialize(T& val,std::span<const char> buf) noexcept{
        static_assert(std::is_trivially_copyable_v<T>, 
                 "Type T must be trivially copyable");
        if(buf.size()<sizeof(T))
            return SerializationEC::BUFFER_SIZE_LESSER;
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
            val = static_cast<T>(value);
            return SerializationEC::NONE;
        }
        else{
            using IntType = ::detail::to_integer_type<sizeof(T)>;
            IntType int_val;
            if (std::is_constant_evaluated())
                std::copy_n(buf.data(), sizeof(IntType), reinterpret_cast<char*>(&int_val));
            else
                std::memcpy(&int_val,buf.data(),sizeof(IntType));
            if constexpr(NETWORK_ORDER && is_little_endian())
                int_val = std::byteswap(int_val);
            val = to_float(int_val);
            return SerializationEC::NONE;
        }
    }

    template<typename T>
    requires numeric_types_concept<T>
    size_t serial_size(const T& val) noexcept{
        return sizeof(T);
    }

    template<typename T>
    requires numeric_types_concept<T>
    constexpr size_t min_serial_size(const T& val) noexcept{
        return serial_size(val);
    }

    template<typename T>
    requires numeric_types_concept<T>
    constexpr size_t max_serial_size(const T& val) noexcept{
        return serial_size(val);
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    SerializationEC serialize(const T& time, std::vector<char>& buf) noexcept{
        using Rep = decltype(time.time_since_epoch().count());
        Rep time_count = time.time_since_epoch().count();
        if constexpr (NETWORK_ORDER && sizeof(Rep) > 1 && is_little_endian())
            time_count = std::byteswap(time_count);
        const auto* begin = reinterpret_cast<const char*>(&time_count);
        buf.insert(buf.end(), begin, begin + sizeof(time_count));
        return SerializationEC::NONE;
    }

    template<bool NETWORK_ORDER,typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    SerializationEC deserialize(T& time_val,std::span<const char> buf) noexcept{
        if(buf.size()<min_serial_size(time_val))
            return SerializationEC::BUFFER_SIZE_LESSER;
        decltype(time_val.time_since_epoch().count()) IntVal = 0;
        std::memcpy(&IntVal,buf.data(),serial_size(time_val));
        time_val = std::chrono::duration_cast<decltype(time_val)::duration>(std::chrono::nanoseconds(IntVal));
        return SerializationEC::NONE;
    }

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t serial_size(const T& val) noexcept{
        return sizeof(val.time_since_epoch().count());
    }

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t min_serial_size(const T& val) noexcept{
        return serial_size(val.time_since_epoch().count());
    }

    template<typename T>
    requires requires (const T& time){
        time.time_since_epoch().count();
    }
    constexpr size_t max_serial_size(const T& val) noexcept{
        return serial_size(val.time_since_epoch().count());
    }

    template<bool NETWORK_ORDER,typename T>
    requires requires (const T& duration){
        duration.count();
    }
    SerializationEC serialize(const T& time, std::vector<char>& buf) noexcept{
        using Rep = decltype(time.count());
        Rep time_count = time.count();
        return serialize(time_count,buf);
    }

    /// @brief For duration
    /// @tparam T duration class
    /// @tparam NETWORK_ORDER 
    /// @param buf 
    /// @return std::chrono duration
    template<bool NETWORK_ORDER = true,typename T=void>
    requires requires (const T& duration){
        duration.count();
    }
    SerializationEC deserialize(T& to_deserialize,std::span<const char> buf) noexcept{
        if(buf.size()<min_serial_size(to_deserialize))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        decltype(T::count()) IntVal = 0;
        std::memcpy(&IntVal,buf.data(),serial_size(to_deserialize));
        if constexpr (NETWORK_ORDER && is_little_endian())
            to_deserialize = T(std::byteswap(IntVal));
        else to_deserialize = T(IntVal);
        return SerializationEC::NONE;
    }

    template<typename T>
    requires requires (const T& duration){
        duration.count();
    }
    constexpr size_t serial_size(const T& duration) noexcept{
        return sizeof(duration.count());
    }

    template<typename T>
    requires requires (const T& duration){
        duration.count();
    }
    constexpr size_t min_serial_size(const T& duration) noexcept{
        return serial_size(duration.count());
    }

    template<typename T>
    requires requires (const T& duration){
        duration.count();
    }
    constexpr size_t max_serial_size(const T& duration) noexcept{
        return serial_size(duration.count());
    }

    template<bool NETWORK_ORDER = true,typename T=void>
    requires serialize_concept<T>
    SerializationEC serialize(const std::optional<T>& val,std::vector<char>& buf){
        auto err = serialize(val.has_value(),buf);
        if(err!=SerializationEC::NONE)
            return err;
        if(!val.has_value())
            return SerializationEC::NONE;
        err = serialize(val.value(),buf);
        if(err!=SerializationEC::NONE)
            return err;
        return SerializationEC::NONE;
    }

    template<bool NETWORK_ORDER,typename T>
    requires deserialize_concept<T>
    SerializationEC deserialize(std::optional<T>& to_deserialize,std::span<const char> buf) noexcept{
        to_deserialize.reset();
        if(buf.size()<min_serial_size(to_deserialize))
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        bool has_value = false;
        auto code = deserialize<NETWORK_ORDER>(has_value,buf.subspan(0,serial_size(code)));
        if(!code!=SerializationEC::NONE)
            return code;
        if(!has_value)
            return SerializationEC::NONE;
        if(buf.size()<max_serial_size(to_deserialize))
            return SerializationEC::BUFFER_SIZE_LESSER;
        T value;
        code = deserialize<NETWORK_ORDER>(value,buf.subspan(0,serial_size(code)));
        if(code!=SerializationEC::NONE)
            return code;
        return std::optional<T>{std::move(value)};
    }

    template<typename T>
    requires serial_size_concept<T>
    size_t serial_size(const std::optional<T>& val) noexcept{
        constexpr size_t flag_size = 1;
        return flag_size + (val ? serial_size(val.value()) : 0);
    }

    template<typename T>
    requires common_types_concept<T> && min_serial_size_concept<T>
    constexpr size_t min_serial_size(const std::optional<T>& val) noexcept{
        return sizeof(val.has_value());
    }

    template<typename T>
    requires common_types_concept<T> && max_serial_size_concept<T>
    constexpr size_t max_serial_size(const std::optional<T>& val) noexcept{
        return min_serial_size(val)+max_serial_size<T>();
    }
}

/// @brief Check if code error is equal serialization::SerializationEC::NONE;
/// @param code 
/// @return if false - has an error
bool operator!(serialization::SerializationEC code){
    return code==serialization::SerializationEC::NONE;
}