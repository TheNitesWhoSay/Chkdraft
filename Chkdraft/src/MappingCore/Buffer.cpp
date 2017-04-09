#include "Buffer.h"
#include "Basics.h"
#include <iostream>
#include <utility>
#include <cstring>
#define DEFAULT_SIZE_MULTIPLIER 1.2

/* Allow file to be partitioned along templates */ #ifndef INCLUDE_TEMPLATES_ONLY

buffer::buffer() : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
    for ( int i=0; i<5; i++ ) // NUL fill title
        bufTitle[0] = '\0';
}

buffer::buffer(const char* bufferTitle) : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
    std::strncpy(bufTitle, bufferTitle, 4);
    bufTitle[4] = '\0';
}

buffer::buffer(u32 bufferTitleVal) : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
    (u32&)bufTitle = bufferTitleVal;
    bufTitle[4] = '\0';
}

buffer::buffer(const buffer &rhs) : data(nullptr), sizeUsed(0), sizeAllotted(0)
{
    for ( int i = 0; i < 5; i++ )
        this->bufTitle[i] = rhs.bufTitle[i];

    try
    {
        data = new s8[rhs.sizeUsed];
        sizeUsed = rhs.sizeUsed;
        sizeAllotted = rhs.sizeUsed;
        std::memcpy(this->data, rhs.data, rhs.sizeUsed);
    }
    catch ( std::exception )
    {
        data = nullptr;
        sizeUsed = 0;
        sizeAllotted = 0;
    }
}

buffer::~buffer()
{
    if ( this != nullptr && data != nullptr )
        delete[] data;
}

u32 buffer::size() const
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

u32 buffer::titleVal()
{
    if ( this != nullptr )
        return (u32&)bufTitle;
    else
        return 0;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
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
/* End templates */ #else

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
            std::memcpy(dest, &data[location], size);
            return true;
        }
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::getArray(valueType* dest, u32 location, u32 numElements)
{
    if ( this != nullptr && dest != nullptr )
    {
        if ( location + numElements*sizeof(valueType) > location && location + numElements*sizeof(valueType) <= sizeUsed )
        {
            std::memcpy(dest, &data[location], numElements*sizeof(valueType));
            return true;
        }
    }
    return false;
}
/* End templates */ #else

const void* buffer::getPtr(u32 location) const
{
    if ( this != nullptr && location < sizeUsed )
        return (void*)&data[location];
    else
        return nullptr;
}

const void* buffer::getPtr(u32 location, u32 sizeRequested) const
{
    if ( this != nullptr && location+sizeRequested > location && location+sizeRequested <= sizeUsed )
        return (void*)&data[location];
    else
        return nullptr;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
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

template <typename valueType>
valueType buffer::get(u32 location)
{
    if ( this != nullptr && location+sizeof(valueType) > location && location+sizeof(valueType) <= sizeUsed )
        return (valueType &)data[location];

    return 0;
}
/* End templates */ #else

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

bool buffer::has(char character, u32 location)
{
    if ( this != nullptr && location < sizeUsed && data[location] == character )
        return true;
    else
        return false;
}

bool buffer::hasCaseless(char character, u32 location)
{
    if ( this != nullptr && location < sizeUsed && toupper(data[location]) == toupper(character) )
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

bool buffer::hasCaseless(const char* str, u32 location, u32 size)
{
    if ( this != nullptr && location + size > location && location + size <= sizeUsed )
    {
        for ( u32 i = 0; i < size; i++ )
        {
            if ( toupper(str[i]) != toupper(data[location + i]) )
                return false;
        }
        return true;
    }
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
                do { i++; }
                while ( i < sizeUsed && !(data[i] == '\"' && (i == 0 || data[i - 1] != '\\')) );
            }
        }
    }
    return false;
}

bool buffer::getNextUnquoted(char character, u32 start, u32 &dest, char terminator)
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
                do { i++; }
                while ( i < sizeUsed && !(data[i] == '\"' && (i == 0 || data[i - 1] != '\\')) );
            }
            else if ( data[i] == terminator )
                return false;
        }
    }
    return false;
}

bool buffer::getNextUnescaped(char character, u32 start, u32 &dest)
{
    if ( this != nullptr )
    {
        for ( u32 i=start; i<sizeUsed; i++ )
        {
            if ( data[i] == character &&
                 ( i == 0 || data[i-1] != '\\' ) )
            {
                dest = i;
                return true;
            }
        }
    }
    return false;
}

