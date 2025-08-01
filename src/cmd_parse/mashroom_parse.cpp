#include "cmd_parse/mashroom_parse.h"

namespace parse{
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        return Extract::instance().parse(input);
    }
    ErrorCode index_notifier(const std::vector<std::string>& input) noexcept{
        return Index::instance().parse(input);
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
        ::Mashroom::instance().save();
        return ErrorCode::NONE;
    }
    ErrorCode exit_notifier(const std::string& input) noexcept{
        if(input=="save")
            ::Mashroom::instance().save();
        exit(0);
    }

    void Mashroom::init() noexcept{
        add_options_instances
        ("extract",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = extract_notifier(items);
        }),"Extract specified data.",Extract::instance())
        ("index",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = index_notifier(items);
        }),"Read specified files and register the contained data properties and data positions. Organize the data by defined hierarchy from unique massive archive.",Index::instance())
        ("integrity",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = integrity_notifier(items);
        }),"Check the integrity (dimensional and temporal) of indexd data and detect the corrupted files of different format.",Integrity::instance())
        ("contains",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = contains_notifier(items);
        }),"Check if indexd data contains the data specified by properties",Contains::instance())
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
        else if(vm.contains("extract")){
            return extract_notifier(args);
        }
        else if(vm.contains("index")){
            return index_notifier(args);
        }
        else if(vm.contains("integrity")){
            return integrity_notifier(args);
        }
        else if(vm.contains("contains")){
            return contains_notifier(args);
        }
        else{
            err_=try_notify(vm);
            return err_;
        }
    }
}