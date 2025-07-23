#include "cmd_parse/information_parse.h"
#include <boost/regex.hpp>

namespace parse{
    namespace detail{
        bool is_byte(std::string_view str) noexcept{
            constexpr std::array<std::string_view,3> to_match = {"b","byte","bytes"};
            return iend_with(str,to_match);
        }
        bool is_kilobyte(std::string_view str) noexcept{
            constexpr std::array<std::string_view,3> to_match = {"kb","kilobyte","kilobytes"};
            return iend_with(str,to_match);
        }
        bool is_megabyte(std::string_view str) noexcept{
            constexpr std::array<std::string_view,3> to_match = {"mb","megabyte","megabytes"};
            return iend_with(str,to_match);
        }
        bool is_gigabyte(std::string_view str) noexcept{
            constexpr std::array<std::string_view,3> to_match = {"gb","gigabyte","gigabytes"};
            return iend_with(str,to_match);
        }
        bool is_terabyte(std::string_view str) noexcept{
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
        auto parse_value = from_chars<double>(str);
        if(parse_value.has_value() && parse_value.value()>=0 && std::isinf(parse_value.value()))
            return parse_value.value();
        else return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                        "doesn't match number (expected floating-point number)",
                        AT_ERROR_ACTION::CONTINUE,str));
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


void boost::program_options::validate(boost::any& v,
              const std::vector<std::string>& values,
              info_quantity* target_type, int)
{
    validators::check_first_occurrence(v);
    const string& s = validators::get_single_string(values);
    v = lexical_cast<info_quantity>(s);
}

template<>
info_quantity boost::lexical_cast(const std::string& input){
    static boost::regex r("^(-?(0|[1-9][0-9]*)(\\.[0-9]+)?)[ ]*([a-zA-Z]+)$");
    smatch match;
    if (regex_match(input, match, r)) {
        auto sz_res = parse::info_size(match[1].str());
        if(sz_res.has_value()){
            auto unit_res = parse::info_size_unit(match[4].str());
            if(unit_res.has_value())
                return unit_res.value()*sz_res.value();
            else throw boost::bad_lexical_cast();
        }
        else throw boost::bad_lexical_cast();
    } else {
        throw boost::bad_lexical_cast();
    }
}