#include "cmd_parse/config/user.h"
#include "types_parse/datetimediff_parse.h"


namespace parse{
    UserConfig::Add::Add(CLI::App* app):
        app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",
                name_val_,
                "user configuration name")
        ->required(1);
        CLI::Option* output_files_root_dir_=app_
        ->add_option("--output-root-dir",
                output_files_root_dir_val_,
                "the root directory where user files will be placed")
        ->default_val((fs::path(getenv("HOME"))/
            "Mashroom"/"output").string())
        ->capture_default_str();
        CLI::Option* index_upd_ti_=app_
        ->add_option("--index-update-ti",
                index_upd_ti_val_,
                "index update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        CLI::Option* mashroom_upd_ti_=app_
        ->add_option("--mashroom-update-ti",
                mashroom_upd_ti_val_,
                "Mashroom update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        app_->callback([this](){execute();});
    }

    void UserConfig::Add::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("user-configuration "s+
                name_val_+" already exists");
        ::user::Settings settings_;
        if(app_->count("--output-root-dir")){
            settings_.output_files_root_dir=
                output_files_root_dir_val_;
        }
        if(app_->count("--index-update-ti")){
            settings_.index_update_ti=
                index_upd_ti_val_;
        }
        if(app_->count("--mashroom-update-ti")){
            settings_.mashroom_update_ti=
                mashroom_upd_ti_val_;
        }
        ::app().config().user_config().add(
            name_val_,std::move(settings_));
    }
    UserConfig::Remove::Remove(CLI::App* app):
        app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",
                name_val_,
                "user configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void UserConfig::Remove::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("user-configuration "s+
                name_val_+" don't exists");
        ::app().config().user_config().remove(name_val_);
    }

    UserConfig::Modify::Modify(CLI::App* app):
        app_(app){
        app_->require_option(2,0);
        CLI::Option* name_ = app_
        ->add_option("--name",name_val_,"user configuration name")
        ->required();
        CLI::Option* output_files_root_dir_=app_
        ->add_option("--output-root-dir",
                output_files_root_dir_val_,
        "the root directory where user files will be placed")
        ->default_val((fs::path(getenv("HOME"))/
            "Mashroom"/"output").string())
        ->capture_default_str();
        CLI::Option* index_upd_ti_=app_
        ->add_option("--index-update-ti",
                index_upd_ti_val_,
        "index update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        CLI::Option* mashroom_upd_ti_=app_
        ->add_option("--mashroom-update-ti",
                mashroom_upd_ti_val_,
        "Mashroom update time interval")
        ->default_val([](){
            std::error_code err;
            return DateTimeDiff(err,days(7));}())
        ->capture_default_str();
        app_->callback([this](){execute();});
    }

    void UserConfig::Modify::execute(){
        using namespace std::string_literals;
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(::app().config().user_config().contains(
            name_val_))
            throw std::runtime_error("user-configuration "s+
                name_val_+" already exists");
        ::user::Settings& settings_=*::app().config().
            user_config().get_config(name_val_);
        if(app_->count("--output-root-dir")){
            settings_.output_files_root_dir=
                output_files_root_dir_val_;
        }
        if(app_->count("--index-update-ti")){
            settings_.index_update_ti=
                index_upd_ti_val_;
        }
        if(app_->count("--mashroom-update-ti")){
            settings_.mashroom_update_ti=
                mashroom_upd_ti_val_;
        }
    }

    UserConfig::Load::Load(CLI::App* app):
        app_(app){
        CLI::Option* name_ = app_
        ->add_option("--name",
            name_val_,
            "user configuration name")
        ->required();
        CLI::Option* path_=app_
        ->add_option("--path",
            path_val_,
            "path of the loading file")
        ->required()->default_str("<PATH>")
        ->check(CLI::ExistingFile);
        app_->callback([this](){execute();});
    }

    void UserConfig::Load::execute(){
        if(name_val_=="default")
            throw std::runtime_error(
                "not allowed to change \"default\" configuration");
        if(!::app().config().user_config().contains(
            name_val_))
            ::app().config().user_config().add_from_file(
                name_val_,path_val_);
        else ::app().config().user_config().modify_from_file(
                name_val_,path_val_);
    }

    UserConfig::Print::Print(CLI::App* app):
        app_(app){
        CLI::Option* name_ = app_
        ->add_option(
            "--name",
            name_val_,
            "user configuration name")
        ->required();
        app_->callback([this](){execute();});
    }

    void UserConfig::Print::execute(){
        if(!::app().config().user_config().contains(
            name_val_))
            ::app().config().user_config().print(name_val_,std::cout);
    }

    UserConfig::UserConfig(CLI::App* app):
    app_(app)
    {
        app_->require_subcommand(1);
        CLI::App* add_ = app_->add_subcommand("add","add new user configuration");
        static std::unique_ptr<Add> add = std::make_unique<Add>(add_);
        CLI::App* remove_ = app_->add_subcommand("remove","remove existing user configuration");
        static std::unique_ptr<Remove> remove = std::make_unique<Remove>(remove_);
        CLI::App* modify_ = app_->add_subcommand("modify","modify existing user configuration");
        static std::unique_ptr<Modify> modify = std::make_unique<Modify>(modify_);
        CLI::App* load_ = app_->add_subcommand("load","load user configuration from JSON file");
        static std::unique_ptr<Load> load = std::make_unique<Load>(load_);
        CLI::App* print_all_ = app_->add_subcommand("print-all","print all accessible user configurations");
        CLI::App* print_named_ = app_->add_subcommand("print","print named user configuration (if exists)");
        static std::unique_ptr<Print> print = std::make_unique<Print>(print_all_);
        CLI::App* current_ = app_->add_subcommand(
            "current","print current active user configuration");
        app_->callback([this](){execute();});
    }
    void UserConfig::execute(){
        if(app_->got_subcommand("print-all"))
            ::app().config().user_config().print_all(std::cout);
        if(app_->got_subcommand("current"))
            ::app().config().user_config().print_current(std::cout);
    }
}