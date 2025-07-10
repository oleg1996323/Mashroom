#pragma once
#include "path_process.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <boost/regex.hpp>

using namespace std;

void boost::program_options::validate(boost::any& v,
              const std::vector<std::string>& values,
              path::Storage<true>* target_type, int);
