#ifndef BASICS_H
#define BASICS_H
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <cstdarg>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include "../CommanderLib/Logger.h"

extern Logger logger;

/**
    Basics contains several things...
        - Logging and debugging
        - Syntactic Sugar (e.g. output_param, inout_param)
        - Definitions for useful data types (e.g. u8: unsigned 8-bit number, s8: signed 8-bit number) and their limits (e.g. s8_min, s8_max)
        - Definitions for useful generic constants (e.g. size_1kb, size_1mb)
        - Definitions for bit and xBit values (xBit being the inversion of the bit for a given data size) and arrays for enumerating the bit values
        - The "NotImplemented" exception class
        - Convenience macros (e.g. enchanced enumerators) and methods

    Basics should not contain any system specific code
*/

#ifdef CHKDRAFT // Globally defined in project properties if this is used in Chkdraft
void Debug();
void DebugIf(bool condition);
void NoDebug();
void mb(const std::string & text); // Basic message box message
void mb(int i, const std::string & text);
void Error(const std::string & ErrorMessage); // Basic error message box
extern void PrintError(const std::string & file, unsigned int line, const std::string msg, ...); // Prints to LastError and LastErrorLoc
void ShoutError(const std::string & file, unsigned int line, const std::string msg, ...);
#if defined(_MSC_VER) && !defined(__clang__)
#define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, __VA_ARGS__) // Prints a detailed error
#define CHKD_SHOUT(msg, ...) ShoutError(__FILE__, __LINE__, msg, __VA_ARGS__) // Shouts an error message
#elif defined(__GNUC__) || defined(__clang__)
#define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, ##__VA_ARGS__) // Prints a detailed error
#define CHKD_SHOUT(msg, ...) ShoutError(__FILE__, __LINE__, msg, ##__VA_ARGS__) // Shouts an error message
#else
#error Other compiler!
#endif 
#else
void IgnoreErr(const std::string & file, unsigned int line, const std::string msg, ...); // Ignores an error message
#if defined(_MSC_VER)
#define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__)
#define CHKD_SHOUT(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__)
#elif defined(__GNUC__)
#define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, ##__VA_ARGS__)
#define CHKD_SHOUT(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#error Other compiler!
#endif
#endif

#ifdef output_param
#undef output_param
#endif
#define output_param /* Syntactic sugar denoting an output parameter - unless a function indicates that there has been an error it's obligated to set out params before returning */

#ifdef inout_param
#undef inout_param
#endif
#define inout_param /* Syntactic sugar denoting a parameter that may optionally be used for input, and unless a function indicates that there's been an error it's obligated to set inout params before returning */

using u8 = std::uint8_t;
using s8 = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u64 = std::uint64_t;
using s64 = std::int64_t;

constexpr u8 u8_max = UINT8_MAX;
constexpr u16 u16_max = UINT16_MAX;
constexpr u32 u32_max = UINT32_MAX;
constexpr u64 u64_max = UINT64_MAX;

constexpr s8 s8_min = INT8_MIN;
constexpr s8 s8_max = INT8_MAX;
constexpr s16 s16_min = INT16_MIN;
constexpr s16 s16_max = INT16_MAX;
constexpr s32 s32_min = INT32_MIN;
constexpr s32 s32_max = INT32_MAX;
constexpr s64 s64_min = INT64_MIN;
constexpr s64 s64_max = INT64_MAX;

constexpr u32 size_1kb = 0x400;
constexpr u32 size_1mb = 0x100000;
constexpr u32 size_1gb = 0x40000000;
constexpr u64 size_1tb = 0x10000000000;

#define BIT_0  0x1
#define BIT_1  0x2
#define BIT_2  0x4
#define BIT_3  0x8
#define BIT_4  0x10
#define BIT_5  0x20
#define BIT_6  0x40
#define BIT_7  0x80

#define BIT_8  0x100
#define BIT_9  0x200
#define BIT_10 0x400
#define BIT_11 0x800
#define BIT_12 0x1000
#define BIT_13 0x2000
#define BIT_14 0x4000
#define BIT_15 0x8000

#define BIT_16 0x10000
#define BIT_17 0x20000
#define BIT_18 0x40000
#define BIT_19 0x80000
#define BIT_20 0x100000
#define BIT_21 0x200000
#define BIT_22 0x400000
#define BIT_23 0x800000

#define BIT_24 0x1000000
#define BIT_25 0x2000000
#define BIT_26 0x4000000
#define BIT_27 0x8000000
#define BIT_28 0x10000000
#define BIT_29 0x20000000
#define BIT_30 0x40000000
#define BIT_31 0x80000000

#define x8BIT_0 0xFE
#define x8BIT_1 0xFD
#define x8BIT_2 0xFB
#define x8BIT_3 0xF7
#define x8BIT_4 0xEF
#define x8BIT_5 0xDF
#define x8BIT_6 0xBF
#define x8BIT_7 0x7F

