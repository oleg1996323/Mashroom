#include "coord_parse.h"
#include <boost/regex.hpp>
#include <iostream>

void boost::program_options::validate(boost::any& v,const std::vector<std::string>& values,
                                    std::optional<Coord>* target_type,int)
{
	namespace po = boost::program_options;
	po::validators::check_first_occurrence(v);
	const std::string& s = po::get_single_string(values);
	v = lexical_cast<std::optional<Coord>>(s);
}

void boost::program_options::validate(boost::any& v,const std::vector<std::string>& values,
                                    Coord* target_type,int)
{
	namespace po = boost::program_options;
	po::validators::check_first_occurrence(v);
	const std::string& s = po::get_single_string(values);
	v = lexical_cast<Coord>(s);
}

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
    namespace po = boost::program_options;
	boost::regex r = regex("^(lat(-?[0-9]+(?:\\.[0-9]+)?)-lon(-?[0-9]+(?:\\.[0-9]+)?))|(lon(-?[0-9]+(?:\\.[0-9]+)?)-lat(-?[0-9]+(?:\\.[0-9]+)?))$");
    boost::smatch match_reg;
    if(boost::regex_match(input,match_reg,r)){
        Coord coord;
        if(!match_reg[1].str().empty()){
            std::cout<<"2: "<<match_reg[2]<<std::endl;
            std::cout<<"3: "<<match_reg[3]<<std::endl;
            coord.lat_ = lexical_cast<Lat>(match_reg[2]);
            coord.lon_ = lexical_cast<Lon>(match_reg[3]);
        }
        else{
            std::cout<<"5: "<<match_reg[5]<<std::endl;
            std::cout<<"6: "<<match_reg[6]<<std::endl;
            coord.lat_ = lexical_cast<Lon>(match_reg[5]);
            coord.lon_ = lexical_cast<Lat>(match_reg[6]);
        }
        if(!is_correct_pos(coord))
            throw po::validation_error(po::validation_error::invalid_option_value,input);
        else return coord;
    }
    else throw po::validation_error(po::validation_error::invalid_option_value,input);
}

template<>
std::string boost::lexical_cast(const Coord& input){
    std::string result = "lat"+std::to_string(input.lat_)+"-"+"lon"+std::to_string(input.lon_);
    return result;
}