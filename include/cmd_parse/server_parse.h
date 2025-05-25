#pragma once
#include <program/mashroom.h>
#include <network/server.h>
#include <cmd_parse/cmd_translator.h>
#include <sys/error_code.h>
#include <sys/error_print.h>
#include <sys/config.h>
#include <sys/log_err.h>

ErrorCode server_config(const std::vector<std::string_view>& args,network::server::Config& config);
ErrorCode server_parse(const std::vector<std::string_view>& args);