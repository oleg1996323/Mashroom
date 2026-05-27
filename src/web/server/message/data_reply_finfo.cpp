#include "web/server/message/data_reply_finfo.h"
#include "utility.h"

namespace network{
    Message<network::Server_MsgT::DATA_REPLY_FILEINFO>::Message(
        server::Status status,const fs::path& file_path, uint64_t offset,uint64_t size):
        status_(status),filename_(file_path),offset_(offset),file_sz_(size){
        
    } 
}