#pragma once
#include "definitions/path_process.h"
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

namespace boost{

namespace program_options{

template<>
void validate(boost::any& v,
              const std::vector<std::string>& values,
                std::vector<path::Storage<true>>*,int);
}

}