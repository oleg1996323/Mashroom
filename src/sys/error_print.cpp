#include "error_print.h"

namespace ErrorPrint{
    using namespace std::string_literals;
    std::string prompt(const std::string& txt){
        return txt.empty()?"":" Prompt: "s+txt;
    }

    std::string message(ErrorCode err,const std::string& prompt_txt){
        return std::vformat(std::string(err_msg.at((size_t)err)),std::make_format_args(""))+prompt(prompt_txt);
    }
}