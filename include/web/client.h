#pragma once
#include <thread>
#include <unistd.h>
#include "web/common/def.h"
#include <sys/eventfd.h>
#include <poll.h>
#include "web/common/credentials.h"
#include <variant>
#include "web/client/connection_process.h"
#include <netdb.h>
#include <future>
#include "network/commonsocket.h"
#include "network/abstractclient.h"

namespace network{
    class Client:public AbstractClient{
        private:
        virtual void before_connect() override final{
        }
        virtual void after_connect(ConnectionHandle hconn) override final{
        }
        virtual AbstractClient& before_disconnect(
                    ConnectionHandle hconn,
                    std::error_code& err) override final{
            return *this;
        }
        virtual AbstractClient& after_disconnect(
                    ConnectionHandle hconn,
                    std::error_code& err) override final{
            return *this;
        }
        public:
        Client(std::error_code& err,uint16_t ev_order):
            AbstractClient(err,ev_order){}
        Connection::Properties connection_properties(ConnectionHandle hconn) const noexcept{
            return connections()->connection_properties(hconn);
        }
    };
}