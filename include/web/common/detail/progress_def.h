#pragma once
#include <cinttypes>
#include "serialization.h"

namespace network{
    namespace progress{
        enum class State :uint8_t{
            NOTHING,
            SEARCH,
            EXTRACT,
            PREPARE,
            SENDING,
            WAITING_REPLY
        };
    }
}