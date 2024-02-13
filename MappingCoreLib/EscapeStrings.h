#ifndef ESCAPESTRINGS_H
#define ESCAPESTRINGS_H
#include "Basics.h"
#include <string>
#include <vector>

/**
    This file defines several string types that extend basic strings in ways useful for mapping purposes
    Using these types removes all ambiguity as to what type of string you're trying to use and aids in conversion between different formats
    The type can tell you if it's the actual raw bytes stored in the scenario file, something with C++ like escape notation, or some more user friendly notation
*/

/** A Scenario string without any modification
    May not contain NUL characters besides the terminator. */
class RawString : public std::string
{
    public:
        using std::string::string;
        RawString();
        RawString(const std::string & str);
        virtual ~RawString();
};

/** A Scenario string in the slash escaped format.
    See MakeEscStr and ParseEscStr for details on use.

    Note that while you may use MakeEscStr to create or
    display text in some places, in many situations you
    may wish to (and can) use ParseChkdStr to read the
    same text after it is edited by the user.  */
class EscString : public std::string
{
    public:
        using std::string::string;
        EscString();
        EscString(const std::string & str);
        virtual ~EscString();
};

/** A Scenario string in Chkdrafts <00> format; it may also
    use the slash escaped format (though by default the <00>
    format will given by programs).
    See MakeChkdStr and ParseChkdStr for details on use */
class ChkdString : public std::string
{
    public:
        using std::string::string;
        ChkdString();
        ChkdString(const std::string & str);
        virtual ~ChkdString();

        virtual bool isOneLine() const;
};

class SingleLineChkdString : public ChkdString
{
    public:
        using ChkdString::ChkdString;
        SingleLineChkdString();
        SingleLineChkdString(const std::string & str);
        virtual ~SingleLineChkdString();

        virtual bool isOneLine() const;
};

/** Makes an EscString from a RawString using these rules...
    All "\n" in the RawString become "\\n" in the EscString.
    All "\r" in the RawString become "\\r" in the EscString.
    All "\t" in the RawString become "\\t" in the EscString.
    All "\"" in the RawString become "\\\"" in the EscString.
    All "\\" in the RawString become "\\\\" in the EscString.
    All ASCII Characters < 32 or >= 127 are given a \xhh format,
        where hh is the corresponding hex string.
    Returns true and outEscString on success, false and an empty outEscString on faliure. */
bool makeEscStr(const std::string & inRawString, size_t inRawStringLength, EscString & outEscString);
bool makeEscStr(const RawString & inRawString, EscString & outEscString);

bool getSlashEscCodeChar(const std::string & escString, size_t escStringLength, size_t slashPos, char & character, size_t & lastCharPos);

/** Parsing an EscString into a RawString uses these rules...
    All "\\a" in the EscString become "\a" in the RawString.
    All "\\b" in the EscString become "\b" in the RawString.
    All "\\f" in the EscString become "\f" in the RawString.
    All "\\n" in the EscString become "\n" in the RawString.
    All "\\r" in the EscString become "\r" in the RawString.
    All "\\t" in the EscString become "\t" in the RawString.
    All "\\v" in the EscString become "\v" in the RawString.
    All "\\\\" in the EscString become "\\" in the RawString.
    All "\\\'" in the EscString become "\'" in the RawString.
    All "\\\"" in the EscString become "\"" in the RawString.
    All "\\?" in the EscString become "\?" in the RawString.
    All "\\%" where % is a character not listed above become "%" in the RawString.
    All "\\ooo" in the EscString become ASCII[ooo] in the RawString
        where ooo is a non-zero octal number.
    All "\\xhh" in the EscString becomes ASCII[hh] in the RawString,
        where hh is a non-zero hex string (this is the standard format
        for escaped characters in many languages, including C/C++).
    All other characters/sequences are added as they are.

    Returns true and outRawString on success, false and an empty outRawString on faliure. */
bool parseEscStr(const EscString & inEscString, RawString & outRawString);
/** Parses a EscString into RawBytes, same rules as ParseEscStr except...
    All "\\0" in the EscString become "\0" in the RawString. */
bool parseEscBytes(const EscString & inEscString, std::vector<u8> & outRawBytes);

/** Makes a ChkdString from a RawString using these rules...
    All "\r\n" (paired only) and "\t" are added as normal.
    All characters/sequences except those listed above that are < 32 or >= 127
        are given a <hh> format, where hh is the corresponding hex string.
    All "\\" in the RawString become "\\\\" in the ChkdString.
    All "<" in the RawString become "\\<" in the ChkdString.
    All other characters >= 32 or < 127 are added as they are.

    Returns true and outChkdString on success, false and an empty outChkdString on faliure. */
bool makeChkdStr(const std::string & inRawString, size_t inRawStringLength, ChkdString & outChkdString);
bool makeChkdStr(const RawString & inRawString, ChkdString & outChkdString);

bool getChkdEscCodeChar(const std::string & chkdString, size_t chkdStringLength, size_t lessThanPos, char & character, size_t & lastCharPos);

/** Parses a ChkdString into a RawString using these rules...
    All "\\<" in the ChkdString become "<" in the RawString.
    All "\\>" in the ChkdString become ">" in the RawString.
    All "<h>" in the ChkdString become ASCII[h] in the RawString,
        where h is a non-zero hex character.
    All "<hh>" in the ChkdString become ASCII[hh] in the RawString,
        where hh is a non-zero hex string.
    All rules for ParseEscStr also apply.
    Upon finding any NUL characters escaped or otherwise this method fails.

    Returns true and outRawString on success, false and an empty outRawString on faliure. */
bool parseChkdStr(const ChkdString & inChkdString, RawString & outRawString);
/** Parses a ChkdString into RawBytes, same rules as ParseChkdStr except...
    All "\\0" in the ChkdString become "\0" in the RawBytes. */
bool parseChkdBytes(const ChkdString & inChkdString, std::vector<u8> & outRawBytes);

bool getOneCharHexVal(const char character, u8 & value);

bool getTwoCharHexVal(const std::string & firstCharPtr, u8 & value); // firstCharPtr must point to a string at least 2 characters long

bool getOneCharOctVal(const char character, u8 & value);

bool getTwoCharOctVal(const std::string & firstCharPtr, u8 & value); // firstCharPtr must point to a string at least 2 characters long

bool getThreeCharOctVal(const std::string & firstCharPtr, u8 & value); // firstCharPtr must point to a string at least 3 characters long

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const StringTypeIn & inString, StringTypeOut & outString);

#endif