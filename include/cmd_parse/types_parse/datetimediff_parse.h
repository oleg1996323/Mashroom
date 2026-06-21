#pragma once
#include "types/time_interval.h"
#include <CLI/CLI.hpp>

namespace CLI {
    namespace detail {
        template <>
        bool lexical_cast<DateTimeDiff>(const std::string& input, DateTimeDiff& output);
        template<>
        std::string to_string(const DateTimeDiff& val);
        template <typename DUR>
        requires IsDuration<DUR>
        bool lexical_cast(const std::string& input, utc_tp_t<DUR>& output){
            try{
                output = boost::lexical_cast<utc_tp_t<DUR>>(input);
            }
            catch(...){
                return false;
            }
        }
        template<typename DUR>
        requires(IsDuration<DUR>)
        std::string to_string(const utc_tp_t<DUR>& val){
            return boost::lexical_cast<std::string>(val);
        }
    }
}