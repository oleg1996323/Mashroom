#pragma once
#include <string>
namespace server{
struct Settings{
    std::string host;
    std::string service;
    int timeout_seconds_ = 20;
};
static Settings settings;
}