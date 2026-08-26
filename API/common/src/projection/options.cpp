#include "common/projection/AbstractProjection.h"
#include "common/projection/options.h"
#include "common/projection/albers.h"
#include "common/projection/cylindric.h"
#include "common/projection/equidistconic.h"
#include "common/projection/gauss-kruger.h"
#include "common/projection/gnomonic.h"
#include "common/projection/lambert.h"
#include "common/projection/mercator.h"
#include "common/projection/miller.h"
#include "common/projection/spacial.h"
#include "common/projection/UTM.h"
#include <flat_map>
#include <functional>
#include <memory>
#include <string_view>

namespace projection{
    struct CaseInsensitiveCompare {
        using is_transparent = std::true_type;
        bool operator()(std::string_view lhs, std::string_view rhs) const {
            size_t min_sz = std::min(lhs.size(), rhs.size());
            for (size_t i = 0; i < min_sz; ++i) {
                unsigned char l = std::tolower(static_cast<unsigned char>(lhs[i]));
                unsigned char r = std::tolower(static_cast<unsigned char>(rhs[i]));
                if (l != r)
                    return l < r;
            }
            return lhs.size() < rhs.size();
        }
    };

    struct CaseInsensitiveHash{
        size_t operator()(std::string_view string) const{
            size_t result = 0;
            for(char ch:string)
                result = result * 31 + tolower(static_cast<unsigned char>(ch));
            return result;
        }
    };

    struct CaseInsensitiveEqual{
        bool operator()(std::string_view lhs,std::string_view rhs) const{
            if(lhs.size()!=rhs.size())
                return false;
            for(size_t i=0;i<lhs.size();++i)
                if(std::tolower(static_cast<unsigned char>(lhs[i]))!=
                    std::tolower(static_cast<unsigned char>(rhs[i])))
                    return false;
            return true;
        }
    };

    std::flat_map<std::string,
        std::function<std::unique_ptr<AbstractProjection>()>,
        CaseInsensitiveCompare>& projections(){
        static std::flat_map<std::string,
            std::function<std::unique_ptr<AbstractProjection>()>,
            CaseInsensitiveCompare> base_projections=
            [](){
                auto register_internal = [](
                    std::string name,
                    std::function<std::unique_ptr<AbstractProjection>()> function){
                        std::transform(name.begin(),name.end(),name.begin(),
                            [](unsigned char c){ return std::tolower(c); });
                        base_projections[std::move(name)] = std::move(function);
                };
                std::flat_map<std::string,
                    std::function<std::unique_ptr<AbstractProjection>()>,
                    CaseInsensitiveCompare> result;
                    register_internal("albers conic",
                        [](){return std::make_unique<Albers>();});
                    register_internal("equidistant conic",
                        [](){return std::make_unique<EquidistConic>();});
                    register_internal("cylindric",
                        [](){return std::make_unique<Cylindric>();});
                    register_internal("gauss-kruger",
                        [](){return std::make_unique<GaussKruger>();});
                return result;
            }();
        return base_projections;
    }

    std::error_code register_projection(
                std::string name,
                std::function<std::unique_ptr<AbstractProjection>()> function) noexcept{

        if(name.empty())
            return std::make_error_code(std::errc::invalid_argument);
        if(!projections().contains(name)){
            std::transform(name.begin(),name.end(),name.begin(),
                [](unsigned char c){ return std::tolower(c); });
            projections()[std::move(name)] = std::move(function);
            return {};
        }
        else return std::make_error_code(std::errc::invalid_argument);
    }

    bool contains(std::string_view projection_name) noexcept{
        if(auto found = projections().find(
                projection_name);found!=projections().end())
            return true;
        else return false;
    }

    std::unique_ptr<AbstractProjection> make_projection(std::string_view name) noexcept{
        if(auto found = projections().find(name);
            found!=projections().end()){
            auto result = found->second();
            result->name(found->first);
            return result;
        }
        return {};
    }
}