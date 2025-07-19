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

namespace boost{
namespace program_options{
void validate(boost::any& v,
              const std::vector<std::string>& values,
              utc_tp* target_type, int);
}
template<>
utc_tp lexical_cast(const std::string& input);
template<>
TimeOffset lexical_cast(const std::string& input);
}