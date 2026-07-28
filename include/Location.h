#pragma once
#include <filesystem>
#include <variant>
#include <memory>
#include "OsterLib/variant.h"
#include "OsterLib/types/time_interval.h"
#include "OsterLib/network/definitions.h"

namespace fs = std::filesystem;
namespace path{
enum class TYPE:uint8_t{
    DIRECTORY,
    FILE,
    HOST
};

template<path::TYPE T>
struct Additional;

template<>
struct Additional<TYPE::HOST>{
    utc_tp last_check_ = utc_tp();
    network::Port port_ = 0;
    Additional(uint16_t port = 0,utc_tp last_check = utc_tp()) noexcept:last_check_(last_check),port_(port){}
    Additional(const Additional&) noexcept = default;
    Additional(Additional&&) noexcept = default;
    Additional& operator=(const Additional& other) noexcept = default;
    Additional& operator=(Additional&& other) noexcept = default;
    bool operator==(const Additional& other) const noexcept{
        return last_check_==other.last_check_ && port_==other.port_;
    }
};

template<>
struct Additional<TYPE::DIRECTORY>{
    utc_tp last_check_ = utc_tp();
    Additional(utc_tp last_check = utc_tp()) noexcept:last_check_(last_check){}
    Additional(const Additional&) noexcept = default;
    Additional(Additional&&) noexcept = default;
    Additional& operator=(const Additional& other) noexcept = default;
    Additional& operator=(Additional&& other) noexcept = default;
    bool operator==(const Additional& other) const noexcept{
        return last_check_==other.last_check_;
    }
};

template<>
struct Additional<TYPE::FILE>{
    utc_tp last_check_ = utc_tp();
    Additional(utc_tp last_check = utc_tp()) noexcept:last_check_(last_check){}
    Additional(const Additional&) noexcept = default;
    Additional(Additional&&) noexcept = default;
    Additional& operator=(const Additional& other) noexcept = default;
    Additional& operator=(Additional&& other) noexcept = default;
    bool operator==(const Additional& other) const noexcept{
        return last_check_==other.last_check_;
    }
};

using ADDITIONAL = std::variant<std::monostate,Additional<TYPE::HOST>,Additional<TYPE::DIRECTORY>,Additional<TYPE::FILE>>;

class Additional_t:public ADDITIONAL{
    public:
    using ADDITIONAL::variant;
    template<path::TYPE T>
    bool is() const noexcept{
        return std::holds_alternative<Additional<T>>(*this);
    };
    template<path::TYPE T>
    Additional<T>& get() noexcept{
        return std::get<Additional<T>>(*this);
    }

    template<path::TYPE T>
    const Additional<T>& get() const noexcept{
        return std::get<Additional<T>>(*this);
    }
};
}
#include "OsterLib/macros.h"
ENABLE_DERIVED_VARIANT(path::Additional_t,path::ADDITIONAL);

template<bool VIEW>
struct Location{
    Location() = default;
    using path_t = std::conditional_t<VIEW,std::string_view,std::string>;
    private:
    explicit Location(const std::string& path,path::TYPE type):path_(path){}
    explicit Location(std::string&& path,path::TYPE type):path_(path){}
    explicit Location(std::string_view path,path::TYPE type):path_(path){}
    path::Additional_t add_;
    path_t path_;
    friend struct Location<!VIEW>;
    template<bool,bool>
    friend struct serialization::Serialize;
    template<bool,bool>
    friend struct serialization::Deserialize;
    template<bool>
    friend struct serialization::Serial_size;
    template<bool>
    friend struct serialization::Min_serial_size;
    template<bool>
    friend struct serialization::Max_serial_size;
    public:
    template<bool VIEW_OTHER>
    Location(const Location<VIEW_OTHER>& other):
    path_(other.path()){
        add_ = other.add_;
    }
    template<bool VIEW_OTHER>
    Location(Location<VIEW_OTHER>&& other):
    path_(other.path()),add_(std::move(other.add_)){}

    const path_t& path() const noexcept{
        return path_;
    }

    path::Additional_t additional() const noexcept{
        return add_;
    }
    
    template<bool VIEW_OTHER>
    Location<VIEW>& operator=(const Location<VIEW_OTHER>& other) noexcept{
        if(this!=&other){
            path_ = other.path();
            add_ = other.add_;
        }
    }
    template<bool VIEW_OTHER>
    Location<VIEW>& operator=(Location<VIEW_OTHER>&& other) noexcept{
        if(this!=&other){
            if constexpr(VIEW_OTHER==VIEW && VIEW==false)
                path_ = std::move(other.path());
            else path_.swap(other.path());
            add_ = std::move(other.add_);
        }
    }

    bool is_path() const noexcept{
        return !std::holds_alternative<std::monostate>(add_);
    }

    bool is_file() const noexcept{
        return type()==path::TYPE::FILE;
    }

