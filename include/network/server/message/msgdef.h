#pragma once
#include <optional>
#include <network/common/def.h>
#include <network/common/message_handler.h>
#include <extract.h>
#include <program/data.h>
#include <network/server/message/cashed_data.h>
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::server{

/**
 * @brief network::server::TYPE_MESSAGE - type of server-side sent messages.
 * 
 * @details DATA_REPLY_FILEINFO: Message about the file being sent. If receiving Client send TYPE_MESSAGE::DATA_TRANSACTION with ACCEPT
 *          a sequence of messages of TYPE_MESSAGE::DATA_REPLY_FILE_PART will be sent by server.
 * @details SERVER_STATUS: Message about the server status. It is sending by server automatically while the connection is established but
 *          there was nothing requeted by client.
 * @details DATA_REPLY_CAPITALIZE: Message with information about shared data ready to be sent by capitalize request. 
 *          If receiving Client send TYPE_MESSAGE::DATA_TRANSACTION with ACCEPT a sequence of messages of 
 *          TYPE_MESSAGE::DATA_REPLY_FILE_PART will be sent by server.
 * @details ERROR: Message with error code. This message is sent by Server when something is gone wrong.
 * @details PROGRESS: This message is sent during the entire time that the remote process is running at the Client's request.
 * @details DATA_REPLY_FILEPART: Message with file-part information and data.
 * @details VERSION: Message with the program version for backward compatibility of network transactions.
 * @details DATA_REPLY_CAPITALIZE_REF: Message with information about hosts data owning.
 * 
 */
enum class TYPE_MESSAGE : int{
    DATA_REPLY_FILEINFO,
    SERVER_STATUS,
    DATA_REPLY_CAPITALIZE,
    ERROR,
    PROGRESS,
    DATA_REPLY_FILEPART,
    VERSION,
    DATA_REPLY_CAPITALIZE_REF,
    DATA_REPLY_EXTRACT
};

template<TYPE_MESSAGE>
class Message;

template<TYPE_MESSAGE MSG>
struct __Message__{
private:
    static void __serialize_base_msg__(const __Message__<MSG>& msg);
    static __Message__<MSG> __deserialize_base_msg__(const std::vector<char>& buf);
    static __Message__<MSG> __deserialize_base_msg__(std::vector<char>&& buf);
protected:
    __Message__(server::Status status,size_t data_sz);
    __Message__(__Message__&& other) noexcept{
        *this=std::move(other);
    }
    __Message__<MSG>& operator=(__Message__<MSG>&& other){
        if(this!=&other){
            data_sz_= other.data_sz_;
            status_ = other.status_;
            buffer().swap(other.buffer());
        }
        return *this;
    }
    uint64_t data_sz_ = 0;
    static constexpr TYPE_MESSAGE type_msg_ = MSG;
    network::server::Status status_=network::server::Status::READY;
    void assign_buffer(std::vector<char>&& buf) noexcept;
    void assign_buffer(const std::vector<char>& buf) const;
    //add assignment buffer
    std::vector<char>& buffer() const;
    static server::Status get_status(const std::vector<char>& buf);
    static size_t get_data_size(const std::vector<char>& buf);
public:
    ~__Message__();
    server::Status get_status() const;
    TYPE_MESSAGE get_msg_t() const;
    size_t data_size() const;
    static void serialize(const Message<MSG>& msg);
    static Message<MSG> deserialize(const std::vector<char>& buf);
    static Message<MSG> deserialize(std::vector<char>&& buf);
};

template<TYPE_MESSAGE MSG>
constexpr size_t base_msg_sz = sizeof(__Message__<MSG>);
}