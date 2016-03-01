#include <cmath>
#include "Math.h"
/* Allow file to be partitioned along templates */ #ifdef INCLUDE_TEMPLATES_ONLY

template <typename valueType>
void AscendingOrder(valueType &low, valueType &high)
{
    if ( low > high )
        std::swap(low, high);
}
/* End templates */ #else

u32 SmallestOf(u32 &first, u32 &second)
{
    if ( first < second )
        return first;
    else
        return second;
}

s32 SmallestOf(s32 &first, s32 &second)
{
    if ( first < second )
        return first;
    else
        return second;
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

s32 TripletToInt(u8* triplet)
{
    return triplet[2]*0x10000 +
           triplet[1]*0x100 +
           triplet[0];
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename T>
s32 round(T value)
{    
    return s32(std::floor(static_cast<double>(value) + 0.5));
}
/* End templates */ #else



/* End file partitioning */ #endif
