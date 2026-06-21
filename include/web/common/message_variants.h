#pragma once
#include <optional>
#include <expected>
#include <set>
#include <variant>
#include "serialization.h"
#include "msgdef.h"

//client messages
#include "web/client/message/application/index.h"
#include "web/client/message/application/index_ref.h"
#include "web/client/message/system/status.h"
#include "web/client/message/application/extract.h"
#include "web/client/message/system/version.h"
#include "web/client/message/system/error.h"
#include "web/client/message/system/credentials.h"
#include "web/client/message/system/progress.h"

//server messages
#include "web/server/message/application/index.h"
#include "web/server/message/application/extract.h"
#include "web/server/message/file/data.h"
#include "web/server/message/file/metadata.h"
#include "web/server/message/system/error.h"
#include "web/server/message/system/version.h"
#include "web/server/message/system/status.h"
#include "web/server/message/system/credentials.h"
#include "web/server/message/system/progress.h"

//common server-client messages
#include "web/common/detail/transaction.h"

using namespace std::chrono;
namespace fs = std::filesystem;

namespace network{
    template<Side S>
    using SystemMsg = std::conditional_t<S==Side::CLIENT,
                                    std::variant<std::monostate,
                                    Message<Client_MsgT::CREDENTIALS>,
                                    Message<Client_MsgT::SERVER_STATUS>,
                                    Message<Client_MsgT::ERROR>,
                                    Message<Client_MsgT::TRANSACTION>,
                                    Message<Client_MsgT::VERSION>,
                                    Message<Client_MsgT::PROGRESS>
                                    >,
                                    std::variant
                                    <std::monostate,
                                    Message<Server_MsgT::CREDENTIALS>,
                                    Message<Server_MsgT::SERVER_STATUS>,
                                    Message<Server_MsgT::ERROR>,
                                    Message<Server_MsgT::TRANSACTION>,
                                    Message<Server_MsgT::VERSION>,
                                    Message<Server_MsgT::PROGRESS>
                                    >
                                    >;

    template<Side S>
    using FileMsg = std::conditional_t<S==Side::CLIENT,
                                    std::variant<std::monostate>,
                                    std::variant<
                                    std::monostate,
                                    Message<Server_MsgT::FILE_METADATA>,
                                    Message<Server_MsgT::FILE_DATA>
                                    >
                                    >;
    template<Side S>
    using AppMsg = std::conditional_t<S==Side::CLIENT,
                                    std::variant<std::monostate,
                                    Message<Client_MsgT::INDEX>,
                                    Message<Client_MsgT::INDEX_REF>,
                                    Message<Client_MsgT::EXTRACT>
                                    >,
                                    std::variant<std::monostate,
                                    Message<Server_MsgT::INDEX>,
                                    Message<Server_MsgT::EXTRACT>
                                    >
                                    >;
    
    template<Side S,MESSAGE_ID<S>::type MSG>
    constexpr bool is_app_message_v = (S==Side::SERVER?(MSG == Server_MsgT::INDEX ||
                                    MSG == Server_MsgT::EXTRACT):
                                    (MSG == Client_MsgT::INDEX ||
                                    MSG == Client_MsgT::INDEX_REF ||
                                    MSG == Client_MsgT::EXTRACT));
    template<Side S,MESSAGE_ID<S>::type MSG>
    constexpr bool is_sys_message_v = (S==Side::SERVER?(MSG == Server_MsgT::SERVER_STATUS ||
                                    MSG == Server_MsgT::ERROR ||
                                    MSG == Server_MsgT::TRANSACTION ||
                                    MSG == Server_MsgT::VERSION ||
                                    MSG == Server_MsgT::CREDENTIALS ||
                                    MSG == Server_MsgT::PROGRESS):
                                    (MSG == Client_MsgT::SERVER_STATUS ||
                                    MSG == Client_MsgT::ERROR ||
                                    MSG == Client_MsgT::TRANSACTION ||
                                    MSG == Client_MsgT::VERSION ||
                                    MSG == Client_MsgT::CREDENTIALS ||
                                    MSG == Client_MsgT::PROGRESS));
    template<Side S,MESSAGE_ID<S>::type MSG>
    constexpr bool is_file_message_v = (S==Side::SERVER?(MSG == Server_MsgT::FILE_DATA ||
                                    MSG == Server_MsgT::FILE_METADATA):
                                    false);

    template<Side S>
    struct list_message;


    template<Side S>
    struct MessageCategory<S,MessageCategoryEnum::SYSTEM>{
        using type = SystemMsg<S>;
    };

    template<Side S>
    struct MessageCategory<S,MessageCategoryEnum::FILE>{
        using type = FileMsg<S>;
    };

    template<Side S>
    struct MessageCategory<S,MessageCategoryEnum::APPLICATION>{
        using type = AppMsg<S>;
    };

    using ClientAppMsg = AppMsg<Side::CLIENT>;
    using ClientSysMsg = SystemMsg<Side::CLIENT>;
    using ClientFileMsg = FileMsg<Side::CLIENT>;

    using ServerAppMsg = AppMsg<Side::SERVER>;
    using ServerSysMsg = SystemMsg<Side::SERVER>;
    using ServerFileMsg = FileMsg<Side::SERVER>;

    template<Side S>
    using Msg = std::variant<std::monostate,
                        SystemMsg<S>,
                        FileMsg<S>,
                        AppMsg<S>>;

    template<>
    struct list_message<Side::SERVER>{
        using type = Msg<Side::SERVER>;
    };

    template<Side S>
    bool is_sys_msg(typename MESSAGE_ID<S>::type id) noexcept{
        using type = typename MESSAGE_ID<S>::type;
        switch(id){
            case type::ERROR:
            case type::TRANSACTION:
            case type::SERVER_STATUS:
            case type::VERSION:
            case type::CREDENTIALS:
            case type::PROGRESS:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    template<Side S>
    bool is_file_msg(typename MESSAGE_ID<S>::type id) noexcept{
        if constexpr (S==Side::SERVER){
            switch(id){
                case Server_MsgT::FILE_DATA:
                case Server_MsgT::FILE_METADATA:
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
        }
        else return false;
    }

    template<Side S>
    bool is_app_msg(typename MESSAGE_ID<S>::type id) noexcept{
        using type = typename MESSAGE_ID<S>::type;
        if constexpr (S==Side::SERVER)
            switch(id){
                case Server_MsgT::INDEX:
                case Server_MsgT::EXTRACT:
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
        else switch(id){
                case type::INDEX:
                case type::INDEX_REF:
                case type::EXTRACT:
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
    }

    template<>
    struct list_message<Side::CLIENT>{
        using type = Msg<Side::CLIENT>;
    };
}

static_assert(std::is_move_constructible_v<network::SystemMsg<network::Side::CLIENT>>);