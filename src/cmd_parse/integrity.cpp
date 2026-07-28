#include "cmd_parse/integrity.h"
#include "Location.h"
#include "sys/application.h"
#include "sys/config.h"
#include "sys/outputdatafileformats.h"
#include "proc/integrity.h"

namespace parse{
    Integrity::Integrity(CLI::App* cli):app_(cli){
        mashroom::errc err_;
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

    void Integrity::execute(){
        std::cout<<"Integrity still not developped"<<std::endl;
    }
}