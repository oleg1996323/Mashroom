#pragma once
#include <string_view>
#include <expected>
#include <boost/units/systems/information.hpp>
#include <boost/units/systems/information/byte.hpp>
#include <algorithm>
#include <array>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "functions.h"

using info_units = boost::units::information::hu::byte::info;
using info_quantity = boost::units::quantity<info_units>;

namespace parse{

    namespace detail{
        bool case_insensitive_char_compare(char ch1,char ch2) noexcept{
            return std::toupper(ch1)==std::toupper(ch2);
        }

        template<size_t NUM_ARGS>
        bool iend_with(std::string_view str,const std::array<std::string_view,NUM_ARGS>& to_match){
            for(std::string_view possible:to_match){
                if(str.size()<possible.size())
                    continue;
                if(auto substr = possible.substr(str.size()-possible.size()-1);
                    std::equal(substr.begin(),substr.end(),str.begin(),str.end(),case_insensitive_char_compare))
                    return true;
                else continue;
            }
            return false;
        }

        bool is_byte(std::string_view str){
            constexpr std::array<std::string_view,3> to_match = {"b","byte","bytes"};
            return iend_with(str,to_match);
        }
        bool is_kilobyte(std::string_view str){
            constexpr std::array<std::string_view,3> to_match = {"kb","kilobyte","kilobytes"};
            return iend_with(str,to_match);
        }
        bool is_megabyte(std::string_view str){
            constexpr std::array<std::string_view,3> to_match = {"mb","megabyte","megabytes"};
            return iend_with(str,to_match);
        }
        bool is_gigabyte(std::string_view str){
            constexpr std::array<std::string_view,3> to_match = {"gb","gigabyte","gigabytes"};
            return iend_with(str,to_match);
        }
        bool is_terabyte(std::string_view str){
            constexpr std::array<std::string_view,3> to_match = {"tb","terabyte","terabytes"};
            return iend_with(str,to_match);
        }
    }

    std::expected<info_quantity,ErrorCode> info_unit(std::string_view str) noexcept{
        using namespace boost::units::information;
        using namespace detail;
        if(is_byte(str))
            return bytes*uint64_t(1);
        else if(is_kilobyte(str))
            return bytes*(uint64_t(1)<<10);
        else if (is_megabyte(str))
            return bytes*(uint64_t(1)<<20);
        else if (is_gigabyte(str))
            return bytes*(uint64_t(1)<<30);
        else if (is_terabyte(str))
            return bytes*(uint64_t(1)<<40);
        else return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                        "doesn't match any information unit",AT_ERROR_ACTION::CONTINUE,str));
    }
    
    std::expected<double,ErrorCode> info_size(std::string_view str) noexcept{
        using namespace boost::units::information;
        using namespace detail;
        ErrorCode code = ErrorCode::NONE;
        auto parse_value = from_chars<double>(str,code);
        if(parse_value.has_value() && parse_value.value()>=0)
            return parse_value.value();
        else return std::unexpected(code);
    }

    std::expected<info_quantity,ErrorCode> info_size_unit(std::string_view str) noexcept{
        auto number_unit_separation = std::find_if(str.begin(),str.end(),[](const char ch){
            return !std::isdigit(ch);
        });
        auto unit=info_unit(std::string_view(number_unit_separation,str.end()));

        if(!unit.has_value())
            return std::unexpected(unit.error());
        else {
            auto size=info_size(std::string_view(str.begin(),number_unit_separation));
            if(!size.has_value())
                return std::unexpected(size.error());
            else{
                if(std::isinf(size.value()))
                    return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                        "too huge value",AT_ERROR_ACTION::CONTINUE,std::string_view(str.begin(),number_unit_separation)));
                return unit.value()*static_cast<double>(size.value());
            }
        }
    }
}