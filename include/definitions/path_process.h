#pragma once
#include <filesystem>
#include <variant>
#include <memory>
#include "variant.h"
#include "types/time_interval.h"
#include "definitions.h"

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
            type_ = other.path_;
            add_ = other.add_;
        }
    }
    template<bool VIEW_OTHER>
    Storage<VIEW>& operator=(Storage<VIEW_OTHER>&& other) noexcept{
        if(this!=&other){
            if constexpr(VIEW_OTHER==VIEW && VIEW==false)
                path_ = std::move(other.path_);
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
        return this->path_==other.path_ && this->type_==other.type_ && (add_.index() == other.add_.index())?(add_ == other.add_):false;
    }

    template<bool VIEW_OTHER>
    bool operator!=(const path::Storage<VIEW_OTHER>& other) const noexcept{
        if(static_cast<const void*>(&other)==static_cast<const void*>(this))
            return false;
        return this->path_!=other.path_ || this->type_!=other.type_;
    }
};
}

template<bool VIEW>
std::ostream& operator<<(std::ostream& stream,const path::Storage<VIEW> path){
    switch (path.type_)
    {
        case path::TYPE::FILE:
            stream<<"file: ";
            break;
        case path::TYPE::DIRECTORY:
            stream<<"dir: ";
            break;
        case path::TYPE::HOST:
            stream<<"host: ";
            break;
        default:
            break;
    }
    stream<<path.path_;
    stream.flush();
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

template<bool VIEW1,bool VIEW2>
bool operator==(const path::Storage<VIEW1>& lhs, const path::Storage<VIEW2>& rhs) noexcept{
    return lhs.path_==rhs.path_ && lhs.type_==rhs.type_;
}

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
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
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
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
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
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
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
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
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