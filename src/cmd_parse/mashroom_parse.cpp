#include "cmd_parse/mashroom_parse.h"

namespace parse{
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        return Extract::instance().parse(input);
    }
    ErrorCode capitalize_notifier(const std::vector<std::string>& input) noexcept{
        return Capitalize::instance().parse(input);
    }
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept{
        return Integrity::instance().parse(input);
    }
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept{
        return Contains::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        return Config::instance().parse(input);
    }
    ErrorCode save_notifier() noexcept{
        if(!hProgram->save())
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"saving failed",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }
    ErrorCode exit_notifier(const std::string& input) noexcept{
        if(input=="save")
            hProgram->save();
        exit(0);
    }

    void Mashroom::init() noexcept{
        add_options_instances("extract",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = extract_notifier(items);
        }),"Extract specified data.",Extract::instance())
        ("capitalize",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = capitalize_notifier(items);
        }),"Read specified files and register the contained data properties and data positions. Organize the data by defined hierarchy from unique massive archive.",Capitalize::instance())
        ("integrity",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = integrity_notifier(items);
        }),"Check the integrity (dimensional and temporal) of capitalized data and detect the corrupted files of different format.",Integrity::instance())
        ("contains",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = contains_notifier(items);
        }),"Check if capitalized data contains the data specified by properties",Contains::instance())
        ("config",po::value<std::vector<std::string>>()->zero_tokens(),"Permits to configure the program or server",Config::instance()).
        add_options("save","Save the current instance")
        ("help,H","Show help")
        ("exit",po::value<std::string>()->implicit_value("save")->notifier([this](const std::string& item){
            err_ = exit_notifier(item);
        }),"Exit from program");
        define_uniques();
    }
    ErrorCode Mashroom::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        if(err_!=ErrorCode::NONE)
            return err_;
        else if(vm.contains("save") && vm.size()==1){
            err_ = save_notifier();
            return err_;
        }
        else if(vm.contains("config")){
            return config_notifier(args);
        }
        // else if(vm.contains("help")){
        //     if(vm.size()==2){
        //         for(auto& var:args)
        //             std::cout<<var<<std::endl;
        //         const std::string& option = std::ranges::find_if_not(vm,[](const auto& option){
        //             return option.first=="help";
        //         })->first;
        //         print_help(std::cout,option,std::span(args));
        //         return ErrorCode::NONE;
        //     }
        //     else if(vm.size()>2)
        //         return ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
        //     else{
        //         descriptor_.print(std::cout);
        //         return ErrorCode::NONE;
        //     }
        // }
        else{
            err_=try_notify(vm);
            return err_;
        }
    }
}