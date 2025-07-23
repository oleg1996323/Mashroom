#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/search_process_parse.h"
#include "proc/extract.h"
#include "cmd_parse/time_separation_parse.h"
#include "cmd_parse/out_format_parse.h"
namespace parse{
    class Extract:public AbstractCLIParser<parse::Extract>{
        friend AbstractCLIParser;
        std::unique_ptr<::Extract> hExtract;
        Extract():AbstractCLIParser("Extract options"){}

        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            if(vm.contains("center")){
                auto center_res = center_notifier(vm.at("center").as<std::string>());
                if(center_res.has_value())
                    hExtract->set_center(center_res.value());
                else{
                    err_ = center_res.error();
                    return err_;
                }
            }
            hExtract = std::make_unique<::Extract>();
            err_ = try_notify(vm);
            return err_;
        }
        public:
    };
}