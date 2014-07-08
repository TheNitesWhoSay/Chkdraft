#include "Math.h"
#include "Debug.h"
#include "CommonFiles.h"

u32 SmallestOf(u32 &first, u32 &second, u32 &third)
{
	if ( first < second )
	{
		if ( first < third )
			return first;
	}
	else if ( second < third )
		return second;

	return third;
}

s32 SmallestOf(s32 &first, s32 &second, s32 &third)
{
	if ( first < second )
	{
		if ( first < third )
			return first;
	}
	else if ( second < third )
		return second;

	return third;
}

void LongToBytes(s32 value, u8* bytes)
{
	(u32&)bytes[0] = value;
}

void ShortToBytes(u16 value, u8* bytes)
{
	(u16&)bytes[0] = value;
}

s32 TripletToInt(byte* triplet)
{
	return triplet[2]*0x10000 +
		   triplet[1]*0x100 +
		   triplet[0];
}
