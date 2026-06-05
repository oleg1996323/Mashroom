#pragma once
#include <filesystem>
#include <variant>
#include <memory>
#include "variant.h"
#include "types/time_interval.h"
#include "network/definitions.h"

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
#include "macros.h"
ENABLE_DERIVED_VARIANT(path::Additional_t,path::ADDITIONAL);

namespace path{
template<bool VIEW>
struct Storage{
    Storage() = default;
    private:
    explicit Storage(const std::string& path,TYPE type):path_(path),type_(type){}
    explicit Storage(std::string&& path,TYPE type):path_(path),type_(type){}
    explicit Storage(std::string_view path,TYPE type):path_(path),type_(type){}
    public:
    template<bool VIEW_OTHER>
    Storage(const Storage<VIEW_OTHER>& other):
    path_(other.path_),type_(other.type_){
        add_ = other.add_;
    }
    template<bool VIEW_OTHER>
    Storage(Storage<VIEW_OTHER>&& other):
    path_(other.path_),type_(other.type_),add_(std::move(other.add_)){}
    Additional_t add_;
    std::conditional_t<VIEW,std::string_view,std::string> path_;
    TYPE type_;

    template<bool VIEW_OTHER>
    Storage<VIEW>& operator=(const Storage<VIEW_OTHER>& other) noexcept{
        if(this!=&other){
            path_ = other.path_;
            type_ = other.type_;
            add_ = other.add_;
        }
    }
    template<bool VIEW_OTHER>
    Storage<VIEW>& operator=(Storage<VIEW_OTHER>&& other) noexcept{
        if(this!=&other){
            if constexpr(VIEW_OTHER==VIEW && VIEW==false)
                path_ = std::move(other.path_);
            else path_.swap(other.path_);
            type_ = other.type_;
            add_ = std::move(other.add_);
        }
    }
    
    static Storage<VIEW> directory(const std::string& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Storage<VIEW>(path,path::TYPE::DIRECTORY);
        strg.add_=std::move(Additional<TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Storage<VIEW> file(const std::string& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Storage<VIEW>(path,path::TYPE::FILE);
        strg.add_=std::move(Additional<TYPE::FILE>(last_check));
        return strg;
    }
    static Storage<VIEW> host(const std::string& host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Storage<VIEW>(host,path::TYPE::HOST);
        strg.add_=std::move(Additional<TYPE::HOST>(port,last_check));
        return strg;
    }
    static Storage<VIEW> directory(std::string&& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = Storage<VIEW>(std::move(path),path::TYPE::DIRECTORY);
        strg.add_=std::move(Additional<TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Storage<VIEW> file(std::string&& path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Storage<VIEW>(std::move(path),path::TYPE::FILE));
        strg.add_=std::move(Additional<TYPE::FILE>(last_check));
        return strg;
    }
    static Storage<VIEW> host(std::string&& host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Storage<VIEW>(std::move(host),path::TYPE::HOST));
        strg.add_=std::move(Additional<TYPE::HOST>(port,last_check));
        return strg;
    }
    static Storage<VIEW> directory(std::string_view path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Storage<VIEW>(path,path::TYPE::DIRECTORY));
        strg.add_=std::move(Additional<TYPE::DIRECTORY>(last_check));
        return strg;
    }
    static Storage<VIEW> file(std::string_view path, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Storage<VIEW>(path,path::TYPE::FILE));
        strg.add_=std::move(Additional<TYPE::FILE>(last_check));
        return strg;
    }
    static Storage<VIEW> host(std::string_view host, uint16_t port, utc_tp last_check = std::chrono::system_clock::now()) noexcept{
        auto strg = std::move(Storage<VIEW>(host,path::TYPE::HOST));
        strg.add_=std::move(Additional<TYPE::HOST>(port,last_check));
        return strg;
    }

    template<bool VIEW_OTHER>
    bool operator==(const path::Storage<VIEW_OTHER>& other) const noexcept{
        if(static_cast<const void*>(&other)==static_cast<const void*>(this))
            return true;
        return this->path_==other.path_ && this->type_==other.type_;
    }

    template<bool VIEW_OTHER>
    bool operator!=(const path::Storage<VIEW_OTHER>& other) const noexcept{
        if(static_cast<const void*>(&other)==static_cast<const void*>(this))
            return false;
        return !(*this==other);
    }
};
}

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <regex>
#include "network/address.h"

