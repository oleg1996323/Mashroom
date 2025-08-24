#pragma once
#include <boost/program_options.hpp>
#include "code_tables/table_6.h"
#include <vector>
#include <string>
#include <expected>
#include "sys/error_code.h"

namespace boost{
template<>
RepresentationType lexical_cast(const std::string& input);
namespace program_options{
    void validate(boost::any& v,const std::vector<std::string>& values,
                                        RepresentationType* target_type,int);
}
}

namespace parse{
    std::expected<RepresentationType,ErrorCode> grid_notifier(const std::vector<std::string>& input) noexcept;
}