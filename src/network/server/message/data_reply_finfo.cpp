#include "network/server/message/data_reply_finfo.h"
#include "network/common/utility.h"

namespace network{
    MessageAdditional<Server_MsgT::DATA_REPLY_FILEINFO>::MessageAdditional(ErrorCode& err,
        server::Status status,const fs::path& file_path, uint64_t offset,uint64_t size):
        status_(status),filename_(file_path),offset_(offset),file_sz_(size){
        
    } 
}