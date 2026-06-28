#include "types_parse/timeval.h"
#include <regex>

namespace boost{
    template<>
    timeval lexical_cast(const std::string& input){
        timeval result{.tv_sec=0,.tv_usec=0};
        if(input=="off"){
            return result;
        }
        else{
            std::regex reg("^([0-9]+)(?:[,.]([0-9]+))?$");
            std::smatch m;
            if(std::regex_match(input,m,reg)){
                std::string sec_str = m[1].str();
                int sec = 0;
                auto sec_res = std::from_chars(sec_str.data(),sec_str.data()+sec_str.size(),sec);
                if(sec_res.ec!=std::errc())
                    throw std::runtime_error("bad cast: timeval");
                else {
                    int msec = 0;
                    if(m.size()==3){
                        std::string msec_str = m[2].str();
                        if(!msec_str.empty()){
                            auto msec_res = std::from_chars(msec_str.data(),
                                msec_str.data()+
                                ((msec_str.size()>3)?
                                3ul:msec_str.size()),msec);
                            if(msec_res.ec!=std::errc())
                                throw std::runtime_error("bad cast: timeval");
                        }
                    }
                    if(sec>0)
                        result.tv_sec=sec;
                    else result.tv_sec=0;
                    if(msec>0)
                        result.tv_usec=msec;
                    else result.tv_usec=0;
                    return result;
                }
            }
            else throw std::runtime_error("bad cast: timeval");
        }
    }
    template<>
    std::string lexical_cast(const timeval& input){
        std::string result;
        int sec = input.tv_sec+input.tv_usec/1000;
        int usec = input.tv_usec%1000;
        if(sec>0)
            result=std::to_string(sec);
        else result="0";
        if(usec>0)
            result+="."+std::to_string(usec);
        return result;
    }
}

namespace CLI::detail{
    template <>
    bool lexical_cast<timeval>(const std::string& input, timeval& output){
        try{
            output = boost::lexical_cast<timeval>(input);
            return true;
        }
        catch(const std::runtime_error& e){
            return false;
        }
    }
    template<>
    std::string to_string(const timeval& input){
        return boost::lexical_cast<std::string>(input);
    }
}