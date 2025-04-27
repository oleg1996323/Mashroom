#pragma once
#include <sections/grid/millers.h>

GridDefinition<RepresentationType::MILLERS_CYLINDR>::GridDefinition(unsigned char* buffer):
resolutionAndComponentFlags(GDS_Lambert_mode(buffer)),scan_mode(GDS_Lambert_scan(buffer)){
    throw std::invalid_argument("Unable to use Millers coordinate-system");
}