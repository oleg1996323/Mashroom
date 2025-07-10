#include "cmd_parse/path_parse.h"

void boost::program_options::validate(boost::any& v,
              const std::vector<std::string>& values,
              path::Storage<true>* target_type, int)
{
    static regex r("^(dir|host|file)=(.+)$");
    using namespace boost::program_options;

    // Make sure no previous assignment to 'a' was made.
    validators::check_first_occurrence(v);
    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const string& s = validators::get_single_string(values);

    // Do regex match and convert the interesting part to
    // int.
    smatch match;
    if (regex_match(s, match, r)) {
        auto type = lexical_cast<std::string_view>(match[1]);
        auto path = lexical_cast<std::string_view>(match[2]);
        if(type=="dir"){
            v = any(path::Storage<true>(path,path::TYPE::DIRECTORY));
        }
        else if(type=="file"){
            v = any(path::Storage<true>(path,path::TYPE::FILE));
        }
        else if(type=="host"){
            v = any(path::Storage<true>(path,path::TYPE::HOST));
        }
        else throw validation_error(validation_error::invalid_option_value);
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}