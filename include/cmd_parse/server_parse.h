#pragma once
#include <program/mashroom.h>
#include <network/server.h>
#include <cmd_parse/cmd_translator.h>
#include <sys/error_code.h>
#include <sys/error_print.h>

ErrorCode server_config(const std::vector<std::string_view>& args,server::ServerConfig& config);
ErrorCode server_parse(const std::vector<std::string_view>& args);