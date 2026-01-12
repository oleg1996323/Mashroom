#include "proc/extract/gen.h"

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