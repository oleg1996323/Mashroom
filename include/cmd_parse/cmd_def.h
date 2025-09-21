#pragma once

#ifndef GRIB
#include "API/grib1/include/cast/center.h"
#include "API/grib1/include/cast/grid.h"
#endif

#ifndef MODE_ENUM
#define MODE_ENUM
enum class MODE{
    NONE,
    CHECK_ALL_IN_PERIOD,
    INDEX,
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

    template<typename T>
    class AbstractCLIParser;

    class BaseCLIParser{
        public:
        template<typename T>
        friend class AbstractCLIParser;
        BaseCLIParser(const std::string& description):descriptor_(description){}
        virtual ~BaseCLIParser() = default;
        po::options_description descriptor_;
        std::vector<std::string> unique_values_;
        ErrorCode err_=ErrorCode::NONE;
        std::unordered_map<std::string_view,const BaseCLIParser&> descriptors_table_;
        BaseCLIParser& add_options_instances(const char *name, const boost::program_options::value_semantic *s,const BaseCLIParser& base) noexcept{
            auto desc = new po::option_description(name,s);
            descriptors_table_.insert({desc->long_name(),base});
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }

        BaseCLIParser& add_options_instances(const char *name, const boost::program_options::value_semantic *s, const char *description,const BaseCLIParser& base) noexcept{
            auto desc = new po::option_description(name,s,description);
            descriptors_table_.insert({desc->long_name(),base});
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }
        BaseCLIParser& add_options_instances(const char *name, const char *description,const BaseCLIParser& base) noexcept{
            auto desc = new po::option_description(name,new po::untyped_value(true),description);
            descriptors_table_.insert({desc->long_name(),base});
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }
        BaseCLIParser& add_options(const char *name, const boost::program_options::value_semantic *s) noexcept{
            auto desc = new po::option_description(name,s);
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }
        BaseCLIParser& add_options(const char *name, const boost::program_options::value_semantic *s, const char *description) noexcept{
            auto desc = new po::option_description(name,s,description);
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }
        BaseCLIParser& add_options(const char *name, const char *description) noexcept{
            auto desc = new po::option_description(name,new po::untyped_value(true),description);
            descriptor_.add(boost::shared_ptr<po::option_description>(desc));
            return *this;
        }
        BaseCLIParser& operator()(const char *name, const char *description) noexcept{
            return add_options(name,description);
        }
        BaseCLIParser& operator()(const char *name, const char *description,const BaseCLIParser& base) noexcept{
            return add_options_instances(name,description,base);
        }
        BaseCLIParser& operator()(const char *name, const boost::program_options::value_semantic *s, const char *description) noexcept{
            return add_options(name,s,description);
        }
        BaseCLIParser& operator()(const char *name, const boost::program_options::value_semantic *s) noexcept{
            return add_options(name,s);
        }
        BaseCLIParser& operator()(const char *name, const boost::program_options::value_semantic *s, const char *description,const BaseCLIParser& base) noexcept{
            return add_options_instances(name,s,description,base);
        }
        BaseCLIParser& operator()(const char *name, const boost::program_options::value_semantic *s,const BaseCLIParser& base) noexcept{
            return add_options_instances(name,s,base);
        }
    };

    template<typename DerivedParser>
    class AbstractCLIParser:public BaseCLIParser{
        protected:
        AbstractCLIParser(const std::string& description):BaseCLIParser(description){}
        virtual ~AbstractCLIParser() = default;
        virtual void init() noexcept = 0 ;

        using vars = po::variables_map;

        virtual ErrorCode execute(vars&,const std::vector<std::string>&) noexcept = 0;
        virtual void callback() noexcept{
            err_=ErrorCode::NONE;
        };
        void print_help(std::ostream& stream, std::span<po::option> args) const{
            const BaseCLIParser* current = this;
            for(const po::option& arg:args){
                if(current->descriptors_table_.contains(arg.string_key)){
                    if(!arg.value.empty()){
                        ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                        return;
                    }
                    else if(current->descriptor_.find_nothrow(arg.string_key,false)!=nullptr){
                        current = &current->descriptors_table_.at(arg.string_key);
                        continue;
                    }
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,arg.string_key);
                        return;
                    }
                }
                else{
                    if(!arg.value.empty()){
                        continue;
                    }
                    else if(auto* option = current->descriptor_.find_nothrow(arg.string_key,false); option!=nullptr){
                        stream<<option->format_name()<<" "<<option->description()<<std::endl;
                        return;
                    }
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,arg.string_key);
                        return;
                    }
                }
            }
            current->descriptor_.print(stream);
            return;
        }
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
                ErrorCode err = ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS, 
                                            "No arguments provided",
                                            AT_ERROR_ACTION::CONTINUE);
                callback();
                return err;
            }

            std::expected<boost::program_options::parsed_options, ErrorCode> parse_result = try_parse(instance().descriptor(),args);
            if(!parse_result.has_value()){
                ErrorCode err = parse_result.error();
                callback();
                return err;
            }
            if(parse_result.value().options.front().unregistered)
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,parse_result.value().options.front().string_key);
                
            if(parse_result.value().options.front().string_key=="help"){
                if(!parse_result.value().options.front().unregistered){
                    if(parse_result.value().options.size()>1)
                        print_help(std::cout,std::span(parse_result.value().options).subspan(1));
                    else descriptor_.print(std::cout);
                    return ErrorCode::NONE;
                }
                else{
                    callback();
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,parse_result.value().options.front().string_key);
                }
            }
            auto vm_res = try_store(parse_result.value());
            if(!vm_res.has_value())
                return vm_res.error();
            if(vm_res.value().empty())
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,args.front());
            std::vector<std::string> tokens = po::collect_unrecognized(parse_result.value().options,po::collect_unrecognized_mode::include_positional);
            auto dublicate = contains_unique_value(args.begin(),args.end(),unique_values_.begin(),unique_values_.end(),[](const std::string& item)
                noexcept->std::string_view
            {return item.starts_with("--")?std::string_view(item).substr(2):std::string_view(item);});

            if(dublicate!=unique_values_.end()){
                ErrorCode err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"must be unique",AT_ERROR_ACTION::CONTINUE,*dublicate);
                callback();
                return err;
            }
            else {
                execute(vm_res.value(),tokens);
                if(err_==ErrorCode::NONE)
                    err_ = try_notify(vm_res.value());
                ErrorCode err = err_;
                callback();
                err_ = ErrorCode::NONE;
                return err;
            }
        }
        void define_uniques() noexcept{
            unique_values_ = [this]() noexcept{
                std::vector<std::string> result;
                for(auto option:descriptor_.options())
                    if(!option->long_name().empty())
                        result.push_back(option->long_name());
                return result;
            }();
        }
    };
}