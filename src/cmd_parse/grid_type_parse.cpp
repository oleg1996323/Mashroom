#include "cmd_parse/grid_type_parse.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "cmd_parse/functions.h"

void boost::program_options::validate(boost::any& v,const std::vector<std::basic_string<char>>& values,
                                    RepresentationType* target_type,int)
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(v);
    const std::string& s = po::validators::get_single_string(values);
    v = lexical_cast<RepresentationType>(s);
}

#include "parsing.h"
template<>
RepresentationType boost::lexical_cast(const std::string& input){
    auto grid_tmp = from_chars<int>(input);
    if(!grid_tmp.has_value() || grid_tmp.value()<0){
        throw boost::bad_lexical_cast();
    }
    else return static_cast<RepresentationType>(grid_tmp.value());
}