#ifndef ESCAPESTRINGS_H
#define ESCAPESTRINGS_H
#include "Basics.h"
#include <string>
#include <vector>

/** A Scenario string without any modification
	May not contain NUL characters besides the terminator. */
class RawString : public std::string
{
	public:
		RawString() : std::string() { }
		RawString(const std::string &str) : std::string(str) { }
		RawString(const std::string &str, size_t pos, size_t len = std::string::npos) : std::string(str, pos, len) { }
		RawString(const char* s) : std::string(s) { }
		RawString(const char* s, size_t n) : std::string(s, n) { }
		RawString(size_t n, char c) : std::string(n, c) { }
		template <class InputIterator>
		RawString(InputIterator first, InputIterator last) : std::string(first, last) { }
		~RawString() { }
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
		EscString() : std::string() { }
		EscString(const std::string &str) : std::string(str) { }
		EscString(const std::string &str, size_t pos, size_t len = std::string::npos)
			: std::string(str, pos, len) { }
		EscString(const char* s) : std::string(s) { }
		EscString(const char* s, size_t n) : std::string(s, n) { }
		EscString(size_t n, char c) : std::string(n, c) { }
		template <class InputIterator>
		EscString(InputIterator first, InputIterator last) : std::string(first, last) { }
		~EscString() { }
};

/** A Scenario string in CHKDrafts <00> format; it may also
	use the slash escaped format (though by default the <00>
	format will given by programs).
	See MakeChkdStr and ParseChkdStr for details on use */
class ChkdString : public std::string
{
	public:
		ChkdString(bool isOneLine = false) : std::string(), isOneLine(isOneLine) { }
		ChkdString(const std::string &str, bool isOneLine = false) : std::string(str), isOneLine(isOneLine) { }
		ChkdString(const std::string &str, size_t pos, bool isOneLine = false, size_t len = std::string::npos)
			: std::string(str, pos, len), isOneLine(isOneLine) { }
		ChkdString(const char* s, bool isOneLine = false) : std::string(s), isOneLine(isOneLine) { }
		ChkdString(const char* s, size_t n, bool isOneLine = false) : std::string(s, n), isOneLine(isOneLine) { }
		ChkdString(size_t n, char c, bool isOneLine = false) : std::string(n, c), isOneLine(isOneLine) { }
		template <class InputIterator>
		ChkdString(InputIterator first, InputIterator last, bool isOneLine = false)
			: std::string(first, last), isOneLine(isOneLine) { }
		~ChkdString() { }
		bool IsOneLine() { return this->isOneLine; }
		void SetOneLine(bool isOneLine) { this->isOneLine = isOneLine; }

	private:
		bool isOneLine;
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
bool MakeEscStr(const char* inRawString, size_t inRawStringLength, EscString &outEscString);
bool MakeEscStr(RawString &inRawString, EscString &outEscString);

bool GetSlashEscCodeChar(const char* escString, size_t escStringLength, size_t slashPos, char &character, size_t &lastCharPos);

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
bool ParseEscStr(EscString &inEscString, RawString &outRawString);
/** Parses a EscString into RawBytes, same rules as ParseEscStr except...
	All "\\0" in the EscString become "\0" in the RawString. */
bool ParseEscBytes(EscString &inEscString, std::vector<u8> &outRawBytes);

/** Makes a ChkdString from a RawString using these rules...
	All "\r\n" (paired only) and "\t" are added as normal.
	All characters/sequences except those listed above that are < 32 or >= 127
		are given a <hh> format, where hh is the corresponding hex string.
	All "\\" in the RawString become "\\\\" in the ChkdString.
	All "<" in the RawString become "\\<" in the ChkdString.
	All other characters >= 32 or < 127 are added as they are.

	Returns true and outChkdString on success, false and an empty outChkdString on faliure. */
bool MakeChkdStr(const char* inRawString, size_t inRawStringLength, ChkdString &outChkdString);
bool MakeChkdStr(RawString &inRawString, ChkdString &outChkdString);

bool GetChkdEscCodeChar(const char* chkdString, size_t chkdStringLength, size_t lessThanPos, char &character, size_t &lastCharPos);

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
bool ParseChkdStr(const ChkdString &inChkdString, RawString &outRawString);
/** Parses a ChkdString into RawBytes, same rules as ParseChkdStr except...
	All "\\0" in the ChkdString become "\0" in the RawBytes. */
bool ParseChkdBytes(const ChkdString &inChkdString, std::vector<u8> &outRawBytes);

bool getOneCharHexVal(const char character, u8 &value);

bool getTwoCharHexVal(const char* firstCharPtr, u8 &value); // firstCharPtr must point to a string at least 2 characters long

bool getOneCharOctVal(const char character, u8 &value);

bool getTwoCharOctVal(const char* firstCharPtr, u8 &value); // firstCharPtr must point to a string at least 2 characters long

bool getThreeCharOctVal(const char* firstCharPtr, u8 &value); // firstCharPtr must point to a string at least 3 characters long

#endif