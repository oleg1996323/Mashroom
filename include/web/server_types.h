#pragma once
#include <array>

namespace network::server{

enum class InfoCodes{
    ExtractMeteo = 0,
    ExtractTopo = 1,
    ExtractCadastre = 2
};

struct Info{
    const char* name;
    int type;
};

//further will be added
std::array<Info,sizeof(InfoCodes)-1> servers = {
    Info({"Extract meteo",(int)InfoCodes::ExtractMeteo}),
    Info({"Extract topo",(int)InfoCodes::ExtractTopo}),
    Info({"Extract cadastre",(int)InfoCodes::ExtractCadastre})
};
}