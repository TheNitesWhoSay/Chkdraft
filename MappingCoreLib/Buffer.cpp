#include "Buffer.h"
#include "Basics.h"
#include <iostream>
#include <utility>
#include <cstring>
#include <algorithm>
#include "../CommanderLib/Logger.h"
#define DEFAULT_SIZE_MULTIPLIER 1.2

extern Logger logger;

/* Allow file to be partitioned along templates */ #ifndef INCLUDE_TEMPLATES_ONLY

buffer::buffer() : data(nullptr), sizeUsed(0), sizeAllotted(0), isSizeLocked(false), bufTitle("")
{
    for ( int i=0; i<5; i++ ) // NUL fill title
        bufTitle[0] = '\0';
}

buffer::buffer(const std::string &bufferTitle) : data(nullptr), sizeUsed(0), sizeAllotted(0), isSizeLocked(false), bufTitle("")
{
    int numChars = std::min((int)bufferTitle.length(), 4);
    for ( int i=0; i<numChars; i++ )
        bufTitle[i] = bufferTitle[i];
    for ( int i=numChars+1; i<5; i++ )
        bufTitle[i] = '\0';
}

buffer::buffer(u32 bufferTitleVal) : data(nullptr), sizeUsed(0), sizeAllotted(0), isSizeLocked(false), bufTitle("")
{
    (u32&)bufTitle = bufferTitleVal;
    bufTitle[4] = '\0';
}

buffer::buffer(const buffer &rhs) : data(nullptr), sizeUsed(0), sizeAllotted(0), isSizeLocked(false), bufTitle("")
{
    for ( int i = 0; i < 5; i++ )
        this->bufTitle[i] = rhs.bufTitle[i];

    try
    {
        data = new s8[(size_t)rhs.sizeUsed];
        sizeUsed = rhs.sizeUsed;
        sizeAllotted = rhs.sizeUsed;
        std::memcpy(this->data, rhs.data, (size_t)rhs.sizeUsed);
    }
    catch ( std::exception )
    {
        data = nullptr;
        sizeUsed = 0;
        sizeAllotted = 0;
    }
}


/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename StructType>
BufferPtr buffer::make(const StructType & data)
{
    BufferPtr bufferPtr(new (std::nothrow) buffer());
    if ( bufferPtr != nullptr )
        bufferPtr->add<StructType>(data);

    return bufferPtr;
}
/* End templates */ #else

buffer::~buffer()
{
    if ( this != nullptr && data != nullptr )
        delete[] data;
}

s64 buffer::size() const
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
bool buffer::get(valueType &dest, s64 location) const
{
    if ( this != nullptr )
    {
        if ( location >= 0 && location+s64(sizeof(valueType)) <= sizeUsed )
        {
            dest = (valueType &)data[location];
            return true;
        }
    }
    return false;
}
/* End templates */ #else

bool buffer::getBit(bool &dest, s64 location, u32 bitNum)
{
    if ( this != nullptr )
    {
        if ( location < 0 )
            return false;

        location += s64(bitNum / 8);

        if ( location < sizeUsed )
        {
            dest = ( (u8(1) << (bitNum%8)) & data[location] ) != 0; // AND the requested bit
            return true;
        }
    }
    return false;
}

bool buffer::getString(char* dest, s64 location, s64 size) const
{
    if ( this != nullptr )
    {
        if ( location >= 0 && size > 0 && location+size <= sizeUsed )
        {
            std::memcpy(dest, &data[location], (size_t)size);
            return true;
        }
    }
    return false;
}