    bool is_dir() const noexcept{
        return type()==path::TYPE::DIRECTORY;
    }

    bool is_host() const noexcept{
        return type()==path::TYPE::HOST;
    }

    path::TYPE type() const{
        auto visit_additional = [](const auto& add){
            if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(add)>>){
                assert(false);
                return static_cast<path::TYPE>(-1);
            }
            else{
                auto define_type = []<path::TYPE type>(const path::Additional<type>& t){
                    return type;
                };
                return define_type(add);
            }
        };
        return std::visit(visit_additional,add_);
    }
    
    static Location<VIEW> directory(const std::string& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Location<VIEW>(path,path::TYPE::DIRECTORY);
        strg.add_=std::move(path::Additional<path::TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Location<VIEW> file(const std::string& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Location<VIEW>(path,path::TYPE::FILE);
        strg.add_=std::move(path::Additional<path::TYPE::FILE>(last_check));
        return strg;
    }
    static Location<VIEW> host(const std::string& host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Location<VIEW>(host,path::TYPE::HOST);
        strg.add_=std::move(path::Additional<path::TYPE::HOST>(port,last_check));
        return strg;
    }
    static Location<VIEW> directory(std::string&& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Location<VIEW>(std::move(path),path::TYPE::DIRECTORY);
        strg.add_=std::move(path::Additional<path::TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Location<VIEW> file(std::string&& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Location<VIEW>(std::move(path),path::TYPE::FILE));
        strg.add_=std::move(path::Additional<path::TYPE::FILE>(last_check));
        return strg;
    }
    static Location<VIEW> host(std::string&& host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Location<VIEW>(std::move(host),path::TYPE::HOST));
        strg.add_=std::move(path::Additional<path::TYPE::HOST>(port,last_check));
        return strg;
    }
    static Location<VIEW> directory(std::string_view path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Location<VIEW>(path,path::TYPE::DIRECTORY));
        strg.add_=std::move(path::Additional<path::TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Location<VIEW> file(std::string_view path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Location<VIEW>(path,path::TYPE::FILE));
        strg.add_=std::move(path::Additional<path::TYPE::FILE>(last_check));
        return strg;
    }
    static Location<VIEW> host(std::string_view host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Location<VIEW>(host,path::TYPE::HOST));
        strg.add_=std::move(path::Additional<path::TYPE::HOST>(port,last_check));
        return strg;
    }

    template<bool VIEW_OTHER>
    bool operator==(const Location<VIEW_OTHER>& other) const noexcept{
        if(static_cast<const void*>(&other)==static_cast<const void*>(this))
            return true;
        return path_==other.path() && type()==other.type();
    }

    template<bool VIEW_OTHER>
    bool operator!=(const Location<VIEW_OTHER>& other) const noexcept{
        if(static_cast<const void*>(&other)==static_cast<const void*>(this))
            return false;
        return !(*this==other);
    }
};

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <regex>
#include "OsterLib/network/address.h"

namespace boost{
    template<>
    Location<false> lexical_cast(const std::string& str);

    template<bool VIEW>
    std::string boost::lexical_cast(const Location<VIEW>& input){
        std::string result;
        switch (input.type())
        {
            case path::TYPE::FILE:
                result+="file:";
                break;
            case path::TYPE::DIRECTORY:
                result+="dir:";
                break;
            case path::TYPE::HOST:
                result+="host:";
                break;
            default:
                break;
        }
        result+=input.path();
        if(input.type()==path::TYPE::HOST)
            result+=" port:"+std::get<path::Additional<path::TYPE::HOST>>(input.additional()).port_;
        return result;
    }
}

template<bool VIEW>
std::ostream& operator<<(std::ostream& stream,const Location<VIEW>& path){
    stream<<boost::lexical_cast(path);
    return stream;
}

template<bool VIEW>
struct std::hash<Location<VIEW>>{
    using is_transparent = std::true_type;
    size_t operator()(const Location<VIEW>& location) const{
        return std::hash<std::conditional_t<VIEW,std::string_view,std::string>>{}(location.path())^std::hash<size_t>{}(size_t(location.type()));
    }
    size_t operator()(const Location<!VIEW>& location) const{
        return std::hash<std::conditional_t<!VIEW,std::string_view,std::string>>{}(location.path())^std::hash<size_t>{}(size_t(location.type()));
    }
};
template<bool VIEW>
struct std::equal_to<Location<VIEW>>{
    using is_transparent = std::true_type;
    bool operator()(const Location<VIEW>& lhs,const Location<VIEW>& rhs) const{
        return lhs.path()==rhs.path() && lhs.type()==rhs.type();
    }
    bool operator()(const Location<!VIEW>& lhs,const Location<VIEW>& rhs) const{
        return lhs.path()==rhs.path() && lhs.type()==rhs.type();
    }
    bool operator()(const Location<VIEW>& lhs,const Location<!VIEW>& rhs) const{
        return lhs.path()==rhs.path() && lhs.type()==rhs.type();
    }
};

