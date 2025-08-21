#include "out_format_parse.h"
#include "cmd_parse/functions.h"
#include <boost/regex.hpp>
#include "sys/outputdatafileformats.h"
#include <type_traits>

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    OutputDataFileFormats* target_type,int)
{
    namespace po = boost::program_options;
    auto r = regex("^((txt|bin|grib)(\\+zip)?|(zip\\+)?(txt|bin|grib))$");
    po::validators::check_first_occurrence(values);
    const std::string s = po::validators::get_single_string(values);
    v = lexical_cast<OutputDataFileFormats>(s);
}

template<>
std::string boost::lexical_cast(const OutputDataFileFormats& input){
    std::string result;
    switch(input&~OutputDataFileFormats::ARCHIVED){
        case OutputDataFileFormats::BIN_F:
            result+="bin";
        break;
        case OutputDataFileFormats::DEFAULT:
        case OutputDataFileFormats::TXT_F:
            result+="txt";
        break;
        case OutputDataFileFormats::GRIB_F:
            result+="grib";
        break;
        default:
            throw std::invalid_argument("invalid input of output format");
    }
    if(std::underlying_type_t<OutputDataFileFormats>(input&OutputDataFileFormats::ARCHIVED)!=0)
        result+="zip";
    return result;
}

template<>
::OutputDataFileFormats boost::lexical_cast(const std::string& input){
    auto r = regex("^((txt|bin|grib)(\\+zip)?|(zip\\+)?(txt|bin|grib))$");
    smatch match;
    if(regex_match(input,match,r)){
        OutputDataFileFormats fmt;
        if((!match[2].str().empty() && match[2]=="txt")  || (!match[5].str().empty() && match[5]=="txt"))
            fmt|=OutputDataFileFormats::TXT_F;
        else if((!match[2].str().empty() && match[2]=="grib")  || (!match[5].str().empty() && match[5]=="grib"))
            fmt|=OutputDataFileFormats::GRIB_F;
        else
            fmt|=OutputDataFileFormats::BIN_F;
        if(!match[3].str().empty() || !match[4].str().empty())
            fmt|=OutputDataFileFormats::ARCHIVED;
        return fmt;
    }
    else throw std::invalid_argument(input);
}