#include "common/projections/definitions.h"
#include <unordered_map>
#include <unordered_set>
#include <flat_set>
#include <flat_map>
#include <set>
#include <string>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace projection{

std::flat_set<std::shared_ptr<
                projection::CommonOptions>,
            std::less<projection::CommonOptions>>
    projections_options;

std::flat_map<std::string_view,
    std::flat_set<std::shared_ptr<
                projection::CommonOptions>>>
    projections_options_by_name;



std::flat_map<std::chrono::sys_seconds,
    std::flat_set<std::shared_ptr<
            projection::CommonOptions>,
            std::less<projection::CommonOptions>>> 
    projections_by_time_upd;

std::flat_map<const boost::json::value&,
    std::shared_ptr<
            projection::CommonOptions>,
            std::less<projection::CommonOptions>> 
    projections_by_attributes;
}

namespace api{
    std::flat_map<int64_t,
    std::flat_set<std::shared_ptr<
                projection::CommonOptions>>> 
    projections_by_api;
#if defined(GRIB1API) || defined(GRIB2API) || defined(GRIB3API)
namespace grib{
    namespace common{
        using common_result_t = std::tuple<std::chrono::sys_seconds,
                std::tuple<
                    Organization,
                    RepresentationType,
                    bool,
                    bool>>;
        std::expected<
            common_result_t,
            std::error_code> 
                add_projection_options(std::shared_ptr<projection::CommonOptions> proj)
        {
            using namespace projection;
            if( !proj || !proj->contains("API")||
                !proj->attribute("API").is_int64()
                #ifdef GRIB1API
                || !proj->attribute("API").as_int64()!=static_cast<int64_t>(API_T::GRIB1)
                #endif
                #ifdef GRIB2API
                || !proj->attribute("API").as_int64()!=static_cast<int64_t>(API_T::GRIB2)
                #endif
                #ifdef GRIB3API
                || !proj->attribute("API").as_int64()!=static_cast<int64_t>(API_T::GRIB3)
                #endif
            )
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            int64_t api = proj->attribute("API").as_int64();
            int64_t id;
            std::string name;
            int64_t org;
            bool stretched;
            bool rotated;
            std::chrono::sys_seconds tp;
            if(!proj->contains("ID") || !proj->attribute("ID").is_int64())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                id = proj->attribute("ID").as_int64();
            }
            if(!proj->contains("name") || !proj->attribute("name").is_string())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(name.empty())
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(!proj->contains("timepoint") || !proj->attribute("timepoint").is_int64())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                tp=std::chrono::sys_seconds(std::chrono::seconds(proj->attribute("timepoint").as_int64()));
            }
            if(!proj->contains("rotated") || !proj->attribute("rotated").is_bool())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto rot = from_json<bool>(proj->attribute("rotated"));rot){
                    rotated = rot.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(!proj->contains("stretched") || !proj->attribute("stretched").is_bool())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto stretch = from_json<bool>(proj->attribute("stretched"));stretch){
                    stretched = stretch.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(!proj->contains("center") || !proj->attribute("center").is_int64())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto center = from_json<int64_t>(proj->attribute("center"));center){
                    org = center.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(auto found = projections_options.find(proj);found==projections_options.end()){
                if(auto inserted = projections_options.insert(proj);!inserted.second){
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                else{
                    if(!projections_options_by_name[proj->attribute("name").as_string()].
                        insert(proj).second){
                        projections_options.erase(inserted.first);
                        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    }
                    else{
                        projections_by_api[api].insert(proj);
                        projections_by_time_upd[std::chrono::sys_seconds(
                            std::chrono::seconds(proj->attribute("timepoint").as_int64()))].insert(proj);
                        projections_by_attributes[proj->attributes()]=proj;
                    }
                    return common_result_t{tp,{org,id,rotated,stretched}};
                }
            }
            else return std::unexpected(std::make_error_code(std::errc::invalid_argument));//already exists
        }

        std::expected<
            std::shared_ptr<projection::CommonOptions>,
            std::error_code> 
                add_projection_options(const boost::json::object& proj)
        {
            std::shared_ptr<projection::CommonOptions> opt = 
                std::make_shared<projection::CommonOptions>();
            int64_t api = -1;
            int64_t id = 0;
            std::string name;
            int64_t org = 0;
            bool stretched = false;
            bool rotated = false;
            std::chrono::sys_seconds tp;
            if(!proj.contains("API"))
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                switch (proj.at("API").as_int64())
                {
                #ifdef GRIB1API
                case static_cast<int64_t>(API_T::GRIB1):
                #endif
                #ifdef GRIB2API
                case static_cast<int64_t>(API_T::GRIB2):
                #endif
                #ifdef GRIB3API
                case static_cast<int64_t>(API_T::GRIB3):
                #endif
                    api = proj.at("API").as_int64();
                    break;
                default:
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    break;
                }
            }
            
            if(!proj.contains("ID") || !proj.at("ID").is_int64())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                id = proj.at("ID").as_int64();
                opt->add_attribute("ID",id);
            }
            if(!proj.contains("name") || !proj.at("name").is_string())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                name = proj.at("name").as_string();
                if(name.empty())
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else opt->add_attribute("name",name);
            }
            if(!proj.contains("timepoint") || !proj.at("timepoint").is_string())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                auto& timepoint = proj.at("timepoint").as_string();
                if(timepoint.empty())
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else {
                    std::istringstream iss(timepoint.c_str());
                    iss>>std::chrono::parse("%Y-%m-%dT%H:%M:%SZ",tp);
                    if(iss.fail())
                        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    else opt->add_attribute("timepoint",tp);
                }
            }
            if(!proj.contains("rotated") || !proj.at("rotated").is_bool())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto rot = from_json<bool>(proj.at("rotated"));rot){
                    opt->add_attribute("rotated",rot.value());
                    rotated = rot.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(!proj.contains("stretched") || !proj.at("stretched").is_bool())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto stretch = from_json<bool>(proj.at("stretched"));stretch){
                    opt->add_attribute("stretched",stretch.value());
                    stretched = stretch.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if(!proj.contains("center") || !proj.at("center").is_int64())
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            else{
                if(auto center = from_json<int64_t>(proj.at("center"));center){
                    opt->add_attribute("center",center.value());
                    org = center.value();
                }
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            return opt;
        }
    }

    #ifdef GRIB1API
    namespace v1{
    std::flat_map<std::tuple<
            Organization,
            RepresentationType,
            bool,
            bool>,
        std::shared_ptr<
                projection::CommonOptions>> 
        grib_options;

        std::shared_ptr<projection::CommonOptions> get_grid(
                Organization center,
                RepresentationType id,
                bool rotated,
                bool stretched)
        {
            if(auto found_grid = grib_options.find({center,id,rotated,stretched});
                    found_grid!=grib_options.end())
                return found_grid->second;
            else return {};
        }

        std::expected<
            std::shared_ptr<projection::CommonOptions>,
            std::error_code> 
                add_projection_options(const boost::json::object& proj)
        {
            return grib::common::add_projection_options(proj);
        }
        std::expected<
            std::shared_ptr<projection::CommonOptions>,
            std::error_code> 
                add_projection_options(std::shared_ptr<projection::CommonOptions> proj)
        {
            auto common_result  = grib::common::add_projection_options(proj);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            const auto& [tp,mapped] = common_result.value();
            const auto& [id,org,rotated,stretched] = mapped;
            grib_options[{org,id,rotated,stretched}]=proj;
            return proj;
        }
    }
    #endif

    #ifdef GRIB2API
    namespace v2{
    std::flat_map<std::tuple<
            Organization,
            RepresentationType,
            bool,
            bool>,
        std::shared_ptr<
                projection::CommonOptions>> 
        grib_options;

        std::shared_ptr<projection::CommonOptions> get_grid(
                Organization center,
                RepresentationType id,
                bool rotated,
                bool stretched)
        {
            if(auto found_grid = grib_options.find({center,id,rotated,stretched});
                    found_grid!=grib_options.end())
                return found_grid->second;
            else return {};
        }

        std::expected<
            std::shared_ptr<projection::CommonOptions>,
            std::error_code> 
                add_projection_options(const boost::json::object& proj)
        {
            return grib::common::add_projection_options(proj);
        }
        std::expected<
            std::shared_ptr<projection::CommonOptions>,
            std::error_code> 
                add_projection_options(std::shared_ptr<projection::CommonOptions> proj)
        {
            auto common_result  = grib::common::add_projection_options(proj);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            const auto& [tp,mapped] = common_result.value();
            const auto& [id,org,rotated,stretched] = mapped;
            grib_options[{org,id,rotated,stretched}]=proj;
            return proj;
        }
    }
    #endif
}
#endif
}

