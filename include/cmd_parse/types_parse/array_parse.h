#pragma once
#include <vector>
#include <string>
#include <expected>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include <ranges>
#include <boost_functional/json.h>
#include <boost/lexical_cast.hpp>

namespace parse{

template<typename ARRAY_T>
class Array{
    std::vector<ARRAY_T> values_;

    public:

    std::vector<ARRAY_T>::const_iterator begin() const{
        return values_.cbegin();
    }

    std::vector<ARRAY_T>::const_iterator end() const{
        return values_.cend();
    }

    const std::vector<ARRAY_T>& data() const{
        return values_;
    }

    //@todo make universal function for multiple types
    static std::expected<Array,ErrorCode> parse(const std::string& input){
        Array processed_tokens;
        auto parse_result = parse_json(input);
        if(!parse_result.has_value() || !parse_result->is_array())
            return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not array",AT_ERROR_ACTION::CONTINUE));
        else {
            processed_tokens.values_.reserve(parse_result.value().as_array().size());
            for(auto& value:parse_result.value().as_array()){
                if constexpr (std::is_same_v<ARRAY_T,bool>){
                    if(value.is_bool())
                        processed_tokens.values_.push_back(static_cast<ARRAY_T>(value.as_bool()));
                    else return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not bool",AT_ERROR_ACTION::CONTINUE));
                }
                else if constexpr (std::is_floating_point_v<ARRAY_T>){
                    if(value.is_double())
                        processed_tokens.values_.push_back(static_cast<ARRAY_T>(value.as_double()));
                    else return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"invalid array type",AT_ERROR_ACTION::CONTINUE));
                }
                else if constexpr (std::is_integral_v<ARRAY_T>){
                    if constexpr (std::is_signed_v<ARRAY_T>){
                        if(value.is_int64())
                            processed_tokens.values_.push_back(static_cast<ARRAY_T>(value.as_int64()));
                        else return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not signed integer",AT_ERROR_ACTION::CONTINUE));
                    }
                    else {
                        if((value.is_int64() && value.as_int64()>0) || value.is_uint64())
                            processed_tokens.values_.push_back(static_cast<ARRAY_T>(value.as_uint64()));
                        else return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not unsigned integer",AT_ERROR_ACTION::CONTINUE));
                    }
                }
                else if constexpr (std::is_convertible_v<ARRAY_T,std::string_view>){
                    if(value.is_string())
                        processed_tokens.values_.push_back(static_cast<ARRAY_T>(value.as_string()));
                    else
                        return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not string-type value",AT_ERROR_ACTION::CONTINUE));
                }
                else {
                    try{
                        processed_tokens.values_.push_back(boost::lexical_cast<ARRAY_T>(value.as_string()));
                    }
                    catch(const std::exception& err){
                        return std::unexpected(ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"not correct type",AT_ERROR_ACTION::CONTINUE));
                    }
                }
            }
            return processed_tokens;
        }
    }
};

}