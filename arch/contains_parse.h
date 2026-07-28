#pragma once
#include <vector>
#include <string_view>
#include "sys/error.h"
#include "proc/contains.h"
#include "cmd_parse/cmd_def.h"
namespace parse{
    class Contains:public AbstractCLIParser<parse::Contains>{
        friend AbstractCLIParser;
        Contains():AbstractCLIParser("Contains options"){}

        virtual void init() noexcept override final{
            //descriptor_.add(SearchProcess::instance().descriptor());
            define_uniques();
        }
        public:
    };
}