#define x16BIT_0 0xFFFE
#define x16BIT_1 0xFFFD
#define x16BIT_2 0xFFFB
#define x16BIT_3 0xFFF7
#define x16BIT_4 0xFFEF
#define x16BIT_5 0xFFDF
#define x16BIT_6 0xFFBF
#define x16BIT_7 0xFF7F

#define x16BIT_8 0xFEFF
#define x16BIT_9 0xFDFF
#define x16BIT_10 0xFBFF
#define x16BIT_11 0xF7FF
#define x16BIT_12 0xEFFF
#define x16BIT_13 0xDFFF
#define x16BIT_14 0xBFFF
#define x16BIT_15 0x7FFF


#define x32BIT_0 0xFFFFFFFE
#define x32BIT_1 0xFFFFFFFD
#define x32BIT_2 0xFFFFFFFB
#define x32BIT_3 0xFFFFFFF7
#define x32BIT_4 0xFFFFFFEF
#define x32BIT_5 0xFFFFFFDF
#define x32BIT_6 0xFFFFFFBF
#define x32BIT_7 0xFFFFFF7F

#define x32BIT_8 0xFFFFFEFF
#define x32BIT_9 0xFFFFFDFF
#define x32BIT_10 0xFFFFFBFF
#define x32BIT_11 0xFFFFF7FF
#define x32BIT_12 0xFFFFEFFF
#define x32BIT_13 0xFFFFDFFF
#define x32BIT_14 0xFFFFBFFF
#define x32BIT_15 0xFFFF7FFF

#define x32BIT_16 0xFFFEFFFF
#define x32BIT_17 0xFFFDFFFF
#define x32BIT_18 0xFFFBFFFF
#define x32BIT_19 0xFFF7FFFF
#define x32BIT_20 0xFFEFFFFF
#define x32BIT_21 0xFFDFFFFF
#define x32BIT_22 0xFFBFFFFF
#define x32BIT_23 0xFF7FFFFF

#define x32BIT_24 0xFEFFFFFF
#define x32BIT_25 0xFDFFFFFF
#define x32BIT_26 0xFBFFFFFF
#define x32BIT_27 0xF7FFFFFF
#define x32BIT_28 0xEFFFFFFF
#define x32BIT_29 0xDFFFFFFF
#define x32BIT_30 0xBFFFFFFF
#define x32BIT_31 0x7FFFFFFF

const u8 u8Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7 };

const u16 u16Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15 };

const u32 u32Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15,
BIT_16, BIT_17, BIT_18, BIT_19, BIT_20, BIT_21, BIT_22, BIT_23,
BIT_24, BIT_25, BIT_26, BIT_27, BIT_28, BIT_29, BIT_30, BIT_31 };

class NotImplemented : public std::logic_error
{
    public:
        NotImplemented() : std::logic_error("Function not yet implemented") { };
        NotImplemented(const std::string & str) : std::logic_error(str) { };
};


#define foreachin(item, range) for ( auto item = range.first; item != range.second; ++ item)

template <typename valueType>
void AscendingOrder(valueType & low, valueType & high);

u32 SmallestOf(u32 & first, u32 & second);

s32 SmallestOf(s32 & first, s32 & second);

u32 SmallestOf(u32 & first, u32 & second, u32 & third);

s32 SmallestOf(s32 & first, s32 & second, s32 & third);

void LongToBytes(s32 value, u8* bytes);

void ShortToBytes(u16 value, u8* bytes);

s32 TripletToInt(u8* triplet);

template <typename T>
s32 round(T value);

template <typename T>
inline std::string to_hex_string(const T & t)
{
    char buf[36];
    std::snprintf(buf, sizeof(buf)/sizeof(char), "0x%X", t);
    return std::string(buf);
}

/**
    enum_t "enum type (scoped)" assumes the property of enum classes that encloses the enum values within a particular scope
    e.g. MyClass::MyEnum::Value cannot be accessed via MyClass::Value (as it could with regular enums) and potentially cause redefinition errors
    while avoiding the property of enum classes that removes the one-to-one relationship with the underlying type (which forces excessive type-casting)

    Usage:
    enum_t(name, type, {
        enumerator = constexpr,
        enumerator = constexpr,
        ...
    });
*/
#pragma warning(disable: 26812) // In the context of using enum_t, enum class is definitely not preferred, disable the warning in visual studios

/** enum_t "enum type (scoped)" documentation minimized for expansion visibility, see definition for description and usage */
#define enum_t(name, type, ...) struct name ## _ { enum type ## _ : type __VA_ARGS__; }; using name = name ## _::type ## _;


#define INCLUDE_TEMPLATES_ONLY
#include "Basics.cpp"
#undef INCLUDE_TEMPLATES_ONLY

#endif