#include "Buffer.h"
#include "Basics.h"
#include <iostream>
using namespace std;

#define DEFAULT_SIZE_MULTIPLIER 1.2

//////////////////
// Constructors //
//////////////////

buffer::buffer() : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
	for ( int i=0; i<5; i++ ) // NUL fill title
		bufTitle[0] = '\0';
}

buffer::buffer(const char* bufferTitle) : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
	strcpy_s(bufTitle, 5, bufferTitle);
	bufTitle[4] = '\0';
}

////////////////
// Destructor //
////////////////

buffer::~buffer()
{
	if ( this != nullptr && data != nullptr )
		delete[] data;
}

///////////////
// Accessors //
///////////////

u32 buffer::size()
{
	if ( this != nullptr )
		return sizeUsed;
	else
		return 0;
}

const char* buffer::title()
{
	if ( this != nullptr )
		return bufTitle;
	else
		return 0;
}

template <typename valueType>
bool buffer::get(valueType &dest, u32 location)
{
	if ( this != nullptr )
	{
		if ( location+sizeof(valueType) > location && location+sizeof(valueType) <= sizeUsed )
		{
			dest = (valueType &)data[location];
			return true;
		}
	}
	return false;
}
template bool buffer::get<u8>(u8 &dest, u32 location);
template bool buffer::get<u16>(u16 &dest, u32 location);
template bool buffer::get<u32>(u32 &dest, u32 location);
template bool buffer::get<s32>(s32 &dest, u32 location);
template bool buffer::get<ChkUnit>(ChkUnit &dest, u32 location);

bool buffer::getBit(bool &dest, u32 location, u32 bitNum)
{
	if ( this != nullptr )
	{
		if ( bitNum/8+location < location ) // Prevent wraparound
			return false;

		location += bitNum / 8;

		if ( location < sizeUsed )
		{
			dest = ( (u8(1) << (bitNum%8)) & data[location] ) != 0; // AND the requested bit
			return true;
		}
	}
	return false;
}

bool buffer::getString(char* dest, u32 location, u32 size)
{
	if ( this != nullptr )
	{
		if ( location+size > location && location+size <= sizeUsed )
		{
			memcpy(dest, &data[location], size);
			return true;
		}
	}
	return false;
}

const void* buffer::getPtr(u32 location)
{
	if ( this != nullptr && location < sizeUsed )
		return (void*)&data[location];
	else
		return nullptr;
}

const void* buffer::getPtr(u32 location, u32 sizeRequested)
{
	if ( this != nullptr && location+sizeRequested > location && location+sizeRequested <= sizeUsed )
		return (void*)&data[location];
	else
		return nullptr;
}