template<>
path::Storage<false> boost::lexical_cast(const std::string& str){
    using namespace std::string_literals;
    static std::regex regex_type("^(dir:|file:|host:).+$",
        std::regex_constants::ECMAScript|
        std::regex_constants::icase);
    path::TYPE type;
    std::smatch match;
    if (std::regex_match(str,match,regex_type)) {
        if(match[1]=="dir:")
            type=path::TYPE::DIRECTORY;
        else if(match[1]=="file:")
            type=path::TYPE::FILE;
        else if(match[1]=="host:")
            type=path::TYPE::HOST;
        else throw std::runtime_error("unknown input");
    }
    if(type==path::TYPE::HOST){
        static const std::regex regex_val("^host:([0-9.]+):([0-9]{1,5})$");
        if (std::regex_search(str.begin(),str.end(), match, regex_val)) {
            std::string host;
            uint64_t port;
            host=match[1].str();
            std::string tmp = match[2].str();
            if(std::from_chars(tmp.data(),tmp.data()+tmp.size(),port).ec==std::errc() && 
                port<=static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
            {
                std::error_code err;
                auto addr = network::make_address(host,port,err);
                if(addr.valid())
                    return path::Storage<false>::host(host,port,std::chrono::system_clock::now());
                else throw std::runtime_error("invalid host/port input");
            }
            else throw std::runtime_error("invalid option value \"port\"");
        }
        else throw std::runtime_error("invalid host input");
    }
    else{
        static const std::regex regex_val("^dir:(.+)|file:(.+)$");
        if (std::regex_match(str.begin(),str.end(), match, regex_val)) {
            std::string path=match[1].str();
            if(path.starts_with("~"s+fs::path::preferred_separator))
                path = std::string(getenv("HOME"))+fs::path::preferred_separator+path.substr(2);
            if(type==path::TYPE::FILE)
                return path::Storage<false>::file(
                    path,std::chrono::system_clock::now());
            else return path::Storage<false>::directory(
                    path,std::chrono::system_clock::now());
        }
        else throw std::runtime_error("invalid input");
    }
}

template<bool VIEW>
std::string boost::lexical_cast(const path::Storage<VIEW>& input){
    std::string result;
    switch (input.type_)
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
    result+=input.path_;
    if(input.type_==path::TYPE::HOST)
        result+=" port:"+std::get<path::Additional<path::TYPE::HOST>>(input.add_);
    return result;
}

template<bool VIEW>
std::ostream& operator<<(std::ostream& stream,const path::Storage<VIEW> path){
    stream<<boost::lexical_cast(path);
    return stream;
}

template<bool VIEW>
struct std::hash<path::Storage<VIEW>>{
    using is_transparent = std::true_type;
    size_t operator()(const path::Storage<VIEW>& path) const{
        return std::hash<std::conditional_t<VIEW,std::string_view,std::string>>{}(path.path_)^std::hash<size_t>{}(size_t(path.type_));
    }
    size_t operator()(const path::Storage<!VIEW>& path) const{
        return std::hash<std::conditional_t<!VIEW,std::string_view,std::string>>{}(path.path_)^std::hash<size_t>{}(size_t(path.type_));
    }
};
template<bool VIEW>
struct std::equal_to<path::Storage<VIEW>>{
    using is_transparent = std::true_type;
    bool operator()(const path::Storage<VIEW>& lhs,const path::Storage<VIEW>& rhs) const{
        return lhs.path_==rhs.path_ && lhs.type_==rhs.type_;
    }
    bool operator()(const path::Storage<!VIEW>& lhs,const path::Storage<VIEW>& rhs) const{
        return lhs.path_==rhs.path_ && lhs.type_==rhs.type_;
    }
    bool operator()(const path::Storage<VIEW>& lhs,const path::Storage<!VIEW>& rhs) const{
        return lhs.path_==rhs.path_ && lhs.type_==rhs.type_;
    }
};

template<bool VIEW>
struct std::less<path::Storage<VIEW>>{
    using is_transparent = std::true_type;
    bool operator()(const path::Storage<VIEW>& lhs,const path::Storage<VIEW>& rhs) const{
        return std::hash<path::Storage<VIEW>>()(lhs)<std::hash<path::Storage<VIEW>>()(lhs);
    }
    bool operator()(const path::Storage<!VIEW>& lhs,const path::Storage<VIEW>& rhs) const{
        return std::hash<path::Storage<!VIEW>>()(lhs)<std::hash<path::Storage<VIEW>>()(lhs);
    }
    bool operator()(const path::Storage<VIEW>& lhs,const path::Storage<!VIEW>& rhs) const{
        return std::hash<path::Storage<VIEW>>()(lhs)<std::hash<path::Storage<!VIEW>>()(lhs);
    }
};

template<bool VIEW1,bool VIEW2>
bool operator==(const path::Storage<VIEW1>& lhs, const path::Storage<VIEW2>& rhs) noexcept{
    return lhs.path_==rhs.path_ && lhs.type_==rhs.type_;
}

DEFINE_SMART_POINTER_HASH_METHODS(path::Storage<false>);

#include "serialization.h"

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
    struct Serialize<NETWORK_ORDER,path::Storage<VIEW>>{
        using type = path::Storage<VIEW>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.type_,msg.path_,msg.add_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,path::Storage<false>>{
        using type = path::Storage<false>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.type_,msg.path_,msg.add_);
        }
    };

    template<bool VIEW>
    struct Serial_size<path::Storage<VIEW>>{
        using type = path::Storage<VIEW>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.type_,msg.path_,msg.add_);
        }
    };

    template<bool VIEW>
    struct Min_serial_size<path::Storage<VIEW>>{
        using type = path::Storage<VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::type_),decltype(type::path_),decltype(type::add_)>();
        }();
    };

    template<bool VIEW>
    struct Max_serial_size<path::Storage<VIEW>>{
        using type = path::Storage<VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::type_),decltype(type::path_),decltype(type::add_)>();
        }();
    };
}