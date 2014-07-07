#ifndef MATH_H
#define MATH_H
#include "Constants.h"

void AscendingOrder(s32 &low, s32 &high);

u32 SmallestOf(u32 &first, u32 &second, u32 &third);

s32 SmallestOf(s32 &first, s32 &second);

s32 SmallestOf(s32 &first, s32 &second, s32 &third);

u32 SmallestOf(u32 &first, u32 &second);

s32 GreatestOf(s32 &first, s32 &second);

u32 GreatestOf(u32 &first, u32 &second);

u8 pow(u8 base, u8 exponent);

void LongToBytes(s32 value, u8* bytes);

void ShortToBytes(u16 value, u8* bytes);

s32 TripletToInt(byte* triplet);

s32 round(double value);

#endif