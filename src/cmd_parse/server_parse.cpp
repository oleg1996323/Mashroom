#include "cmd_parse/network.h"
#include "functions.h"
#include "sys/application.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"


namespace parse{
    ErrorCode launch(const std::string& name) noexcept{
        if(name.empty() || name==app().config().server_config().current_name()){
            std::error_code err;
            Mashroom::instance().server().launch(err);
            if(err!=std::error_code())
                return ErrorPrint::print_error(ErrorCode::SERVER_ERROR,
                    err.message(),AT_ERROR_ACTION::CONTINUE);
            else return ErrorCode::NONE;
        }
        else{
            if(!Application::config().server_config().set_current(name))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "removing server config failed (config don't exists)",AT_ERROR_ACTION::CONTINUE,name);
            std::error_code err;
            Mashroom::instance().server().launch(err);
            if(err!=std::error_code())
                return ErrorPrint::print_error(ErrorCode::SERVER_ERROR,
                    err.message(),AT_ERROR_ACTION::CONTINUE);
            else return ErrorCode::NONE;
        }
        return ErrorCode::NONE;
    }

    ErrorCode close(bool wait=false,uint16_t timeout_sec = (uint16_t)60U) noexcept{
        Mashroom::instance().server().close(wait,timeout_sec);
        return ErrorCode::NONE;
    }

    ErrorCode get_current_config() noexcept{
        Application::config().server_config().print(std::cout);
        return ErrorCode::NONE;
    }

    void Network::init() noexcept{
        descriptor_.add_options()
                ("launch,L",po::value<std::string>()->value_name("<config name>")->implicit_value("")->zero_tokens(),"Launch a server with named configuration or configuration which was previously set.")
                ("shutdown,S",po::value<bool>()->default_value("true"),"Shutdown an instance of the server with set configuration.")
                ("close",po::value<bool>()->default_value("true"),"Close a launched server instance");
        define_uniques();
    }

    ErrorCode Network::execute(vars& vm,const std::vector<std::string>& tokens) noexcept{
        if(vm.contains("launch")){
            launch(vm.at("launch").as<std::string>());
        }
        else if(vm.contains("close")){
            close(vm.at("close").as<bool>());
        }
        else if(vm.contains("shutdown")){
            shutdown(vm.at("shutdown").as<bool>());
        }
        else 
            err_ = try_notify(vm);
        return err_;
    }
}