#include "sys/outputdatafileformats.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

template<>
std::string boost::lexical_cast(const OutputDataFileFormats& input){
    std::string result;
    switch(input&~OutputDataFileFormats::ARCHIVED){
        case OutputDataFileFormats::BIN_F:
            result+="bin";
        break;
        case OutputDataFileFormats::DEFAULT:
        case OutputDataFileFormats::TXT_F:
            result+="txt";
        break;
        case OutputDataFileFormats::JSON_F:
            result+="json";
        default:
            throw std::invalid_argument("invalid input of output format");
    }
    if(std::underlying_type_t<OutputDataFileFormats>(input&OutputDataFileFormats::ARCHIVED)!=0)
        result+="zip";
    return result;
}

template<>
::OutputDataFileFormats boost::lexical_cast(const std::string& input){
    auto r = regex("^((txt|bin|json)(\\+zip)?|(zip\\+)?(txt|bin|json))$");
    smatch match;
    if(regex_match(input,match,r)){
        OutputDataFileFormats fmt;
        if((!match[2].str().empty() && match[2]=="txt")  || (!match[5].str().empty() && match[5]=="txt"))
            fmt|=OutputDataFileFormats::TXT_F;
        else if((!match[2].str().empty() && match[2]=="bin")  || (!match[5].str().empty() && match[5]=="bin"))
            fmt|=OutputDataFileFormats::BIN_F;
        else fmt|=OutputDataFileFormats::JSON_F;
        if(!match[3].str().empty() || !match[4].str().empty())
            fmt|=OutputDataFileFormats::ARCHIVED;
        return fmt;
    }
    else throw std::invalid_argument(input);
}