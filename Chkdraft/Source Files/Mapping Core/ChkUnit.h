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
		#define UNIT_STATE_CLOAKED		BIT_0
		#define UNIT_STATE_BURROWED		BIT_1
		#define UNIT_STATE_LIFTED		BIT_2
		#define UNIT_STATE_HALLUCINATED	BIT_3
		#define UNIT_STATE_INVINCIBLE	BIT_4

	u32 unused;
	u32 link;
};
#define UNIT_STRUCT_SIZE 36
#define NUM_UNIT_FIELDS 16
#define UNIT_FIELD_SERIAL	   0
#define UNIT_FIELD_XC		   1
#define UNIT_FIELD_YC		   2
#define UNIT_FIELD_ID		   3
#define UNIT_FIELD_LINKTYPE	   4
#define UNIT_FIELD_SPECIAL	   5
#define UNIT_FIELD_VALIDFLAGS  6
#define UNIT_FIELD_OWNER	   7
#define UNIT_FIELD_HITPOINTS   8
#define UNIT_FIELD_SHIELDS	   9
#define UNIT_FIELD_ENERGY	  10
#define UNIT_FIELD_RESOURCES  11
#define UNIT_FIELD_HANGER	  12
#define UNIT_FIELD_STATEFLAGS 13
#define UNIT_FIELD_UNUSED	  14
#define UNIT_FIELD_LINK		  15
const u8 unitFieldSize[] = {  4,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  4,  2,  2,  4,  4 };
const u8 unitFieldLoc [] = {  0,  4,  6,  8, 10, 12, 14, 16, 17, 18, 19, 20, 24, 26, 28, 32 };

#endif