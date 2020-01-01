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

#ifndef enum_t
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
#endif

#endif