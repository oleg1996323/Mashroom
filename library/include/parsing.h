#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

template <class T, class... Args>
std::optional<T> from_chars(std::string_view s, Args... args) noexcept{
    const char *end = s.data() + s.size();
    
    if constexpr (std::is_enum_v<T>){
        std::underlying_type_t<T> number;
        auto result = std::from_chars(s.data(), end, number, args...);
        if (result.ec != std::errc{} || result.ptr != end){
            return std::nullopt;
        }
        return static_cast<T>(number);
    }
    else{
        T number;
        auto result = std::from_chars(s.data(), end, number, args...);
        if (result.ec != std::errc{} || result.ptr != end){
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

#include <unordered_set>
bool iend_with(std::string_view str,const std::ranges::range auto& to_match){
    if constexpr(std::ranges::range<typename std::decay_t<decltype(to_match)>::value_type>){
        for(const typename std::decay_t<decltype(to_match)>::value_type& possible:to_match){
            if(str.size()<possible.size())
                continue;
            if(auto substr = possible.substr(str.size()-possible.size()-1);
                std::equal(substr.begin(),substr.end(),str.begin(),str.end(),case_insensitive_char_compare))
                return true;
            else continue;
        }
    }
    else{
        if(str.size()<to_match.size())
            false;
        if(auto substr = to_match.substr(str.size()-to_match.size()-1);
            std::equal(substr.begin(),substr.end(),str.begin(),str.end(),case_insensitive_char_compare))
            return true;
        else false;
    }
    return false;
}

// bool iend_with(std::string_view str,const char to_match[]){
//     if(std::strlen(to_match))
//         false;
//     if(auto substr = to_match.substr(str.size()-to_match.size()-1);
//         std::equal(substr.begin(),substr.end(),str.begin(),str.end(),case_insensitive_char_compare))
//         return true;
//     else false;
//     return false;
// }