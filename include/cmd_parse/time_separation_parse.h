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
#include "code_tables/table_0.h"
#include "proc/interfaces/abstracttimeseparation.h"

ErrorCode time_separation_parse(std::string_view arg, AbstractTimeSeparation& obj);
std::string_view commands_from_time_separation(std::string_view arg,ErrorCode& err);

namespace boost{
namespace program_options{
void validate(boost::any& v,
              const std::vector<std::string>& values,
              utc_tp* target_type, int);
}
template<>
utc_tp lexical_cast(const std::string& input);
}