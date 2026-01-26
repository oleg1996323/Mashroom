#pragma once
#include "data/msg.h"
#include <variant>

using IndexResultVariant = std::variant<std::monostate,GribMsgDataInfo>;
