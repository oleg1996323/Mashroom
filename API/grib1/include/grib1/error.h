#pragma once
#include "OsterLib/contexted_error.h"
#include "common/error.h"

#ifdef GRIB1API
namespace api{
    template<>
    class ErrorCategory<API_T::GRIB1>:public std::error_category{
        public:
        enum class code:int{
            none,
            open_error,
            memory_error,
            bad_file,
            run_out,
            data_empty,
            read_position,
            missed_grib_record,
            length_unconsistent,
            missed_end_section
        };
        virtual const char * name() const noexcept override{
            return "common API error";
        }
        virtual std::string message(int ev) const{
            using namespace std::string_literals;
            switch (static_cast<code>(ev))
            {
            case code::none:
                return "no error";
                break;
            case code::open_error:
                return "error at openning file";
                break;
            case code::memory_error:
                return "memory allocation error (not enough memory)";
                break;
            case code::bad_file:
                return "corrupted file";
                break;
            case code::run_out:
                return "ran out of memory";
                break;
            case code::data_empty:
                return "data not found at file";
                break;
            case code::read_position:
                return "read position error";
                break;
            case code::missed_grib_record:
                return "missed grib record";
                break;
            case code::missed_end_section:
                return "missed GRIB v1 end section";
                break;
            case code::length_unconsistent:
                return "unconsistent GRIB v1 length message";
                break;
            default:
                return "unknown";
                break;
            }
        }
        static const auto& instance() noexcept{
            static ErrorCategory inst;
            return inst;
        }
    };
}
namespace std{
    template<> struct is_error_code_enum<
        api::ErrorCategory<API_T::GRIB1>::code> : true_type {};

    inline std::error_code make_error_code(
                    api::ErrorCategory<API_T::GRIB1>::code code) noexcept
    {
        return std::error_code(
            static_cast<std::underlying_type_t<
                    api::ErrorCategory<API_T::GRIB1>::code>>(code),
                    api::ErrorCategory<API_T::GRIB1>::instance());
    }
}
#endif