#include "types_parse/grid_type_parse.h"
#include "sys/error.h"
#include "grib1/cast/grid.h"


#include "OsterLib/parsing.h"
#include "grib1/sections.h"

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

std::expected<RepresentationType,osterlib::ContextedError> parse::grid_notifier(const std::vector<std::string>& input) noexcept{
    auto grids = multitoken_approx_match_grid(input);
    //if abbreviation
    if(grids.empty()){
        osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,"grid type not matched");
        ctx_err.with_field("at","grid type resolver");
        for(auto& arg_in:input)
            ctx_err.with_field("input",arg_in);
        return std::unexpected(std::move(ctx_err));
    }
    else if(grids.size()==1)
        return grids.front();
    else{
        osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,"matched more than 1 center");
        ctx_err.with_field("at","grid type resolver");
        for(auto& grid:grids)
            ctx_err.with_field("matched",grid_to_text(grid));
        return std::unexpected(std::move(ctx_err));
    }
}