#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <charconv>
#include <stdexcept>

#include "types/coord.h"
#include "cmd_def.h"
#include "sys/error_print.h"
#include "types/time_interval.h"
#include "definitions/def.h"
#include <set>
#include <type_traits>
#include "sys/error_code.h"
#include <expected>

template <class T, class... Args>
std::optional<T> from_chars(std::string_view s,ErrorCode& err, Args... args) noexcept{
    const char *end = s.data() + s.size();
    
    if constexpr (std::is_enum_v<T>){
        std::underlying_type_t<T> number;
        auto result = std::from_chars(s.data(), end, number, args...);
        if (result.ec != std::errc{} || result.ptr != end){
            err = ErrorCode::COMMAND_INPUT_X1_ERROR;
            return std::nullopt;
        }
        return static_cast<T>(number);
    }
    else{
        T number;
        auto result = std::from_chars(s.data(), end, number, args...);
        if (result.ec != std::errc{} || result.ptr != end){
            err = ErrorCode::COMMAND_INPUT_X1_ERROR;
            return std::nullopt;
        }
        return number;
    }
}
template<typename T>
std::vector<T> split(std::string_view str, const char* delimiter) noexcept{
    std::vector<T> result;
    size_t beg_pos = 0;
    size_t pos = 0;
    while(pos!=std::string::npos){
        pos = str.find_first_of(delimiter,beg_pos);
        result.emplace_back(str.substr(beg_pos,pos-beg_pos));
        beg_pos=pos+1;
    }
    return result;
}

bool case_insensitive_char_compare(char ch1,char ch2) noexcept;

template<size_t NUM_ARGS>
bool iend_with(std::string_view str,const std::array<std::string_view,NUM_ARGS>& to_match){
    for(std::string_view possible:to_match){
        if(str.size()<possible.size())
            continue;
        if(auto substr = possible.substr(str.size()-possible.size()-1);
            std::equal(substr.begin(),substr.end(),str.begin(),str.end(),case_insensitive_char_compare))
            return true;
        else continue;
    }
    return false;
}

std::expected<boost::program_options::parsed_options,ErrorCode> try_parse(const boost::program_options::options_description& opt_desc,const std::vector<std::string>& args);
ErrorCode try_notify(boost::program_options::variables_map& vm);

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