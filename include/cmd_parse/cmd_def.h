#pragma once

#ifndef MODE_ENUM
#define MODE_ENUM
enum class MODE{
    NONE,
    CHECK_ALL_IN_PERIOD,
    CAPITALIZE,
    EXTRACT,
    CONFIG,
    HELP,
    EXIT
};
#endif

#ifndef DATA_EXTRACT_MODE
#define DATA_EXTRACT_MODE
enum class DataExtractMode{
    UNDEF,
    POSITION,
    RECT
};
#endif

#include "sys/error_print.h"
#include "sys/error_code.h"
#include <boost/program_options.hpp>
#include <vector>
#include <expected>

namespace parse{
    namespace po = boost::program_options;

    template<typename DerivedParser>
    class BaseParser{
        protected:
        po::options_description descriptor_;
        std::vector<std::string> unique_values_;
        BaseParser(const std::string& description):descriptor_(description){
            __init__();
        }
        ~BaseParser() = default;
        virtual void __init__() = 0 noexcept;

        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept = 0;

        public:

        static ServerAction& instance(){
            static DerivedParser inst;
            return inst;
        }

        const po::options_description& descriptor(){
            return descriptor_;
        }
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept = 0;

        void define_uniques() const{
            unique_values_ = [this](){
                std::vector<std::string> result;
                for(auto option:descriptor_.options())
                    result.push_back(option->long_name());
                return result;
            }();
        }
    };


    std::expected<po::parsed_options,ErrorCode> try_parse(const po::options_description& opt_desc,const std::vector<std::string>& args){
        try{
            po::parsed_options parsed = po::command_line_parser(args).options(opt_desc).allow_unregistered().run();
            return parsed;
        }
        catch(const po::multiple_occurrences& err){
            return std::unexpected(ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,err.what(),AT_ERROR_ACTION::CONTINUE));
        }
        catch(const po::ambiguous_option& err){
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE,err.get_option_name()));
        }
        catch(const po::required_option& err){
            return std::unexpected(ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,err.what(),AT_ERROR_ACTION::CONTINUE));
        }
        catch(const po::unknown_option& err){
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE,err.get_option_name()));
        }
        catch(const po::error& err){
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE));
        }
    }

    ErrorCode try_notify(po::variables_map& vm){
        try{
            po::notify(vm);
            return ErrorCode::NONE;
        }
        catch(const po::multiple_occurrences& err){
            return ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,err.what(),AT_ERROR_ACTION::CONTINUE);
        }
        catch(const po::ambiguous_option& err){
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE,err.get_option_name());
        }
        catch(const po::required_option& err){
            return ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,err.what(),AT_ERROR_ACTION::CONTINUE);
        }
        catch(const po::unknown_option& err){
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE,err.get_option_name());
        }
        catch(const po::error& err){
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
        }
    }
}