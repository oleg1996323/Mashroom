#include "cmd_parse/functions.h"
#include "error_print.h"
#include <iostream>

bool case_insensitive_char_compare(char ch1,char ch2) noexcept{
    return std::toupper(ch1)==std::toupper(ch2);
}

namespace po = boost::program_options;


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