#include <network/server/message/data_reply_finfo.h>

namespace network{
// bool MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEINFO>::sendto(int sock,const fs::path& file_send){
//     if(sock<0)
//         return false;
//     if(file_send.empty() || !fs::exists(file_send) || !fs::is_regular_file(file_send))
//         return false;
//     strcpy(finfo_.filename,file_send.c_str());
//     finfo_.data_sz = fs::file_size(file_send);
//     if(int fd=open(file_send.c_str(),O_RDONLY|O_DIRECT)!=-1){
//         int val = 1;
//         send(sock,(const void*)this,sizeof(*this),0);
//         off_t offset = 0;
//         size_t buffering = 4096*16;
//         while(offset<finfo_.data_sz){
//             ssize_t written = sendfile(sock,fd,&offset,buffering);
//             if(written==-1)
//                 ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"",AT_ERROR_ACTION::CONTINUE);
//             else offset+=written;
//         }
//         val = 0;
//         return true;
//     }
//     return false;
// }
}