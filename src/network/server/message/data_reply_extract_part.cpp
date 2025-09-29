#include "network/server/message/data_reply_extract_part.h"
#include "sys/error_print.h"

namespace network{
    MessageAdditional<network::Server_MsgT::DATA_REPLY_EXTRACT>::MessageAdditional(const fs::path& file_path, server::Status status)
    {
        if(file_path.empty())
            throw std::invalid_argument(ErrorPrint::message(ErrorCode::INVALID_ARGUMENT,"empty file path"));
        if(fs::exists(file_path) && fs::is_regular_file(file_path))
            filename_ = file_path.string();
        else
            throw std::invalid_argument(ErrorPrint::message(ErrorCode::FILE_X1_DONT_EXISTS,"",file_path.string()));
        status_ = status;
    }
}