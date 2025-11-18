#include "proc/index/gen.h"
#include <string>
#include <stdexcept>
using namespace std::string_literals;

namespace index_gen{

bool check_format(std::string_view fmt){
	return check_format(index_gen::input_to_token_sequence(std::string(fmt)));
}
bool check_format(const std::vector<DIR_TREE_TOKEN>& tokens){
    std::unordered_map<DIR_TREE_TOKEN,int> counter;
    for(auto token:tokens){
        if(++counter[token]>1)
            return false;
    }
	return true;
}

const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>> token_to_txt_ = {
    {DIR_TREE_TOKEN::HOUR,{"hour"s,"hours"s}},
    {DIR_TREE_TOKEN::DAY,{"day"s,"days"s}},
    {DIR_TREE_TOKEN::MONTH,{"month"s,"months"s}},
    {DIR_TREE_TOKEN::YEAR,{"year"s,"years"s}},
    {DIR_TREE_TOKEN::LATITUDE,{"lat"s,"latitude"s}},
    {DIR_TREE_TOKEN::LONGITUDE,{"lon"s,"longitude"s}},
    {DIR_TREE_TOKEN::LATLON,{"latlon"s}},
    {DIR_TREE_TOKEN::GRID,{"grid"s}}
};

const std::unordered_map<std::string,DIR_TREE_TOKEN> txt_to_token_ = [](const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>> tokens){
    std::unordered_map<std::string,DIR_TREE_TOKEN> result;
    for(auto& [token,txts]:tokens){
        for(auto& txt:txts)
            result[txt]=token;
    }
    return result;
}(token_to_txt_);



const std::unordered_map<DIR_TREE_TOKEN,std::string> token_to_format_ = {
    {DIR_TREE_TOKEN::HOUR,"{:%H}"s},
    {DIR_TREE_TOKEN::DAY,"{:%d}"s},
    {DIR_TREE_TOKEN::MONTH,"{:%m}"s},
    {DIR_TREE_TOKEN::YEAR,"{:%Y}"s},
    {DIR_TREE_TOKEN::LATITUDE,"{}"s},
    {DIR_TREE_TOKEN::LONGITUDE,"{}"s},
    {DIR_TREE_TOKEN::LATLON,"{}_{}"s},
    {DIR_TREE_TOKEN::GRID,"{}"s}
};

const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>>& token_to_txts() noexcept{
    return token_to_txt_;
}
const std::unordered_map<std::string,DIR_TREE_TOKEN>& txt_to_tokens() noexcept{
    return txt_to_token_;
}

std::vector<DIR_TREE_TOKEN> input_to_token_sequence(const std::string& sequence_str){
    std::vector<DIR_TREE_TOKEN> result;
    std::string_view sequence = !sequence_str.empty() && sequence_str.starts_with(fs::path::preferred_separator)?std::string_view(sequence_str).substr(1):std::string_view(sequence_str);
    while(!sequence.empty()){
        auto pos_found = sequence.find_first_of(fs::path::preferred_separator);
        if(auto found = txt_to_token_.find(std::string(sequence.substr(0,pos_found)));found==txt_to_token_.end())
            throw std::invalid_argument("invalid sequence input. Prompt: token \""s+std::string(sequence.substr(0,pos_found))+"\""s);
        else{
            result.push_back(found->second);
            if(pos_found==std::string::npos)
                return result;
            sequence = sequence.substr(pos_found+1);
        }
    }
    return result;
}

std::string format(DIR_TREE_TOKEN token){
    return token_to_format_.at(token);
}

std::string generate_filename_format(std::vector<DIR_TREE_TOKEN> tokens) noexcept{
    std::string format_tmp;
    for(auto token:tokens)
        format_tmp+=format(token)+"_";
    if(!format_tmp.empty())
        format_tmp.pop_back();
    return format_tmp;
}

std::string generate_directory_format(const std::vector<DIR_TREE_TOKEN>& tokens) noexcept{
    std::string path_format_tmp;
    for(auto token:tokens)
        path_format_tmp += format(token)+fs::path::preferred_separator;
    return path_format_tmp;
}

}