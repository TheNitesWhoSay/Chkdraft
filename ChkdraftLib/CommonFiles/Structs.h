#ifndef STRUCTS_H
#define STRUCTS_H
#include "Constants.h"
#include "../../MappingCoreLib/Sc.h"

class points // Mirrors win32 POINTS
{
    public:
        s16 x;
        s16 y;
        points() : x(0), y(0) {}
        points(s16 x, s16 y) : x(x), y(y) {}
        virtual ~points();
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

class MapGrid // A simple grid that can be displayed over a map
{
    public:
        points size;
        point offset;
        Sc::SystemColor color;

        MapGrid(); // ctor
        virtual ~MapGrid();
};

#endif