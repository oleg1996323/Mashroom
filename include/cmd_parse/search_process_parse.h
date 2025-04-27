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
#include <abstractsearchprocess.h>
#include <abstractcommandholder.h>

ErrorCode search_process_parse(std::string_view option,std::string_view arg, AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters);
void commands_from_search_process(std::string_view option,std::string_view arg,AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters, ErrorCode& err);