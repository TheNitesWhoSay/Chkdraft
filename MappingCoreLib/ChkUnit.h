#ifndef CHKUNIT_H
#define CHKUNIT_H
#include "Basics.h"

#pragma pack(push, 1)
__declspec(align(1))
class ChkUnit // A unit as found in a scenario.chk's UNIT section
{
public:
    u32 serial; // 0-4
    u16 xc; // 4-6
    u16 yc; // 6-8
    u16 id; // 8-10
    u16 linkType; // 10-12
    u16 special; // 12-14
    u16 validFlags; // 14-16
    u8 owner; // 16-17
    u8 hitpoints; // 17-18
    u8 shields; // 18-19
    u8 energy; // 19-20
    u32 resources; // 20-24
    u16 hanger; // 24-26
    u16 stateFlags; // 26-28
        #define UNIT_STATE_CLOAKED      BIT_0
        #define UNIT_STATE_BURROWED     BIT_1
        #define UNIT_STATE_LIFTED       BIT_2
        #define UNIT_STATE_HALLUCINATED BIT_3
        #define UNIT_STATE_INVINCIBLE   BIT_4

    u32 unused; // 28-32
    u32 link; // 32-36
};
#pragma pack(pop)
#define UNIT_STRUCT_SIZE 36
#define NUM_UNIT_FIELDS 16
enum class ChkUnitField { Serial, Xc, Yc, Id, LinkType, Special, ValidFlags, Owner,
    Hitpoints, Shields, Energy, Resources, Hanger, StateFlags, Unused, Link };
const u8 unitFieldSize[] = {  4,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  4,  2,  2,  4,  4 };
const u8 unitFieldLoc [] = {  0,  4,  6,  8, 10, 12, 14, 16, 17, 18, 19, 20, 24, 26, 28, 32 };

#endif