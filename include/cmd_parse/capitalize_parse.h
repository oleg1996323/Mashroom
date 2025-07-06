#pragma once
#include <string_view>
#include <vector>
#include <string>
#include <sys/error_code.h>

ErrorCode capitalize_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_capitalize_parse(const std::vector<std::string_view>& input,ErrorCode& err);


#include "cmd_parse/cmd_def.h"
namespace parse{
    class Capitalize:public BaseParser<parse::Capitalize>{
        Capitalize():BaseParser("Capitalize options:"){}

        virtual void __init__() noexcept override final{
            descriptor_.add_options()
            ("caporder","")
            ("capfmt","")
            ("ref","")
            ("part","");
            define_uniques();
        }
        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final{
            
        }
        public:
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{
            
        }
    };
}