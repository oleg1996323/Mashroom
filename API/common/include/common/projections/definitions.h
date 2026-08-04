#pragma once
#include "options.h"
#include <memory>
#include <filesystem>

#ifdef GRIB1API
namespace api::grib{
    using RepresentationType = uint8_t;
    using Organization = uint8_t;
namespace v1{
    std::shared_ptr<projection::CommonOptions> get_grid(
            Organization center,
            RepresentationType id,
            bool rotated,
            bool stretched);
}
}
#endif

namespace projection{

enum class Type{
    Conformal,
	Equivalent,
	Equidist,
	Other
};

std::expected<std::shared_ptr<projection::CommonOptions>,
    std::error_code> add_projection_options(
        const boost::json::object& options);

std::expected<std::shared_ptr<projection::CommonOptions>,
    std::error_code> add_projection_options(
    std::shared_ptr<projection::CommonOptions>);

std::error_code add_projections_options_from_json(const std::filesystem::path& file);

std::shared_ptr<projection::CommonOptions> get_projection_options(
        const boost::json::object& attributes);
}