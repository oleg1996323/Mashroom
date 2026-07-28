#pragma once
#include "options.h"
#include <memory>
#include <filesystem>

namespace projection{

enum class Type{
    Conformal,
	Equivalent,
	Equidist,
	Other
};

std::error_code add_projection(
    std::string,
    std::unique_ptr<projection::CommonOptions>);

void define_from_json(const std::filesystem::path& file);

std::unique_ptr<projection::CommonOptions> projection_options(
        const std::string name,
        const std::string& format,
        bool rotatable,
        bool stretchable);
}