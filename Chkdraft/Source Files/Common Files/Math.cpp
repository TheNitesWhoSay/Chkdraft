#include "Math.h"
#include "Debug.h"
#include "CommonFiles.h"

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

s32 TripletToInt(byte* triplet)
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
	return s32(floor(static_cast<double>(value) + 0.5));
}
/* End templates */ #else

bool getOneByteHexVal(char character, u8& value)
{
	if		( character >= '0' && character <= '9' )
		value = (character-'0'   );
	else if ( character >= 'A' && character <= 'F' )
		value = (character-'A'+10);
	else if ( character >= 'a' && character <= 'f' )
		value = (character-'a'+10);
	else
		return false;

	return true;
}

bool getTwoByteHexVal(const char* string, u8& value)
{
	if		( string[0] >= '0' && string[0] <= '9' )
		value = ((string[0]-'0'   ) << 4);
	else if ( string[0] >= 'A' && string[0] <= 'F' )
		value = ((string[0]-'A'+10) << 4);
	else if ( string[0] >= 'a' && string[0] <= 'f' )
		value = ((string[0]-'a'+10) << 4);
	else
		return false;
		
	if		( string[1] >= '0' && string[1] <= '9' )
		value += (string[1]-'0'   );
	else if ( string[1] >= 'A' && string[1] <= 'F' )
		value += (string[1]-'A'+10);
	else if ( string[1] >= 'a' && string[1] <= 'f' )
		value += (string[1]-'a'+10);
	else
		return false;
	
	return true;
}

/* End file partitioning */ #endif
