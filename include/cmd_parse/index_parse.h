#pragma once
#include <string_view>
#include <vector>
#include <string>
#include "sys/error_code.h"
#include "proc/index.h"
#include "information_parse.h"
#include <tuple>
#include "cmd_parse/cmd_def.h"
#include "path_parse.h"
namespace parse{
    class Index:public AbstractCLIParser<parse::Index>{
        friend AbstractCLIParser;
        std::unique_ptr<::Index> hIndex;
        Index():AbstractCLIParser("Index options"){}

        virtual void init() noexcept override final;
        public:
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
}