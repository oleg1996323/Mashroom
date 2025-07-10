#pragma once
#include <filesystem>

namespace fs = std::filesystem;
namespace path{
enum class TYPE:uint8_t{
    DIRECTORY,
    FILE,
    HOST
};
template<bool VIEW>
struct Storage;
template<>
struct Storage<false>{
    explicit Storage(const std::string& path,TYPE type);
    explicit Storage(std::string&& path,TYPE type);
    explicit Storage(std::string_view path,TYPE type);
    Storage(std::initializer_list<Storage<false>> list);
    Storage(const Storage<false>& other);
    Storage(const Storage<true>& other);
    std::string path_;
    TYPE type_;
};
template<>
struct Storage<true>{
    explicit Storage(const std::string& path,TYPE type);
    explicit Storage(std::string&& path,TYPE type);
    explicit Storage(std::string_view path,TYPE type);
    Storage(std::initializer_list<Storage<true>> list);
    Storage(const Storage<false>& other);
    Storage(const Storage<true>& other);
    std::string_view path_;
    TYPE type_;
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
