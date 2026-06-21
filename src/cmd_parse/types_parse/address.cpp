#include "cmd_parse/types_parse/address.h"

namespace CLI {
namespace detail {
    template <>
    bool lexical_cast(const std::string& input, ::network::Address& output) {
        static const std::regex regex_val(
        "^(?:(?:(?:[0-9a-fA-F]{1,4}:){7,7}\
[0-9a-fA-F]{1,4}|(?:[0-9a-fA-F]{1,4}:){1,7}\
:|(?:[0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}\
|(?:[0-9a-fA-F]{1,4}:){1,5}(?::[0-9a-fA-F]{1,4}){1,2}\
|(?:[0-9a-fA-F]{1,4}:){1,4}(?::[0-9a-fA-F]{1,4}){1,3}\
|(?:[0-9a-fA-F]{1,4}:){1,3}(?::[0-9a-fA-F]{1,4}){1,4}\
|(?:[0-9a-fA-F]{1,4}:){1,2}(?::[0-9a-fA-F]{1,4}){1,5}\
|[0-9a-fA-F]{1,4}:(?:(?::[0-9a-fA-F]{1,4}){1,6})\
|:(?:(?::[0-9a-fA-F]{1,4}){1,7}|:)\
|fe80:(?::[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}\
|::(?:ffff(?::0{1,4}){0,1}:){0,1}\
(?:(?:25[0-5]|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9]).)\
{3,3}(?:25[0-5]|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9])\
|(?:[0-9a-fA-F]{1,4}:){1,4}:(?:(?:25[0-5]\
|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9]).)\
{3,3}(?:25[0-5]|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9])))\
(?::([0-9]{1,5}))?$");
        std::smatch match;
        if (std::regex_search(input.begin(),input.end(), match, regex_val)) {
            std::string host;
            uint64_t port;
            host=match[0].str();
            if(match.size()>1){
                std::string tmp=match[1].str();
                if(std::from_chars(tmp.data(),tmp.data()+tmp.size(),port).ec==std::errc() && 
                    port<=static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
                {
                    std::error_code err;
                    output = network::make_address(host,port,err);
                    if(err || output.valid())
                        return true;
                    else throw std::runtime_error("invalid host/port input");
                }
                else throw std::runtime_error("invalid option value \"port\"");
            }
            else{
                std::error_code err;
                output = network::make_address(host,0,err);
                if(output.valid())
                    return true;
                else throw std::runtime_error("invalid host input");
            }
        }
        else throw std::runtime_error("invalid host input");
    }

    
}
}

std::ostream& operator<<(std::ostream& stream,const network::Address address){
    network::print_ip_port(stream,address);
    return stream;
}

std::istream& operator>>(std::istream& stream,network::Address& address){
    std::string input;
    CLI::detail::lexical_cast(input,address);
    return stream;
}