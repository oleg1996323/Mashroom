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
#include <cstring>

namespace serialization{
    
    enum class SerializationEC{
        NONE,
        BUFFER_SIZE_LESSER,
        UNMATCHED_TYPE,
        BUFFER_OVERFLOW
    };

}

namespace serialization{

    template<bool NETWORK_ORDER,typename T>
    struct Serialize;

    template<bool NETWORK_ORDER,typename T>
    struct Deserialize;

    template<typename T>
    struct Serial_size;

    template<typename T>
    struct Max_serial_size;

    template<typename T>
    struct Min_serial_size;

    template<typename T>
    SerializationEC serialize_native(const T& val,std::vector<char>& buf) noexcept;
    template<typename T>
    SerializationEC serialize_network(const T& val,std::vector<char>& buf) noexcept;
    template<typename T>
    SerializationEC deserialize_native(const T& to_deserialize,std::span<const char> buf) noexcept;
    template<typename T>
    SerializationEC deserialize_network(const T& to_deserialize,std::span<const char> buf) noexcept;

    template<typename T>
    size_t serial_size(const T& val) noexcept;

    template<typename T>
    constexpr size_t min_serial_size(const T& val) noexcept;
    template<typename T>
    constexpr size_t min_serial_size() noexcept;

    template<typename T>
    constexpr size_t max_serial_size(const T& val) noexcept;
    template<typename T>
    constexpr size_t max_serial_size() noexcept;

