#ifndef CHKSPRITE_H
#define CHKSPRITE_H
#include "Basics.h"

struct ChkSprite // A sprite as found in a scenario.chk's THG2 section
{
	u16 id;
	u16 xc;
	u16 yc;
	u8 owner;
	u8 unused;
	u16 flags;
		#define FLAG_DRAW_AS_SPRITE		BIT_12
		#define FLAG_SPRITE_DISABLED	BIT_15
};
#define SPRITE_STRUCT_SIZE 10

#endif