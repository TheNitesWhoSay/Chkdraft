#ifndef CHKLOCATION_H
#define CHKLOCATION_H
#include "Basics.h"

struct ChkLocation // A location as found in a scenario.chk's MRGN section
{
    s32 xc1;
    s32 yc1;
    s32 xc2;
    s32 yc2;
    u16 stringNum;
    u16 elevation;
        #define LOC_ELEVATION_LOWGROUND     BIT_0
        #define LOC_ELEVATION_MEDGROUND     BIT_1
        #define LOC_ELEVATION_HIGHGROUND    BIT_2
        #define LOC_ELEVATION_LOWAIR        BIT_3
        #define LOC_ELEVATION_MEDAIR        BIT_4
        #define LOC_ELEVATION_HIGHAIR       BIT_5
};
#define CHK_LOCATION_SIZE 20
#define NUM_LOCATION_FIELDS 6
#define LOC_FIELD_XC1         0
#define LOC_FIELD_YC1         1
#define LOC_FIELD_XC2         2
#define LOC_FIELD_YC2         3
#define LOC_FIELD_STRING_NUM  4
#define LOC_FIELD_ELEVATION   5
const u8 locationFieldSize[] = {  4,  4,  4,  4,  2,  2  };
const u8 locationFieldLoc [] = {  0,  4,  8,  12, 16, 18 };

#endif