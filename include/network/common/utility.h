#pragma once
#include <cinttypes>
#include <ctype.h>
#include <netinet/in.h>
#include "byte_order.h"
#include <bit>

namespace network::utility{
uint64_t htonll(uint64_t value);
uint64_t ntohll(uint64_t value);
float htonf(float value);
float ntohf(float value);
double htond(double value);
double ntohd(double value);
}