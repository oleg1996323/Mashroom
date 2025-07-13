#include "cmd_parse/out_format_parse.h"
#include "cmd_parse/functions.h"
#include <boost/regex.hpp>

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    Extract::ExtractFormat* target_type,int)
{
    namespace po = boost::program_options;
    auto r = regex("^((txt|bin|grib)(\\+zip)?|(zip\\+)?(txt|bin|grib))$");
    po::validators::check_first_occurrence(values);
    const std::string s = po::validators::get_single_string(values);
    v = lexical_cast<Extract::ExtractFormat>(s);
}

template<>
::Extract::ExtractFormat boost::lexical_cast(const std::string& input){
    auto r = regex("^((txt|bin|grib)(\\+zip)?|(zip\\+)?(txt|bin|grib))$");
    smatch match;
    if(regex_match(input,match,r)){
        Extract::ExtractFormat fmt;
        if(match[2]=="txt")
            fmt|=Extract::ExtractFormat::TXT_F;
        else if(match[2]=="grib")
            fmt|=Extract::ExtractFormat::GRIB_F;
        else if(match[2]=="bin")
            fmt|=Extract::ExtractFormat::BIN_F;
        else fmt|=Extract::ExtractFormat::ARCHIVED;
        if(match.size()>3){
            if(static_cast<int>(fmt&Extract::ExtractFormat::ARCHIVED)!=0)
                ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"error at parsing: already archive format defined",
                AT_ERROR_ACTION::ABORT);
            fmt|=Extract::ExtractFormat::ARCHIVED;
        }
        return fmt;
    }
    else throw boost::bad_lexical_cast();
}