namespace projection{
    std::expected<std::shared_ptr<projection::CommonOptions>,
        std::error_code> add_projection_options(
            const boost::json::object& options)
    {
        std::chrono::sys_seconds tp;
        if(options.empty())
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        if(!options.contains("name"))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        else if(auto name_result = from_json<std::string>(options.at("name"));!name_result ||
            name_result->empty())
        {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else if(!options.contains("API"))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        else if(auto API_result = from_json<int64_t>(options.at("API"));
                !API_result){
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else {
            switch(static_cast<API_T>(API_result.value())){
                #ifdef GRIB1API
                case API_T::GRIB1:
                    return api::grib::v1::add_projection_options(options);
                    break;
                #endif
                case API_T::COMMON:
                default:
                if(!options.contains("timepoint"))
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else if(auto tp_res = from_json<std::string>(options.at("timepoint"));!tp_res)
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else{
                    std::chrono::sys_seconds tp;
                    std::istringstream iss(tp_res->c_str());
                    iss>>std::chrono::parse("%Y-%m-%dT%H:%M:%SZ",tp);
                    if(iss.fail())
                        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    std::shared_ptr<projection::CommonOptions> result = 
                        std::make_shared<projection::CommonOptions>();
                    for(auto& [key,value]:options)
                        result->add_attribute(key,value);
                    if(auto found = projections_options.find(result);found==projections_options.end()){
                        if(auto inserted = projections_options.insert(result);!inserted.second){
                            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                        }
                        else{
                            if(!projections_options_by_name[name_result.value()].insert(result).second){
                                projections_options.erase(inserted.first);
                                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                            }
                            else{
                                api::projections_by_api[API_result.value()].insert(result);
                                projections_by_time_upd[tp].insert(result);
                                projections_by_attributes[result->attributes()]=result;
                            }
                        }
                    }
                    else return std::unexpected(std::make_error_code(std::errc::invalid_argument));//already exists
                }
            }
            
        }
    }

    std::expected<std::shared_ptr<projection::CommonOptions>,
        std::error_code> add_projection_options(
            std::shared_ptr<projection::CommonOptions> options)
    {
        std::chrono::sys_seconds tp;
        if(!options)
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        else if(auto name_result = from_json<std::string>(options->attribute("name"));!name_result ||
            name_result->empty())
        {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else if(auto API_result = from_json<int64_t>(options->attribute("API"));
                !API_result){
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else {
            switch(static_cast<API_T>(API_result.value())){
                #ifdef GRIB1API
                case API_T::GRIB1:
                    return api::grib::v1::add_projection_options(options);
                    break;
                #endif
                case API_T::COMMON:
                default:
                if(auto& tp_attr = options->attribute("timepoint");!tp_attr.is_int64())
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else{
                    if(auto found = projections_options.find(options);found==projections_options.end()){
                        if(auto inserted = projections_options.insert(options);!inserted.second){
                            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                        }
                        else{
                            if(!projections_options_by_name[name_result.value()].insert(options).second){
                                projections_options.erase(inserted.first);
                                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                            }
                            else{
                                api::projections_by_api[API_result.value()].insert(options);
                                projections_by_time_upd[tp].insert(options);
                                projections_by_attributes[options->attributes()]=options;
                            }
                        }
                    }
                    else return std::unexpected(std::make_error_code(std::errc::invalid_argument));//already exists
                }
            }
        }
    }

    std::shared_ptr<projection::CommonOptions> get_grid(
            const boost::json::object& attributes)
    {
        if(auto found = projections_by_attributes.find(attributes);
            found!=projections_by_attributes.end())
            return found->second;
        else {};
    }

    std::error_code add_projections_options_from_json(const std::filesystem::path& file){
        std::expected<boost::json::value, std::error_code>
            value = parse_json_from_file(file);
        if(!value)
            return value.error();
        else{
            const auto& json = value.value();
            if(json.is_object()){
                auto& obj1 = json.as_object();
                if(!obj1.contains("version")||
                    !obj1.contains("projections"))
                    return std::make_error_code(std::errc::invalid_argument);
                else{
                    //by version control
                    if(!obj1.at("projections").is_array())
                        return std::make_error_code(std::errc::invalid_argument);
                    else{
                        auto& projections = obj1.at("projections").as_array();
                        if(projections.empty())
                            return {};
                        else{
                            for(auto& proj:projections){
                                if(!proj.is_object())
                                    continue;
                                else{
                                    auto& proj_obj = proj.as_object();
                                    if(proj_obj.empty())
                                        continue;
                                    add_projection_options(proj_obj);
                                }
                            }
                            return {};
                        }
                    }
                }
            }
            else return std::make_error_code(std::errc::invalid_argument);
        }
        return {};
    }
}