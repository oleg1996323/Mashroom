#include <network/client/message.h>
#include <network/common/def.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::client{
Extract Message<TYPE_MESSAGE::DATA_REQUEST>::prepare_and_check_integrity_extractor(ErrorCode& err) const{
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
bool Message<TYPE_MESSAGE::SERVER_STATUS>::sendto(int sock){
    send(sock,this,sizeof(*this),0);
    return true;
}
}