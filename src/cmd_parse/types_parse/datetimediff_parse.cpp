#include "cmd_parse/types_parse/datetimediff_parse.h"

namespace CLI {
    namespace detail {
        template <>
        bool lexical_cast<DateTimeDiff>(const std::string& input, DateTimeDiff& output) {
            try{
                output = boost::lexical_cast<DateTimeDiff>(input);
                return true;
            }
            catch(...){
                return false;
            }
        }
        template<>
        std::string to_string(const DateTimeDiff& val){
            return boost::lexical_cast<std::string>(val);
        }
    }
}