#include "network/server/message/msgdef.h"
#include "network/server/message/message.h"

namespace network::server{

template<TYPE_MESSAGE MSG>
void __Message__<MSG>::__serialize_base_msg__(const __Message__<MSG>& msg){
    auto& buf = msg.buffer();
    buf.insert(buf.end(),(__Message__<MSG>*)&msg,(__Message__<MSG>*)&msg+sizeof(&msg));
}
template<TYPE_MESSAGE MSG>
__Message__<MSG> __Message__<MSG>::__deserialize_base_msg__(const std::vector<char>& buf){
    data_sz_ = *(uint64_t*)(buf.data());
    status_ = *(server::Status*)(buf.data()+sizeof(uint64_t)+sizeof(type_msg_));
}
template<TYPE_MESSAGE MSG>
__Message__<MSG> __Message__<MSG>::__deserialize_base_msg__(std::vector<char>&& buf){
    data_sz_ = *(uint64_t*)(buf.data());
    status_ = *(server::Status*)(buf.data()+sizeof(uint64_t)+sizeof(type_msg_));
}
template<TYPE_MESSAGE MSG>
__Message__<MSG>::__Message__(server::Status status,size_t data_sz):
data_sz_(data_sz),status_(status){
    CashedData::add(this);
}
template<TYPE_MESSAGE MSG>
void __Message__<MSG>::serialize(const Message<MSG>& msg)
{
    if(!msg.buffer().empty())
        msg.buffer().clear();
    __serialize_base_msg__();
    msg.serialize_impl();
}
template<TYPE_MESSAGE MSG>
Message<MSG> __Message__<MSG>::deserialize(const std::vector<char>& buf){
    Message<type_msg_> result_msg(__deserialize_base_msg__(buf));
    result_msg.deserialize_impl();
    return result_msg;
}
template<TYPE_MESSAGE MSG>
Message<MSG> __Message__<MSG>::deserialize(std::vector<char>&& buf){
    Message<type_msg_> result_msg(__deserialize_base_msg__(std::move(buf)));
    result_msg.deserialize_impl();
    return result_msg;
}
template<TYPE_MESSAGE MSG>
void __Message__<MSG>::assign_buffer(std::vector<char>&& buf) noexcept{
    if(&buf!=msg.buffer())
        msg.buffer().swap(buf);
}
template<TYPE_MESSAGE MSG>
void __Message__<MSG>::assign_buffer(const std::vector<char>& buf) const{
    if(&buf!=msg.buffer())
        msg.buffer() = buf;
}
//add assignment buffer
template<TYPE_MESSAGE MSG>
std::vector<char>& __Message__<MSG>::buffer() const{
    return CashedData::get(this);
}
template<TYPE_MESSAGE MSG>
server::Status __Message__<MSG>::get_status(const std::vector<char>& buf){
    if(!buf.empty())
        return *(server::Status*)(buf.data()+sizeof(TYPE_MESSAGE));
}
template<TYPE_MESSAGE MSG>
size_t __Message__<MSG>::get_data_size(const std::vector<char>& buf){
    if(!buf.empty())
        return *(size_t*)(buf.data()+sizeof(TYPE_MESSAGE)+sizeof(server::Status));
}
template<TYPE_MESSAGE MSG>
__Message__<MSG>::~__Message__(){
    CashedData::erase(this);
}
template<TYPE_MESSAGE MSG>
server::Status __Message__<MSG>::get_status() const{
    return status_;
}
template<TYPE_MESSAGE MSG>
TYPE_MESSAGE __Message__<MSG>::get_msg_t() const{
    return type_msg_;
}
template<TYPE_MESSAGE MSG>
size_t __Message__<MSG>::data_size() const{
    return data_sz_;
}
}