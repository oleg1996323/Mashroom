#include "cmd_parse/coord_parse.h"
#include <boost/regex.hpp>

void boost::program_options::validate(boost::any& v,const std::vector<std::string>& values,
                                    Coord* target_type,int)
{
	namespace po = boost::program_options;
	po::validators::check_first_occurrence(v);
	const std::string& s = po::get_single_string(values);
	v = lexical_cast<Coord>(s);
}

template<>
Coord boost::lexical_cast(const std::string& input){
    namespace po = boost::program_options;
	regex r = regex("^lat((-)?[0-9]+(\\.[0-9]+)?)-lon((-)?[0-9]+(\\.[0-9]+)?)$");
    smatch match;
    if(regex_match(input,match,r)){
        Coord coord;
        coord.lat_ = lexical_cast<Lat>(match[1]);
        coord.lon_ = lexical_cast<Lon>(match[3]);
        if(!is_correct_pos(coord))
            throw po::validation_error(po::validation_error::invalid_option_value,input);
        else return coord;
    }
}