#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/cmd_def.h"
#include "proc/extract.h"
#include "time_separation_parse.h"
#include "out_format_parse.h"
#include "cmd_parse/types_parse/center_parse.h"
#include "cmd_parse/types_parse/grid_type_parse.h"
namespace parse{
    class Extract:public AbstractCLIParser<parse::Extract>{
        friend AbstractCLIParser;
        std::unique_ptr<::Extract> hExtract;
        Extract():AbstractCLIParser("Extract options"){}

        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        public:
    };
}