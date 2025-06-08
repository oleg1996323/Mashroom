#include <network/server/message/error.h>

namespace network::server{
std::vector<char> Message<TYPE_MESSAGE::ERROR>::serialize_impl(const Message<TYPE_MESSAGE::ERROR>& msg){
    std::vector<char> result;
    result.insert(result.end(),(const char*)&msg,(const char*)&msg+sizeof(msg));
    return result;
}
}