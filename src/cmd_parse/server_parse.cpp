#include "cmd_parse/server_parse.h"
#include "functions.h"
#include "sys/application.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"


namespace parse{
    ErrorCode launch_notifier(const std::string& name) noexcept{
        if(name.empty() || name==app().config().get_current_server_config().name_)
            Mashroom::instance().launch_server();
        else{
            if(!Application::config().set_server_config(name))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"removing server config failed (config don't exists)",AT_ERROR_ACTION::CONTINUE,name);
            Mashroom::instance().launch_server();
        }
        return ErrorCode::NONE;
    }

    ErrorCode shutdown_notifier(bool wait) noexcept{
        return ErrorCode::NONE;
    }

    ErrorCode closing_notifier(bool wait) noexcept{
        Mashroom::instance().close_server(wait);
        return ErrorCode::NONE;
    }

    ErrorCode get_current_config() noexcept{
        Application::config().get_current_server_config().print_server_config(std::cout);
        return ErrorCode::NONE;
    }

    void ServerAction::init() noexcept{
        descriptor_.add_options()
                ("launch,L",po::value<std::string>()->implicit_value(::app().config().get_current_server_config().name_)->zero_tokens(),"Launch a server with named configuration or configuration which was previously set.")
                ("shutdown,S",po::bool_switch()->default_value(true),"Shutdown an instance of the server with set configuration.")
                ("close",po::bool_switch()->default_value(true),"Close a launched server instance");
        define_uniques();
    }

    ErrorCode ServerAction::execute(vars& vm,const std::vector<std::string>& tokens) noexcept{
        if(vm.contains("launch"))
            launch_notifier(vm.at("launch").as<std::string>());
        else if(vm.contains("shutdown"))
            shutdown_notifier(vm.at("shutdown").as<bool>());
        else if(vm.contains("close"))
            closing_notifier(vm.at("close").as<bool>());
        else 
            err_ = try_notify(vm);
        return err_;
    }
}