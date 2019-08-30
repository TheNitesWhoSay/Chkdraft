#ifndef BUFFER_H
#define BUFFER_H
#include "Basics.h"
#include <stdexcept>
#include <memory>

class buffer;
typedef std::shared_ptr<buffer> BufferPtr;

// TODO: Let this throw OutOfMemory exceptions
class buffer
{
    public:

/* Constructors */  buffer();
                    buffer(const std::string &bufferTitle);
                    buffer(u32 bufferTitleVal);
                    buffer(const buffer &rhs);
                    template <typename StructType>
                        static BufferPtr make(StructType & data);

/*  Destructor  */  virtual ~buffer();

/*  Accessors   */  s64 size() const; // Returns sizeUsed
                    const char* title(); // Returns title of buffer
                    u32 titleVal(); // Returns first for bytes of the title as an unsigned int

                    // Puts data in 'dest' if 'location' is within the buffer
                    template <typename valueType>
                        bool get(valueType &dest, s64 location) const;
                    bool getBit(bool &dest, s64 location, u32 bitNum);
                    bool getString(char* dest, s64 location, s64 size) const;
                    std::string getString(s64 location, s64 size) const;
                    template <typename valueType>
                        bool getArray(valueType* dest, s64 location, s64 numElements);

                    // Use the data pointer with care, don't read past dest[sizeUsed-location-1]
                    const void* getPtr(s64 location) const;
                    const void* getPtr(s64 location, s64 sizeRequested) const; // sizeRequested in bytes
                    template <typename valueType>
                        bool getPtr(valueType* &dest, s64 location, s64 sizeRequested); // sizeRequested in bytes

                    // Returns data if the location is < sizeUsed, otherwise returns 0
                    template <typename valueType>
                        valueType get(s64 location) const;
                    bool getBit(s64 location, u32 bitNum);

/*   Searching  */  // Returns true if the sequence was found at the location
                    bool is(const char* str, s64 size);
                    template <typename valueType>
                        bool is(const valueType &value);
                    bool has(char character, s64 location);
                    bool hasCaseless(char character, s64 location);
                    bool has(const char* str, s64 location, s64 size);
                    bool hasCaseless(const char* str, s64 location, s64 size);

                    // Puts next instance of 'character' in 'dest' if found
                    bool getNext(char character, s64 start, s64 &dest) const;
                    bool getNextUnquoted(char character, s64 start, s64 &dest) const;
                    bool getNextUnquoted(char character, s64 start, s64 &dest, char terminator) const;
                    bool getNextUnquoted(char character, s64 start, s64 end, s64 &dest) const;
                    bool getNextUnescaped(char character, s64 start, s64 &dest) const;

/*   Mutators   */  // Adds data onto the end of the buffer
                    template <typename valueType>
                        bool add(const valueType &value);
                    template <typename valueType>
                        bool add(const valueType &value, s64 amount);
                    bool addStr(const std::string &chunk);
                    bool addStr(const char* chunk, s64 chunkSize);

                    // Inserts data at a position in the buffer
                    template <typename valueType>
                        bool insert(s64 location, valueType value);
                    template <typename valueType>
                        bool insert(s64 location, valueType value, s64 amount);
                    bool insertStr(s64 startLocation, const char* chunk, s64 chunkSize);

                    // Replaces the specified portion of the buffer
                    template <typename valueType>
                        bool replace(s64 location, valueType value);
                    template <typename valueType>
                        bool replace(s64 location, valueType value, s64 amount);
                    bool replaceStr(s64 startLocation, const char* chunk, s64 chunkSize);
                    bool replaceStr(s64 startLocation, s64 initialSize, const char* chunk, s64 chunkSize);
                    bool setBit(s64 location, u32 bitNum, bool bitValue);

                    // Swap two parts of a buffer
                    template <typename valueType>
                        bool swap(s64 location1, s64 location2);
                    bool swapStr(s64 location1, s64 location2, s64 swapSize);

                    // Replace the whole buffer with the new string
                    bool overwrite(const char* chunk, s64 chunkSize);

                    // Transfer all contents of source to this buffer, this buffer is flushed before transfer, source is flushed after
                    bool takeAllData(buffer& source);

                    // Gives the section a new name
                    bool setTitle(const std::string &newTitle);
                    bool setTitle(u32 newTitle);

                    // Deletes a part of the buffer
                    template <typename valueType>
                        bool del(s64 location);
                    bool delRange(s64 startLocation, s64 endLocation);
                    bool del(s64 startLocation, s64 size);

                    // Sets a buffer to the given size, trimming any excess data and zero-filling any data that is not present
                    bool trimTo(s64 size);

/*  IO Methods  */  // Writes the data to 'pFile'
                    void write(FILE* pFile, bool includeHeader);
                    
                    // Writes the data to 'outputBuffer'
                    std::ostream & write(std::ostream &outputBuffer, bool includeHeader);

                    // Sets the title to the first four characters and extracts data the size of the following signed 32 bit int
                    bool extract(buffer &buf, s64 position);

                    // Grabs all bytes from position to position+(length-1) from src and places them in this buffer
                    bool extract(buffer &src, s64 position, s64 length);

                    // Converts the buffer to raw data
                    std::shared_ptr<void> serialize(); // First four bytes: bufTitle, second four: sizeUsed, after that is sizeUsed bytes of data

                    // Converts raw data to buffer data, be sure to free any allocated data afterwards
                    bool deserialize(const void* incomingData); // First four bytes: bufTitle, second four: sizeUsed, after is sizeUsed bytes of data

                    std::shared_ptr<buffer> makeCopy();
                    std::shared_ptr<buffer> makeCopy(s64 size);

                    friend bool getFile(void* mpqFile, const std::string &mpqPath, buffer &fileData); // Used by MpqFile
                    friend bool FileToBuffer(const std::string &fileName, buffer &buf);

/*     Misc     */  // Checks whether a referance or pointer to a buffer is valid
                    bool exists();
                    
                    // Sets the amount of data allocated for the buffer, does not change 'sizeUsed'
                    bool setSize(s64 size);

                    // Deletes data and sets sizeUsed and sizeAllotted to zero, bufTitle is unaffected
                    void flush();

    private:

/*     Data     */  s8* data; // The actual data contained by the buffer
                    s64 sizeUsed; // Size of data being used
                    s64 sizeAllotted; // Size allocated for data
                    char bufTitle[5]; // Title of the buffer
                    bool isSizeLocked;

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