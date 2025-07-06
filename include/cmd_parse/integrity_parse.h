#pragma once
#include <vector>
#include <string_view>
#include <sys/error_code.h>

ErrorCode integrity_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_integrity_parse(const std::vector<std::string_view>& input,ErrorCode& err);

#include "cmd_parse/cmd_def.h"
#include "cmd_parse/search_process_parse.h"
namespace parse{
    class Integrity:public BaseParser<parse::Integrity>{
        Integrity():BaseParser("Capitalize options:"){}

        virtual void __init__() noexcept override final{
            descriptor_.add(SearchProcess::instance().descriptor());
        }
        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final{
            return SearchProcess::instance().parse(args);
        }
        public:
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{
            return __parse__(args);
        }
    };
}