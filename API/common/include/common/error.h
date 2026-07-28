#pragma once
#include <system_error>
#include "api_types.h"
#include "OsterLib/contexted_error.h"

namespace api{
    template<API_T TYPE>
    class ErrorCategory;

    template<>
    class ErrorCategory<API_T::COMMON>:public std::error_category{
        public:
        enum class code:int{
            none,
            open_error,
            memory_error,
            bad_file,
            run_out,
            data_empty
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
                return "bad file";
                break;
            case code::run_out:
                return "ran out of memory";
                break;
            case code::data_empty:
                return "data not found at file";
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
    
    template<API_T T>
    using errc = typename ErrorCategory<T>::code;
}
namespace std{
    template<> struct is_error_code_enum<
            api::ErrorCategory<API_T::COMMON>::code> : true_type {};
    inline std::error_code make_error_code(
                    api::ErrorCategory<API_T::COMMON>::code code) noexcept
    {
        return error_code(
            static_cast<underlying_type_t<
                    api::ErrorCategory<API_T::COMMON>::code>>(code),
                    api::ErrorCategory<API_T::COMMON>::instance());
    }
}