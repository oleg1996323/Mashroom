#include "cmd_parse/mashroom_parse.h"
#include "cmd_parse/types_parse/grid_type_parse.h"
#include "cmd_parse/types_parse/center_parse.h"
#include "cmd_parse/types_parse/information_parse.h"
#include "cmd_parse/index.h"
#include "cmd_parse/contains.h"
#include "cmd_parse/extract.h"
#include "cmd_parse/integrity.h"
#include "cmd_parse/config_parse.h"
#include "cmd_parse/network.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "program/mashroom.h"


namespace parse{
    using namespace std::string_literals;
    Mashroom::Mashroom(CLI::App& app):
        app_(&app){
        //app_->require_subcommand(1);
        CLI::App* index_ = app_->add_subcommand("index",
            "read specified files and register "s+
            "the contained data properties and "+
            "data positions. Organize the data "+
            "by defined hierarchy from unique "+
            "massive archive.")->
            alias("-I");
        static std::unique_ptr<parse::Index> index = std::make_unique<parse::Index>(index_);
        CLI::App* extract_ = app_->add_subcommand("extract","Extract specified data.");
        static std::unique_ptr<parse::Extract> extract = std::make_unique<parse::Extract>(extract_);
        CLI::App* contains_ = app_->add_subcommand("contains",
        "check if indexed data contains the data specified by properties");
        static std::unique_ptr<parse::Contains> contains = std::make_unique<parse::Contains>(contains_);
        CLI::App* integrity_ = app_->add_subcommand("check",
        "check the integrity (dimensional and temporal) of indexed data and detect the corrupted files of different format.");
        static std::unique_ptr<parse::Integrity> integrity = std::make_unique<parse::Integrity>(integrity_);
        CLI::App* save_cmd = app_->add_subcommand("save",
            "save the current instance (data, configurations)");
        CLI::App* config_=app_->add_subcommand("configure",
        "set configurations");
        static std::unique_ptr<parse::Configuration> config = std::make_unique<parse::Configuration>(config_);
        CLI::App* server_=app_->add_subcommand("server",
        "server activities");
        static std::unique_ptr<parse::Network> network = std::make_unique<parse::Network>(server_);
        CLI::App* exit_ = app_->add_subcommand("exit","Exit from program");
        exit_->add_flag("--save","flag if saving is needed before exit")->expected(0,1);
        app_->callback([this](){
            this->execute();
        });
    }

    void Mashroom::execute(){
        if(app_->got_subcommand("save")){
            ::Mashroom::instance().save();
            ::Application::config().save();
        }
        if(app_->got_subcommand("exit")){
            if(app_->get_subcommand("exit")->get_option("--save")->count())
                ::Mashroom::instance().save();
            else{
                std::cout<<"Save changes? (yes/no)"<<std::endl;
                std::string input;
                std::getline(std::cin,input);
                if(input=="yes"){
                    ::Mashroom::instance().save();
                    ::Application::config().save();
                }
            }
            exit(0);
        }
    }
}