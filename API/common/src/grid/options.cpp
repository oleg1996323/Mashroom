#include "common/grid/options.h"
#include <flat_set>
#include <flat_map>
#include "common/projection/AbstractProjection.h"

namespace grid{

    namespace detail{
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

        struct OptionsAttributesCompare {
            using is_transparent = std::true_type;
            bool operator()(const boost::json::object& lhs,
                    const boost::json::object& rhs) const noexcept{
                assert(lhs.contains("name") && lhs.contains("timepoint") &&
                    lhs.contains("API") && lhs.contains("ID") && lhs.contains("REF"));
                if(rhs.contains("name") && lhs.at("name").as_string()<rhs.at("name").as_string())
                    return true;
                else if(!rhs.contains("name") || lhs.at("name").as_string()==rhs.at("name").as_string()){
                    if(rhs.contains("timepoint") && lhs.at("timepoint").as_int64()<rhs.at("timepoint").as_int64())
                        return true;
                    else if(!rhs.contains("timepoint") || lhs.at("timepoint").as_int64()==rhs.at("timepoint").as_int64()){
                        if(rhs.contains("API") && lhs.at("API").as_int64()<rhs.at("API").as_int64())
                            return true;
                        else if(!rhs.contains("API") || lhs.at("API").as_int64()==rhs.at("API").as_int64()){
                            if(rhs.contains("ID") && lhs.at("ID").as_int64()<rhs.at("ID").as_int64())
                                return true;
                            else if(!rhs.contains("ID") || lhs.at("ID").as_int64()==rhs.at("ID").as_int64()){
                                if(rhs.contains("REF") && lhs.at("REF").as_string()<rhs.at("REF").as_string())
                                    return true;
                                else return false;
                            }
                            else return false;
                        }
                        else return false;
                    }
                    else return false;
                }
                else return false;
            }
            bool operator()(const std::shared_ptr<CommonOptions>& lhs,
                    const boost::json::object& rhs) const noexcept{
                return (*this)(lhs->attributes(),rhs);
            }
            bool operator()(const boost::json::object& lhs,
                    const std::shared_ptr<CommonOptions>& rhs) const noexcept{
                return (*this)(lhs,rhs->attributes());
            }
            bool operator()(const std::shared_ptr<CommonOptions>& lhs,
                    const std::shared_ptr<CommonOptions>& rhs) const noexcept{
                return (*this)(lhs->attributes(),rhs->attributes());
            }
        };
    }

    std::flat_set<std::shared_ptr<
                grid::CommonOptions>,detail::OptionsAttributesCompare>
        projections_options;

    std::flat_map<std::string_view,
        std::flat_set<std::shared_ptr<
                    grid::CommonOptions>>,detail::CaseInsensitiveCompare>
        projections_options_by_name;

    std::flat_map<std::chrono::sys_seconds,
        std::flat_set<std::shared_ptr<
                grid::CommonOptions>,
                std::less<grid::CommonOptions>>> 
        projections_by_time_upd;

    std::flat_map<int64_t,
        std::flat_set<std::shared_ptr<
                    grid::CommonOptions>>> 
        projections_by_api;
    
    

    struct CommonAttributes{
        std::function<std::unique_ptr<AbstractProjection>(std::string_view)>* function_ = nullptr;
        std::string_view name_;
        int64_t id_;
        int64_t api_;
        std::chrono::sys_seconds timepoint_;

        virtual std::error_code extract(std::shared_ptr<CommonOptions> opt) noexcept{
            if(!is_correct_options(opt))
                return std::make_error_code(std::errc::invalid_argument);
            if(auto found = detail::base_projections.find(
                    opt->attribute("REF").as_string().c_str());found!=detail::base_projections.end())
                function_ = &found->second;
            else return std::make_error_code(std::errc::invalid_argument);
            name_ = opt->attribute("name").as_string();
            api_ = opt->attribute("API").as_int64();
            id_ = opt->attribute("ID").as_int64();
            timepoint_ = std::chrono::sys_seconds(
                    std::chrono::seconds(opt->attribute("timepoint").as_int64()));
            return {};
        }

