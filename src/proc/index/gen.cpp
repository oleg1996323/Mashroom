#include "proc/index/gen.h"
#include <string>
#include <stdexcept>
using namespace std::string_literals;

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
    {DIR_TREE_TOKEN::HOUR,"{:%H}"},
    {DIR_TREE_TOKEN::DAY,"{:%D}"s},
    {DIR_TREE_TOKEN::MONTH,"{:%B}"s},
    {DIR_TREE_TOKEN::YEAR,"{:%Y}"s},
    {DIR_TREE_TOKEN::LATITUDE,"{%.2f}"s},
    {DIR_TREE_TOKEN::LONGITUDE,"{%.2f}"s},
    {DIR_TREE_TOKEN::LATLON,"{%.2f}_{%.2f}"},
    {DIR_TREE_TOKEN::GRID,"{%s}"}
};

const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>>& token_to_txt() noexcept{
    return token_to_txt_;
}
const std::unordered_map<std::string,DIR_TREE_TOKEN>& txt_to_token() noexcept{
    return txt_to_token_;
}

std::vector<DIR_TREE_TOKEN> input_to_token_sequence(const std::string& sequence){
    std::vector<DIR_TREE_TOKEN> result;
    std::size_t pos = 0;
    size_t prev_pos = 0;
    while(prev_pos<sequence.size()){
        pos = sequence.find_first_of(fs::separator(),prev_pos);
        if(auto found = txt_to_token_.find(sequence.substr(prev_pos,pos));found==txt_to_token_.end())
            throw std::invalid_argument("invalid sequence input. Prompt: token \""s+sequence.substr(prev_pos,pos)+"\""s);
        else{
            result.push_back(found->second);
            if(pos!=std::string::npos)
                prev_pos = pos+1;
            else return result;
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
        path_format_tmp += format(token)+fs::separator();
    return path_format_tmp;
}