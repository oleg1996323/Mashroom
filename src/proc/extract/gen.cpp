#include "proc/extract/gen.h"

std::string generate_format_from_time_period(const TimePeriod& t_off) noexcept{
    auto time_unit = t_off.min_valuable();
    switch(time_unit){
    case TIME_UNITS::SECONDS:
        return "{:%Y_%m_%d_%h_%M_%S}";
        break;
    case TIME_UNITS::MINUTES:
        return "{:%Y_%m_%d_%h_%M}";
        break;
    case TIME_UNITS::HOURS:
        return "{:%Y_%m_%d_%h}";
        break;
    case TIME_UNITS::DAYS:
        return "{:%Y_%m_%d}";
        break;
    case TIME_UNITS::MONTHS:
        return "{:%Y_%m}";
        break;
    case TIME_UNITS::YEARS:
        return "{:%Y}";
        break;
    default:
        return "{:%Y_%m_%d_%h_%M_%S}";
        break;
    }
}

std::string get_file_fmt(OutputDataFileFormats fmt) noexcept{
    OutputDataFileFormats fmt_tmp = fmt&~OutputDataFileFormats::ARCHIVED;
    switch(fmt_tmp){
        case OutputDataFileFormats::TXT_F:
            return ".txt";
            break;
        case OutputDataFileFormats::BIN_F:
            return ".bin";
            break;
        case OutputDataFileFormats::JSON_F:
            return ".json";
        default:
            return ".txt";
            break;
    }
}