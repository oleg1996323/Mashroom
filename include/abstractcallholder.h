#pragma once
#include <variant>
#include <program/mashroom.h>
#include <network/server.h>

using Caller = std::variant<const Mashroom*,const server::Server*>;

class AbstractCallHolder:public Caller
{
    public:
    using Caller::Caller;
    AbstractCallHolder(const Caller& caller):Caller(caller){}
    AbstractCallHolder(Caller&& caller):Caller(std::move(caller)){}
};