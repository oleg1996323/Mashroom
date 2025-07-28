#include "path_parse.h"

namespace po = boost::program_options;

template<>
void boost::program_options::validate(boost::any& v,
              const std::vector<std::string>& values,
              std::vector<path::Storage<true>>* target_type, int)
{
    static regex r("^(dir|host|file)=(.+)$");
    using namespace boost::program_options;

    // Make sure no previous assignment to 'a' was made.
    validators::check_first_occurrence(v);

    std::vector<path::Storage<true>> result;
    // Do regex match and convert the interesting part to
    // int.
    for(auto& s:values){
        smatch match;
        if (regex_match(s, match, r)) {
            auto type = match[1].str();
            auto path = match[2].str();
            if(type=="dir"){
                result.push_back(path::Storage<true>(path,path::TYPE::DIRECTORY));
            }
            else if(type=="file"){
                result.push_back(path::Storage<true>(path,path::TYPE::FILE));
            }
            else if(type=="host"){
                result.push_back(path::Storage<true>(path,path::TYPE::HOST));
            }
            else throw validation_error(validation_error::invalid_option_value);
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }
    v = std::move(result);
}