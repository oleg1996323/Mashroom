#include "cmd_parse/types_parse/center_parse.h"
#include <ranges>

namespace parse{
    std::expected<Organization,ErrorCode> center_notifier(const std::vector<std::string>& input) noexcept{
        auto centers = multitoken_approx_match_center(input);
        //if abbreviation
        if(centers.empty())
            return std::unexpected(ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "not matched center",AT_ERROR_ACTION::CONTINUE,input.front()));
        else if(centers.size()==1)
            return centers.front();
        else{
            std::cout<<"Matched more than 1 center:"<<std::endl;
            std::cout<<centers_to_txt(centers)<<std::endl;
            return std::unexpected(ErrorCode::INTERNAL_ERROR);
        }
    }
}