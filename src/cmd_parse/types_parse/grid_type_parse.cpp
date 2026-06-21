#include "types_parse/grid_type_parse.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "cast/grid.h"


#include "parsing.h"
#include "grib1/include/sections/grid/grid.h"

template<>
std::string_view boost::lexical_cast(const RepresentationType& input){
    return grid_to_abbr(input);
}

template<>
RepresentationType boost::lexical_cast(const std::string& input){
    auto grid_tmp = from_chars<int>(input);
    if(!grid_tmp.has_value() || grid_tmp.value()<0){
        throw boost::bad_lexical_cast();
    }
    else return static_cast<RepresentationType>(grid_tmp.value());
}

std::expected<RepresentationType,ErrorCode> parse::grid_notifier(const std::vector<std::string>& input) noexcept{
    auto grids = multitoken_approx_match_grid(input);
    //if abbreviation
    if(grids.empty())
        return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "not matched grid",AT_ERROR_ACTION::CONTINUE,input.front()));
    else if(grids.size()==1)
        return grids.front();
    else{
        std::cout<<"Matched more than 1 grid:"<<std::endl;
        std::cout<<grid_to_txt(grids)<<std::endl;
        return std::unexpected(ErrorCode::INTERNAL_ERROR);
    }
}