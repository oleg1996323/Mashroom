#pragma once
#include <optional>
#include <string_view>
#include "sys/internaldatafileformats.h"
#include <boost/program_options.hpp>
#include <boost/any.hpp>

namespace boost{
    template<>
    ::InternalDateFileFormats lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const ::InternalDateFileFormats& input);
    namespace program_options{
        void validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                        InternalDateFileFormats* target_type,int);
    }
}