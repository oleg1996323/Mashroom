#pragma once
#include <system_error>
#include "OsterLib/contexted_error.h"

namespace mashroom{
    class ErrorCategory:public std::error_category{
        public:
        enum class code:int{
            internal_error=1,
            command_input_error,
            to_few_arguments,
            to_many_arguments,
            create_directory_denied,
            file_permission_denied,
            not_file,
            not_directory,
            no_exists_path,
            unknown_file_format,
            undefined_value,
            integrity_violated,
            invalid_index_order,
            interrupted,
            not_file_or_directory,
            invalid_argument,
            deserialization_error,
            serialization_error,
            data_not_found,
            buffer_low_size,
            timeout,
            file_corrupted,
            version_error,
            file_reading_error,
            file_writing_error
        };
        virtual const char * name() const noexcept override{
            return "Mashroom error";
        }
        virtual std::string message(int ev) const{
            using namespace std::string_literals;
            switch (static_cast<code>(ev))
            {
            case code::internal_error:
                return "internal error";
                break;
            case code::command_input_error:
                return "command input error";
                break;
            case code::to_few_arguments:
                return "to few arguments";
                break;
            case code::to_many_arguments:
                return "to many arguments";
                break;
            case code::create_directory_denied:
                return "create directory denied";
                break;
            case code::file_permission_denied:
                return "file permission denied";
                break;
            case code::not_file:
                return "not file";
                break;
            case code::not_directory:
                return "not directory";
                break;
            case code::no_exists_path:
                return "no exists path";
                break;
            case code::unknown_file_format:
                return "unknown file format";
                break;
            case code::undefined_value:
                return "undefined value";
                break;
            case code::integrity_violated:
                return "integrity violated";
                break;
            case code::invalid_index_order:
                return "invalid index order";
                break;
            case code::interrupted:
                return "interrupted";
                break;
            case code::not_file_or_directory:
                return "not file or directory";
                break;
            case code::invalid_argument:
                return "invalid argument";
                break;
            case code::deserialization_error:
                return "deserialization error";
                break;
            case code::serialization_error:
                return "serialization error";
                break;
            case code::data_not_found:
                return "data not found";
                break;
            case code::buffer_low_size:
                return "buffer lower size";
                break;
            case code::timeout:
                return "timeout";
                break;
            case code::file_corrupted:
                return "file corrupted";
                break;
            case code::version_error:
                return "version error";
                break;
            case code::file_reading_error:
                return "file reading error";
                break;
            case code::file_writing_error:
                return "file writing error";
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
    using errc = ErrorCategory::code;
}
namespace std{
    template<> struct is_error_code_enum<
        mashroom::ErrorCategory::code> : true_type {};

    inline std::error_code make_error_code(
                    mashroom::ErrorCategory::code code) noexcept
    {
        return std::error_code(
            static_cast<std::underlying_type_t<
                    mashroom::ErrorCategory::code>>(code),
                    mashroom::ErrorCategory::instance());
    }
}