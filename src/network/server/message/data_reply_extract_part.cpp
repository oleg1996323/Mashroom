#include "network/server/message/data_reply_extract_part.h"

namespace network{
    MessageAdditional<network::Server_MsgT::DATA_REPLY_EXTRACT>::MessageAdditional(ErrorCode& err,const fs::path& file_path, server::Status status)
    {
        if(file_path.empty()){
            err = ErrorPrint::print_error(ErrorCode::INVALID_ARGUMENT,"empty file path",AT_ERROR_ACTION::CONTINUE);
            return;
        }
        if(fs::exists(file_path) && fs::is_regular_file(file_path))
            filename_ = file_path.string();
        else{
            err = ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,file_path.string());
            return;
        }
        status_ = status;
    }
}