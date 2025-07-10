#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "contains.h"

ErrorCode integrity_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_integrity_parse(const std::vector<std::string_view>& input,ErrorCode& err);

#include "cmd_parse/cmd_def.h"
#include "cmd_parse/search_process_parse.h"
namespace parse{
    class Contains:public BaseParser<parse::Contains>{
        std::unique_ptr<::Contains> hContains;
        Contains():BaseParser("Contains options:"){}

        virtual void init() noexcept override final{
            descriptor_.add(SearchProcess::instance().descriptor());
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            return SearchProcess::instance().parse(args);
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