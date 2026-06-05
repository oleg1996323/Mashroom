#include "proc/index/indexoutputfileformat.h"

const std::unordered_map<IndexOutputFileFormat::token,std::string> extension_txt_ = {
    {IndexOutputFileFormat::token::BINARY,".bin"},
    {IndexOutputFileFormat::token::JSON,".json"},
    {IndexOutputFileFormat::token::XML,".xml"}
};

#include <stdexcept>
std::string output_index_token_to_extension(IndexOutputFileFormat::token token){
    if(!extension_txt_.contains(token))
        throw std::invalid_argument("undefined indexing format");
    return extension_txt_.at(token);
}

const std::unordered_map<IndexOutputFileFormat::token,std::string>& output_index_token_extensions() noexcept{
    return extension_txt_;
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

#include <regex>
#include <ranges>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

template<>
::IndexOutputFileFormat::token boost::lexical_cast(const std::string& input){
    using namespace std::string_literals;
    std::string regex_seq = std::views::values(output_index_token_extensions()) | std::views::join_with('|') | std::ranges::to<std::string>();
    auto r = boost::regex("^("s+regex_seq+")$",std::regex_constants::icase);
    boost::smatch match;
    if(boost::regex_match(input,match,r)){
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