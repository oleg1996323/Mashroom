#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include <iostream>
#include "concepts.h"

bool case_insensitive_char_compare(char ch1,char ch2) noexcept{
    return std::toupper(ch1)==std::toupper(ch2);
}

namespace po = boost::program_options;


std::expected<po::variables_map,ErrorCode> try_store(const po::parsed_options& options){
    try{
        po::variables_map vm;
        po::store(options,vm);
        return vm;
    }
    catch(const po::error& err){
        return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,err.what(),AT_ERROR_ACTION::CONTINUE));
    }
    catch(const boost::bad_lexical_cast& err){
        return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,err.what(),AT_ERROR_ACTION::CONTINUE));
    }
    catch(const po::validation_error& err){
        return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,err.what(),AT_ERROR_ACTION::CONTINUE));
    }
}

std::expected<po::parsed_options,ErrorCode> try_parse(const po::options_description& opt_desc,const std::vector<std::string>& args){
    try{
        po::parsed_options parsed = po::command_line_parser(args).options(opt_desc).allow_unregistered().style(
                                            po::command_line_style::default_style
                                            & ~po::command_line_style::allow_guessing
                                            | po::command_line_style::long_allow_next
                                            | po::command_line_style::short_allow_next
                                            | po::command_line_style::allow_sticky
                                            | po::command_line_style::case_insensitive).run();
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