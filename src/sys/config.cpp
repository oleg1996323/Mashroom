#include "config.h"
#include <cassert>
#include <boost/json.hpp>
#include <ranges>
#include "filesystem.h"
#include "proj.h"
#include <netdb.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/json/parser.hpp>
#include "boost_functional/json.h"

using namespace std::string_view_literals;
using namespace std::string_literals;

ErrorCode Config::load() noexcept{
    using namespace boost;
    {
        std::string sys_fn = system_config().configurations_directory()/sys::system_config_filename();
        if(fs::exists(sys_fn)){
            std::expected<json::value,std::error_code> sys = parse_json_from_file(
                sys_fn);
            if(!sys.has_value())
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                        sys.error().message(),
                        AT_ERROR_ACTION::CONTINUE);
            else{
                if(auto sys_tmp = from_json<sys::Config>(sys.value());
                    !sys_tmp.has_value())
                    return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                            "system configuration reading error",
                            AT_ERROR_ACTION::CONTINUE);
            }
        }
    }
    {
        std::string usr_fn = system_config().configurations_directory()/sys::user_config_filename();
        if(fs::exists(usr_fn)){
            std::expected<json::value,std::error_code> user = parse_json_from_file(
                usr_fn);
            if(user.has_value()){
                if(auto usr_tmp = from_json<user::Config>(user.value());
                    usr_tmp.has_value())
                    user_configs_ = std::move(usr_tmp.value());
            }
        }
    }
    {
        std::string network_fn = system_config().configurations_directory()/sys::network_config_filename();
        if(fs::exists(network_fn)){
            std::expected<json::value,std::error_code> network = parse_json_from_file(
                network_fn);
            if(network.has_value()){
                if(auto network_tmp = from_json<network::Config>(network.value());
                    network_tmp.has_value())
                    network_configs_ = std::move(network_tmp.value());
            }
        }
        return ErrorCode::NONE;
    }
}

ErrorCode Config::save() noexcept{
    if(!directory_accessible(sys_settings_.configurations_directory()) || 
            safe_write_to_file(
                sys_settings_.configurations_directory(),
                sys::system_config_filename().data(),
                [this](std::ofstream& stream){
                    boost::json::value val = to_json(sys_settings_);
                    stream<<val.as_object();
                    stream.flush();
                    return false;
                })!=std::error_code())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                "system settings file writing error",
                AT_ERROR_ACTION::CONTINUE);
    if(!directory_accessible(sys_settings_.configurations_directory()) ||
            safe_write_to_file(
                sys_settings_.configurations_directory(),
                sys::user_config_filename().data(),
                [this](std::ofstream& stream){
                    boost::json::value val = to_json(user_configs_);
                    stream<<val.as_object();
                    stream.flush();
                    return false;
                })!=std::error_code())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                "user configurations file writing error",
                AT_ERROR_ACTION::CONTINUE);
    if(!directory_accessible(sys_settings_.configurations_directory()) ||
            safe_write_to_file(
                sys_settings_.configurations_directory(),
                sys::network_config_filename().data(),
                [this](std::ofstream& stream){
                    boost::json::value val = to_json(network_configs_);
                    stream<<val.as_object();
                    stream.flush();
                    return false;
                })!=std::error_code())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                "network configurations file writing error",
                AT_ERROR_ACTION::CONTINUE);
    return ErrorCode::NONE;
}