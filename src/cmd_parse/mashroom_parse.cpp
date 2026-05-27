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
    ErrorCode server_notifier(const std::vector<std::string>& input) noexcept{
        return Network::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        return ProgramConfig::instance().parse(input);
    }
    ErrorCode serverconfig_notifier(const std::vector<std::string>& input) noexcept{
        return ServerConfig::instance().parse(input);
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
        }),"Check if indexed data contains the data specified by properties",Contains::instance())
        ("config",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            err_ = config_notifier(items);
        }),"Configurations",ProgramConfig::instance())
        ("network",po::value<std::vector<std::string>>()->zero_tokens()->notifier([this](const std::vector<std::string>& items){
            
        }),"Network activities",Network::instance())
        ("save",po::value<void>()->notifier([this](){
            
        }),"Save the current instance (data, configurations)")
        ("help,H","Show help")
        ("exit",po::value<void>()->notifier([](){
            exit(0);
        }),"Exit from program");
        assert(!descriptor_.find_nothrow("add",false));
        define_uniques();
    }
}