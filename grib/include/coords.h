#pragma once

typedef double Lat;
typedef double Lon;

typedef struct COORDS_S
{
    Lat lat_;
    Lon lon_;
}Coord;

#define Coord(...) ((Coord) { (.Lat = -999), (.Lon = -999)},##__VA_ARGS__)