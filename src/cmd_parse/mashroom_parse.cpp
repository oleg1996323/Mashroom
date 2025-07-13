#include "cmd_parse/mashroom_parse.h"

namespace parse{
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Extract::instance().parse(input);
    }
    ErrorCode capitalize_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Capitalize::instance().parse(input);
    }
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Integrity::instance().parse(input);
    }
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Contains::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Config::instance().parse(input);
    }
    ErrorCode save_notifier(const std::vector<std::string>& input) noexcept{
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
        descriptor_.add_options()
        ("extract",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
            err_ = extract_notifier(items);
        }),"Extract specified data.")
        ("capitalize",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
            err_ = capitalize_notifier(items);
        }),"Read specified files and register the contained data properties and data positions")
        ("integrity",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
            err_ = integrity_notifier(items);
        }),"Check the integrity (dimensional and temporal) of capitalized data")
        ("contains",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
            err_ = contains_notifier(items);
        }),"Check if capitalized data contains the data specified by properties")
        ("config",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& items){
            err_ = config_notifier(items);
        }),"Permits to configure the program or server")
        ("save","Save the current instance")
        ("help","Show help")
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
        else if(vm.contains("help")){
            if(vm.size()>2)
                return ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,
                "",AT_ERROR_ACTION::CONTINUE);
            else{

            }
        }
        else{
            err_=try_notify(vm);
            return err_;
        }
    }
    void Mashroom::print_help(std::ostream& os,const std::span<std::string>& args) const noexcept{
        if(!args.empty()){
            po::options_description help_tmp;
            if(descriptors_table_.contains(args.back()))
                help_tmp.add(descriptors_table_.at(args.back()).descriptor_);
            else ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "",AT_ERROR_ACTION::CONTINUE,args.back());
        }
        else descriptor_.print(os);
    }
}