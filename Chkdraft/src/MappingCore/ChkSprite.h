#ifndef CHKSPRITE_H
#define CHKSPRITE_H
#include "Basics.h"

class ChkSprite // A sprite as found in a scenario.chk's THG2 section
{
    public:
        u16 id;
        u16 xc;
        u16 yc;
        u8 owner;
        u8 unused;
        u16 flags;

        enum class SpriteFlags : u16 { DrawAsSprite = BIT_12, SpriteDisabled = BIT_15 };
        bool IsDrawnAsSprite();
        bool IsDisabled();
};
#define SPRITE_STRUCT_SIZE 10

#endif