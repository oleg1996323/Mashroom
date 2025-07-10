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
#include <functional>

namespace parse{
    namespace po = boost::program_options;

    namespace po = boost::program_options;
    template<typename IterCheck,typename UniqueIter>
    UniqueIter contains_unique_value(IterCheck check1, IterCheck check2, UniqueIter unique1,UniqueIter unique2){
        bool contains_unique = false;
        for(UniqueIter iter = unique1;iter!=unique2;++iter){
            ptrdiff_t count = std::count(check1,check2,*iter);
            if(count==1)
                if(contains_unique==false)
                    contains_unique=true;
                else return iter;
            else if(count>1)
                return iter;
            else continue;
        }
        return unique2;
    }

    template<typename IterCheck,typename UniqueIter,typename ConvertString>
    UniqueIter contains_unique_value(IterCheck check1, IterCheck check2, UniqueIter unique1,UniqueIter unique2,ConvertString converter){
        bool contains_unique = false;
        for(UniqueIter iter = unique1;iter!=unique2;++iter){
            ptrdiff_t count = std::count(check1,check2,converter(*iter));
            if(count==1)
                if(contains_unique==false)
                    contains_unique=true;
                else return iter;
            else if(count>1)
                return iter;
            else continue;
        }
        return unique2;
    }

    template<typename DerivedParser>
    class BaseParser{
        protected:
        po::options_description descriptor_;
        std::vector<std::string> unique_values_;
        ErrorCode err_=ErrorCode::NONE;
        BaseParser(const std::string& description):descriptor_(description){
            init();
        }
        ~BaseParser() = default;
        virtual void init() noexcept = 0 ;

        using vars = po::variables_map;
        
        template<typename T>
        void notifier(std::function<ErrorCode(T)> function){
            err_ = function;
        }

        virtual ErrorCode execute(vars&,const std::vector<std::string>&) = 0;
        virtual void callback() noexcept{};
        public:

        static DerivedParser& instance() noexcept{
            static DerivedParser inst;
            return inst;
        }

        const po::options_description& descriptor() noexcept{
            return descriptor_;
        }
        ErrorCode parse(const std::vector<std::string>& args) noexcept
        {
            if(args.empty()){
                err_ = ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS, 
                                            "No arguments provided",
                                            AT_ERROR_ACTION::CONTINUE);
                callback();
                return err_;
            }

            auto parse_result = try_parse(instance().descriptor(),args);
            if(!parse_result.has_value()){
                err_ = parse_result.error();
                callback();
                return err_;
            }
            po::variables_map vm;
            po::store(parse_result.value(),vm);
            std::vector<std::string> tokens = po::collect_unrecognized(parse_result.value().options,po::collect_unrecognized_mode::include_positional);
            auto dublicate = contains_unique_value(args.begin(),args.end(),unique_values_.begin(),unique_values_.end(),[](const std::string& item)
                ->std::string_view
            {return item.starts_with("--")?std::string_view(item).substr(2):std::string_view(item);});

            if(dublicate!=unique_values_.end()){
                err_ = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"must be unique",AT_ERROR_ACTION::CONTINUE,*dublicate);
                callback();
                return err_;
            }
            else {
                err_ = execute(vm,tokens);
                callback();
                return err_;
            }
        }

        void define_uniques() const noexcept{
            unique_values_ = [this](){
                std::vector<std::string> result;
                for(auto option:descriptor_.options())
                    if(!option->long_name().empty())
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