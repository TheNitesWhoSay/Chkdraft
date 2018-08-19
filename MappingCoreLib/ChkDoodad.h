#ifndef CHKDOODAD_H
#define CHKDOODAD_H
#include "Basics.h"

struct ChkDoodad // A doodad as found in a scenario.chk's DD2 section
{
    u16 id;
    u16 xc;
    u16 yc;
    u8 owner;
    u8 enabled; // 0 enabled, 1 disabled
};
#define DOODAD_STRUCT_SIZE 8

#endif