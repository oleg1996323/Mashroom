#pragma once
#include <array>

enum class ServerInfoCodes{
    ExtractMeteo = 0,
    ExtractTopo = 1,
    ExtractCadastre = 2
};

struct ServerInfo{
    const char* name;
    int type;
};

//further will be added
std::array<ServerInfo,sizeof(ServerInfoCodes)-1> servers = {
    ServerInfo({"Extract meteo",(int)ServerInfoCodes::ExtractMeteo}),
    ServerInfo({"Extract topo",(int)ServerInfoCodes::ExtractTopo}),
    ServerInfo({"Extract cadastre",(int)ServerInfoCodes::ExtractCadastre})
};