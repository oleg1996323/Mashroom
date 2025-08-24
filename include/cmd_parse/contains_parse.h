#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "proc/contains.h"
#include "cmd_parse/cmd_def.h"
namespace parse{
    class Contains:public AbstractCLIParser<parse::Contains>{
        friend AbstractCLIParser;
        std::unique_ptr<::Contains> hContains;
        Contains():AbstractCLIParser("Contains options"){}

        virtual void init() noexcept override final{
            //descriptor_.add(SearchProcess::instance().descriptor());
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            //return SearchProcess::instance().parse(hContains.get(),args);
            hContains = std::make_unique<::Contains>();
            if(err_!=ErrorCode::NONE)
                return err_;
            if(err_=hContains->properties_integrity();err_!=ErrorCode::NONE)
                return err_;
            return hContains->execute();
        }
        public:
    };
}