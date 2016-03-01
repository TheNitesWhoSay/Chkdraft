#ifndef CHKUNIT_H
#define CHKUNIT_H
#include "Basics.h"

class ChkUnit // A unit as found in a scenario.chk's UNIT section
{
public:
    u32 serial;
    u16 xc;
    u16 yc;
    u16 id;
    u16 linkType;
    u16 special;
    u16 validFlags;
    u8 owner;
    u8 hitpoints;
    u8 shields;
    u8 energy;
    u32 resources;
    u16 hanger;
    u16 stateFlags;
        #define UNIT_STATE_CLOAKED      BIT_0
        #define UNIT_STATE_BURROWED     BIT_1
        #define UNIT_STATE_LIFTED       BIT_2
        #define UNIT_STATE_HALLUCINATED BIT_3
        #define UNIT_STATE_INVINCIBLE   BIT_4

    u32 unused;
    u32 link;
};
#define UNIT_STRUCT_SIZE 36
#define NUM_UNIT_FIELDS 16
enum class ChkUnitField { Serial, Xc, Yc, Id, LinkType, Special, ValidFlags, Owner,
    Hitpoints, Shields, Energy, Resources, Hanger, StateFlags, Unused, Link };
const u8 unitFieldSize[] = {  4,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  4,  2,  2,  4,  4 };
const u8 unitFieldLoc [] = {  0,  4,  6,  8, 10, 12, 14, 16, 17, 18, 19, 20, 24, 26, 28, 32 };

#endif