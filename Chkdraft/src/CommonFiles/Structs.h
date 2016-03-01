#ifndef STRUCTS_H
#define STRUCTS_H
#include "Constants.h"

struct points // Mirrors win32 POINTS
{
    s16 x;
    s16 y;
};

struct point // Mirrors win32 POINT
{
    s32 x;
    s32 y;
};

class MapGrid // A simple grid that can be displayed over a map
{
    public:
        points size;
        point offset;
        u8 red;
        u8 green;
        u8 blue;
        u8 reserved;

        MapGrid(); // ctor
};

#endif