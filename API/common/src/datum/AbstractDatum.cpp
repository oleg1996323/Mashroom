#include "common/datum/AbstractDatum.h"
#include <flat_set>
#include "common/datum/krasovski.h"
#include "common/datum/gsk2011.h"
#include "common/datum/itrf2008_2014.h"
#include "common/datum/pz9011.h"
#include "common/datum/wgs84.h"
#include "common/datum/CGMS.h"

namespace datum{
    struct CaseInsensitiveCompare {
        using is_transparent = std::true_type;
        bool operator()(std::shared_ptr<AbstractDatum> lhs, 
                std::shared_ptr<AbstractDatum> rhs) const {
            if(lhs && rhs)
                return operator()(lhs->name(),rhs->name());
            else{
                if(lhs)
                    return false;
                else if(rhs)
                    return true;
                else return false;
            }
        }
        bool operator()(std::shared_ptr<AbstractDatum> lhs, 
                std::string_view rhs) const {
            if(lhs)
                return operator()(lhs->name(),rhs);
            else return false;
        }
        bool operator()(std::string_view lhs, 
                std::shared_ptr<AbstractDatum> rhs) const {
            if(rhs)
                return operator()(lhs,rhs->name());
            else return false;
        }
        bool operator()(std::string_view lhs, 
                std::string_view rhs) const {
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
    std::flat_set<std::shared_ptr<AbstractDatum>,CaseInsensitiveCompare> datums_ ={
        std::make_shared<Krasovsky>(),
        std::make_shared<WGS84>(),
        std::make_shared<GSK2011>(),
        std::make_shared<PZ9011>(),
        std::make_shared<ITRF2008_2014>()
    };

    std::error_code register_datum(std::string name, double a,double f,type t) noexcept{
        if(name.empty())
            return std::make_error_code(std::errc::invalid_argument);
        if(auto found = datums_.find(name);found==datums_.end()){
            switch (t)
            {
            case type::spheric:
                datums_.insert(std::make_shared<SphericDatum>(std::move(name),a));
                break;
            case type::elliptic:
                datums_.insert(std::make_shared<EllipticDatum>(std::move(name),a,f));
                break;
            case type::geoid:
                datums_.insert(std::make_shared<GeoidDatum>(std::move(name),a,f));
                break;
            default:
                return std::make_error_code(std::errc::invalid_argument);
                break;
            }
            return {};
        }
        else return std::make_error_code(std::errc::invalid_argument);
    }

    std::shared_ptr<AbstractDatum> get_datum(std::string_view name) noexcept{
        if(name.empty())
            return {};
        else{
            if(auto found = datums_.find(name);found!=datums_.end())
                return *found;
            else return {};
        }
    }
}