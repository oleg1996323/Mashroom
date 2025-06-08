#pragma once
#include <cinttypes>

namespace network::server{
struct ProgressBase{
    enum struct Process:uint8_t{
        NOTHING,
        SEARCH,
        EXTRACT,
        PREPARE,
        SENDING
    };
    float progress_=0;
    Process server_process_ = Process::NOTHING;
};
}