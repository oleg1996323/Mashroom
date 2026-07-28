#pragma once
#include "AbstractProjection.h"
#include "api_types.h"
#include <string>
#include <memory>

namespace projection{
std::unique_ptr<AbstractProjection> make_from_grib1(const char* buffer, 
                    const std::string& proj_name) noexcept;
}