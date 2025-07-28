#pragma once
#include <string_view>
#include <expected>
#include <boost/units/systems/information.hpp>
#include <boost/units/systems/information/byte.hpp>
#include <boost/units/quantity.hpp>
#include <algorithm>
#include <array>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "functions.h"
#include <boost/program_options.hpp>

using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace parse{

    namespace detail{
        bool is_byte(std::string_view str) noexcept;
        bool is_kilobyte(std::string_view str) noexcept;
        bool is_megabyte(std::string_view str) noexcept;
        bool is_gigabyte(std::string_view str) noexcept;
        bool is_terabyte(std::string_view str) noexcept;
    }

    std::expected<info_quantity,ErrorCode> info_unit(std::string_view str) noexcept;
    
    std::expected<double,ErrorCode> info_size(std::string_view str) noexcept;

    std::expected<info_quantity,ErrorCode> info_size_unit(std::string_view str) noexcept;
}

#include <boost/regex.hpp>

using namespace std;
namespace boost{
    template<>
    info_quantity lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const info_quantity& input);
    namespace program_options{
        void validate(boost::any& v,
                    const std::vector<std::string>& values,
                    info_quantity* target_type, int);
    }
}