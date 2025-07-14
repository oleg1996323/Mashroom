#pragma once
#include "definitions/def.h"
#include <boost/program_options.hpp>
#include <vector>
#include <string_view>
#include <string>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <expected>
#include "sys/error_code.h"
#include "sys/error_print.h"

struct SearchParamTableVersion;

namespace boost::program_options{

template<>
void validate(boost::any& v,const std::vector<std::string>& values,
                                        std::vector<SearchParamTableVersion>* target_type,int);

}
namespace parse::parameter_tv{
std::expected<SearchParamTableVersion,ErrorCode> param_by_tv_abbr(Organization center,std::string_view input);
}