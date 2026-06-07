#include "definitions/path_process.h"
#include <regex>

template<>
path::Storage<false> boost::lexical_cast(const std::string& str){
    using namespace std::string_literals;
    static std::regex regex_type("^(dir:|file:|host:).+$",
        std::regex_constants::ECMAScript|
        std::regex_constants::icase);
    path::TYPE type;
    std::smatch match;
    if (std::regex_match(str,match,regex_type)) {
        if(match[1]=="dir:")
            type=path::TYPE::DIRECTORY;
        else if(match[1]=="file:")
            type=path::TYPE::FILE;
        else if(match[1]=="host:")
            type=path::TYPE::HOST;
        else throw std::runtime_error("unknown input");
    }
    if(type==path::TYPE::HOST){
        static const std::regex regex_val("^host:([0-9.]+):([0-9]{1,5})$");
        if (std::regex_search(str.begin(),str.end(), match, regex_val)) {
            std::string host;
            uint64_t port;
            host=match[1].str();
            std::string tmp = match[2].str();
            if(std::from_chars(tmp.data(),tmp.data()+tmp.size(),port).ec==std::errc() && 
                port<=static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
            {
                std::error_code err;
                auto addr = network::make_address(host,port,err);
                if(addr.valid())
                    return path::Storage<false>::host(host,port,std::chrono::system_clock::now());
                else throw std::runtime_error("invalid host/port input");
            }
            else throw std::runtime_error("invalid option value \"port\"");
        }
        else throw std::runtime_error("invalid host input");
    }
    else{
        static const std::regex regex_val("^dir:(.+)|file:(.+)$");
        if (std::regex_match(str.begin(),str.end(), match, regex_val)) {
            std::string path=match[1].str();
            if(path.starts_with("~"s+fs::path::preferred_separator))
                path = std::string(getenv("HOME"))+fs::path::preferred_separator+path.substr(2);
            if(type==path::TYPE::FILE)
                return path::Storage<false>::file(
                    path,std::chrono::system_clock::now());
            else return path::Storage<false>::directory(
                    path,std::chrono::system_clock::now());
        }
        else throw std::runtime_error("invalid input");
    }
}

template<bool VIEW>
std::string boost::lexical_cast(const path::Storage<VIEW>& input){
    std::string result;
    switch (input.type_)
    {
        case path::TYPE::FILE:
            result+="file:";
            break;
        case path::TYPE::DIRECTORY:
            result+="dir:";
            break;
        case path::TYPE::HOST:
            result+="host:";
            break;
        default:
            break;
    }
    result+=input.path_;
    if(input.type_==path::TYPE::HOST)
        result+=" port:"+std::get<path::Additional<path::TYPE::HOST>>(input.add_);
    return result;
}