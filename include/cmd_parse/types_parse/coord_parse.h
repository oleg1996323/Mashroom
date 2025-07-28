#pragma once
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include "types/coord.h"
#include <vector>

namespace boost{
namespace program_options{
    void validate(boost::any& v,const std::vector<std::string>& values,
                                        std::optional<Coord>* target_type,int);
    void validate(boost::any& v,const std::vector<std::string>& values,
                                        Coord* target_type,int);
}
template<>
std::optional<Coord> lexical_cast(const std::string& input);
template<>
Coord boost::lexical_cast(const std::string& input);
template<>
std::string boost::lexical_cast(const Coord& input);
}