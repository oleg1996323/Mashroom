#include "cmd_parse/mashroom_parse.h"
#include "cmd_parse/types_parse/grid_type_parse.h"
#include "cmd_parse/types_parse/center_parse.h"
#include "cmd_parse/types_parse/information_parse.h"
#include "cmd_parse/index.h"
#include "cmd_parse/contains.h"
#include "cmd_parse/extract.h"
#include "cmd_parse/integrity.h"


namespace parse{
    void Mashroom::execute(){
        if(app_->got_subcommand("index")){
            static parse::Index index(app_);
            index.execute();
        }
        if(app_->got_subcommand("extract")){
            static parse::Extract extract(app_);
            extract.execute();
        }
        if(app_->got_subcommand("contains")){
            static parse::Contains contains(app_);
            contains.execute();
        }
        if(app_->got_subcommand("integrity")){
            static parse::Integrity integrity(app_);
            integrity.execute();
        }
        if(app_->got_subcommand("config")){
            static parse::Configuration config(app_);
            config.execute();
        }
        if(app_->got_subcommand("server")){
            static parse::Network network(app_);
            network.execute();
        }
        if(app_->got_subcommand("save"))
            ::Mashroom::instance().save();
        if(app_->got_subcommand("help"))
            CLI::CallForAllHelp(app_);
        if(app_->got_subcommand("exit")){
            if(exit_->get_option("--save")->count())
                ::Mashroom::instance().save();
            else{
                std::cout<<"Save changes? (yes/no)"<<std::endl;
                std::string input;
                std::getline(std::cin,input);
                if(input=="yes")
                    ::Mashroom::instance().save();
            }
            exit(0);
        }
    }
}