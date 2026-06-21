#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <optional>
#include "serialization.h"
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/sendfile.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <thread>

namespace network{
    enum class Side{
        SERVER,
        CLIENT
    };

    enum class MessageCategoryEnum{
        SYSTEM,
        FILE,
        APPLICATION
    };

    template<Side S,MessageCategoryEnum CAT>
    struct MessageCategory;

    template<Side S>
    constexpr Side sent_from(){
        if constexpr(S == Side::SERVER)
            return Side::CLIENT;
        else return Side::SERVER;
    }

    namespace server{
        enum class Status:int{
            READY,
            SUSPENDED,
            INACTIVE          
        };
    }
    enum class Transaction{
        ACCEPT,
        DECLINE,
        CANCEL,
        NEW,
        CONTINUE
    };
}
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
     * @details DATA_REPLY_INDEX: Message with information about shared data ready to be sent by index request. 
     *          If receiving Client send client::MESSAGE_ID::TRANSACTION with ACCEPT a sequence of messages of 
     *          MESSAGE_ID::DATA_REPLY_FILE_PART will be sent by server.
     * @details ERROR: Message with error code. This message is sent by Server when something is gone wrong.
     * @details PROGRESS: This message is sent during the entire time that the remote process is running at the Client's request.
     * @details DATA_REPLY_FILEPART: Message with file-part information and data.
     * @details VERSION: Message with the program version for backward compatibility of network transactions.
     * @details DATA_REPLY_INDEX_REF: Message with information about hosts data owning.
     * 
     */
    template<>
    struct MESSAGE_ID<Side::SERVER>{
        constexpr static Side side(){
            return Side::SERVER;
        }
        enum type:int{
            CREDENTIALS,
            TRANSACTION,
            VERSION,
            ERROR,
            PROGRESS,
            SERVER_STATUS,
            FILE_DATA,
            FILE_METADATA,
            INDEX,
            EXTRACT
        };

        static constexpr size_t msg_number() noexcept{
            return EXTRACT+1;
        }
    };


    /**
     * @brief network::client::MESSAGE_ID - type of client-side sent messages.
     * 
     * @details EXTRACT: Client request of specified (matched) data to be extracted from server-device.
     * @details SERVER_STATUS: Request of the server status (if connection can be established).
     * @details INDEX: Index request with sharing some part of data (specified by memory (in either KB, or MB, or GB, or TB))
     * @details INDEX_REF: Index request (only references about owners and owned information)
     * @details TRANSACTION: Client replied message to server's transaction-request (for example, sending file)
     * 
     */
    template<>
    struct MESSAGE_ID<Side::CLIENT>{
        constexpr static Side side(){
            return Side::CLIENT;
        }
        enum type:int{
            CREDENTIALS,
            TRANSACTION,
            VERSION,
            ERROR,
            PROGRESS,
            SERVER_STATUS,
            INDEX,
            INDEX_REF,
            EXTRACT,
        };

        static constexpr size_t msg_number() noexcept{
            return EXTRACT+1;
        }
    };

    template<Side S>
    using Message_t = typename MESSAGE_ID<S>::type;
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
    class Message;
}