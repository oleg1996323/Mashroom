#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include "common/api_types.h"
#include <memory>

namespace projection{
    class CommonOptions{
        protected:
        static std::string empty_attr_;
        private:
        uint32_t id_;
        public:
        CommonOptions(uint32_t id) noexcept:
        id_(id){}

        CommonOptions(const CommonOptions& other):
        id_(other.id_){}

        CommonOptions(CommonOptions&& other) noexcept:
        id_(other.id_){}
        CommonOptions& operator=(const CommonOptions& other){
            if(this!=&other){
                id_=other.id_;;
            }
            return *this;
        }
        CommonOptions& operator=(CommonOptions&& other) noexcept{
            if(this!=&other){
                std::swap(id_,other.id_);
            }
            return *this;
        }
        uint32_t grid_id() const noexcept{
            return id_;
        }
        virtual const std::string& attribute(std::string_view)const noexcept{
            return empty_attr_;
        }
        virtual size_t hash() const noexcept{
            return std::hash<uint32_t>()(id_);
        }
        bool operator==(const CommonOptions& other) const noexcept{
            return id_==other.id_;
        }
    };

    class APIOptions:public CommonOptions{
        API_T api_t;
        public:
        APIOptions(API_T api,
            uint32_t id):
            CommonOptions(id),
            api_t(api){}
    };

    template<API_T T>
    class Options;

    #ifdef GRIB1API
    template<>
    class Options<API_T::GRIB1>:public APIOptions{
        std::string center_;
        public:
        Options(
            std::string center_name,
            uint32_t id) noexcept:
        APIOptions(API_T::GRIB1,id),
        center_(std::move(center_name)){}
        virtual const std::string& attribute(std::string_view name)const noexcept override{
            if(name=="center")
                return center_;
            else return empty_attr_;
        }
        virtual size_t hash() const noexcept{
            return std::hash<uint32_t>()(grid_id())<<
                    (sizeof(size_t)-sizeof(grid_id()))^
                    std::hash<std::string>()(center_);
        }
        bool operator==(const Options& other) const noexcept{
            return center_==other.center_;
        }
    };
    #endif
};

template<>
struct std::hash<projection::CommonOptions>{
    size_t operator()(const projection::CommonOptions& opt) const noexcept{
        return opt.hash();
    }
};

template<>
struct std::hash<std::unique_ptr<projection::CommonOptions>>{
    using is_transparent = std::true_type;
    size_t operator()(const std::unique_ptr<projection::CommonOptions>& opt) const noexcept{
        if(opt)
            return opt->hash();
        else return 0;
    }
};

template<>
struct std::equal_to<std::unique_ptr<projection::CommonOptions>>{
    using is_transparent = std::true_type;
    bool operator()(const std::unique_ptr<projection::CommonOptions>& lhs,
            const std::unique_ptr<projection::CommonOptions>& rhs) const{
        if((lhs && rhs))
            return *lhs==*rhs;
        else if(!lhs && !rhs)
            return true;
        else return false;
    }
};
