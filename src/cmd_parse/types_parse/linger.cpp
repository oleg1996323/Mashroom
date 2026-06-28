#include "types_parse/linger.h"
#include <regex>

namespace boost{
    template<>
    linger lexical_cast(const std::string& input){
        linger result{.l_onoff=true,.l_linger=0};
        if(input=="off")
            return result;
        else{
            std::regex reg("^[0-9]+$");
            std::smatch m;
            if(std::regex_match(input,m,reg)){
                std::string sub = m[0].str();
                int res = 0;
                auto from_res = std::from_chars(sub.data(),sub.data()+sub.size(),res);
                if(from_res.ec!=std::errc())
                    throw std::runtime_error("bad cast: linger");
                else {
                    if(res<-1)
                        return result;
                    else{
                        result.l_onoff=1;
                        result.l_linger=res;
                        return result;
                    }
                }
            }
            else throw std::runtime_error("bad cast: linger");
        }
    }
    template<>
    std::string lexical_cast(const linger& input){
        if(static_cast<bool>(input.l_onoff)==true && input.l_linger>0)
            return std::to_string(input.l_linger);
        else return "off";
    }
}

namespace CLI::detail{
    template <>
        bool lexical_cast<linger>(const std::string& input, linger& output){
            try{
                output = boost::lexical_cast<linger>(input);
                return true;
            }
            catch(const std::runtime_error& e){
                return false;
            }
        }
        template<>
        std::string to_string(const linger& input){
            return boost::lexical_cast<std::string>(input);
        }
}