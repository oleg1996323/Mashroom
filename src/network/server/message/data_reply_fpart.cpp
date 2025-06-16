#include "network/server/message/data_reply_fpart.h"
//#include "blake3.h"

namespace network::server{
    Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::Message(
                                        associated_t& a_t,const fs::path& filename,
                                        uint64_t file_part_sz,uint64_t offset = 0,
                                        uint32_t chunk=UINT16_MAX,
                                        server::Status status=server::Status::READY):
    __Message__<TYPE_MESSAGE::DATA_REPLY_FILEPART>(
        status,
        sizeof(file_part_sz_)+
        sizeof(offset_)),
        file_part_sz_(file_part_sz),
        offset_(offset),
        file_hash_(std::hash<fs::path>{}(filename.filename()))
    {
        if(!fs::exists(filename)){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
            return;
        }
        if(chunk==0)
            a_t.chunk_=UINT16_MAX;
        else a_t.chunk_ = chunk;
        if(a_t.fdescriptor==-1)
            a_t.fdescriptor = open(filename.c_str(),O_RDONLY);
        if(a_t.fdescriptor<0){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,filename.c_str());
            return;
        }
        a_t.file_sz_=lseek(a_t.fdescriptor,offset,SEEK_END);
        lseek(a_t.fdescriptor,offset,SEEK_SET);
        a_t.from_mapping_ = (char*)mmap(nullptr,a_t.chunk_,PROT_READ,MAP_PRIVATE|MAP_LOCKED,a_t.fdescriptor,0);
            
        if(a_t.from_mapping_==nullptr || errno!=0)
            a_t.err_=ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"cannot map file memory",AT_ERROR_ACTION::CONTINUE);
        madvise(a_t.from_mapping_, a_t.chunk_, MADV_SEQUENTIAL);
    }
    Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::Message(__Message__&& other_base):__Message__(std::move(other_base)){}
    
    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::serialize_impl() const{
        decltype(file_part_sz_) file_part_sz_n = network::utility::htonll(file_part_sz_);
        decltype(offset_) offset_n = network::utility::htonll(offset_);
        decltype(file_hash_) file_hash_n = htonl(file_hash_);
        auto& buf = __buffer__();
        buf.insert(buf.end(),(char*)&file_part_sz_n,(char*)(&file_part_sz_n+sizeof(file_part_sz_n)));
        buf.insert(buf.end(),(char*)&offset_n,(char*)(&offset_n+sizeof(offset_n)));
        buf.insert(buf.end(),(char*)&file_hash_n,(char*)(&file_hash_n+sizeof(file_hash_n)));
        return ErrorCode::NONE;
    }

    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::deserialize_impl(){

    }

    ErrorCode Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::next_chunk(associated_t& a_t){
        assert(a_t.fdescriptor>=0);
        if(errno!=0){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"access error to file",AT_ERROR_ACTION::CONTINUE);
            return a_t.err_;
        }
        offset_+=file_part_sz_;
        if(munmap(a_t.from_mapping_,file_part_sz_)==-1){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            return a_t.err_;
        }
        a_t.from_mapping_ = (char*)mmap(NULL,a_t.chunk_,PROT_READ,MAP_PRIVATE|MAP_LOCKED,a_t.fdescriptor,offset_);
        if(!a_t.from_mapping_){
            a_t.err_ = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            return a_t.err_;
        }
        else return ErrorCode::NONE;
    }
    uint64_t Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::offset() const{
        return offset_;
    }
    uint64_t Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::file_part_size() const{
        return file_part_sz_;
    }
    size_t Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::hash() const{
        return file_hash_;
    }
}