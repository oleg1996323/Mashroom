#pragma once
#include <system_error>

namespace mashroom::network{
    class ErrorCategory:public std::error_category{
        public:
        enum class code:int{
            internal_error,
            invalid_host,
            connection_error,
            invalid_client_request,
            sending_message_error,
            connection_closed,
            receiving_message_error,
            invalid_argument,
            transaction_refused,
            invalid_credentials,
            deserialization_error,
            serialization_error,
            server_error,
            buffer_low_size,
            version_error
        };
        virtual const char * name() const noexcept override{
            return "common API error";
        }
        virtual std::string message(int ev) const{
            using namespace std::string_literals;
            switch (static_cast<code>(ev))
            {
            case code::internal_error:
                return "internal error";
                break;
            case code::invalid_host:
                return "invalid host";
                break;
            case code::connection_error:
                return "connection error";
                break;
            case code::invalid_client_request:
                return "invalid client request";
                break;
            case code::sending_message_error:
                return "sending message error";
                break;
            case code::connection_closed:
                return "connection closed";
                break;
            case code::receiving_message_error:
                return "receiving message error";
                break;
            case code::invalid_argument:
                return "invalid argument";
                break;
            case code::transaction_refused:
                return "transaction refused";
                break;
            case code::invalid_credentials:
                return "invalid credentials";
                break;
            case code::deserialization_error:
                return "deserialization error";
                break;
            case code::serialization_error:
                return "serialization error";
                break;
            case code::server_error:
                return "server error";
                break;
            case code::buffer_low_size:
                return "buffer lower size";
                break;
            case code::version_error:
                return "version error";
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
        mashroom::network::ErrorCategory::code> : true_type {};

    inline std::error_code make_error_code(
            mashroom::network::ErrorCategory::code code) noexcept
    {
        return std::error_code(
            static_cast<std::underlying_type_t<
                    mashroom::network::ErrorCategory::code>>(code),
                    mashroom::network::ErrorCategory::instance());
    }
}