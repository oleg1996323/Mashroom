#include "cmd_parse/network.h"
#include "program/mashroom.h"

namespace parse{

    Network::Network(CLI::App* app):app_(app){
        close_->add_flag("--wait",wait_,"wait finishing all connection-process");
        close_->add_option("--timeout",timeout_,"timeout closing in seconds");
        suspend_->add_flag("--wait", wait_, "wait finishing all connection-process before suspending");
        suspend_->add_option("--timeout", timeout_, "timeout suspending in seconds");
    }
    void Network::execute(){
        try{
            if(*launch_){
                std::error_code err;
                ::Mashroom::instance().server().launch(err);
                if(err!=std::error_code())
                    throw std::runtime_error(err.message());
            }
            if(*close_)
                ::Mashroom::instance().server().close(wait_,timeout_);
            if(*suspend_)
                ::Mashroom::instance().server().collapse(wait_,timeout_);
        }
        catch(const std::runtime_error& err){
            wait_=0;
            timeout_=0;
            throw err;
        }
    }
}