template<bool VIEW>
struct std::less<Location<VIEW>>{
    using is_transparent = std::true_type;
    bool operator()(const Location<VIEW>& lhs,const Location<VIEW>& rhs) const{
        return std::hash<Location<VIEW>>()(lhs)<std::hash<Location<VIEW>>()(lhs);
    }
    bool operator()(const Location<!VIEW>& lhs,const Location<VIEW>& rhs) const{
        return std::hash<Location<!VIEW>>()(lhs)<std::hash<Location<VIEW>>()(lhs);
    }
    bool operator()(const Location<VIEW>& lhs,const Location<!VIEW>& rhs) const{
        return std::hash<Location<VIEW>>()(lhs)<std::hash<Location<!VIEW>>()(lhs);
    }
};

template<bool VIEW1,bool VIEW2>
bool operator==(const Location<VIEW1>& lhs, const Location<VIEW2>& rhs) noexcept{
    return lhs.path()==rhs.path() && lhs.type()==rhs.type();
}

DEFINE_SMART_POINTER_HASH_METHODS(Location<false>);

#include "OsterLib/serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,path::Additional<path::TYPE::FILE>>{
        using type = path::Additional<path::TYPE::FILE>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.last_check_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,path::Additional<path::TYPE::FILE>>{
        using type = path::Additional<path::TYPE::FILE>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.last_check_);
        }
    };

    template<>
    struct Serial_size<path::Additional<path::TYPE::FILE>>{
        using type = path::Additional<path::TYPE::FILE>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.last_check_);
        }
    };

    template<>
    struct Min_serial_size<path::Additional<path::TYPE::FILE>>{
        using type = path::Additional<path::TYPE::FILE>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::last_check_)>();
        }();
    };

    template<>
    struct Max_serial_size<path::Additional<path::TYPE::FILE>>{
        using type = path::Additional<path::TYPE::FILE>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::last_check_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,path::Additional<path::TYPE::DIRECTORY>>{
        using type = path::Additional<path::TYPE::DIRECTORY>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.last_check_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,path::Additional<path::TYPE::DIRECTORY>>{
        using type = path::Additional<path::TYPE::DIRECTORY>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.last_check_);
        }
    };

    template<>
    struct Serial_size<path::Additional<path::TYPE::DIRECTORY>>{
        using type = path::Additional<path::TYPE::DIRECTORY>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.last_check_);
        }
    };

    template<>
    struct Min_serial_size<path::Additional<path::TYPE::DIRECTORY>>{
        using type = path::Additional<path::TYPE::DIRECTORY>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::last_check_)>();
        }();
    };

    template<>
    struct Max_serial_size<path::Additional<path::TYPE::DIRECTORY>>{
        using type = path::Additional<path::TYPE::DIRECTORY>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::last_check_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,path::Additional<path::TYPE::HOST>>{
        using type = path::Additional<path::TYPE::HOST>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.port_,msg.last_check_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,path::Additional<path::TYPE::HOST>>{
        using type = path::Additional<path::TYPE::HOST>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.port_,msg.last_check_);
        }
    };

    template<>
    struct Serial_size<path::Additional<path::TYPE::HOST>>{
        using type = path::Additional<path::TYPE::HOST>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.port_,msg.last_check_);
        }
    };

    template<>
    struct Min_serial_size<path::Additional<path::TYPE::HOST>>{
        using type = path::Additional<path::TYPE::HOST>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::port_),decltype(type::last_check_)>();
        }();
    };

    template<>
    struct Max_serial_size<path::Additional<path::TYPE::HOST>>{
        using type = path::Additional<path::TYPE::HOST>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::port_),decltype(type::last_check_)>();
        }();
    };

    template<bool NETWORK_ORDER, bool VIEW>
    struct Serialize<NETWORK_ORDER,Location<VIEW>>{
        using type = Location<VIEW>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.path_,msg.add_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,Location<false>>{
        using type = Location<false>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.path_,msg.add_);
        }
    };

    template<bool VIEW>
    struct Serial_size<Location<VIEW>>{
        using type = Location<VIEW>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.path_,msg.add_);
        }
    };

    template<bool VIEW>
    struct Min_serial_size<Location<VIEW>>{
        using type = Location<VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<typename type::path_t,decltype(type::add_)>();
        }();
    };

    template<bool VIEW>
    struct Max_serial_size<Location<VIEW>>{
        using type = Location<VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<typename type::path_t,decltype(type::add_)>();
        }();
    };
}

#include <CLI/CLI.hpp>

namespace CLI {
    namespace detail {
        template <>
        bool lexical_cast<Location<false>>(const std::string& input, Location<false>& output);
        template<bool VIEW>
        std::string to_string(const Location<VIEW>& input){
            return boost::lexical_cast<std::string>(input);
        }
    }
}