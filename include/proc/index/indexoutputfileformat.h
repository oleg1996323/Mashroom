#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

struct IndexOutputFileFormat{
    enum token:uint32_t{
        BINARY,
        JSON,
        XML
    };
};

std::string output_index_token_to_extension(IndexOutputFileFormat::token token);

const std::unordered_map<IndexOutputFileFormat::token,std::string>& output_index_token_extensions() noexcept;