#include "config.h"
#include <cassert>
#include <boost/json.hpp>
#include <ranges>
#include "OsterLib/filesystem.h"
#include "proj.h"
#include <netdb.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/json/parser.hpp>
#include "OsterLib/boost_functional/json.h"

using namespace std::string_view_literals;
using namespace std::string_literals;

osterlib::ContextedError Config::load() noexcept{
    using namespace boost;
    {
        std::string sys_fn = system_config().configurations_directory()/sys::system_config_filename();
        if(fs::exists(sys_fn)){
            std::expected<json::value,std::error_code> sys = parse_json_from_file(
                sys_fn);
            if(!sys.has_value())
                return osterlib::ContextedError(mashroom::errc::deserialization_error).
                    with_context("").with_field("file",sys_fn);
            else{
                if(auto sys_tmp = from_json<sys::Config>(sys.value());
                    !sys_tmp.has_value())
                    return osterlib::ContextedError(mashroom::errc::file_reading_error).
                        with_context("system configuration reading error");
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
        return {};
    }
}

osterlib::ContextedError Config::save() noexcept{
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
        return osterlib::ContextedError(mashroom::errc::file_writing_error).
                with_context("system settings file writing error").
                with_field("config","system");
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
        return osterlib::ContextedError(mashroom::errc::file_writing_error).
                with_context("system settings file writing error").
                with_field("config","user");
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
        return osterlib::ContextedError(mashroom::errc::file_writing_error).
                with_context("system settings file writing error").
                with_field("config","network");
    return {};
}