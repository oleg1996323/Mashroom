#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "proc/integrity.h"
#include "cmd_parse/cmd_def.h"
namespace parse{
    class Integrity:public AbstractCLIParser<parse::Integrity>{
        friend AbstractCLIParser;
        std::unique_ptr<::Integrity> hIntegrity;
        Integrity():AbstractCLIParser("Integrity options"){}

        virtual void init() noexcept override final{
            //descriptor_.add(SearchProcess::instance().descriptor());
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            hIntegrity = std::make_unique<::Integrity>();
            if(err_!=ErrorCode::NONE)
                return err_;
            if(err_=hIntegrity->properties_integrity();err_!=ErrorCode::NONE)
                return err_;
            return hIntegrity->execute();
        }
        public:
    };
}