#include "common/EarthProjection.h"
#include "common/projections/albers.h"
#include "common/projections/conical.h"
#include "common/projections/cylindric.h"
#include <functional>
#include <flat_map>

using namespace std::string_literals;

namespace projection{
    std::array<std::string_view,10> names={
        "albers",
        "cylindric",
        "conical",
        "polyconic",
        "lambert",
        "gauss",
        "harmonic"
    };
}

struct CaseInsensitiveCompare {
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

template<typename PROJ>
std::unique_ptr<
    AbstractProjection> make_projection(std::string_view name)
{
    static_assert(std::is_base_of_v<AbstractProjection,PROJ>);
    PROJ proj(name);
    return std::make_unique<PROJ>(std::move(proj));
}

std::flat_map<std::string_view,
    std::function<
        std::unique_ptr<
            AbstractProjection>()>,
            CaseInsensitiveCompare> projections=
{
    {projection::names[0],
        [](){
            return make_projection<projection::Albers>(projection::names[0]);
        }
    },
    {projection::names[1],
        [](){
            return make_projection<projection::Cylindric>(projection::names[1]);
        }
    },
    {projection::names[2],
        [](){
            return make_projection<projection::Conical>(projection::names[2]);
        }
    }
};

namespace projection{
    std::unique_ptr<AbstractProjection> make_from_grib1(const char* buffer, 
                    const std::string& proj_name) noexcept
    {
        if(auto found = projections.find(proj_name);found!=projections.end()){
            
        }
    }
}

std::unique_ptr<AbstractProjection> make_projection(std::string_view projection) noexcept{
    if(auto found = projections.find(projection);found!=projections.end())
        return found->second();
    else return {};
}