        virtual std::error_code extract(const boost::json::object& attributes) noexcept{
            if(CommonAttributes::is_correct_attributes(attributes)){
                
                if(auto found = detail::base_projections.find(
                        attributes.at("REF").as_string().c_str());found!=detail::base_projections.end())
                    function_ = &found->second;
                else return std::make_error_code(std::errc::invalid_argument);
                name_ = attributes.at("name").as_string();
                api_ = attributes.at("API").as_int64();
                id_ = attributes.at("ID").as_int64();
                std::istringstream iss(attributes.at("timepoint").as_string().c_str());
                iss>>std::chrono::parse("%Y-%m-%dT%H:%M:%SZ",timepoint_);
                if(iss.fail())
                    return std::make_error_code(std::errc::invalid_argument);
                return {};
            }
            else{
                return std::make_error_code(std::errc::invalid_argument);
            }
        }
        virtual bool is_correct_attributes(const boost::json::object& attr) noexcept{
            if(!attr.contains("REF") || !attr.at("REF").is_string())
                return false;
            else{
                if(attr.at("REF").as_string().empty())
                    return false;
            }
            if(!attr.contains("name") || !attr.at("name").is_string())
                return false;
            else{
                if(attr.at("name").as_string().empty())
                    return false;
            }
            if(!attr.contains("API") || !attr.at("API").is_int64())
                return false;
            if(!attr.contains("ID") || !attr.at("ID").is_int64())
                return false;
            if(!attr.contains("timepoint") || !attr.at("timepoint").is_int64())
                return false;
            return true;
        }
        virtual bool is_correct_options(std::shared_ptr<grid::CommonOptions> proj) noexcept{
            if(!proj->contains("REF") || !proj->attribute("REF").is_string())
                return false;
            else{
                if(proj->attribute("REF").as_string().empty())
                    return false;
            }
            if(!proj->contains("name") || !proj->attribute("name").is_string())
                return false;
            else{
                if(proj->attribute("name").as_string().empty())
                    return false;
            }
            if(!proj->contains("API") || !proj->attribute("API").is_int64())
                return false;
            if(!proj->contains("ID") || !proj->attribute("ID").is_int64())
                return false;
            if(!proj->contains("timepoint") || !proj->attribute("timepoint").is_int64())
                return false;
            return true;
        }
    };

    struct AddResult{
        std::unique_ptr<CommonAttributes> attributes;
        std::shared_ptr<CommonOptions> options;
    };

    std::expected<CommonAttributes,std::error_code> get_common_attributes(std::shared_ptr<CommonOptions> proj) noexcept{
        if(!proj)
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        CommonAttributes result;
        if(result.extract(proj))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        return result;
    }
    std::expected<CommonAttributes,std::error_code> get_common_attributes(const boost::json::object proj) noexcept{
        CommonAttributes result;
        if(result.extract(proj))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        return result;
    }

