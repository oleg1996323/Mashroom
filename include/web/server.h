#pragma once
#include "web/common/msgdef.h"
#include "web/server/connection_process.h"

#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <sys/eventfd.h>
#include "web/common/credentials.h"
#include "network/abstractserver.h"

using namespace std::string_view_literals;
namespace network{
    class Server:public AbstractServer{
    protected:
        server::Status status_=server::Status::INACTIVE;
    public:
        Server() = default;
        server::Status status() const;
        virtual ~Server() override final{
            close(false,0);
        }
    };
}