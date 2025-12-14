#include "internal_format_parse.h"
#include "cmd_parse/functions.h"
#include <boost/regex.hpp>
#include <type_traits>
#include <boost/algorithm/string.hpp>
#include "proc/index/indexoutputfileformat.h"

using namespace std::string_literals;

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    IndexOutputFileFormat::token* target_type,int)
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(values);
    const std::string s = po::validators::get_single_string(values);
    v = lexical_cast<IndexOutputFileFormat::token>(s);
}

template<>
std::string boost::lexical_cast(const IndexOutputFileFormat::token& input){
    try{
        return output_index_token_to_extension(input);
    }
    catch(const std::exception& err){
        throw boost::bad_lexical_cast();
    }    
}

template<>
::IndexOutputFileFormat::token boost::lexical_cast(const std::string& input){
    std::string regex_seq = std::views::values(output_index_token_extensions()) | std::views::join_with('|') | std::ranges::to<std::string>();
    auto r = regex("^("s+regex_seq+")$",boost::regex_constants::icase);
    smatch match;
    if(regex_match(input,match,r)){
        IndexOutputFileFormat::token fmt;
        if(boost::ifind_first(match[2],output_index_token_to_extension(IndexOutputFileFormat::JSON)))
            return IndexOutputFileFormat::token::JSON;
        else if(boost::ifind_first(match[2],output_index_token_to_extension(IndexOutputFileFormat::XML)))
            return IndexOutputFileFormat::token::XML;
        else if(boost::ifind_first(match[2],output_index_token_to_extension(IndexOutputFileFormat::BINARY)))
            return IndexOutputFileFormat::token::BINARY;
        else{
            assert(false); //code error (not added IndexOutputFileFormat::token to switch-case)
            return static_cast<IndexOutputFileFormat::token>(0);
        }
    }
    else throw boost::bad_lexical_cast();
}