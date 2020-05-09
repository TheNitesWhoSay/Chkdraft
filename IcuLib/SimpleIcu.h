#ifndef SIMPLEICU_H
#define SIMPLEICU_H
#include <memory>
#include <string>

/** Some simple extensions of ICU string handling */

namespace icux {

    /** 
        The general philosophy by projects using this extension
        should be to only use UTF-8

        For situations where the libraries demand a different encoding
        use of that encoding should be minimized

        If a strings in other encodings have a clearly identifiable purpose
        such as filesystem operations or ui displays, then that string
        should have an explicit type defined so the string type/encoding
        can easily be swapped out depending on #defined variables;
        converters should be setup to and from UTF-8, and rules regarding
        how far a type should persist should be clearly defined
    */

    #ifdef _WIN32 // Compiling on windows
        #ifdef UNICODE // Compiling on windows with UNICODE defined
            #define WINDOWS_UTF16 // At present compiling with UNICODE on windows means windows functions take UTF-16 input
            #define CODEPOINT_16BIT // One codepoint is 16 bits
            #define UTF16_FILESYSTEM // The windows functions will treat input as UTF-16
            #define UTF16_UI // The windows functions will treat input as UTF-16
        #else // Compiling on windows without UNICODE defined
            #define WINDOWS_MULTIBYTE // At present compiling without UNICODE means windows functions take 8-bit ASCII
            #define CODEPOINT_8BIT // One codepoint is 8 bits
            #define UTF8_FILESYSTEM // The windows functions will treat input as 8-bit ASCII, any characters represented with more bits will have each byte treated as their own character
            #define UTF8_UI // The windows functions will treat input as 8-bit ASCII, any characters represented with more bits will have each byte treated as their own character
        #endif
    #else // Not compiling on windows
        #define NON_WINDOWS // The operating system is not explicitly supported, though UTF-8 strings are assumed
        #define CODEPOINT_8BIT // One codepoint is 8 bits
        #define UTF8_FILESYSTEM // System function are assumed to take UTF-8 input
        #define UTF8_UI // UI functions are assumed to take UTF-8 input
    #endif

    #ifdef CODEPOINT_16BIT
        typedef wchar_t codepoint; // The type of a singular codepoint on the system being compiled against (multiple codepoints may make up a single character) [UTF-16]
    #else
        typedef char codepoint; // The type of a singular codepoint on the system being compiled against (multiple codepoints may make up a single character) [UTF-8]
    #endif

    #ifdef UTF16_FILESYSTEM
        typedef std::wstring filestring; // The type of a string capable of representing the paths or names of files or directories on the current filesystem, filestrings should not persist beyond the scope of the system function call [UTF-16] 
    #else
        typedef std::string filestring; // The type of a string capable of representing the paths or names of files or directories on the current filesystem, filestrings should not persist beyond the scope of the system function call [UTF-8]
    #endif

    #ifdef UTF16_UI
        typedef std::wstring uistring; // The type of a string that can be rendered in the UI using system functions, uistrings should not persist beyond the scope of the system function call [UTF-16]
    #else
        typedef std::string uistring; // The type of a string that can be rendered in the UI using system functions, uistrings should not persist beyond the scope of the system function call [UTF-8]
    #endif

    constexpr char u8NullChar = '\0';
    constexpr wchar_t u16NullChar = '\0';
    constexpr codepoint nullChar = '\0'; // It is fairly safe to assume that a null character will always be a single codepoint


    /**
        Functions to simplify converting between UTF-8, UTF-16 and various types of strings
    */

    std::string toUtf8(const wchar_t* utf16Str, size_t length);

    std::string toUtf8(const std::string & utf8Str); // Simply returns the same string

    std::string toUtf8(const std::wstring & utf16Str);

    std::string toUtf8(wchar_t utf32Char);

    std::string toUtf8(char32_t utf32Char);

    std::wstring toUtf16(const char* utf8Str, size_t length);

    std::wstring toUtf16(const std::string & utf8Str);

    std::wstring toUtf16(const std::wstring & utf16Str); // Simply returns the same string

    filestring toFilestring(const char* utf8Str, size_t length);

    filestring toFilestring(const std::string & utf8Str);

    uistring toUistring(const char* utf8Str, size_t length);

    uistring toUistring(const std::string & utf8Str);

    uistring toUistring(const filestring & filestring);

}

#endif