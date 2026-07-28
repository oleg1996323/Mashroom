#pragma once
#include <optional>
#include <string>
#include <cstdint>
#include "common/api_types.h"
#include <vector>
#include <string_view>

class API{
    std::string_view name_;
    uint64_t version_;
    API_T software_id_;
    public:
    API(std::string_view name,
            uint64_t version,
            API_T software_id) noexcept:
        name_(std::move(name)),
        version_(version){}
    API(const API& other):
        name_(other.name_),
        version_(other.version_),
        software_id_(other.software_id_){}
    API(API&& other):
        name_(std::move(other.name_)),
        version_(other.version_),
        software_id_(other.software_id_)
    {
        other.software_id_=static_cast<API_T>(-1);
        other.version_=0;
    }
    API& operator=(const API& other){
        if(this!=&other){
            name_ = other.name_;
            version_ = other.version_;
            software_id_ = other.software_id_;
        }
        return *this;
    }
    API& operator=(API&& other) noexcept{
        if(this!=&other){
            name_ = other.name_;
            version_ = other.version_;
            software_id_ = other.software_id_;
        }
        return *this;
    }
    API_T type() noexcept{
        return software_id_;
    }
    static API_T define_type_from_data(const char* buffer) noexcept;
    static std::optional<API_T> type_by_name(std::string_view api_name) noexcept;
    static std::string_view name_by_type(API_T type) noexcept;
    static const API& type_options(API_T type) noexcept;
    static const std::vector<std::string_view>& accessible() noexcept;
};