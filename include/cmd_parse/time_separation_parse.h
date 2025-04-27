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
#include "extract.h"
#include "code_tables/table_0.h"
#include <abstracttimeseparation.h>

ErrorCode time_separation_parse(std::string_view arg, AbstractTimeSeparation& obj);
std::string_view commands_from_time_separation(std::string_view arg,ErrorCode& err);