#include "network/server/message/data_reply_fpart.h"
#include "sys/error_print.h"

namespace network{
    MessageAdditional<network::Server_MsgT::DATA_REPLY_FILEPART>::MessageAdditional(ErrorCode& err,
                                        const fs::path& filename,
                                        uint64_t file_part_sz,uint64_t offset,
                                        uint32_t chunk,
                                        server::Status status):
                                        file_part_sz_(file_part_sz),
                                        offset_(offset),
                                        status_(status)
                                        {}

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

    SendingFileInstance::SendingFileInstance(const fs::path& filename,uint32_t chunk,uint64_t offset, uint64_t size){
        if(!fs::exists(filename)){
            err_ = ::ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
            return;
        }
        if(chunk==0)
            chunk_=UINT16_MAX;
        else chunk_ = chunk;
        if(fdescriptor==-1)
            fdescriptor = open(filename.c_str(),O_RDONLY);
        if(fdescriptor<0){
            err_ = ::ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
            return;
        }
        file_sz_=lseek(fdescriptor,offset,SEEK_END);
        if(file_sz_>offset+size)
            file_sz_=size;
        lseek(fdescriptor,offset,SEEK_SET);
        from_mapping_ = (char*)mmap(nullptr,chunk_,PROT_READ,MAP_PRIVATE|MAP_LOCKED,fdescriptor,0);
            
        if(from_mapping_==nullptr || errno!=0)
            err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"cannot map file memory",AT_ERROR_ACTION::CONTINUE);
        madvise(from_mapping_, chunk_, MADV_SEQUENTIAL);
    }

    SendingFileInstance::~SendingFileInstance(){
        if(from_mapping_)
            munmap(from_mapping_,chunk_);
        if(fdescriptor>-1)
            close(fdescriptor);
    }
}