#pragma once
#include <string_view>
#include <vector>
#include <string>
#include "sys/error.h"
#include "proc/index.h"
#include <tuple>
#include "cmd_parse/cmd_def.h"
#include "types_parse/information_parse.h"
#include "types_parse/path_parse.h"
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