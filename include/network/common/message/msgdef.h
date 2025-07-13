#pragma once
#include <optional>
#include "network/common/def.h"
#include "program/data.h"
#include "network/common/message/cashed_data.h"
#include "serialization.h"
#include "buffer.h"
#include <expected>

using namespace std::chrono;
namespace fs = std::filesystem;

namespace network{
    template<Side S>
    struct MESSAGE_ID;

    /**
     * @brief network::server::MESSAGE_ID - type of server-side sent messages.
     * 
     * @details DATA_REPLY_FILEINFO: Message about the file being sent. If receiving Client send client::MESSAGE_ID::TRANSACTION with ACCEPT
     *          a sequence of messages of MESSAGE_ID::DATA_REPLY_FILE_PART will be sent by server.
     * @details SERVER_STATUS: Message about the server status. It is sending by server automatically while the connection is established but
     *          there was nothing requeted by client.
     * @details DATA_REPLY_CAPITALIZE: Message with information about shared data ready to be sent by capitalize request. 
     *          If receiving Client send client::MESSAGE_ID::TRANSACTION with ACCEPT a sequence of messages of 
     *          MESSAGE_ID::DATA_REPLY_FILE_PART will be sent by server.
     * @details ERROR: Message with error code. This message is sent by Server when something is gone wrong.
     * @details PROGRESS: This message is sent during the entire time that the remote process is running at the Client's request.
     * @details DATA_REPLY_FILEPART: Message with file-part information and data.
     * @details VERSION: Message with the program version for backward compatibility of network transactions.
     * @details DATA_REPLY_CAPITALIZE_REF: Message with information about hosts data owning.
     * 
     */
    template<>
    struct MESSAGE_ID<Side::SERVER>{
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

        constexpr static size_t count(){
            return DATA_REPLY_EXTRACT+1;
        }
    };


    /**
     * @brief network::client::MESSAGE_ID - type of client-side sent messages.
     * 
     * @details DATA_REQUEST: Client request of specified (matched) data to be extracted from server-device.
     * @details SERVER_STATUS: Request of the server status (if connection can be established).
     * @details CAPITALIZE: Capitalize request with sharing some part of data (specified by memory (in either KB, or MB, or GB, or TB))
     * @details CAPITALIZE_REF: Capitalize request (only references about owners and owned information)
     * @details TRANSACTION: Client replied message to server's transaction-request (for example, sending file)
     * 
     */
    template<>
    struct MESSAGE_ID<Side::CLIENT>{
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
        constexpr static size_t count(){
            return TRANSACTION+1;
        }
    };

    using Server_MsgT = MESSAGE_ID<Side::SERVER>;
    using Client_MsgT = MESSAGE_ID<Side::CLIENT>;

    template<auto MSG_T>
    concept ServerMessageEnumConcept = std::is_same_v<decltype(MSG_T),Server_MsgT::type>;

    template<typename MSG_T>
    concept ServerMessageEnumConcept_t = std::is_same_v<MSG_T,Server_MsgT::type>;

    template<auto MSG_T>
    concept ClientMessageEnumConcept = std::is_same_v<decltype(MSG_T),Client_MsgT::type>;

    template<typename MSG_T>
    concept ClientMessageEnumConcept_t = std::is_same_v<MSG_T,Client_MsgT::type>;

    template<auto MSG_T>
    concept MessageEnumConcept = 
        ServerMessageEnumConcept<MSG_T> ||
        ClientMessageEnumConcept<MSG_T>;

    template<typename MSG_T>
    concept MessageEnumConcept_t = 
        ServerMessageEnumConcept_t<MSG_T> ||
        ClientMessageEnumConcept_t<MSG_T>;

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct MessageAdditional;

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    struct MessageBase;

    template<auto MSG_T>
    requires MessageEnumConcept<MSG_T>
    class Message;

    template<auto MSG_T, template<auto> class MsgImpl>
    concept MessageConcept = requires(){
        requires MessageEnumConcept<MSG_T>;
        requires std::is_same_v<MsgImpl<MSG_T>,Message<MSG_T>>;
    };
}