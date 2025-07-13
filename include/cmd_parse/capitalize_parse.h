#pragma once
#include <string_view>
#include <vector>
#include <string>
#include "sys/error_code.h"
#include "capitalize.h"
#include "cmd_parse/information_parse.h"
#include <tuple>
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/path_parse.h"
namespace parse{
    class Capitalize:public AbstractCLIParser<parse::Capitalize>{
        friend AbstractCLIParser;
        std::unique_ptr<::Capitalize> hCapitalize;
        Capitalize():AbstractCLIParser("Capitalize options:"){}

        virtual void init() noexcept override final;
        public:
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;

        virtual void callback() noexcept override final{
            
        }
    };
}