#include "network/server/message/data_reply_finfo.h"
#include "network/common/utility.h"

namespace network::server{
    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::serialize_impl() const{
        auto& buf = buffer();
        if(!buf.empty())
            buf.clear();
        assert(!filename.empty() && filename.size()==fn_sz && file_hash_!=0);
        buf.reserve(sizeof(fn_sz)+fn_sz+sizeof(file_sz)+sizeof(file_hash_));
        buf.resize(0);
        decltype(fn_sz) fn_sz_n = network::utility::htonll(fn_sz);
        decltype(file_sz) file_sz_n = network::utility::htonll(file_sz);
        decltype(file_hash_) file_hash_n = htonl(file_hash_);
        buf.insert(buf.end(),(char*)&fn_sz_n,(char*)&fn_sz_n+sizeof(fn_sz_n));
        buf.insert(buf.end(),filename.data(),filename.data()+fn_sz);
        buf.insert(buf.end(),(char*)&file_sz_n,(char*)&file_sz_n+sizeof(file_sz_n));
        buf.insert(buf.end(),(char*)&file_hash_n,(char*)&file_hash_n+sizeof(file_hash_n));
    }
    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::deserialize_impl(){
        auto& buf = buffer();
        if(!buf.empty())
            fn_sz = network::utility::ntohll(*(decltype(fn_sz)*)buf.data());
        else
            return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"empty message",AT_ERROR_ACTION::CONTINUE);
        if(buf.size()==sizeof(fn_sz)+fn_sz+sizeof(file_sz)+sizeof(file_hash_)){
            if(fn_sz==0)
                return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"empty filename",AT_ERROR_ACTION::CONTINUE);
            else
                filename = std::string_view(buf.data()+sizeof(fn_sz),fn_sz);
            file_sz = network::utility::ntohll(*(decltype(file_sz)*)(buf.data()+sizeof(fn_sz)+fn_sz));
            if(file_sz==0)
                return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"empty file",AT_ERROR_ACTION::CONTINUE);
            file_hash_ = ntohl(*(decltype(file_hash_)*)(buf.data()+sizeof(fn_sz)+fn_sz+sizeof(file_sz)));
            if(file_hash_==0)
                return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"incorrect hash",AT_ERROR_ACTION::CONTINUE);
            return ErrorCode::NONE;
        }
        else return ErrorPrint::print_error(ErrorCode::SERVER_RECEIVING_MSG_ERROR,"invalid message size",AT_ERROR_ACTION::CONTINUE);
    }
    Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::Message(const fs::path& file_path, server::Status status):
    Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::__Message__<TYPE_MESSAGE::DATA_REPLY_FILEINFO>(
        status,sizeof(fn_sz)+fn_sz+sizeof(file_sz)
    ){} 
}