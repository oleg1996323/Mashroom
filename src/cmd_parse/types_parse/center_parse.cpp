#include "cmd_parse/types_parse/center_parse.h"
#include <ranges>
#include "grib1/cast/center.h"

namespace parse{
    std::expected<Organization,osterlib::ContextedError> center_notifier(const std::vector<std::string>& input) noexcept{
        auto centers = multitoken_approx_match_center(input);
        //if abbreviation
        if(centers.empty())
        {
            osterlib::ContextedError ctx_err(mashroom::errc::command_input_error);
            ctx_err.with_field("at","center resolver")
            .with_field("token","");
            return std::unexpected(std::move(ctx_err));
        }
        else if(centers.size()==1)
            return centers.front();
        else{
            osterlib::ContextedError ctx_err(mashroom::errc::command_input_error,"matched more than 1 center");
            ctx_err.with_field("at","center resolver");
            for(auto& center:centers)
                ctx_err.with_field("matched",center_to_abbr(center));
            return std::unexpected(std::move(ctx_err));
        }
    }
}