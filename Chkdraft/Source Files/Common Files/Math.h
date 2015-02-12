#ifndef MATH_H
#define MATH_H
#include "Constants.h"

template <typename valueType>
void AscendingOrder(valueType &low, valueType &high);

u32 SmallestOf(u32 &first, u32 &second, u32 &third);

s32 SmallestOf(s32 &first, s32 &second, s32 &third);

void LongToBytes(s32 value, u8* bytes);

void ShortToBytes(u16 value, u8* bytes);

s32 TripletToInt(byte* triplet);

template <typename T>
s32 round(T value);

bool getOneByteHexVal(char character, u8& value);

bool getTwoByteHexVal(const char* string, u8& value);

#define INCLUDE_TEMPLATES_ONLY
#include "Math.cpp"
#undef INCLUDE_TEMPLATES_ONLY

#endif