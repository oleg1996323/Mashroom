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

namespace api{

using RepresentationType = uint8_t;
using Organization = uint8_t;

std::flat_set<std::shared_ptr<
                projection::CommonOptions>,
            std::less<projection::CommonOptions>>
    projections_options;

std::flat_map<std::string_view,
    std::flat_set<std::shared_ptr<
                projection::CommonOptions>>>
    projections_options_by_name;

std::flat_map<int64_t,
    std::flat_set<std::shared_ptr<
                projection::CommonOptions>>> 
    projections_by_api;

std::flat_map<std::chrono::sys_seconds,
    std::flat_set<std::shared_ptr<
            projection::CommonOptions>,
            std::less<projection::CommonOptions>>> 
    projections_by_time_upd;

#ifdef GRIB1API
namespace grib::v1{
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
            add_projections(const boost::json::object& proj)
    {
        std::shared_ptr<projection::CommonOptions> opt = 
            std::make_shared<projection::CommonOptions>();
        int64_t id;
        std::string name;
        int64_t org;
        bool stretched;
        bool rotated;
        std::chrono::sys_seconds tp;
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
                    std::unexpected(std::make_error_code(std::errc::invalid_argument));
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
        if(grib_options.contains({org,id,rotated,stretched})){
            std::cerr<<"Already contains "<<grib_options
                .at({org,id,rotated,stretched})->attribute("name")<<std::endl;
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        grib_options[{org,id,rotated,stretched}]=opt;
        projections_by_api[static_cast<int64_t>(API_T::GRIB1)].insert(opt);

        return opt;
    }
    std::expected<
        std::shared_ptr<projection::CommonOptions>,
        std::error_code> 
            add_projections(std::shared_ptr<projection::CommonOptions> proj)
    {
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
        if(grib_options.contains({org,id,rotated,stretched})){
            std::cerr<<"Already contains "<<grib_options
                .at({org,id,rotated,stretched})->attribute("name")<<std::endl;
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
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
                    grib_options[{org,id,rotated,stretched}]=proj;
                    projections_by_api[static_cast<int64_t>(API_T::GRIB1)].insert(proj);
                    projections_by_time_upd[std::chrono::sys_seconds(
                        std::chrono::seconds(proj->attribute("timepoint").as_int64()))].insert(proj);
                }
            }
        }
        else return std::unexpected(std::make_error_code(std::errc::invalid_argument));//already exists
        return proj;
    }
}
#endif

std::error_code add_projection(
    std::shared_ptr<projection::CommonOptions> options)
{
    std::chrono::sys_seconds tp;
    if(!options)
        return std::make_error_code(std::errc::invalid_argument);
    if(auto& name_attr = options->attribute("name");!name_attr.is_string() ||
        options->attribute("name").as_string().empty())
    {
        return std::make_error_code(std::errc::invalid_argument);
    }
    else if(auto& API_attr = options->attribute("API");
            !name_attr.is_int64()){
        return std::make_error_code(std::errc::invalid_argument);
    }
    else if(auto& tp_res = options->attribute("timepoint");!tp_res.is_int64())
        return std::make_error_code(std::errc::invalid_argument);
    else {
        switch(static_cast<API_T>(API_attr.as_int64())){
            #ifdef GRIB1API
            case API_T::GRIB1:
                if(auto result = api::grib::v1::add_projections(options);
                        result)
                    return {};
                else return result.error();
            break;
            #endif
            case API_T::COMMON:
            default:
        }
        if(auto found = projections_options.find(options);found==projections_options.end()){
            if(auto inserted = projections_options.insert(options);!inserted.second){
                return std::make_error_code(std::errc::invalid_argument);
            }
            else{
                if(!projections_options_by_name[name_attr.as_string()].insert(std::move(options)).second){
                    projections_options.erase(inserted.first);
                    return std::make_error_code(std::errc::invalid_argument);
                }
                else{
                    projections_by_api[static_cast<int64_t>(API_T::GRIB1)].insert(options);
                    projections_by_time_upd[std::chrono::sys_seconds(
                        std::chrono::seconds(options->attribute("timepoint").as_int64()))].insert(options);
                }
            }
        }
        else return std::make_error_code(std::errc::invalid_argument);//already exists
    }
}

std::unique_ptr<projection::CommonOptions> projection_options(
        const std::string name,
        const std::string& format,
        bool rotatable,
        bool stretchable)
{

}

std::error_code define_from_json(const std::filesystem::path& file){
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
                        return;
                    else{
                        for(auto& proj:projections){
                            if(!proj.is_object())
                                continue;
                            else{
                                auto& proj_obj = proj.as_object();
                                if(proj_obj.empty())
                                    continue;
                                if(proj_obj.contains("API")){
                                    if(auto api_res = from_json<int64_t>(proj_obj.at("API"));
                                        api_res){
                                        switch(static_cast<API_T>(api_res.value())){
                                            #ifdef GRIB1API
                                            case API_T::GRIB1:
                                            grib::v1::add_projections(proj_obj);
                                            #endif
                                            default:
                                            std::cerr<<"undefined API id: "<<api_res.value()<<std::endl;
                                            continue;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return {};
}
}