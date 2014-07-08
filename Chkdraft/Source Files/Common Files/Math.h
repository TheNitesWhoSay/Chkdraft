#ifndef MATH_H
#define MATH_H
#include "Constants.h"

template <typename T>
void AscendingOrder(T &low, T &high)
{
	if ( low > high )
		std::swap(low, high);
}

u32 SmallestOf(u32 &first, u32 &second, u32 &third);

s32 SmallestOf(s32 &first, s32 &second, s32 &third);

void LongToBytes(s32 value, u8* bytes);

void ShortToBytes(u16 value, u8* bytes);

s32 TripletToInt(byte* triplet);

template <typename T>
s32 round(T value)
{    
	return s32(floor(static_cast<double>(value) + 0.5));
}

#endif