    bool try_indexate(std::shared_ptr<CommonOptions> opt) noexcept{
        if(auto found = projections_options.find(opt);found==projections_options.end()){
            if(auto inserted = projections_options.insert(opt);!inserted.second){
                return false;
            }
            else{
                if(!projections_options_by_name[opt->attribute("name").as_string()].
                    insert(opt).second){
                    projections_options.erase(inserted.first);
                    return false;
                }
                else{
                    projections_by_api[opt->attribute("API").as_int64()].insert(opt);
                    projections_by_time_upd[std::chrono::sys_seconds(
                            std::chrono::seconds(opt->attribute("timepoint").as_int64()))].insert(opt);
                }
                return true;
            }
        }
    }

namespace api{
#if defined(GRIB1API) || defined(GRIB2API) || defined(GRIB3API)
namespace grib{
    struct CommonAttributes:grid::CommonAttributes{
        Organization org;
        bool stretched;
        bool rotated;
        std::error_code extract(const boost::json::object& attributes) noexcept override{
            if(auto err = grid::CommonAttributes::extract(attributes);err)
                return err;
            else {
                if(!is_correct_attributes(attributes))
                    return std::make_error_code(std::errc::invalid_argument);
                rotated = attributes.at("rotated").as_bool();
                stretched = attributes.at("stretched").as_bool();
                org = attributes.at("center").as_int64();
                return {};
            }
        }
        std::error_code extract(std::shared_ptr<grid::CommonOptions> proj) noexcept override{
            if(!proj)
                return std::make_error_code(std::errc::invalid_argument);
            if(auto err = grid::CommonAttributes::extract(proj);err)
                return err;
            else {
                if(!is_correct_options(proj))
                    return std::make_error_code(std::errc::invalid_argument);
                rotated = proj->attribute("rotated").as_bool();
                stretched = proj->attribute("stretched").as_bool();
                org = proj->attribute("center").as_int64();
                return {};
            }
        }
        virtual bool is_correct_attributes(const boost::json::object& attributes) noexcept override{
            if(!grid::CommonAttributes::is_correct_attributes(attributes))
                return false;
            if(!attributes.contains("rotated") || !attributes.at("rotated").is_bool())
                return false;
            if(!attributes.contains("stretched") || !attributes.at("stretched").is_bool())
                return false;
            if(!attributes.contains("center") || !attributes.at("center").is_int64())
                return false;
            return true;
        }
        virtual bool is_correct_options(std::shared_ptr<grid::CommonOptions> proj) noexcept override{
            if(!grid::CommonAttributes::is_correct_options(proj))
                return false;
            if(!proj->contains("rotated") || !proj->attribute("rotated").is_bool())
                return false;
            if(!proj->contains("stretched") || !proj->attribute("stretched").is_bool())
                return false;
            if(!proj->contains("center") || !proj->attribute("center").is_int64())
                return false;
            return true;
        }
    };
    namespace common{
        std::expected<
            AddResult,
            std::error_code> 
                add_grid_options(std::shared_ptr<grid::CommonOptions> opt)
        {
            using namespace projection;
            AddResult result;
            result.options=opt;
            result.attributes = std::make_unique<grib::CommonAttributes>();
            if(auto attr_res = result.attributes->is_correct_options(opt);!attr_res){
                switch (static_cast<API_T>(result.attributes->api_))
                {
                #if defined(GRIB1API) || defined(GRIB2API) || defined(GRIB3API)
                #ifdef GRIB1API
                case API_T::GRIB1:
                #endif
                #ifdef GRIB2API
                case API_T::GRIB2:
                #endif
                #ifdef GRIB3API
                case API_T::GRIB3:
                    break;
                #endif
                break;
                #endif
                default:
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    break;
                }
                if(try_indexate(opt))
                    return result;
                else return std::unexpected(std::make_error_code(std::errc::invalid_argument));//already exists
            }
            else return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        std::expected<
            AddResult,
            std::error_code> 
                add_grid_options(const boost::json::object& proj)
        {               
            using namespace projection;
            AddResult result;
            result.attributes=std::make_unique<grib::CommonAttributes>();
            result.options=std::make_shared<grid::CommonOptions>();
            if(auto attr_res = result.attributes->extract(proj);attr_res)
                return std::unexpected(attr_res);
            else{
                switch (static_cast<API_T>(result.attributes->api_))
                {
                #if defined(GRIB1API) || defined(GRIB2API) || defined(GRIB3API)
                #ifdef GRIB1API
                case API_T::GRIB1:
                #endif
                #ifdef GRIB2API
                case API_T::GRIB2:
                #endif
                #ifdef GRIB3API
                case API_T::GRIB3:
                    break;
                #endif
                break;
                #endif
                default:
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    break;
                }
                for(auto& [attr,val]:proj)
                    result.options->add_attribute(attr,val);
                if(try_indexate(result.options))
                    return result;
                else 
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
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
                grid::CommonOptions>> 
        grib_options;

        std::shared_ptr<grid::CommonOptions> get_grid(
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
            std::shared_ptr<grid::CommonOptions>,
            std::error_code> 
                add_grid_options(const boost::json::object& attributes)
        {
            auto common_result  = grib::common::add_grid_options(attributes);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            grib::CommonAttributes* attr = static_cast<grib::CommonAttributes*>(common_result->attributes.get());
            grib_options[{attr->org,attr->id_,attr->rotated,attr->stretched}]=common_result->options;
            return common_result->options;
        }
        std::expected<
            std::shared_ptr<grid::CommonOptions>,
            std::error_code> 
                add_grid_options(std::shared_ptr<grid::CommonOptions> proj)
        {
            auto common_result  = grib::common::add_grid_options(proj);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            grib::CommonAttributes* attr = static_cast<grib::CommonAttributes*>(common_result->attributes.get());
            grib_options[{attr->org,attr->id_,attr->rotated,attr->stretched}]=common_result->options;
            return common_result->options;
        }
        std::shared_ptr<grid::CommonOptions> get_grid_options(
            Organization center,
            RepresentationType id,
            bool rotated,
            bool stretched) noexcept
        {

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
                grid::CommonOptions>> 
        grib_options;

        std::shared_ptr<grid::CommonOptions> get_grid(
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
            std::shared_ptr<grid::CommonOptions>,
            std::error_code> 
                add_grid_options(const boost::json::object& attributes)
        {
            auto common_result  = grib::common::add_grid_options(attributes);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            grib::CommonAttributes* attr = static_cast<grib::CommonAttributes*>(common_result->attributes.get());
            grib_options[{attr->org,attr->id_,attr->rotated,attr->stretched}]=common_result->options;
            return common_result->options;
        }
        std::expected<
            std::shared_ptr<grid::CommonOptions>,
            std::error_code> 
                add_grid_options(std::shared_ptr<grid::CommonOptions> proj)
        {
            auto common_result  = grib::common::add_grid_options(proj);
            if(!common_result)
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            std::string name;
            decltype(grib_options)::key_type key;
            grib::CommonAttributes* attr = static_cast<grib::CommonAttributes*>(common_result->attributes.get());
            grib_options[{attr->org,attr->id_,attr->rotated,attr->stretched}]=common_result->options;
            return common_result->options;
        }
    }
    #endif
}
#endif
}

    std::expected<std::shared_ptr<grid::CommonOptions>,
        std::error_code> add_grid_options(
            const boost::json::object& opt_attributes) noexcept
    {
        if(!opt_attributes.contains("API"))
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        else if(auto API_result = from_json<int64_t>(opt_attributes.at("API"));
                !API_result){
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else {
            switch(static_cast<API_T>(API_result.value())){
                #ifdef GRIB1API
                case API_T::GRIB1:
                    return api::grib::v1::add_grid_options(opt_attributes);
                    break;
                #endif
                case API_T::COMMON:
                default:{
                    grid::CommonAttributes attributes;
                    if(auto err = attributes.extract(opt_attributes);err)
                        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    else{
                        std::shared_ptr<CommonOptions> options = std::make_shared<CommonOptions>();
                        options->add_attribute("name",attributes.name_);
                        options->add_attribute("API",attributes.api_);
                        options->add_attribute("ID",attributes.id_);
                        options->add_attribute("timepoint",attributes.timepoint_);
                        options->add_attribute("REF",opt_attributes.at("REF").as_string());
                        if(!try_indexate(options))
                            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                        else return options;
                    }
                }
            }
            
        }
    }

    std::expected<std::shared_ptr<grid::CommonOptions>,
        std::error_code> add_grid_options(
            std::shared_ptr<grid::CommonOptions> options) noexcept
    {
        
        if(auto API_result = from_json<int64_t>(options->attribute("API"));
                !API_result){
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        else {
            switch(static_cast<API_T>(API_result.value())){
                #ifdef GRIB1API
                case API_T::GRIB1:
                    return api::grib::v1::add_grid_options(options);
                    break;
                #endif
                case API_T::COMMON:
                default:
                if(!try_indexate(options))
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                else return options;
            }
        }
    }

    std::shared_ptr<grid::CommonOptions> get_grid_options(
            const boost::json::object& attributes) noexcept
    {
        if(auto found = projections_options.find(attributes);
            found!=projections_options.end())
            return *found;
        else {};
    }

    std::error_code add_projections_from_json(const std::filesystem::path& file) noexcept{
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
                                    add_grid_options(proj_obj);
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
