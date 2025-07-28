#include "internal_format_parse.h"
#include "cmd_parse/functions.h"
#include <boost/regex.hpp>
#include <type_traits>
#include <boost/algorithm/string.hpp>

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    InternalDateFileFormats* target_type,int)
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(values);
    const std::string s = po::validators::get_single_string(values);
    v = lexical_cast<InternalDateFileFormats>(s);
}

template<>
std::string boost::lexical_cast(const InternalDateFileFormats& input){
    switch(input){
        case InternalDateFileFormats::NATIVE:
            return "native";
        case InternalDateFileFormats::BINARY:
            return "bin";
        case InternalDateFileFormats::TEXT:
            return "txt";
        case InternalDateFileFormats::GRIB:
            return "grib";
        case InternalDateFileFormats::IEEE:
            return "IEEE";
        case InternalDateFileFormats::HGT:
            return "hgt";
        case InternalDateFileFormats::NETCDF:
            return "netcdf";
        default:
            throw boost::bad_lexical_cast();
    }
}

template<>
::InternalDateFileFormats boost::lexical_cast(const std::string& input){
    auto r = regex("^(native|txt|bin|grib|IEEE|netcdf|hgt)$",boost::regex_constants::icase);
    smatch match;
    if(regex_match(input,match,r)){
        InternalDateFileFormats fmt;
        if(boost::ifind_first(match[2],"native"))
            return InternalDateFileFormats::NATIVE;
        else if(boost::ifind_first(match[2],"txt"))
            return InternalDateFileFormats::TEXT;
        else if(boost::ifind_first(match[2],"grib"))
            return InternalDateFileFormats::GRIB;
        else if(boost::ifind_first(match[2],"bin"))
            return InternalDateFileFormats::BINARY;
        else if(boost::ifind_first(match[2],"IEEE"))
            return InternalDateFileFormats::IEEE;
        else if(boost::ifind_first(match[2],"hgt"))
            return InternalDateFileFormats::HGT;
        else return InternalDateFileFormats::NETCDF;
    }
    else throw boost::bad_lexical_cast();
}