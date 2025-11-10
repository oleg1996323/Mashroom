#pragma once
#include <optional>
#include <string_view>
#include "proc/index/indexdatafileformat.h"
#include <boost/program_options.hpp>
#include <boost/any.hpp>

namespace boost{
    template<>
    ::IndexDataFileFormats::token lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const ::IndexDataFileFormats::token& input);
    namespace program_options{
        void validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                        IndexDataFileFormats::token* target_type,int);
    }
}