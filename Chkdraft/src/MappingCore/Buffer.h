#ifndef BUFFER_H
#define BUFFER_H
#include "Basics.h"
#include <stdexcept>

class buffer
{
    public:

/* Constructors */  buffer();
                    buffer(const char* bufferTitle);
                    buffer(u32 bufferTitleVal);
                    buffer(const buffer &rhs);

/*  Destructor  */  ~buffer();

/*  Accessors   */  u32 size() const; // Returns sizeUsed
                    const char* title(); // Returns title of buffer
                    u32 titleVal(); // Returns first for bytes of the title as an unsigned int

                    // Puts data in 'dest' if 'location' is within the buffer
                    template <typename valueType>
                        bool get(valueType &dest, u32 location);
                    bool getBit(bool &dest, u32 location, u32 bitNum);
                    bool getString(char* dest, u32 location, u32 size);
                    template <typename valueType>
                        bool getArray(valueType* dest, u32 location, u32 numElements);

                    // Use the data pointer with care, don't read past dest[sizeUsed-location-1]
                    const void* getPtr(u32 location) const;
                    const void* getPtr(u32 location, u32 sizeRequested) const; // sizeRequested in bytes
                    template <typename valueType>
                        bool getPtr(valueType* &dest, u32 location, u32 sizeRequested); // sizeRequested in bytes

                    // Returns data if the location is < sizeUsed, otherwise returns 0
                    template <typename valueType>
                        valueType get(u32 location);
                    bool getBit(u32 location, u32 bitNum);

/*   Searching  */  // Returns true if the sequence was found at the location
                    bool has(char character, u32 location);
                    bool hasCaseless(char character, u32 location);
                    bool has(const char* str, u32 location, u32 size);
                    bool hasCaseless(const char* str, u32 location, u32 size);

                    // Puts next instance of 'character' in 'dest' if found
                    bool getNext(char character, u32 start, u32 &dest);
                    bool getNextUnquoted(char character, u32 start, u32 &dest);
                    bool getNextUnquoted(char character, u32 start, u32 &dest, char terminator);
                    bool getNextUnquoted(char character, u32 start, u32 end, u32 &dest);
                    bool getNextUnescaped(char character, u32 start, u32 &dest);

/*   Mutators   */  // Adds data onto the end of the buffer
                    template <typename valueType>
                        bool add(valueType value);
                    template <typename valueType>
                        bool add(valueType value, u32 amount);
                    bool addStr(std::string chunk);
                    bool addStr(const char* chunk, u32 chunkSize);

                    // Inserts data at a position in the buffer
                    template <typename valueType>
                        bool insert(u32 location, valueType value);
                    template <typename valueType>
                        bool insert(u32 location, valueType value, u32 amount);
                    bool insertStr(u32 startLocation, const char* chunk, u32 chunkSize);

                    // Replaces the specified portion of the buffer
                    template <typename valueType>
                        bool replace(u32 location, valueType value);
                    template <typename valueType>
                        bool replace(u32 location, valueType value, u32 amount);
                    bool replaceStr(u32 startLocation, const char* chunk, u32 chunkSize);
                    bool replaceStr(u32 startLocation, u32 initialSize, const char* chunk, u32 chunkSize);
                    bool setBit(u32 location, u32 bitNum, bool bitValue);

                    // Swap two parts of a buffer
                    template <typename valueType>
                        bool swap(u32 location1, u32 location2);
                    bool swapStr(u32 location1, u32 location2, u32 swapSize);

                    // Replace the whole buffer with the new string
                    bool overwrite(const char* chunk, u32 chunkSize);

                    // Transfer all contents of source to this buffer, this buffer is flushed before transfer, source is flushed after
                    bool takeAllData(buffer& source);

                    // Gives the section a new name
                    bool setTitle(const char* newTitle);
                    bool setTitle(u32 newTitle);

                    // Deletes a part of the buffer
                    template <typename valueType>
                        bool del(u32 location);
                    bool delRange(u32 startLocation, u32 endLocation);
                    bool del(u32 startLocation, u32 size);

/*  IO Methods  */  // Writes the data to 'pFile'
                    void write(FILE* pFile, bool includeHeader);

                    // Sets the title to the first four characters and extracts data the size of the following signed 32 bit int
                    bool extract(buffer &buf, u32 position);

                    // Grabs all bytes from position to position+(length-1) from src and places them in this buffer
                    bool extract(buffer &src, u32 position, u32 length);

                    // Converts the buffer to raw data, be sure to free memory if successful
                    bool serialize(void* &destData); // First four bytes: bufTitle, second four: sizeUsed, after that is sizeUsed bytes of data

                    // Converts raw data to buffer data, be sure to free any allocated data afterwards
                    bool deserialize(const void* incomingData); // First four bytes: bufTitle, second four: sizeUsed, after is sizeUsed bytes of data

                    friend bool FileToBuffer(void* &hMpq, const std::string &fileName, buffer &buf);
                    friend bool FileToBuffer(const std::string &fileName, buffer &buf);

/*     Misc     */  // Checks whether a referance or pointer to a buffer is valid
                    bool exists();
                    
                    // Sets the amount of data allocated for the buffer, does not change 'sizeUsed'
                    bool setSize(u32 size);

                    // Deletes data and sets sizeUsed and sizeAllotted to zero, bufTitle is unaffected
                    void flush();


    private:

/*     Data     */  s8* data; // The actual data contained by the buffer
                    u32 sizeUsed; // Size of data being used
                    u32 sizeAllotted; // Size allocated for data
                    char bufTitle[5]; // Title of the buffer

/* Priv Methods */  /** Called to increase or decrease buffer size
                        If multiplySize is set, it multiplies the size
                        increased to by DEFAULT_SIZE_MULTIPLIER */
                    bool resize(s64 sizeChange, bool multiplySize);
};

/**
    Thrown if allocation of extra buffer space fails,
    You can pass the number (shown below) of the
    error message you wish to have returned by a
    call to what()

    0: "could not create a temporary buffer - you may have ran out of memory.",
    1: "could not initialize buffer - you may have ran out of memory."
*/
class BadResize : public std::bad_alloc
{
    public:
        explicit BadResize(u8 inExNum) : exNum(inExNum) { }
        virtual const char* what() const throw();
        virtual ~BadResize() throw() { }

    private:
        int exNum;
};

#define INCLUDE_TEMPLATES_ONLY
#include "Buffer.cpp"
#undef INCLUDE_TEMPLATES_ONLY

#endif