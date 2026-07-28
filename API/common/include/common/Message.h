#pragma once
#include "api_types.h"
#include "grib1/message.h"

namespace api{

    template<API_T T>
    struct Message;

    template<API_T T>
    bool define_message(const char* buffer,api::Message<T>&);
}