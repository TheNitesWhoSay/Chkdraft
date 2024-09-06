#ifndef STRUCTS_H
#define STRUCTS_H
#include "constants.h"

struct points // Mirrors win32 POINTS
{
    s16 x = 0;
    s16 y = 0;
    inline bool operator==(const points & rhs) const { return x == rhs.x && y == rhs.y; }
    inline bool operator<(const points & rhs) const {
        if ( y < rhs.y) return true;
        else if ( y > rhs.y ) return false;
        else return x < rhs.x;
    }
};

struct point // Mirrors win32 POINT
{
    s32 x;
    s32 y;
};

struct rect // Mirrors win32 RECT
{
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
};

template <typename Color>
struct MapGrid // A simple grid that can be displayed over a map
{
    points size = {0, 0};
    point offset = {0, 0};
    Color color {};
};

#endif