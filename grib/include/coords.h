#pragma once

typedef double Lat;
typedef double Lon;

typedef struct COORDS_S
{
    Lat lat_;
    Lon lon_;
}Coord;

#define Coord(...) ((Coord) { .lat_ = -999, .lon_ = -999},##__VA_ARGS__)