#include "path_parse.h"
#include "filesystem.h"
#include "network/common/def.h"
#include "definitions.h"

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
    for(auto& s:input){
        result.push_back(lexical_cast<path::Storage<false>>(s));
        // if(s.starts_with("port=") && 
        // s.size()>std::size(std::string_view("port=")) &&
        // result.size()>0 &&
        // result.back().type_==path::TYPE::HOST)
        //     result.back().add_.get<path::TYPE::HOST>().port_ = lexical_cast<uint16_t>(s.substr(5));
        // else
            
    }
    return result;
}

template<>
path::Storage<false> boost::lexical_cast(const std::string& input){
    using namespace std::string_literals;
    static regex r("(^(dir|file)=(.+)$)|(^host=([0-9.]+):([0-9]{1,5}))$");
    using namespace boost::program_options;
    boost::smatch match;
    if (boost::regex_match(input, match, r)) {
        std::string type;
        if(match[1].str().size()>0){
            std::string path;
            if(match[2].str().size()>0)
                match[2].str();
            else throw po::validation_error(po::validation_error::invalid_option_value,input);
            type = match[1].str();
            if(path.starts_with("~"s+fs::separator()))
                path = std::string(getenv("HOME"))+fs::separator()+path.substr(2);
            if(type=="dir")
                return path::Storage<false>::directory(path,std::chrono::system_clock::now());
            else if(type=="file")
                return path::Storage<false>::file(path,std::chrono::system_clock::now());
            else throw po::validation_error(po::validation_error::invalid_option_value,input);
        }
        else if(match[5].str().size()>0 && match[6].str().size()>0){
            try{
                if(network::is_correct_address(match[5]))
                    return path::Storage<false>::host(match[5].str(),lexical_cast<uint16_t>(match[6].str()),std::chrono::system_clock::now());
                else throw po::validation_error(po::validation_error::invalid_option_value,input);
            }
            catch(const boost::bad_lexical_cast& err){
                throw po::validation_error(po::validation_error::invalid_option_value,input);
            }
        }
        else throw po::validation_error(po::validation_error::invalid_option_value,input);
    }
    else throw po::validation_error(po::validation_error::invalid_option_value,input);
}

