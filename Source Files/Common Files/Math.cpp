#include "Math.h"
#include "Debug.h"
#include "CommonFiles.h"

void AscendingOrder(s32 &low, s32 &high)
{
	if ( low > high )
		std::swap(low, high);
}

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

s32 SmallestOf(s32 &first, s32 &second)
{
	if ( first < second )
		return first;

	return second;
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

u32 SmallestOf(u32 &first, u32 &second)
{
	if ( first < second )
		return first;

	return second;
}

s32 GreatestOf(s32 &first, s32 &second)
{
	if ( first > second )
		return first;

	return second;
}

u32 GreatestOf(u32 &first, u32 &second)
{
	if ( first < second )
		return first;

	return second;
}

u8 pow(u8 base, u8 exponent)
{
	if ( exponent == 0 )
		return 1;

	u8 result = base;

	while ( exponent > 0 )
	{
		result *= base;
		exponent --;
	}

	return result;
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

s32 round(double value)
{    
	return s32(floor(value + 0.5));
}