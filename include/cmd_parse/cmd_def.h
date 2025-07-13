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
#include <span>
#include "cmd_parse/functions.h"

namespace parse{
    namespace po = boost::program_options;

    struct BaseCLIParser{
        BaseCLIParser(const std::string& description):descriptor_(description){}
        virtual ~BaseCLIParser() = default;
        po::options_description descriptor_;
        std::vector<std::string> unique_values_;
        ErrorCode err_=ErrorCode::NONE;
        static std::unordered_map<std::string_view,const BaseCLIParser&> descriptors_table_;
        po::option_description& add_parse_instance(po::option_description& option,const BaseCLIParser& base) const{
            descriptors_table_.insert({option.long_name(),base});
            return option;
        }
    };

    template<typename DerivedParser>
    class AbstractCLIParser:public BaseCLIParser{
        protected:
        
        AbstractCLIParser(const std::string& description):BaseCLIParser(description){}
        virtual ~AbstractCLIParser() = default;
        virtual void init() noexcept = 0 ;

        using vars = po::variables_map;

        virtual ErrorCode execute(vars&,const std::vector<std::string>&) = 0;
        virtual void callback() noexcept{};
        
        public:
        static DerivedParser& instance() noexcept{
            static DerivedParser inst;
            static bool initialized = false;
            if(!initialized){
                inst.init();
                initialized = true;
            }
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

        void define_uniques() noexcept{
            unique_values_ = [this](){
                std::vector<std::string> result;
                for(auto option:descriptor_.options())
                    if(!option->long_name().empty())
                        result.push_back(option->long_name());
                return result;
            }();
        }
        virtual std::expected<po::options_description,ErrorCode> get_help(const std::string& key,const std::span<std::string>& args) const noexcept{
            return descriptor_;
        }
    };
}