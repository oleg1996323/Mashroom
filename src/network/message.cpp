#include <network/message.h>
#include <network/def.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace network{
Extract Message<TYPE_MESSAGE::METEO_REQUEST>::prepare_and_check_integrity_extractor(ErrorCode& err) const{
    Extract hExtract;
    if(center.has_value())
        hExtract.set_center(center.value());
    if(from.has_value())
        hExtract.set_from_date(from.value());
    if(to.has_value())
        hExtract.set_to_date(to.value());
    if(pos.has_value())
        hExtract.set_position(pos.value());
    if(fmt_.has_value())
        hExtract.set_output_format(fmt_.value());
    if(rep_t.has_value())
        hExtract.set_grid_respresentation(rep_t.value());
    if(time_off_.has_value())
        hExtract.set_offset_time_interval(time_off_.value());
    err = hExtract.properties_integrity();
    return hExtract;
}
bool Message<TYPE_MESSAGE::METEO_REPLY>::sendto(int sock,const fs::path& file_send){
    if(sock<0)
        return false;
    if(file_send.empty() || !fs::exists(file_send) || !fs::is_regular_file(file_send))
        return false;
    strcpy(finfo_.filename,file_send.c_str());
    finfo_.data_sz = fs::file_size(file_send);
    if(int fd=open(file_send.c_str(),O_RDONLY|O_DIRECT)!=-1){
        int val = 1;
        setsockopt(sock,SOL_TCP,TCP_CORK,&val,sizeof(val));
        send(sock,(const void*)this,sizeof(*this),0);
        off_t offset = 0;
        size_t buffering = 4096*16;
        while(offset<finfo_.data_sz){
            ssize_t written = sendfile(sock,fd,&offset,buffering);
            if(written==-1)
                ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"",AT_ERROR_ACTION::CONTINUE);
            else offset+=written;
        }
        val = 0;
        setsockopt(sock,SOL_TCP,TCP_CORK,&val,sizeof(val));
        return true;
    }
    return false;
}
bool Message<TYPE_MESSAGE::ERROR>::sendto(int sock,ErrorCode err, std::string msg){
    strcpy(error_message.data(),msg.data());
    err_ = err;
    send(sock,this,sizeof(*this),0);
    return true;
}
bool Message<TYPE_MESSAGE::SERVER_CHECK>::sendto(int sock){
    send(sock,this,sizeof(*this),0);
    return true;
}
}