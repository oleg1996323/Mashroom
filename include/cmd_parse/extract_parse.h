#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"

ErrorCode extract_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input,ErrorCode& err);

#include "cmd_parse/cmd_def.h"
#include "cmd_parse/search_process_parse.h"
namespace parse{
    class Extract:public BaseParser<parse::Extract>{
        Extract():BaseParser("Capitalize options:"){}

        virtual void __init__() noexcept override final{
            descriptor_.add_options()
            ("extoutfmt","")
            ("extti","");
            define_uniques();
            descriptor_.add(SearchProcess::instance().descriptor());
        }
        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final{

        }
        public:
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{
            
        }
    };
}