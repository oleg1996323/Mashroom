#include "sys/error_print.h"

namespace ErrorPrint{
    std::string prompt(const std::string& txt){
        return std::string("Prompt: ")+txt;
    }

    std::string message(ErrorCode err,const std::string& prompt_txt){
        std::string res = std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(""));
        if(prompt_txt.empty())
            return res;
        else return res+". "+prompt(prompt_txt)+".";
    }
}