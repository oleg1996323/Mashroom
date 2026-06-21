#include "types_parse/coord_parse.h"
#include <boost/regex.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>

template<>
std::optional<Coord> boost::lexical_cast(const std::string& input){
	regex r = regex("^(lat(-?[0-9]+(?:\\.[0-9]+)?)-lon(-?[0-9]+(?:\\.[0-9]+)?))|(lon(-?[0-9]+(?:\\.[0-9]+)?)-lat(-?[0-9]+(?:\\.[0-9]+)?))$");
    smatch match;
    if(regex_match(input,match,r)){
        Coord coord;
        coord.lat_ = lexical_cast<Lat>(match[2]);
        coord.lon_ = lexical_cast<Lon>(match[3]);
        if(!is_correct_pos(coord))
            return std::nullopt;
        else return coord;
    }
    else return std::nullopt;
}

template<>
Coord boost::lexical_cast(const std::string& input){
	boost::regex r = regex("^(lat(-?[0-9]+(?:\\.[0-9]+)?)-lon(-?[0-9]+(?:\\.[0-9]+)?))|(lon(-?[0-9]+(?:\\.[0-9]+)?)-lat(-?[0-9]+(?:\\.[0-9]+)?))$");
    boost::smatch match_reg;
    if(boost::regex_match(input,match_reg,r)){
        Coord coord;
        if(!match_reg[1].str().empty()){
            // std::cout<<"2: "<<match_reg[2]<<std::endl;
            // std::cout<<"3: "<<match_reg[3]<<std::endl;
            coord.lat_ = lexical_cast<Lat>(match_reg[2]);
            coord.lon_ = lexical_cast<Lon>(match_reg[3]);
        }
        else{
            // std::cout<<"5: "<<match_reg[5]<<std::endl;
            // std::cout<<"6: "<<match_reg[6]<<std::endl;
            coord.lat_ = lexical_cast<Lon>(match_reg[5]);
            coord.lon_ = lexical_cast<Lat>(match_reg[6]);
        }
        if(!is_correct_pos(coord))
            throw std::runtime_error(input);
        else return coord;
    }
    else throw std::runtime_error(input);
}

template<>
std::string boost::lexical_cast(const Coord& input){
    std::string result = "lat"+std::to_string(input.lat_)+"-"+"lon"+std::to_string(input.lon_);
    return result;
}

namespace CLI {
    namespace detail {
        template <>
        bool lexical_cast<Coord>(const std::string& input, Coord& output) {
            try{
                output = boost::lexical_cast<Coord>(input);
                return true;
            }
            catch(...){
                return false;
            }
        }
        template<>
        std::string to_string(const Coord& val){
            return boost::lexical_cast<std::string>(val);
        }
    }
}