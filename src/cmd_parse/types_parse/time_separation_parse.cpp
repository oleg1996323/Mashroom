#include "time_separation_parse.h"

namespace boost::program_options{

void validate(boost::any& v,
              const std::vector<std::string>& values,
              utc_tp* target_type, int)
{
    validators::check_first_occurrence(v);
    v = lexical_cast<utc_tp>(validators::get_single_string(values));
}

}