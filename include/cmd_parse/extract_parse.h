#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/search_process_parse.h"
#include "extract.h"
#include "cmd_parse/time_separation_parse.h"
#include "cmd_parse/out_format_parse.h"
namespace parse{
    class Extract:public AbstractCLIParser<parse::Extract>{
        friend AbstractCLIParser;
        std::unique_ptr<::Extract> hExtract;
        Extract():AbstractCLIParser("Extract options:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
            ("ext-out-format",po::value<::Extract::ExtractFormat>()->notifier([this](::Extract::ExtractFormat input){
                hExtract->set_output_format(input);
            }),"")
            ("ext-time-interval",po::value<std::string>()->notifier([this](const std::string& input){
                err_ = time_separation_parse(input,*hExtract);
            }),"");
            define_uniques();
            //descriptor_.add(SearchProcess::instance().descriptor());
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            hExtract = std::make_unique<::Extract>();
            err_ = try_notify(vm);
        }
        public:
    };
}