#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <charconv>
#include <stdexcept>

#include "types/coord.h"
#include "cmd_def.h"
#include "sys/error_print.h"
#include "types/time_interval.h"
#include "definitions/def.h"
#include <set>
#include <type_traits>
#include "sys/error_code.h"
#include <expected>
#include "parsing.h"


std::expected<boost::program_options::parsed_options,ErrorCode> try_parse(const boost::program_options::options_description& opt_desc,const std::vector<std::string>& args);
ErrorCode try_notify(boost::program_options::variables_map& vm);

template<typename IterCheck,typename UniqueIter>
UniqueIter contains_unique_value(IterCheck check1, IterCheck check2, UniqueIter unique1,UniqueIter unique2){
    bool contains_unique = false;
    for(UniqueIter iter = unique1;iter!=unique2;++iter){
        ptrdiff_t count = std::count(check1,check2,*iter);
        if(count==1)
            if(contains_unique==false)
                contains_unique=true;
            else return iter;
        else if(count>1)
            return iter;
        else continue;
    }
    return unique2;
}

template<typename IterCheck,typename UniqueIter,typename ConvertString>
UniqueIter contains_unique_value(IterCheck check1, IterCheck check2, UniqueIter unique1,UniqueIter unique2,ConvertString converter){
    bool contains_unique = false;
    for(UniqueIter iter = unique1;iter!=unique2;++iter){
        ptrdiff_t count = std::count(check1,check2,converter(*iter));
        if(count==1)
            if(contains_unique==false)
                contains_unique=true;
            else return iter;
        else if(count>1)
            return iter;
        else continue;
    }
    return unique2;
}