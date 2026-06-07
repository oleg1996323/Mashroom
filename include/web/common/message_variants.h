#pragma once
#include <optional>
#include <expected>
#include <set>
#include <variant>
#include "serialization.h"
#include "msgdef.h"

//client messages
#include "web/client/message/index.h"
#include "web/client/message/index_ref.h"
#include "web/client/message/status.h"
#include "web/client/message/data_request.h"
#include "web/client/message/transaction.h"

//server messages
#include "web/server/message/index.h"
#include "web/server/message/data_reply_index_info.h"
#include "web/server/message/data_reply_extract_part.h"
#include "web/server/message/data_reply_finfo.h"
#include "web/server/message/data_reply_fpart.h"
#include "web/server/message/error.h"
#include "web/server/message/progress.h"
#include "web/server/message/version.h"
#include "web/server/message/status.h"

using namespace std::chrono;
namespace fs = std::filesystem;

namespace network{
    template<Side S>
    struct list_message;

    template<>
    struct list_message<Side::CLIENT>{
        using type = std::variant<std::monostate,
                        Message<network::Client_MsgT::DATA_REQUEST>,
                        Message<network::Client_MsgT::SERVER_STATUS>,
                        Message<network::Client_MsgT::INDEX>,
                        Message<network::Client_MsgT::INDEX_REF>,
                        Message<network::Client_MsgT::TRANSACTION>>;
    };
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::DATA_REQUEST>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::DATA_REQUEST>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::INDEX>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::INDEX>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::INDEX_REF>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::INDEX_REF>>);
    static_assert(std::is_move_constructible_v<Message<network::Client_MsgT::TRANSACTION>>);
    static_assert(std::is_move_assignable_v<Message<network::Client_MsgT::TRANSACTION>>);

    template<>
    struct list_message<Side::SERVER>{
        using type =    std::variant<std::monostate,
                        Message<network::Server_MsgT::DATA_REPLY_FILEINFO>,
                        Message<network::Server_MsgT::SERVER_STATUS>,
                        Message<network::Server_MsgT::DATA_REPLY_INDEX>,
                        Message<network::Server_MsgT::ERROR>,
                        Message<network::Server_MsgT::PROGRESS>,
                        Message<network::Server_MsgT::DATA_REPLY_FILEPART>,
                        Message<network::Server_MsgT::VERSION>,
                        Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>,
                        Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>;
    };
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_FILEINFO>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_FILEINFO>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::SERVER_STATUS>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::ERROR>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::ERROR>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::PROGRESS>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::PROGRESS>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_FILEPART>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::VERSION>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::VERSION>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
    static_assert(std::is_move_constructible_v<Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);
    static_assert(std::is_move_assignable_v<Message<network::Server_MsgT::DATA_REPLY_EXTRACT>>);

    constexpr Side get_side(MessageEnumConcept_t auto msg_t){
        using type = decltype(msg_t);
        if constexpr(std::is_same_v<type,MESSAGE_ID<Side::SERVER>::type>)
            return MESSAGE_ID<Side::SERVER>::side();
        else if constexpr(std::is_same_v<type,MESSAGE_ID<Side::CLIENT>::type>)
            return MESSAGE_ID<Side::CLIENT>::side();
        else static_assert(false,"Undefined side");
    }
}