    template<typename T,typename... ARGS>
    SerializationEC serialize_native(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept;
    template<typename T,typename... ARGS>
    SerializationEC serialize_network(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept;
    template<typename T,typename... ARGS>
    SerializationEC deserialize_native(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept;
    template<typename T,typename... ARGS>
    SerializationEC deserialize_network(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept;

    template<typename... ARGS>
    size_t serial_size(const ARGS&... val) noexcept;
    template<typename... ARGS>
    constexpr size_t min_serial_size(const ARGS&... val) noexcept;
    template<typename... ARGS>
    constexpr size_t max_serial_size(const ARGS&...val) noexcept;

    template<bool NETWORK_ORDER,typename T>
    SerializationEC serialize(const T& val,std::vector<char>& buf) noexcept;
    template<bool NETWORK_ORDER,typename T>
    SerializationEC deserialize(T& val,std::span<const char> buf) noexcept;

    template<bool NETWORK_ORDER,typename T,typename... ARGS>
    requires (sizeof...(ARGS)>1)
    SerializationEC serialize(const T& val,std::vector<char>& buf,const ARGS&... args) noexcept;
    template<bool NETWORK_ORDER,typename T,typename... ARGS>
    requires (sizeof...(ARGS)>1)
    SerializationEC deserialize(const T& to_deserialize,std::span<const char> buf,ARGS&... args) noexcept;

    template<typename T>
    concept numeric_types_concept = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>;

    template<typename T>
    concept common_types_concept = numeric_types_concept<T> || std::is_class_v<T>;

    template<typename T>
    concept serialize_concept = 
    requires(const T& val, const std::vector<char>& buf){
        { Serialize<true,T>{}(val, buf) } -> std::same_as<SerializationEC>;
    };

    template<typename T>
    concept deserialize_concept = 
    requires(const T& val,std::span<const char> buf){
        { Deserialize<true,T>{}(val,buf) } -> std::same_as<SerializationEC>;
    };

    template<typename T>
    concept serial_size_concept = 
    requires(const T& val){
        { Serial_size<T>{}(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    concept min_serial_size_concept = 
    requires(const T& val){
        { Min_serial_size<T>()(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    concept max_serial_size_concept = 
    requires(const T& val){
        { Max_serial_size<T>{}(val) } -> std::same_as<size_t>;
    };

    template<typename T>
    concept time_point_concept = requires (const T& time){
        time.time_since_epoch().count();
    };

    template<typename T>
    concept duration_concept = requires (const T& duration){
        duration.count();
    };

    template<bool NETWORK_ORDER,typename T>
    struct Serialize{
        SerializationEC operator()(const T& val,std::vector<char> buf) noexcept{
            if constexpr (numeric_types_concept<T>){
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
            else if constexpr (time_point_concept<T>){
                using Rep = decltype(val.time_since_epoch().count());
                Rep time_count = val.time_since_epoch().count();
                if constexpr (NETWORK_ORDER && sizeof(Rep) > 1 && is_little_endian())
                    time_count = std::byteswap(time_count);
                const auto* begin = reinterpret_cast<const char*>(&time_count);
                buf.insert(buf.end(), begin, begin + sizeof(time_count));
                return SerializationEC::NONE;
            }
            else if constexpr (time_point_concept<T>){
                using Rep = decltype(val.count());
                Rep time_count = val.count();
                if constexpr (NETWORK_ORDER && sizeof(Rep) > 1 && is_little_endian())
                    time_count = std::byteswap(time_count);
                const auto* begin = reinterpret_cast<const char*>(&time_count);
                buf.insert(buf.end(), begin, begin + sizeof(time_count));
                return SerializationEC::NONE;
            }
            else{
                static_assert(false,"serialize unspecified");
                return SerializationEC::NONE;
            }
        }

        template<serialize_concept... ARGS>
        SerializationEC operator()(const T& to_serialize, std::vector<char>& buf, const ARGS&... args) noexcept;
    };

    template<bool NETWORK_ORDER,typename T>
    struct Deserialize{
        /// @brief Deserialize data from buffer to specified type
        /// @tparam T Supported types: integral, floating-point, enum (including scoped enum)
        /// @tparam NETWORK_ORDER If true, converts from network (big-endian) byte order
        /// @param buf Input data buffer (read-only)
        /// @return std::expected<T, SerializationEC> - value or error code
        /// @note Supports both runtime and constexpr contexts
        /// @warning Buffer must be properly aligned for type T
        SerializationEC operator()(std::remove_const<T>& to_deserialize,std::span<const char> buf) noexcept{
            if constexpr (numeric_types_concept<T>){
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
                    to_deserialize = static_cast<T>(value);
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
                    to_deserialize = to_float(int_val);
                    return SerializationEC::NONE;
                }
            }
            else if constexpr (time_point_concept<T>){
                if(buf.size()<min_serial_size(to_deserialize))
                    return SerializationEC::BUFFER_SIZE_LESSER;
                decltype(to_deserialize.time_since_epoch().count()) IntVal = 0;
                auto code = deserialize<NETWORK_ORDER>(IntVal,buf);
                if(!code){
                    std::chrono::duration_cast<decltype(to_deserialize)::duration>(std::chrono::nanoseconds(IntVal));
                    return SerializationEC::NONE;
                }
                else
                    return code;
            }
            else if constexpr (duration_concept<T>){
                if(buf.size()<min_serial_size(to_deserialize))
                    return SerializationEC::BUFFER_SIZE_LESSER;
                decltype(to_deserialize.count()) IntVal = 0;
                auto code = deserialize<NETWORK_ORDER>(IntVal,buf);
                if(!code){
                    to_deserialize = T(IntVal);
                    return SerializationEC::NONE;
                }
                else
                    return code;
            }
            else {
                static_assert(false,"deserialize unspecified operator()");
                return SerializationEC::NONE;
            }
        }

        template<deserialize_concept... ARGS>
        requires (sizeof...(ARGS)>1)
        SerializationEC operator()(const T& to_deserialize, std::span<const char> buf, ARGS&... args) noexcept;
    };

    template<typename T>
    struct Serial_size{
        size_t operator()(const T& val) noexcept{
            if constexpr (numeric_types_concept<T>)
                return sizeof(val);
            else if constexpr (time_point_concept<T>)
                return sizeof(val.time_since_epoch().count());
            else if constexpr (duration_concept<T>)
                return sizeof(val.count());
            else {
                static_assert(false,"serial_size unspecified operator()");
                return 0;
            }
        }

        template<typename... ARGS>
        requires ((serial_size_concept<ARGS> && ...) && sizeof...(ARGS)>1)
        size_t operator()(const std::decay<ARGS>&...  val) noexcept{
            return (serial_size(val) + ...);
        }
    };

    template<typename T>
    struct Min_serial_size{
        constexpr size_t operator()() noexcept{
            if constexpr (numeric_types_concept<T>)
                return sizeof(T);
            else if constexpr (time_point_concept<T>)
                return sizeof(T{}.time_since_epoch().count());
            else if constexpr (duration_concept<T>)
                return sizeof(T{}.count());
            else{
                static_assert(false,"min_serial_size unspecified operator()");
                return 0;
            }
        }

        constexpr size_t operator()(const T& val) noexcept{
            return Min_serial_size{}();
        }

        template<typename... ARGS>
        requires ((min_serial_size_concept<ARGS> && ...) && sizeof...(ARGS)>1)
        constexpr size_t operator()(const ARGS&... val) noexcept{
            return (min_serial_size(val) + ...);
        }
    };

    template<typename T>
    struct Max_serial_size{
        constexpr size_t operator()() noexcept{
            if constexpr (numeric_types_concept<T>)
                return sizeof(T);
            else if constexpr (time_point_concept<T>)
                return sizeof(T{}.time_since_epoch().count());
            else if constexpr (duration_concept<T>)
                return sizeof(T{}.count());
            else{
                static_assert(false,"max_serial_size unspecified operator()");
                return 0;
            }
        }

        constexpr size_t operator()(const T& val) noexcept{
            return Max_serial_size{}();
        }

        template<typename... ARGS>
        requires ((max_serial_size_concept<ARGS> && ...) && sizeof...(ARGS)>1)
        constexpr size_t operator()(const ARGS&... val) noexcept{
            return (max_serial_size(val) + ...);
        }
    };

    template<typename T>
    SerializationEC serialize_native(const T& val,std::vector<char>& buf) noexcept{
        return serialize<false,T>(val,buf);
    }
    template<typename T>
    SerializationEC serialize_network(const T& val,std::vector<char>& buf) noexcept{
        return serialize<true,T>(val,buf);
    }
    template<typename T>
    SerializationEC deserialize_native(T& to_deserialize,std::span<const char> buf) noexcept{
        return deserialize<false,T>(to_deserialize,buf);
    }
    template<typename T>
    SerializationEC deserialize_network(T& to_deserialize,std::span<const char> buf) noexcept{
        return deserialize<true,T>(to_deserialize,buf);
    }

    template<bool NETWORK_ORDER,typename T>
    SerializationEC serialize(const T& val,std::vector<char>& buf) noexcept{
        if constexpr (NETWORK_ORDER)
            return serialize_network(val,buf);
        else return serialize_native(val,buf);
    }
    template<bool NETWORK_ORDER,typename T>
    SerializationEC deserialize(T& val,std::span<const char> buf) noexcept{
        if constexpr (NETWORK_ORDER)
            return deserialize_network(val,buf);
        else return deserialize_native(val,buf);
    }

    template<typename T>
    size_t serial_size(const T& val) noexcept{
        return Serial_size<T>{}(val);
    }

    template<typename T>
    constexpr size_t min_serial_size(const T& val) noexcept{
        return Min_serial_size<T>{}(val);
    }
    template<typename T>
    constexpr size_t min_serial_size() noexcept{
        return Min_serial_size<T>{}();
    }

    template<typename T>
    constexpr size_t max_serial_size(const T& val) noexcept{
        return Max_serial_size<T>{}(val);
    }
    template<typename T>
    constexpr size_t max_serial_size() noexcept{
        return Max_serial_size<T>{}();
    }

    template<typename T,typename... ARGS>
    SerializationEC serialize_native(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept{
        return serialize<false,T>(val,buf,args...);
    }
    template<typename T,typename... ARGS>
    SerializationEC serialize_network(const T& val,std::vector<char>& buf,ARGS&&... args) noexcept{
        return serialize<true,T>(val,buf,args...);
    }
    template<typename T,typename... ARGS>
    SerializationEC deserialize_native(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept{
        return deserialize<false,T>(to_deserialize,buf,args...);
    }
    template<typename T,typename... ARGS>
    SerializationEC deserialize_network(const T& to_deserialize,std::span<const char> buf,ARGS&&... args) noexcept{
        return deserialize<true,T>(to_deserialize,buf,args...);
    }

    template<typename... ARGS>
    size_t serial_size(const ARGS&... val) noexcept{
        return (serial_size(val) + ...);
    }
    template<typename... ARGS>
    constexpr size_t min_serial_size(const ARGS&... val) noexcept{
        return (min_serial_size(val) + ...);
    }
    template<typename... ARGS>
    constexpr size_t max_serial_size(const ARGS&...args) noexcept{
        size_t result = 0;
        auto add_sz = [&result](size_t value) -> bool
        {
            if(value<=std::numeric_limits<size_t>::max()-result){
                result+=value;
                return true;
            }
            else {
                result = std::numeric_limits<size_t>::max();
                return false;
            }
        };
        (add_sz(max_serial_size(args)) && ...);
        return result;
    }

    template<bool NETWORK_ORDER,typename T,typename... ARGS>
    requires (sizeof...(ARGS)>1)
    SerializationEC serialize(const T& val,std::vector<char>& buf,const ARGS&... args) noexcept{
        if constexpr (NETWORK_ORDER)
            return serialize_network(val,buf,args...);
        else return serialize_native(val,buf,args...);
    }
    template<bool NETWORK_ORDER,typename T,typename... ARGS>
    requires (sizeof...(ARGS)>1)
    SerializationEC deserialize(const T& val,std::span<const char> buf, ARGS&... args) noexcept{
        if constexpr (NETWORK_ORDER)
            return deserialize_network(val,buf,args...);
        else return deserialize_native(val,buf,args...);
    }
    
    template<bool NETWORK_ORDER,typename T>
    requires serialize_concept<std::optional<T>>
    struct Serialize<NETWORK_ORDER,std::optional<T>>{
        SerializationEC operator()(const T& val,std::vector<char> buf) noexcept{
            SerializationEC err = serialize<NETWORK_ORDER>(val.has_value(),buf);
            if(err!=SerializationEC::NONE)
                return err;
            if(!val.has_value())
                return SerializationEC::NONE;
            err = serialize<NETWORK_ORDER>(val.value(),buf);
            if(err!=SerializationEC::NONE)
                return err;
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER,typename T>
    requires deserialize_concept<T>
    struct Deserialize<NETWORK_ORDER,std::optional<T>>{
        /// @brief Deserialize data from buffer to specified type
        /// @tparam T Supported types: integral, floating-point, enum (including scoped enum)
        /// @tparam NETWORK_ORDER If true, converts from network (big-endian) byte order
        /// @param buf Input data buffer (read-only)
        /// @return std::expected<T, SerializationEC> - value or error code
        /// @note Supports both runtime and constexpr contexts
        /// @warning Buffer must be properly aligned for type T
        SerializationEC operator()(std::optional<T>& to_deserialize,std::span<const char> buf) noexcept{
            to_deserialize.reset();
            if(buf.size()<min_serial_size(to_deserialize))
                return SerializationEC::BUFFER_SIZE_LESSER;
            bool has_value = false;
            auto code = deserialize<NETWORK_ORDER>(has_value,buf.subspan(0,serial_size(has_value)));
            if(!code!=SerializationEC::NONE)
                return code;
            if(!has_value)
                return SerializationEC::NONE;
            if(buf.size()<max_serial_size(to_deserialize))
                return SerializationEC::BUFFER_SIZE_LESSER;
            T value;
            code = deserialize<NETWORK_ORDER>(value,buf.subspan(0,serial_size(value)));
            if(code!=SerializationEC::NONE)
                return code;
            return SerializationEC::NONE;
        }
    };

    template<typename T>
    struct Serial_size<std::optional<T>>{
        size_t operator()(const std::optional<T>& val) noexcept{
            constexpr size_t flag_size = 1;
            return flag_size + (val.has_value() ? serial_size(*val) : 0);
        }
    };

    template<min_serial_size_concept T>
    struct Min_serial_size<std::optional<T>>{
        constexpr size_t operator()(const std::optional<T>& val) noexcept{
            return sizeof(std::optional<T>{}.has_value());
        }
    };

    template<max_serial_size_concept T>
    struct Max_serial_size<std::optional<T>>{
        constexpr size_t operator()(const std::optional<T>& val) noexcept{
            return min_serial_size<std::optional<T>>()+max_serial_size<T>();
        }
    };

    template<bool NETWORK_ORDER,std::ranges::common_range T>
    requires serialize_concept<std::ranges::range_value_t<T>>
    struct Serialize<NETWORK_ORDER,T>{
        SerializationEC operator()(const T& range,std::vector<char> buf) noexcept{
            SerializationEC err = serialize<NETWORK_ORDER>(range.size(),buf);
            for(const auto& item:range){
                err = serialize<NETWORK_ORDER>(item,buf);
                if(err==SerializationEC::NONE)
                    continue;
                else
                    return err;
            }
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER,std::ranges::common_range T>
    requires deserialize_concept<std::ranges::range_value_t<T>>
    struct Deserialize<NETWORK_ORDER,T>{
        SerializationEC operator()(T& to_deserialize,std::span<const char> buf) noexcept{
            if(buf.size()<min_serial_size(to_deserialize))
                return SerializationEC::BUFFER_SIZE_LESSER;
            size_t range_sz = 0;
            SerializationEC code = deserialize<NETWORK_ORDER>(range_sz,buf.subspan(0,serial_size(range_sz)));
            if(code!=SerializationEC::NONE)
                return code;
            
            for(int i = 0;i<range_sz;++i){
                std::ranges::range_value_t<T> item;
                code = deserialize<NETWORK_ORDER>(item,buf);
                if(code!=SerializationEC::NONE)
                    return code;
            }
            return SerializationEC::NONE;
        }
    };

    template<std::ranges::common_range T>
    requires serial_size_concept<std::ranges::range_value_t<T>>
    struct Serial_size<T>{
        size_t operator()(const T& range) noexcept{
            constexpr size_t size_sz = sizeof(T{}.size());
            size_t total = size_sz;
            for(const auto& item:range)
                total+=serial_size(item);
            return total;
        }
    };

    template<std::ranges::common_range T>
    requires min_serial_size_concept<std::ranges::range_value_t<T>>
    struct Min_serial_size<T>{
        constexpr size_t operator()(const T& val) noexcept{
            return sizeof(T{}.size());
        }
    };

    template<std::ranges::common_range T>
    requires max_serial_size_concept<std::ranges::range_value_t<T>>
    struct Max_serial_size<T>{
        constexpr size_t operator()(const T& val) noexcept{
            return std::numeric_limits<decltype(T{}.size())>::max();
        }
    };

    template<bool NETWORK_ORDER,typename T>
    template<serialize_concept... ARGS>
    SerializationEC Serialize<NETWORK_ORDER,T>::operator()(const T& to_serialize, std::vector<char>& buf, const ARGS&... args) noexcept{
        static_assert(min_serial_size<T>()==min_serial_size(args...),
        "Minimal serial size of serialized struct must be equal to the minimal serial size of all its members to be serialized");
        static_assert(max_serial_size<T>()==max_serial_size(args...),
        "Maximal serial size of serialized struct must be equal to the maximal serial size of all its members to be serialized");
        return (serialize<NETWORK_ORDER>(args, buf) && ...);
    }

    template<bool NETWORK_ORDER,typename T>
    template<deserialize_concept... ARGS>
    requires (sizeof...(ARGS)>1)
    SerializationEC Deserialize<NETWORK_ORDER,T>::operator()(const T& to_deserialize, std::span<const char> buf, ARGS&... args) noexcept{
        static_assert(min_serial_size_concept<decltype(to_deserialize)>,"Expected specification of min_serial_size function");
        static_assert(max_serial_size_concept<decltype(to_deserialize)>,"Expected specification of max_serial_size function");
        static_assert(min_serial_size(to_deserialize)==min_serial_size(args...),"Expected equal minimal serial size of object and its fields' summary minimal serial size");
        static_assert(min_serial_size(to_deserialize)==min_serial_size(args...),"Expected equal maximal serial size of object and its fields' summary maximal serial size");
        size_t offset = 0;
        SerializationEC result_code = SerializationEC::NONE;
        if (buf.size() < min_serial_size(to_deserialize))
            return SerializationEC::BUFFER_SIZE_LESSER;

        SerializationEC deserialize_field = [&](auto field) {
            if (offset >= buf.size()) {
                return SerializationEC::BUFFER_OVERFLOW;
            }
            auto subspan = buf.subspan(offset);
            SerializationEC code = deserialize<NETWORK_ORDER>(field.get(),subspan);
            
            if (code!=SerializationEC::NONE) {
                return code;
            }

            offset += serial_size(field.get());
            return SerializationEC::NONE;
        };

        return (((result_code = deserialize_field(args))==SerializationEC::NONE) && ...);
    }
}