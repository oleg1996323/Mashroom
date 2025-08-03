#include "CLI/CLInavig.h"
#include "linenoise.h"

void CLIHandler::__add_to_history__(std::string_view str) noexcept{
    linenoiseHistoryAdd(str.data());
}
bool CLIHandler::save_history() const noexcept{
    return linenoiseHistorySave(hist_path_.c_str())==0?true:false;
}
void CLIHandler::set_history_length(uint16_t length) noexcept{
    linenoiseHistorySetMaxLen(length);
}
bool CLIHandler::load_history() noexcept{
    return linenoiseHistoryLoad(hist_path_.c_str())==0?true:false;
}
std::string_view CLIHandler::__input__(std::string_view input) noexcept{
    if(line)
        free(line);
    line = linenoise(std::string_view(input.begin(),input.end()).data());
    if(line==nullptr)
        return std::string_view();
    else {
        add_to_history(std::string_view(line));
        save_history();
    }
    return std::string_view(line,line+std::strlen(line));
}