template <typename valueType>
bool buffer::getPtr(valueType* &dest, u32 location, u32 sizeRequested)
{
	if ( this != nullptr && location+sizeRequested > location && location+sizeRequested <= sizeUsed )
	{
		dest = (valueType*)&data[location];
		return true;
	}
	return false;
}
template bool buffer::getPtr<char>(char* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<u8>(u8* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<u16>(u16* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<u32>(u32* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<ChkUnit>(ChkUnit* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<ChkSprite>(ChkSprite* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<ChkLocation>(ChkLocation* &dest, u32 location, u32 sizeRequested);
template bool buffer::getPtr<Trigger>(Trigger* &dest, u32 location, u32 sizeRequested);

template <typename valueType>
valueType buffer::get(u32 location)
{
	if ( this != nullptr && location+sizeof(valueType) > location && location+sizeof(valueType) <= sizeUsed )
		return (valueType &)data[location];

	return 0;
}
template u8 buffer::get<u8>(u32 location);
template u16 buffer::get<u16>(u32 location);
template u32 buffer::get<u32>(u32 location);

bool buffer::getBit(u32 location, u32 bitNum)
{
	if ( this != nullptr )
	{
		if ( location + bitNum/8 < location ) // Prevent wraparound
			return false;

		location += bitNum / 8;

		if ( location < sizeUsed )
			return ( (char(1) << (bitNum%8)) & data[location] ) != 0; // AND the bit given by bitNum
	}
	return false;
}

///////////////
// Searching //
///////////////

bool buffer::has(char character, u32 location)
{
	if ( this != nullptr && location < sizeUsed && data[location] == character )
		return true;
	else
		return false;
}

bool buffer::has(const char* str, u32 location, u32 size)
{
	if ( this != nullptr && location+size > location && location+size <= sizeUsed )
		return memcmp(str, &data[location], size) == 0;
	else
		return false;
}

bool buffer::getNext(char character, u32 start, u32 &dest)
{
	if ( this != nullptr )
	{
		for ( u32 i=start; i<sizeUsed; i++ )
		{
			if ( data[i] == character )
			{
				dest = i;
				return true;
			}
		}
	}
	return false;
}

bool buffer::getNextUnquoted(char character, u32 start, u32 &dest)
{
	if ( this != nullptr )
	{
		for ( u32 i=start; i<sizeUsed; i++ )
		{
			if ( data[i] == character )
			{
				dest = i;
				return true;
			}
			else if ( data[i] == '\"' )
			{
				do { i ++; }
				while ( i < sizeUsed && data[i] != '\"' );
			}
		}
	}
	return false;
}

//////////////
// Mutators //
//////////////

template <typename valueType>
bool buffer::add(valueType value)
{
	if ( this != nullptr && sizeUsed+sizeof(valueType) > sizeUsed )
	{
		if ( sizeUsed+sizeof(valueType) > sizeAllotted )
		{
			try {
				resize(sizeof(valueType), true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}

		(valueType &)data[sizeUsed] = value;
		sizeUsed += sizeof(valueType);

		return true;
	}
	return false;
}
template bool buffer::add<char>(char value);
template bool buffer::add<u8>(u8 value);
template bool buffer::add<u16>(u16 value);
template bool buffer::add<u32>(u32 value);
template bool buffer::add<ChkUnit&>(ChkUnit &value);
template bool buffer::add<Trigger&>(Trigger &value);

template <typename valueType>
bool buffer::add(valueType value, u32 amount)
{
	if ( this != nullptr && sizeof(valueType)*amount+sizeUsed > sizeUsed )
	{
		if ( sizeUsed+amount*sizeof(valueType) >= sizeAllotted )
		{
			try {
				resize(amount*sizeof(valueType), true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}

		for ( u32 i=sizeUsed; i<amount*sizeof(valueType)+sizeUsed; i+=sizeof(valueType) )
			(valueType &)data[i] = value;

		sizeUsed += amount*sizeof(valueType);
		return true;
	}
	return false;
}
template bool buffer::add<u8>(u8 value, u32 amount);
template bool buffer::add<u16>(u16 value, u32 amount);
template bool buffer::add<u32>(u32 value, u32 amount);

bool buffer::addStr(const char* chunk, u32 chunkSize)
{
	if ( this != nullptr && sizeUsed+chunkSize > sizeUsed )
	{
		if ( sizeUsed+chunkSize >= sizeAllotted )
		{
			try {
				resize(chunkSize, true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}
		memcpy(&data[sizeUsed], chunk, chunkSize);
		sizeUsed += chunkSize;
		return true;
	}
	return false;
}

template <typename valueType>
bool buffer::insert(u32 location, valueType value)
{
	if ( this != nullptr && sizeUsed+sizeof(valueType) > sizeUsed && location <= sizeUsed )
	{
		if ( sizeUsed+sizeof(valueType) >= sizeAllotted )
		{
			try {
				resize(sizeof(valueType), true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}

		memmove( &data[location+sizeof(valueType)],
				 &data[location],
				 sizeUsed - location );
		
		(valueType &)data[location] = value;
		sizeUsed += sizeof(valueType);
		return true;
	}
	return false;
}
template bool buffer::insert<char>(u32 location, char value);
template bool buffer::insert<u16>(u32 location, u16 value);
template bool buffer::insert<u32>(u32 location, u32 value);
template bool buffer::insert<ChkUnit&>(u32 location, ChkUnit& unit);

template <typename valueType>
bool buffer::insert(u32 location, valueType value, u32 amount)
{
	if ( this != nullptr && amount*sizeof(valueType)+location > location && location <= sizeUsed )
	{
		if ( sizeUsed+(sizeof(valueType)*amount) >= sizeAllotted )
		{
			try {
				resize(sizeof(valueType)*amount, true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}

		memmove( &data[location+sizeof(valueType)*amount],
				 &data[location],
				 sizeUsed - location );
		
		for ( u32 i=0; i<amount; i++)
			(valueType &)data[location+(i*sizeof(valueType))] = value;

		sizeUsed += sizeof(valueType)*amount;
		return true;
	}
	return false;
}
template bool buffer::insert<u8>(u32 location, u8 value, u32 amount);
template bool buffer::insert<u16>(u32 location, u16 value, u32 amount);

bool buffer::insertStr(u32 startLocation, const char* chunk, u32 chunkSize)
{
	if ( this != nullptr && sizeUsed+chunkSize > sizeUsed && startLocation <= sizeUsed )
	{
		if ( sizeUsed+chunkSize >= sizeAllotted )
		{
			try {
				resize(chunkSize, true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}

		memmove( &data[startLocation+chunkSize],
				 &data[startLocation],
				 sizeUsed-startLocation );

		memcpy( &data[startLocation],
				chunk,
				chunkSize );

		sizeUsed += chunkSize;
		return true;
	}
	return false;
}

template <typename valueType>
bool buffer::replace(u32 location, valueType value)
{
	if ( this != nullptr && location+sizeof(valueType) <= sizeUsed )
	{
		(valueType &)data[location] = value;
		return true;
	}
	return false;
}
template bool buffer::replace<char>(u32 location, char value);
template bool buffer::replace<u8>(u32 location, u8 value);
template bool buffer::replace<u16>(u32 location, u16 value);
template bool buffer::replace<u32>(u32 location, u32 value);

template <typename valueType>
bool buffer::replace(u32 location, valueType value, u32 amount)
{
	if ( this != nullptr && location+amount*sizeof(value) <= sizeUsed )
	{
		for ( u32 i=0; i<amount; i++ )
			(valueType &)data[location+i*sizeof(value)] = value;

		return true;
	}
	return false;
}
template bool buffer::replace<char>(u32 location, char value, u32 amount);
template bool buffer::replace<u8>(u32 location, u8 value, u32 amount);
template bool buffer::replace<u16>(u32 location, u16 value, u32 amount);
template bool buffer::replace<u32>(u32 location, u32 value, u32 amount);

bool buffer::replaceStr(u32 startLocation, const char* chunk, u32 chunkSize)
{
	if ( this != nullptr && startLocation+chunkSize <= sizeUsed )
	{
		memcpy( &data[startLocation],
				chunk,
				chunkSize );

		return true;
	}
	return false;
}

bool buffer::replaceStr(u32 startLocation, u32 initialSize, const char* chunk, u32 chunkSize)
{
	if ( this != nullptr && startLocation+initialSize <= sizeUsed )
	{
		s64 difference = chunkSize-initialSize;

		if ( difference > 0 && sizeUsed+(u32)difference > sizeUsed ) // Expand area, insert data
		{
			try {
				resize(difference, true);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
				return false;
			}

			memmove( &data[startLocation+initialSize], // Move everything after initial chunk forward differance
					 &data[startLocation+initialSize+difference],
					 sizeUsed-startLocation-initialSize );

			memcpy( &data[startLocation],
					chunk,
					chunkSize );
		}
		else if ( difference < 0 ) // Contract area, insert data
		{
			memmove( &data[startLocation+chunkSize], // Move everything after initial chunk back differance
					 &data[startLocation+initialSize],
					 sizeUsed-startLocation-initialSize );

			memcpy( &data[startLocation],
					chunk,
					chunkSize );

			try {
				resize(difference, false);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to decrease size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
		}
		else // difference == 0
		{
			memcpy( &data[startLocation],
					chunk,
					chunkSize );
		}

		sizeUsed += (s32)difference;
		return true;
	}
	return false;
}

bool buffer::setBit(u32 location, u32 bitNum, bool bitValue)
{
	if ( this != nullptr )
	{
		if ( bitNum/8+location < location ) // Prevent wraparound
			return false;

		location += bitNum / 8;
		if ( location < sizeUsed )
		{
			if ( bitValue == true )
				data[location] |= (char(1) << (bitNum%8)); // OR on the bit
			else
				data[location] &= (~(char(1) << (bitNum%8))); // AND off the bit

			return true;
		}
	}
	return false;
}

template <typename valueType>
bool buffer::swap(u32 location1, u32 location2)
{
	if ( this != nullptr && location1+sizeof(valueType) <= sizeUsed && location2+sizeof(valueType) <= sizeUsed )
	{
		std::swap<valueType>((valueType &)data[location1], (valueType &)data[location2]);
		return true;
	}
	return false;
}
template bool buffer::swap<ChkUnit>(u32 location1, u32 location2);
template bool buffer::swap<u16>(u32 location1, u32 location2);
template bool buffer::swap<u32>(u32 location1, u32 location2);


bool buffer::swapStr(u32 location1, u32 location2, u32 swapSize)
{
	if ( this != nullptr && location1+swapSize <= sizeUsed && location2+swapSize <= sizeUsed )
	{
		char* temp;
		try {
			temp = new char[swapSize];
		} catch ( std::bad_alloc ) {
			return false;
		}
		memcpy(temp, &data[location1], swapSize);
		memcpy(&data[location2], temp, swapSize);
		return true;
	}
	return false;
}

bool buffer::overwrite(const char* chunk, u32 chunkSize)
{
	if ( this != nullptr )
	{
		flush();

		if ( setSize(chunkSize) )
		{
			memcpy(data, chunk, chunkSize);
			sizeUsed = chunkSize;
			return true;
		}
	}
	return false;
}

bool buffer::takeAllData(buffer& source)
{
	if ( this != nullptr )
	{
		flush();

		data = source.data;
		sizeUsed = source.sizeUsed;
		sizeAllotted = source.sizeAllotted;

		source.data = nullptr;
		source.sizeUsed = 0;
		source.sizeAllotted = 0;

		return true;
	}
	return false;
}

bool buffer::setTitle(char* newTitle)
{
	if ( this != nullptr )
	{
		strcpy_s(bufTitle, 5, newTitle);
		bufTitle[4] = '\0';
		return true;
	}
	else
		return false;
}

bool buffer::setTitle(u32 newTitle)
{
	if ( this != nullptr )
	{
		*((u32*)&bufTitle[0]) = newTitle;
		bufTitle[4] = '\0';
		return true;
	}
	else
		return false;
}

template <typename valueType>
bool buffer::del(u32 location)
{
	if ( this != nullptr && location+sizeof(valueType) <= sizeUsed )
	{
		if ( s64(sizeUsed)-s64(location)+s64(sizeof(valueType)) > 0 )
		{
			memmove(
				&data[location],
				&data[location+sizeof(valueType)],
				sizeUsed-location-sizeof(valueType)
			);
		}

		sizeUsed -= sizeof(valueType);

		try {
			resize(-(s64)sizeof(valueType), false);
		} catch ( const BadResize &e ) {
			CHKD_ERR("Failed to decrease size of %s buffer: %s", bufTitle, e.what());
			return false;
		}
		return true;
	}
	return false;
}
template bool buffer::del<u8>(u32 location);
template bool buffer::del<ChkUnit>(u32 location);

bool buffer::delRange(u32 startLocation, u32 endLocation)
{
	if ( this != nullptr && endLocation <= sizeUsed && startLocation < endLocation )
	{
		u32 differance = endLocation-startLocation;

		if ( endLocation < sizeUsed )
			memmove(&data[startLocation], &data[endLocation], sizeUsed-endLocation);

		sizeUsed -= differance;

		try {
			resize(-(s64)differance, false);
		} catch ( const BadResize &e ) {
			CHKD_ERR("Failed to decrease size of %s buffer: %s", bufTitle, e.what());
			return false;
		}
		return true;
	}
	return false;
}

bool buffer::del(u32 startLocation, u32 size)
{
	if ( this != nullptr )
	{
		u32 endLocation = startLocation+size;
		if ( endLocation <= sizeUsed )
		{
			if ( endLocation < sizeUsed )
				memmove(&data[startLocation], &data[endLocation], sizeUsed-endLocation);

			sizeUsed -= size;

			try {
				resize(-(s64)size, false);
			} catch ( const BadResize &e ) {
				CHKD_ERR("Failed to decrease size of %s buffer: %s", bufTitle, e.what());
				return false;
			}
			return true;
		}
	}
	return false;
}

////////////////
// IO Methods //
////////////////

void buffer::write(FILE* pFile)
{
	if ( this != nullptr )
	{
		fwrite(bufTitle, 4, 1, pFile);
		fwrite(&sizeUsed, 4, 1, pFile);
		if ( sizeUsed > 0 )
			fwrite(data, sizeUsed, 1, pFile);
	}
}

bool buffer::extract(buffer &buf, u32 &position)
{
	if ( this != nullptr )
	{
		if ( sizeUsed > 0 ) // Specified buffer already contains data!
			flush(); // Clear the buffer: StarCraft only reads the last instance of the given section

		position += 4;

		u32 bufSize;
		if ( !buf.get<u32>(bufSize, position) )
			return false;

		position += 4;

		if ( position+bufSize > buf.sizeUsed )
			return false;

		try {
			resize(bufSize, false);
		} catch ( const BadResize &e ) {
			CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
			return false;
		}

		sizeUsed = 0;
		sizeAllotted = bufSize;

		addStr((const char*)&buf.data[position], bufSize);

		position += bufSize;
		
		return true;
	}
	return false;
}

bool buffer::serialize(void* &destData)
{
	if ( this != nullptr )
	{
		char* rawData = nullptr;
		try {
			rawData = new char[sizeUsed+8];
		} catch ( std::bad_alloc ) {
			destData = nullptr;
			return false;
		}

		(u32&)rawData[0] = (u32&)bufTitle;
		(u32&)rawData[4] = sizeUsed;
		if ( sizeUsed > 0 )
			memcpy(&rawData[8], data, sizeUsed);

		destData = (void*)rawData;
		return true;
	}
	return false;
}

bool buffer::deserialize(const void* incomingData)
{
	if ( this != nullptr && incomingData != nullptr )
	{
		const char* rawData = (const char*)incomingData;
		u32 incomingSize = (u32&)rawData[4];
		return overwrite(&rawData[8], incomingSize) && setTitle((u32&)rawData[0]);
	}
	return false;
}

//////////
// Misc //
//////////

bool buffer::exists()
{
	return this != nullptr;	
}

bool buffer::setSize(u32 size)
{
	if ( this != nullptr && size >= sizeUsed )
	{
		try {
			resize(size-sizeAllotted, false);
		} catch ( const BadResize &e ) {
			CHKD_ERR("Failed to set new size of %s buffer: %s", bufTitle, e.what());
			return false;
		}
		return true;
	}
	return false;
}

void buffer::flush()
{
	if ( this != nullptr )
	{
		if ( data )
		{
			delete[] data;
			data = nullptr;
		}

		sizeUsed = 0;
		sizeAllotted = 0;
	}
}

//////////////////
// Priv Methods //
//////////////////

bool buffer::resize(s64 sizeChange, bool multiplySize)
{
	if ( this != nullptr )
	{
		if ( sizeChange > 0 && multiplySize )
			sizeAllotted = (u32)(sizeAllotted*1.2+sizeChange); // Increase allotted size by 20% + required space
		else
			sizeAllotted = (u32)(sizeAllotted+sizeChange); // Decrease allotted size by amount specified

		if ( sizeUsed > 0 )
		{
			s8* newBuffer;
			try {
				newBuffer = new s8[sizeAllotted];
			} catch ( std::bad_alloc ) {
				throw( const BadResize(0) );
			}

			if ( sizeUsed <= sizeAllotted )
				memcpy( newBuffer, data, sizeUsed*sizeof(char) );
			else
				memcpy( newBuffer, data, sizeAllotted*sizeof(char) );

			if ( data )
			{
				delete[] data;
				data = nullptr;
			}

			data = newBuffer;
		}
		else
		{
			if ( data )
			{
				delete[] data;
				data = nullptr;
			}

			if ( sizeAllotted > 0 )
			{
				try {
					data = new s8[sizeAllotted];
				} catch ( std::bad_alloc ) {
					sizeAllotted = 0;
					throw( const BadResize(1) );
				}
			}
		}
		return true;
	}
	return false;
}

static const char* errorMessages[] = { 
	"could not create a temporary buffer - you may have ran out of memory.",
	"could not initialize buffer - you may have ran out of memory.",
	""
};

const char* BadResize::what() const throw()
{
	switch ( exNum )
	{
		case 0: case 1:
			return errorMessages[exNum];
			break;
		default:
			return errorMessages[2];
			break;
	}
}
