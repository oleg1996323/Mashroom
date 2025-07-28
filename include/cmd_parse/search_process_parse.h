#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "functions.h"
#include "data/sublimed_info.h"
#include "code_tables/table_0.h"
#include "proc/interfaces/abstractsearchprocess.h"
#include "types/time_interval.h"
#include "grid_type_parse.h"

#include "cmd_parse/cmd_def.h"
#include "sections/section_1.h"
#include "param_tablev_parse.h"
#include "path_parse.h"
#include "coord_parse.h"
#include "time_separation_parse.h"
#include <expected>
namespace parse{

    std::expected<Organization,ErrorCode> center_notifier(const std::vector<std::string>& input) noexcept;
    std::expected<RepresentationType,ErrorCode> grid_notifier(const std::vector<std::string>& input) noexcept;

    class SearchProcess:public AbstractCLIParser<parse::SearchProcess>{
        friend AbstractCLIParser;
        ::AbstractSearchProcess* search_proc_ = nullptr;
        SearchProcess():AbstractCLIParser(""){}
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        public:
        ErrorCode parse(::AbstractSearchProcess* ptr,const std::vector<std::string>& args);
    };
}

void validate(boost::any& v,
              const std::vector<std::string>& values,
              utc_tp* target_type, int);

