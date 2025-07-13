#pragma once
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include "types/coord.h"
#include <vector>

namespace boost{
namespace program_options{
    void validate(boost::any& v,const std::vector<std::string>& values,
                                        Coord* target_type,int);
}
template<>
Coord lexical_cast(const std::string& input);
}