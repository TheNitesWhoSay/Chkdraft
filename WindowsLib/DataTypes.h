#ifndef DATATYPES_H
#define DATATYPES_H
#include <cstdint>
#include <string>

namespace WinLib {

    typedef uint64_t u64;
    typedef int64_t s64;
    typedef uint32_t u32;
    typedef int32_t s32;
    typedef uint16_t u16;
    typedef int16_t s16;
    typedef uint8_t u8;
    typedef int8_t s8;

    struct rect // Mirrors win32 RECT
    {
        s32 left;
        s32 top;
        s32 right;
        s32 bottom;
    };

}

#ifdef output_param
#undef output_param
#endif
#define output_param /* Syntactic sugar denoting an output parameter - unless a function indicates that there has been an error it's obligated to set out params before returning */

#endif