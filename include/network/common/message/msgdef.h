#pragma once
#include <optional>
#include <network/common/def.h>
#include <network/common/message/message_handler.h>
#include <extract.h>
#include <program/data.h>
#include <network/common/message/cashed_data.h>
#include <network/common/def.h>
using namespace std::chrono;
namespace fs = std::filesystem;

namespace network{

    class __MessageBase__{
        protected:
        void assign_buffer(std::vector<char>&& buf) noexcept;
        void assign_buffer(const std::vector<char>& buf) const;
        std::vector<char>& __buffer__() const;
        public:
        const std::vector<char>& buffer() const{
            return __buffer__();
        }
    };

    template<Side S>
    struct TYPE_MESSAGE;

    /**
     * @brief network::server::TYPE_MESSAGE - type of server-side sent messages.
     * 
     * @details DATA_REPLY_FILEINFO: Message about the file being sent. If receiving Client send client::TYPE_MESSAGE::TRANSACTION with ACCEPT
     *          a sequence of messages of TYPE_MESSAGE::DATA_REPLY_FILE_PART will be sent by server.
     * @details SERVER_STATUS: Message about the server status. It is sending by server automatically while the connection is established but
     *          there was nothing requeted by client.
     * @details DATA_REPLY_CAPITALIZE: Message with information about shared data ready to be sent by capitalize request. 
     *          If receiving Client send client::TYPE_MESSAGE::TRANSACTION with ACCEPT a sequence of messages of 
     *          TYPE_MESSAGE::DATA_REPLY_FILE_PART will be sent by server.
     * @details ERROR: Message with error code. This message is sent by Server when something is gone wrong.
     * @details PROGRESS: This message is sent during the entire time that the remote process is running at the Client's request.
     * @details DATA_REPLY_FILEPART: Message with file-part information and data.
     * @details VERSION: Message with the program version for backward compatibility of network transactions.
     * @details DATA_REPLY_CAPITALIZE_REF: Message with information about hosts data owning.
     * 
     */
    template<>
    struct TYPE_MESSAGE<Side::SERVER>{
        constexpr static Side side(){
            return Side::SERVER;
        }
        enum type:int{
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
    };

    template<Side S,TYPE_MESSAGE<S>>
    class Message;

    template<Side S,TYPE_MESSAGE<S> MSG>
    struct AssociatedData;

    template<Side S,TYPE_MESSAGE<S> MSG>
    struct __Message__:public __MessageBase__{
    private:
        static void __serialize_base_msg__(const __Message__& msg);
        static __Message__ __deserialize_base_msg__(const std::vector<char>& buf);
        static __Message__ __deserialize_base_msg__(std::vector<char>&& buf);
    protected:
        __Message__(server::Status status,size_t data_sz);
        __Message__(__Message__&& other) noexcept{
            *this=std::move(other);
        }
        __Message__& operator=(__Message__&& other){
            if(this!=&other){
                data_sz_= other.data_sz_;
                status_ = other.status_;
                __buffer__().swap(other.__buffer__());
            }
            return *this;
        }
        uint64_t data_sz_ = 0;
        static constexpr TYPE_MESSAGE<S> type_msg_ = MSG;
        network::server::Status status_=network::server::Status::READY;
        static server::Status get_status(const std::vector<char>& buf);
        static size_t get_data_size(const std::vector<char>& buf);
    public:
        ~__Message__();
        server::Status get_status() const;
        constexpr static TYPE_MESSAGE<S> get_msg_t(){
            return type_msg_;
        }
        size_t data_size() const;
        static void serialize(const Message<MSG>& msg);
        static Message<S,MSG> deserialize(const std::vector<char>& buf);
        static Message<S,MSG> deserialize(std::vector<char>&& buf);
        constexpr static size_t header_base_sz(){
            return sizeof(__Message__<MSG>);
        }
    };

    /// @todo erase
    template<TYPE_MESSAGE MSG>
    constexpr size_t base_msg_sz = __Message__<MSG>::header_base_sz();

    using namespace std::chrono;
    namespace fs = std::filesystem;
    /**
     * @brief network::client::TYPE_MESSAGE - type of client-side sent messages.
     * 
     * @details DATA_REQUEST: Client request of specified (matched) data to be extracted from server-device.
     * @details SERVER_STATUS: Request of the server status (if connection can be established).
     * @details CAPITALIZE: Capitalize request with sharing some part of data (specified by memory (in either KB, or MB, or GB, or TB))
     * @details CAPITALIZE_REF: Capitalize request (only references about owners and owned information)
     * @details TRANSACTION: Client replied message to server's transaction-request (for example, sending file)
     * 
     */
    template<>
    struct TYPE_MESSAGE<Side::CLIENT>{
        constexpr static Side side(){
            return Side::CLIENT;
        }
        enum type:int{
            DATA_REQUEST,
            SERVER_STATUS,
            CAPITALIZE,
            CAPITALIZE_REF,
            TRANSACTION
        };
    };

    template<network::TYPE_MESSAGE MSG>
    struct AssociatedData;

    template<TYPE_MESSAGE>
    class Message;

    template<TYPE_MESSAGE MSG>
    struct __Message__:public __MessageBase__{
    private:
        static void __serialize_base_msg__(const __Message__<MSG>& msg);
        static __Message__<MSG> __deserialize_base_msg__(const std::vector<char>& buf);
        static __Message__<MSG> __deserialize_base_msg__(std::vector<char>&& buf);
    protected:
        __Message__(size_t data_sz);
        __Message__(__Message__&& other) noexcept{
            *this=std::move(other);
        }
        __Message__<MSG>& operator=(__Message__<MSG>&& other){
            if(this!=&other){
                data_sz_= other.data_sz_;
                __buffer__().swap(other.__buffer__());
            }
            return *this;
        }
        uint64_t data_sz_ = 0;
        static constexpr TYPE_MESSAGE type_msg_ = MSG;
        static size_t get_data_size(const std::vector<char>& buf);

    public:
        ~__Message__();
        server::Status get_status() const;
        constexpr static TYPE_MESSAGE get_msg_t(){
            return type_msg_;
        }
        size_t data_size() const;
        static void serialize(const Message<MSG>& msg);
        static Message<MSG> deserialize(const std::vector<char>& buf);
        static Message<MSG> deserialize(std::vector<char>&& buf);
        constexpr static size_t header_base_sz(){
            return sizeof(__Message__<MSG>);
        }
    };

    /// @todo erase
    template<TYPE_MESSAGE MSG>
    constexpr size_t base_msg_sz = __Message__<MSG>::header_base_sz();
}