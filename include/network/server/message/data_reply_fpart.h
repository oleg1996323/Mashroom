#pragma once
#include "progress_base.h"
#include <network/common/message/msgdef.h>
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"
//#include "blake3.h"

namespace network{
template<>
struct MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>{
    server::Status status_;
    uint64_t file_part_sz_ = 0;
    uint64_t offset_ = 0;
    size_t file_hash_ = 0;
    //const size_t current_hash_ = 0; //TODO: std::hash<fs::path> is temporary solution
    MessageAdditional(ErrorCode& err,const fs::path& filename,
                uint64_t file_part_sz,uint64_t offset = 0, 
                uint32_t chunk=UINT16_MAX,
                server::Status status=server::Status::READY);
    MessageAdditional() = default;
};

struct SendingFileInstance{
    uint64_t file_sz_ = 0;
    char* from_mapping_ = nullptr;
    uint64_t offset_ = 0;
    uint32_t chunk_ = 0;
    int fdescriptor = -1;
    ErrorCode err_ = ErrorCode::NONE;

    SendingFileInstance(const fs::path& filename,uint32_t chunk,uint64_t offset, uint64_t size = std::numeric_limits<uint64_t>::max()){
        if(!fs::exists(filename)){
            err_ = ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
            return;
        }
        if(chunk==0)
            chunk_=UINT16_MAX;
        else chunk_ = chunk;
        if(fdescriptor==-1)
            fdescriptor = open(filename.c_str(),O_RDONLY);
        if(fdescriptor<0){
            err_ = ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
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

    ~SendingFileInstance(){
        if(from_mapping_)
            munmap(from_mapping_,chunk_);
        if(fdescriptor>-1)
            close(fdescriptor);
    }
};

bool next_chunk(SendingFileInstance& a_t);
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.file_part_sz_,msg.offset_,msg.file_hash_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.file_part_sz_,msg.offset_,msg.file_hash_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.file_part_sz_,msg.offset_,msg.file_hash_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.file_part_sz_,msg.offset_,msg.file_hash_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>>{
        using type = MessageAdditional<Server_MsgT::DATA_REPLY_FILEPART>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.file_part_sz_,msg.offset_,msg.file_hash_);
        }
    };
}