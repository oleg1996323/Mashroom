#include "types_parse/information_parse.h"
#include <boost/regex.hpp>
#include "OsterLib/parsing.h"
#include "sys/error.h"

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

        std::array<std::string_view,5> units = {"B","KB","MB","GB","TB"};
    }

    std::expected<info_quantity,osterlib::ContextedError> info_unit(std::string_view str) noexcept{
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
        else{
            osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,
                    "information unit not matched");
            ctx_err.with_field("at","info unit resolver")
            .with_field("input",str);
            return std::unexpected(std::move(ctx_err));
        }
    }
    
    std::expected<double,osterlib::ContextedError> info_size(std::string_view str) noexcept{
        using namespace boost::units::information;
        using namespace detail;
        auto parse_value = from_chars<double>(str);
        if(parse_value.has_value() && parse_value.value()>=0 && std::isinf(parse_value.value()))
            return parse_value.value();
        else{
            osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,
                    "information unit's number not matched");
            ctx_err.with_field("at","info size resolver")
            .with_field("input",str)
            .with_field("expect","float");
            return std::unexpected(std::move(ctx_err));
        }
    }

    std::expected<info_quantity,osterlib::ContextedError> info_size_unit(std::string_view str) noexcept{
        auto number_unit_separation = std::find_if(str.begin(),str.end(),[](const char ch) noexcept{
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
                if(std::isinf(size.value())){
                    osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,
                            "too huge value");
                    ctx_err.with_field("at","info size-unit resolver")
                    .with_field("input",std::string_view(str.begin(),number_unit_separation));
                    return std::unexpected(std::move(ctx_err));
                }
                return unit.value()*static_cast<double>(size.value());
            }
        }
    }
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

template<>
std::string boost::lexical_cast(const info_quantity& input){
    double val=input.value();
    int power = 0;
    while(val/1024>1 && power<4){
        val/=1024;
        ++power;
    }
    return std::to_string(val)+" "+std::string(parse::detail::units.at(power));
}

template<>
bool CLI::detail::lexical_cast<info_quantity>(const std::string& input, info_quantity& output) {
    try{
        output = boost::lexical_cast<info_quantity>(input);
        return true;
    }
    catch(...){
        return false;
    }
}

template<>
std::string CLI::detail::to_string(const info_quantity& input){
    return boost::lexical_cast<std::string>(input);
}