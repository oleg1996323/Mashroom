#pragma once
#include <optional>
#include <string_view>
#include "sys/outputdatafileformats.h"
#include <boost/program_options.hpp>
#include <boost/any.hpp>

namespace boost{
    template<>
    ::OutputDataFileFormats lexical_cast(const std::string& input);
    namespace program_options{
        void validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                        OutputDataFileFormats* target_type,int);
    }
}