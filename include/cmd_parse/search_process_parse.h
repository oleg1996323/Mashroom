#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "data/info.h"
#include "code_tables/table_0.h"
#include "abstractsearchprocess.h"
#include "abstractcommandholder.h"
#include "types/time_interval.h"

ErrorCode search_process_parse(std::string_view option,std::string_view arg, AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters);
void commands_from_search_process(std::string_view option,std::string_view arg,AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters, ErrorCode& err);

#include "cmd_parse/cmd_def.h"
namespace parse{
    class SearchProcess:public BaseParser<parse::SearchProcess>{
        ::AbstractSearchProcess* search_proc_ = nullptr;
        SearchProcess():BaseParser(""){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
            ("outp",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& paths){
                for(auto& path:paths)
                    if(err_ = search_proc_->add_in_path(path);err_!=ErrorCode::NONE)
                        return;
            }),"Output path. May be directory or file path")
            ("inp",po::value<std::string>(),"Specify the input path")
            ("datefrom",po::value<utc_tp>(),"")
            ("dateto",po::value<utc_tp>(),"")
            ("pos",po::value<Coord>(),"")
            ("lattop",po::value<double>(),"Top latitude of rectangle")
            ("latbot",po::value<double>(),"Latitude bottom of rectangle")
            ("lonleft",po::value<double>(),"Left-side longitude of rectangle")
            ("lonrig",po::value<double>(),"Right-side longitude of rectangle")
            ("j",po::value<uint8_t>(),"Number of used threads. Number may be discarded to the maximal physical number threads")
            ("center",po::value<uint8_t>()->required(),"Specify the center that released the data")
            ("params",po::value<uint8_t>()->required(),"Specify the expected to process")
            ("tablev",po::value<uint8_t>()->required(),"specify the expected table version")
            ("collect",po::value<std::vector<std::string>>(),"Specify by name of collection")
            ("time_fcst",po::value<std::string>(),"Specify the forecast time of the released data")
            ("grid",po::value<std::string>(),"Specify the expected grid type");
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{

        }
        public:
        ErrorCode parse(::AbstractSearchProcess* ptr,const std::vector<std::string>& args){
            if(ptr==nullptr)
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,
                    "zero pointer passed. Abort.",AT_ERROR_ACTION::ABORT);
            else{
                search_proc_ = ptr;
                return BaseParser::parse(args);
            }
        }
    };
}