std::string buffer::getString(s64 location, s64 size) const
{
    if ( this != nullptr )
    {
        if ( location > 0 && size > 0 && location+size <= sizeUsed )
        {
            return std::string((char*)&data[location], (size_t)size);
        }
    }
    return std::string();
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::getArray(valueType* dest, s64 location, s64 numElements)
{
    if ( this != nullptr && dest != nullptr )
    {
        if ( location >= 0 && numElements > 0 && location + numElements*s64(sizeof(valueType)) <= sizeUsed )
        {
            std::memcpy(dest, &data[location], numElements*sizeof(valueType));
            return true;
        }
    }
    return false;
}
/* End templates */ #else

const void* buffer::getPtr(s64 location) const
{
    if ( this != nullptr && location >= 0 && location < sizeUsed )
        return (void*)&data[location];
    else
        return nullptr;
}

const void* buffer::getPtr(s64 location, s64 sizeRequested) const
{
    if ( this != nullptr && location >= 0 && sizeRequested > 0 && location+sizeRequested <= sizeUsed )
        return (void*)&data[location];
    else
        return nullptr;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::getPtr(valueType* &dest, s64 location, s64 sizeRequested)
{
    if ( this != nullptr && location >= 0 && sizeRequested > 0 && location+sizeRequested <= sizeUsed )
    {
        dest = (valueType*)&data[location];
        return true;
    }
    return false;
}

template <typename valueType>
valueType* buffer::getPtr()
{
    if ( this != nullptr && sizeUsed >= sizeof(valueType) )
        return (valueType*)&data[0];
    else
        return nullptr;
}

template <typename valueType>
valueType buffer::get(s64 location) const
{
    if ( this != nullptr && location >= 0 && location+s64(sizeof(valueType)) <= sizeUsed )
        return (valueType &)data[location];

    return {};
}
/* End templates */ #else

bool buffer::getBit(s64 location, u32 bitNum)
{
    if ( this != nullptr )
    {
        if ( location < 0 ) // Prevent wraparound
            return false;

        location += s64(bitNum / 8);

        if ( location < sizeUsed )
            return ( (char(1) << (bitNum%8)) & data[location] ) != 0; // AND the bit given by bitNum
    }
    return false;
}

bool buffer::is(const char* str, s64 size)
{
    return this != nullptr && this->sizeUsed == size && memcmp(str, data, (size_t)size) == 0;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::is(const valueType &value)
{
    return this != nullptr && sizeUsed == s64(sizeof(valueType)) && memcmp(data, &value, sizeUsed) == 0;
}
/* End templates */ #else

bool buffer::has(char character, s64 location)
{
    if ( this != nullptr && location >= 0 && location < sizeUsed && data[location] == character )
        return true;
    else
        return false;
}

bool buffer::hasCaseless(char character, s64 location)
{
    if ( this != nullptr && location >= 0 && location < sizeUsed && toupper(data[location]) == toupper(character) )
        return true;
    else
        return false;
}

bool buffer::has(const char* str, s64 location, s64 size)
{
    if ( this != nullptr && location >= 0 && size > 0 && location+size <= sizeUsed )
        return memcmp(str, &data[location], (size_t)size) == 0;
    else
        return false;
}

bool buffer::hasCaseless(const char* str, s64 location, s64 size)
{
    if ( this != nullptr && location >= 0 && size > 0 && location + size <= sizeUsed )
    {
        for ( s64 i = 0; i < size; i++ )
        {
            if ( toupper(str[i]) != toupper(data[location + i]) )
                return false;
        }
        return true;
    }
    return false;
}

bool buffer::getNext(char character, s64 start, s64 &dest) const
{
    if ( this != nullptr && start >= 0 )
    {
        for ( s64 i=start; i<sizeUsed; i++ )
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

bool buffer::getNextUnquoted(char character, s64 start, s64 &dest) const
{
    if ( this != nullptr && start >= 0 )
    {
        for ( s64 i=start; i<sizeUsed; i++ )
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

bool buffer::getNextUnquoted(char character, s64 start, s64 &dest, char terminator) const
{
    if ( this != nullptr && start >= 0 )
    {
        for ( s64 i=start; i<sizeUsed; i++ )
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

bool buffer::getNextUnescaped(char character, s64 start, s64 &dest) const
{
    if ( this != nullptr && start >= 0 )
    {
        for ( s64 i=start; i<sizeUsed; i++ )
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

bool buffer::getNextUnquoted(char character, s64 start, s64 end, s64 &dest) const
{
    if ( this != nullptr && start >= 0 && end > start )
    {
        for ( s64 i=start; (i<end && i<sizeUsed); i++ )
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
bool buffer::add(const valueType &value)
{
    if ( this != nullptr )
    {
        if ( sizeUsed + s64(sizeof(valueType)) > sizeAllotted )
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
bool buffer::add(const valueType &value, s64 amount)
{
    if ( this != nullptr && amount > 0 )
    {
        if ( sizeUsed+amount*s64(sizeof(valueType)) >= sizeAllotted )
        {
            try {
                resize(amount*s64(sizeof(valueType)), true);
            } catch ( const BadResize &e ) {
                CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
                return false;
            }
        }

        for ( s64 i=sizeUsed; i<amount*s64(sizeof(valueType))+sizeUsed; i+=sizeof(valueType) )
            (valueType &)data[i] = value;

        sizeUsed += amount*sizeof(valueType);
        return true;
    }
    return false;
}
/* End templates */ #else

bool buffer::addStr(const std::string &chunk)
{
    return addStr(chunk.c_str(), chunk.size());
}

bool buffer::addStr(const char* chunk, s64 chunkSize)
{
    if ( chunkSize <= 0 )
        return true; // Successful at adding nothing

    if ( this != nullptr )
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
        std::memcpy(&data[sizeUsed], chunk, (size_t)chunkSize);
        sizeUsed += chunkSize;
        return true;
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::insert(s64 location, valueType value)
{
    if ( this != nullptr && location >=0 && location <= sizeUsed )
    {
        if ( sizeUsed+s64(sizeof(valueType)) >= sizeAllotted )
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
bool buffer::insert(s64 location, valueType value, s64 amount)
{
    if ( this != nullptr && location >= 0 && location <= sizeUsed && amount > 0 )
    {
        if ( sizeUsed+s64(sizeof(valueType))*amount >= sizeAllotted )
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
                      size_t(sizeUsed - location) );
        
        for ( s64 i=0; i<amount; i++)
            (valueType &)data[location+(i*sizeof(valueType))] = value;

        sizeUsed += sizeof(valueType)*amount;
        return true;
    }
    return false;
}
/* End templates */ #else

bool buffer::insertStr(s64 startLocation, const char* chunk, s64 chunkSize)
{
    if ( this != nullptr && startLocation >= 0 && startLocation <= sizeUsed && chunkSize > 0 )
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
                      size_t(sizeUsed-startLocation) );

        std::memcpy( &data[startLocation],
                     chunk,
                     (size_t)chunkSize );

        sizeUsed += chunkSize;
        return true;
    }
    return false;
}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
bool buffer::replace(s64 location, valueType value)
{
    if ( this != nullptr && location >= 0 && location+s64(sizeof(valueType)) <= sizeUsed )
    {
        (valueType &)data[location] = value;
        return true;
    }
    return false;
}

template <typename valueType>
bool buffer::replace(s64 location, valueType value, s64 amount)
{
    if ( this != nullptr && location >= 0 && amount > 0 && location+amount*sizeof(value) <= sizeUsed )
    {
        for ( s64 i=0; i<amount; i++ )
            (valueType &)data[location+i*sizeof(value)] = value;

        return true;
    }
    return false;
}
/* End templates */ #else

bool buffer::replaceStr(s64 startLocation, const char* chunk, s64 chunkSize)
{
    if ( this != nullptr && startLocation >= 0 && chunkSize > 0 && startLocation+chunkSize <= sizeUsed )
    {
        std::memcpy( &data[startLocation],
                     chunk,
                     (size_t)chunkSize );

        return true;
    }
    return false;
}

bool buffer::replaceStr(s64 startLocation, s64 initialSize, const char* chunk, s64 chunkSize)
{
    if ( this != nullptr && startLocation >= 0 && initialSize > 0 && chunkSize > 0 && startLocation+initialSize <= sizeUsed )
    {
        s64 difference = chunkSize-initialSize;

        if ( difference > 0 && sizeUsed+difference > sizeUsed ) // Expand area, insert data
        {
            try {
                resize(difference, true);
            } catch ( const BadResize &e ) {
                CHKD_ERR("Failed to increase size of %s buffer: %s", bufTitle, e.what());
                return false;
            }

            std::memmove( &data[startLocation+initialSize], // Move everything after initial chunk forward differance
                          &data[startLocation+initialSize+difference],
                          size_t(sizeUsed-startLocation-initialSize) );

            std::memcpy( &data[startLocation],
                         chunk,
                         (size_t)chunkSize );
        }
        else if ( difference < 0 ) // Contract area, insert data
        {
            std::memmove( &data[startLocation+chunkSize], // Move everything after initial chunk back differance
                          &data[startLocation+initialSize],
                          size_t(sizeUsed-startLocation-initialSize) );

            std::memcpy( &data[startLocation],
                         chunk,
                         (size_t)chunkSize );

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
                         (size_t)chunkSize );
        }

        sizeUsed += difference;
        return true;
    }
    return false;
}

bool buffer::setBit(s64 location, u32 bitNum, bool bitValue)
{
    if ( this != nullptr )
    {
        if ( location >= 0 ) // Prevent wraparound
            return false;

        location += s64(bitNum / 8);
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
bool buffer::swap(s64 location1, s64 location2)
{
    if ( this != nullptr && location1 >= 0 && location2 >= 0 && location1+s64(sizeof(valueType)) <= sizeUsed && location2+s64(sizeof(valueType)) <= sizeUsed )
    {
        // Explictly specifying the template type in std::swap causes errors
        std::swap((valueType &)data[location1], (valueType &)data[location2]);
        return true;
    }
    return false;
}
/* End templates */ #else

bool buffer::swapStr(s64 location1, s64 location2, s64 swapSize)
{
    if ( this != nullptr && location1 >= 0 && location2 >= 0 && swapSize > 0 && location1+swapSize <= sizeUsed && location2+swapSize <= sizeUsed )
    {
        std::unique_ptr<char> temp = std::unique_ptr<char>(new char[(size_t)swapSize]);
        std::memcpy(temp.get(), &data[location1], (size_t)swapSize);
        std::memcpy(&data[location2], temp.get(), (size_t)swapSize);
        return true;
    }
    return false;
}

bool buffer::overwrite(const char* chunk, s64 chunkSize)
{
    if ( this != nullptr )
    {
        if ( chunkSize == 0 )
            flush();
        else if ( setSize(chunkSize) )
        {
            std::memcpy(data, chunk, (size_t)chunkSize);
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

bool buffer::setTitle(const std::string &newTitle)
{
    if ( this != nullptr )
    {
        int numChars = std::min((int)newTitle.length(), 4);
        for ( int i=0; i<numChars; i++ )
            bufTitle[i] = newTitle[i];
        for ( int i=numChars+1; i<5; i++ )
            bufTitle[i] = '\0';
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
bool buffer::del(s64 location)
{
    if ( this != nullptr && location >= 0 && location+s64(sizeof(valueType)) <= sizeUsed )
    {
        if ( sizeUsed-location+s64(sizeof(valueType)) > 0 )
        {
            std::memmove(
                &data[location],
                &data[location+sizeof(valueType)],
                size_t(sizeUsed-location-sizeof(valueType))
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

bool buffer::delRange(s64 startLocation, s64 endLocation)
{
    if ( this != nullptr && startLocation >= 0 && endLocation <= sizeUsed && startLocation < endLocation )
    {
        s64 difference = endLocation-startLocation;

        if ( endLocation < sizeUsed )
            std::memmove(&data[startLocation], &data[endLocation], size_t(sizeUsed-endLocation));

        sizeUsed -= difference;

        try {
            resize(-difference, false);
        } catch ( const BadResize &e ) {
            CHKD_ERR("Failed to decrease size of %s buffer: %s", bufTitle, e.what());
            return false;
        }
        return true;
    }
    return false;
}

bool buffer::del(s64 startLocation, s64 size)
{
    if ( this != nullptr && startLocation >= 0 && size > 0 )
    {
        s64 endLocation = startLocation+size;
        if ( endLocation <= sizeUsed )
        {
            if ( endLocation < sizeUsed )
                std::memmove(&data[startLocation], &data[endLocation], size_t(sizeUsed-endLocation));

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

bool buffer::trimTo(s64 size)
{
    if ( this != nullptr && size >= 0 )
    {
        s64 sizeChange = size - sizeUsed;
        if ( sizeChange == 0 )
            return true;
        else // if ( sizeChange != 0 )
        {
            try {
                resize(sizeChange, false);
            } catch ( const BadResize &e ) {
                CHKD_ERR("Failed to trim size of %s buffer: %s", bufTitle, e.what());
                return false;
            }

            if ( sizeUsed < size )
                memset(&data[sizeUsed], 0, size_t(size-sizeUsed));

            sizeUsed = size;
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
            fwrite(data, (size_t)sizeUsed, 1, pFile);
    }
}

std::ostream & buffer::write(std::ostream &outputBuffer, bool includeHeader)
{
    if ( this != nullptr )
    {
        if ( includeHeader )
        {
            outputBuffer.write(bufTitle, 4);
            outputBuffer << sizeUsed;
        }

        if ( sizeUsed > 0 )
            outputBuffer.write((const char*)data, sizeUsed);
    }
    return outputBuffer;
}

bool buffer::extract(buffer &buf, s64 position)
{
    if ( this != nullptr && position >= 0 )
    {
        if ( sizeUsed > 0 ) // Specified buffer already contains data!
            flush(); // Clear the buffer: StarCraft only reads the last instance of the given section

        position += 4;

        u32 bufSize;
        if ( !buf.get<u32>(bufSize, position) )
            return false;

        position += 4;

        if ( position+(s64)bufSize > buf.sizeUsed )
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

bool buffer::extract(buffer &src, s64 position, s64 length)
{
    if ( this != nullptr && position >= 0 && length > 0 )
    {
        flush();
        if ( length == 0 )
            return true;
        else if ( position + length > position && position + length <= src.sizeUsed )
        { // length > 0, no overflow in the position+length sum or the src buffer 
            try
            {
                data = new s8[(size_t)length];
                sizeUsed = length;
                sizeAllotted = length;
                std::memcpy(data, &src.data[position], (size_t)length);
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

std::shared_ptr<void> buffer::serialize()
{
    if ( this != nullptr )
    {
        std::shared_ptr<void> rawDataBuf = std::shared_ptr<void>(new u8[size_t(sizeUsed+8)]);
        u8* rawData = (u8*)rawDataBuf.get();
        (u32&)rawData[0] = (u32&)bufTitle;
        (u32&)rawData[4] = (u32)sizeUsed;
        if ( sizeUsed > 0 )
            std::memcpy(&rawData[8], data, (size_t)sizeUsed);

        return rawDataBuf;
    }
    return nullptr;
}

bool buffer::deserialize(const void* incomingData)
{
    if ( this != nullptr && incomingData != nullptr )
    {
        const char* rawData = (const char*)incomingData;
        u32 incomingSize = (u32&)rawData[4];
        return overwrite(&rawData[8], (s64)incomingSize) && setTitle((u32&)rawData[0]);
    }
    return false;
}

std::shared_ptr<buffer> buffer::makeCopy()
{
    return makeCopy(sizeUsed);
}

std::shared_ptr<buffer> buffer::makeCopy(s64 size)
{
    std::shared_ptr<buffer> bufferCopy(new (std::nothrow) buffer(this->titleVal()));
    if ( bufferCopy != nullptr && bufferCopy->setSize(size) )
    {
        bufferCopy->sizeUsed = size;
        if ( size > sizeUsed )
        {
            std::memcpy(bufferCopy->data, data, (size_t)sizeUsed);
            std::memset(&bufferCopy->data[sizeUsed], 0, size_t(size-sizeUsed));
        }
        else // if ( size <= sizeUsed )
            std::memcpy(bufferCopy->data, data, (size_t)size);

        return bufferCopy;
    }
    return nullptr;
}

bool FileToBuffer(const std::string &fileName, buffer &buf)
{
    throw std::exception("Not Implemented");
}

bool buffer::exists()
{
    return this != nullptr;
}

bool buffer::setSize(s64 size)
{
    if ( this != nullptr )
    {
        if ( size == sizeUsed )
            return true;
        else // if ( size != sizeUsed )
        {
            try {
                return resize(size-sizeAllotted, false);
            } catch ( const BadResize &e ) {
                CHKD_ERR("Failed to set new size of %s buffer: %s", bufTitle, e.what());
            }
        }
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
    if ( this != nullptr && sizeChange != 0 )
    {
        if ( sizeChange > 0 && multiplySize )
            sizeAllotted = (s64)(sizeAllotted*1.2+sizeChange); // Increase allotted size by 20% + required space
        else
            sizeAllotted = (s64)(sizeAllotted+sizeChange); // Decrease allotted size by amount specified

        if ( sizeUsed > 0 )
        {
            s8* newBuffer;
            try {
                newBuffer = new s8[(size_t)sizeAllotted];
            } catch ( std::bad_alloc ) {
                throw( BadResize(0) );
            }

            if ( sizeUsed <= sizeAllotted )
                std::memcpy( newBuffer, data, size_t(sizeUsed*sizeof(char)) );
            else
                std::memcpy( newBuffer, data, size_t(sizeAllotted*sizeof(char)) );

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
                    data = new s8[(size_t)sizeAllotted];
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

const char* errorMessages[] = { 
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

const char sizeLockedMessage[] = "Buffer size is locked and cannot be altered.";

/* End file partitioning */ #endif