bool buffer::getNextUnquoted(char character, u32 start, u32 end, u32 &dest)
{
    if ( this != nullptr )
    {
        for ( u32 i=start; (i<end && i<sizeUsed); i++ )
        {
            if ( data[i] == character )
            {
                dest = i;
                return true;
            }
            else if ( data[i] == '\"' )
            {
                do { i++; }
                while ( i < sizeUsed && !(data[i] == '\"' && (i == 0 || data[i-1] != '\\')) );
            }
        }
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::add(valueType value)
{
    if ( this != nullptr && sizeUsed + sizeof(valueType) > sizeUsed )
    {
        if ( sizeUsed + sizeof(valueType) > sizeAllotted )
        {
            try {
                resize(sizeof(valueType), true);
            }
            catch ( const BadResize &e ) {
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
/* End templates */ #else

bool buffer::addStr(std::string chunk)
{
    return this->addStr(chunk.c_str(), chunk.length());
}

bool buffer::addStr(const char* chunk, u32 chunkSize)
{
    if ( chunkSize == 0 )
        return true; // Successful at adding nothing

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
        std::memcpy(&data[sizeUsed], chunk, chunkSize);
        sizeUsed += chunkSize;
        return true;
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
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

        std::memmove( &data[location+sizeof(valueType)],
                      &data[location],
                      sizeUsed - location );
        
        (valueType &)data[location] = value;
        sizeUsed += sizeof(valueType);
        return true;
    }
    return false;
}

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

        std::memmove( &data[location+sizeof(valueType)*amount],
                      &data[location],
                      sizeUsed - location );
        
        for ( u32 i=0; i<amount; i++)
            (valueType &)data[location+(i*sizeof(valueType))] = value;

        sizeUsed += sizeof(valueType)*amount;
        return true;
    }
    return false;
}
/* End templates */ #else

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

        std::memmove( &data[startLocation+chunkSize],
                      &data[startLocation],
                      sizeUsed-startLocation );

        std::memcpy( &data[startLocation],
                     chunk,
                     chunkSize );

        sizeUsed += chunkSize;
        return true;
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
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
/* End templates */ #else

bool buffer::replaceStr(u32 startLocation, const char* chunk, u32 chunkSize)
{
    if ( this != nullptr && startLocation+chunkSize <= sizeUsed )
    {
        std::memcpy( &data[startLocation],
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

            std::memmove( &data[startLocation+initialSize], // Move everything after initial chunk forward differance
                          &data[startLocation+initialSize+difference],
                          sizeUsed-startLocation-initialSize );

            std::memcpy( &data[startLocation],
                         chunk,
                         chunkSize );
        }
        else if ( difference < 0 ) // Contract area, insert data
        {
            std::memmove( &data[startLocation+chunkSize], // Move everything after initial chunk back differance
                          &data[startLocation+initialSize],
                          sizeUsed-startLocation-initialSize );

            std::memcpy( &data[startLocation],
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
            std::memcpy( &data[startLocation],
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
                data[location] |= (u8(1) << (bitNum%8)); // OR on the bit
            else
                data[location] &= (~(u8(1) << (bitNum%8))); // AND off the bit

            return true;
        }
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::swap(u32 location1, u32 location2)
{
    if ( this != nullptr && location1+sizeof(valueType) <= sizeUsed && location2+sizeof(valueType) <= sizeUsed )
    {
        // Explictly specifying the template type in std::swap causes errors
        std::swap((valueType &)data[location1], (valueType &)data[location2]);
        return true;
    }
    return false;
}
/* End templates */ #else

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
        std::memcpy(temp, &data[location1], swapSize);
        std::memcpy(&data[location2], temp, swapSize);
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
            std::memcpy(data, chunk, chunkSize);
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

bool buffer::setTitle(const char* newTitle)
{
    if ( this != nullptr )
    {
        std::strncpy(bufTitle, newTitle, 4);
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

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::del(u32 location)
{
    if ( this != nullptr && location+sizeof(valueType) <= sizeUsed )
    {
        if ( s64(sizeUsed)-s64(location)+s64(sizeof(valueType)) > 0 )
        {
            std::memmove(
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
/* End templates */ #else

bool buffer::delRange(u32 startLocation, u32 endLocation)
{
    if ( this != nullptr && endLocation <= sizeUsed && startLocation < endLocation )
    {
        u32 differance = endLocation-startLocation;

        if ( endLocation < sizeUsed )
            std::memmove(&data[startLocation], &data[endLocation], sizeUsed-endLocation);

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
                std::memmove(&data[startLocation], &data[endLocation], sizeUsed-endLocation);

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

void buffer::write(FILE* pFile, bool includeHeader)
{
    if ( this != nullptr )
    {
        if ( includeHeader )
        {
            fwrite(bufTitle, 4, 1, pFile);
            fwrite(&sizeUsed, 4, 1, pFile);
        }

        if ( sizeUsed > 0 )
            fwrite(data, sizeUsed, 1, pFile);
    }
}

bool buffer::extract(buffer &buf, u32 position)
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
        return true;
    }
    return false;
}

bool buffer::extract(buffer &src, u32 position, u32 length)
{
    if ( this != nullptr )
    {
        flush();
        if ( length == 0 )
            return true;
        else if ( position + length > position && position + length <= src.sizeUsed )
        { // length > 0, no overflow in the position+length sum or the src buffer 
            try
            {
                data = new s8[length];
                sizeUsed = length;
                sizeAllotted = length;
                std::memcpy(data, &src.data[position], length);
                return true;
            }
            catch ( std::exception )
            {
                data = nullptr;
                sizeUsed = 0;
                sizeAllotted = 0;
            }
        }
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
            std::memcpy(&rawData[8], data, sizeUsed);

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
                throw(  BadResize(0) );
            }

            if ( sizeUsed <= sizeAllotted )
                std::memcpy( newBuffer, data, sizeUsed*sizeof(char) );
            else
                std::memcpy( newBuffer, data, sizeAllotted*sizeof(char) );

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
                    throw( BadResize(1) );
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

/* End file partitioning */ #endif
