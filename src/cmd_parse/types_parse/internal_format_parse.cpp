#include "internal_format_parse.h"
#include "cmd_parse/functions.h"
#include <boost/regex.hpp>
#include <type_traits>
#include <boost/algorithm/string.hpp>

using namespace std::string_literals;

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    IndexDataFileFormats::token* target_type,int)
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(values);
    const std::string s = po::validators::get_single_string(values);
    v = lexical_cast<IndexDataFileFormats::token>(s);
}

template<>
std::string boost::lexical_cast(const IndexDataFileFormats::token& input){
    try{
        if(input == IndexDataFileFormats::NATIVE)
            return "native";
        return token_to_extension(input);
    }
    catch(const std::exception& err){
        throw boost::bad_lexical_cast();
    }    
}

template<>
::IndexDataFileFormats::token boost::lexical_cast(const std::string& input){
    std::string regex_seq = std::views::values(token_extension()) | std::views::join_with('|') | std::ranges::to<std::string>();
    auto r = regex("^("s+regex_seq+")$",boost::regex_constants::icase);
    smatch match;
    if(regex_match(input,match,r)){
        IndexDataFileFormats::token fmt;
        if(boost::ifind_first(match[2],"native"))
            return IndexDataFileFormats::token::NATIVE;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::JSON)))
            return IndexDataFileFormats::token::JSON;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::XML)))
            return IndexDataFileFormats::token::XML;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::GRIB)))
            return IndexDataFileFormats::token::GRIB;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::BINARY)))
            return IndexDataFileFormats::token::BINARY;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::IEEE)))
            return IndexDataFileFormats::token::IEEE;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::HGT)))
            return IndexDataFileFormats::token::HGT;
        else if(boost::ifind_first(match[2],token_to_extension(IndexDataFileFormats::NETCDF)) || boost::ifind_first(match[2],"netcdf"))
            return IndexDataFileFormats::token::NETCDF;
        else assert(false); //code error (not added IndexDataFileFormats::token to switch-case)
    }
    else throw boost::bad_lexical_cast();
}