#include "cmd_parse/contains.h"

#include "sys/error_print.h"
#include "sys/application.h"
#include "sys/config.h"
#include "sys/outputdatafileformats.h"
#include "proc/contains.h"

namespace parse{
    Contains::Contains(CLI::App* cli):app_(cli){
        jobs_ = app_->add_option("-j",
        "Number of used threads.\
        Number may be discarded to\
        the maximal physical number\
        threads")->
        capture_default_str()->
        default_val(16);
        app_->callback([this](){
            this->execute();
        });
    }

    void Contains::execute(){
        std::cout<<"Contains still not developped"<<std::endl;
    }
}