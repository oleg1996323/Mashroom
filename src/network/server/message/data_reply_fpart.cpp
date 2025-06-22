#include "network/server/message/data_reply_fpart.h"

namespace network{
    MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>::MessageAdditional(ErrorCode& err,
                                        const fs::path& filename,
                                        uint64_t file_part_sz,uint64_t offset = 0,
                                        uint32_t chunk=UINT16_MAX,
                                        server::Status status=server::Status::READY){}


    bool next_chunk(SendingFileInstance& a_t){
        assert(a_t.fdescriptor>=0);
        if(errno!=0){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"access error to file",AT_ERROR_ACTION::CONTINUE);
            return false;
        }
        a_t.offset_+=a_t.chunk_;
        if(a_t.offset_>=a_t.file_sz_){
            if(munmap(a_t.from_mapping_,a_t.chunk_)==-1){
                a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
                return false;
            }
        }
        if(munmap(a_t.from_mapping_,a_t.chunk_)==-1){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            return false;
        }
        a_t.from_mapping_ = (char*)mmap(NULL,a_t.chunk_,PROT_READ,MAP_PRIVATE|MAP_LOCKED,a_t.fdescriptor,a_t.offset_);
        if(!a_t.from_mapping_){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            return false;
        }
        else return true;
    }
}