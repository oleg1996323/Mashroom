#include "path_parse.h"

namespace po = boost::program_options;

template<>
void boost::program_options::validate(boost::any& v,
              const std::vector<std::string>& values,
              std::vector<path::Storage<false>>* target_type, int)
{
    // Make sure no previous assignment to 'a' was made.
    validators::check_first_occurrence(v);
    // Do regex match and convert the interesting part to
    // int.
    v = lexical_cast<std::vector<path::Storage<false>>>(values);
}

template<>
std::vector<path::Storage<false>> boost::lexical_cast(const std::vector<std::string>& input){
    std::vector<path::Storage<false>> result;
    for(auto& s:input)
        result.push_back(lexical_cast<path::Storage<false>>(s));
    return result;
}

template<>
path::Storage<false> boost::lexical_cast(const std::string& input){
    static regex r("^(dir|host|file)=(.+)$");
    using namespace boost::program_options;
    boost::smatch match;
    if (boost::regex_match(input, match, r)) {
        auto type = match[1].str();
        auto path = match[2].str();
        if(type=="dir")
            return path::Storage<false>(path,path::TYPE::DIRECTORY);
        else if(type=="file")
            return path::Storage<false>(path,path::TYPE::FILE);
        else if(type=="host")
            return path::Storage<false>(path,path::TYPE::HOST);
        else throw po::validation_error(po::validation_error::invalid_option_value,input);
    }
    else throw po::validation_error(po::validation_error::invalid_option_value,input);
}

