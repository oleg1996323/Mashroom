#pragma once
#include <optional>
#include <string_view>
#include "proc/index/indexoutputfileformat.h"
#include <boost/program_options.hpp>
#include <boost/any.hpp>

namespace boost{
    template<>
    ::IndexOutputFileFormat::token lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const ::IndexOutputFileFormat::token& input);
    namespace program_options{
        void validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                        IndexOutputFileFormat::token* target_type,int);
    }
}