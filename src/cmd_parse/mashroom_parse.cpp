#include "cmd_parse/mashroom_parse.h"

namespace parse{
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Extract::instance().parse(input);
    }
    ErrorCode capitalize_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Capitalize::instance().parse(input);
    }
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Integrity::instance().parse(input);
    }
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Contains::instance().parse(input);
    }
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Config::instance().parse(input);
    }
    ErrorCode save_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = Capitalize::instance().parse(input);
    }
    ErrorCode help_notifier(const std::vector<std::string>& input) noexcept{

    }
    ErrorCode exit_notifier(const std::vector<std::string>& input) noexcept{

    }
}