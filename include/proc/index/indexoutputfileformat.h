#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <boost/lexical_cast.hpp>

struct IndexOutputFileFormat{
    enum token:uint32_t{
        BINARY,
        JSON,
        XML
    };
};

std::string output_index_token_to_extension(IndexOutputFileFormat::token token);

const std::unordered_map<IndexOutputFileFormat::token,std::string>& output_index_token_extensions() noexcept;

template<>
std::string boost::lexical_cast(const IndexOutputFileFormat::token& input);

template<>
::IndexOutputFileFormat::token boost::lexical_cast(const std::string& input);