#include <time_separation_parse.h>

ErrorCode time_separation_parse(std::string_view arg, AbstractTimeSeparation& obj){
    ErrorCode err = ErrorCode::NONE;
    auto interval = get_time_offset_from_token(arg,err);
    if(!interval.has_value())
        return err;
    obj.set_offset_time_interval(interval.value());
    return ErrorCode::NONE;
}
std::string_view commands_from_time_separation(std::string_view arg,ErrorCode& err){
    if(!get_time_offset_from_token(arg,err).has_value())
        return {};
    else return arg;
}

