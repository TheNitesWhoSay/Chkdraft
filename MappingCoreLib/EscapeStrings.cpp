#include "EscapeStrings.h"
#include <iostream>

bool getOneCharHexVal(char character, u8 & value)
{
    if ( character >= '0' && character <= '9' )
        value = (u8)(character - '0');
    else if ( character >= 'A' && character <= 'F' )
        value = (u8)(character - 'A' + 10);
    else if ( character >= 'a' && character <= 'f' )
        value = (u8)(character - 'a' + 10);
    else
        return false;

    return true;
}

bool getTwoCharHexVal(const std::string & firstCharPtr, u8 & value)
{
    if ( firstCharPtr[0] >= '0' && firstCharPtr[0] <= '9' )
        value = (u8)((firstCharPtr[0] - '0') << 4);
    else if ( firstCharPtr[0] >= 'A' && firstCharPtr[0] <= 'F' )
        value = (u8)((firstCharPtr[0] - 'A' + 10) << 4);
    else if ( firstCharPtr[0] >= 'a' && firstCharPtr[0] <= 'f' )
        value = (u8)((firstCharPtr[0] - 'a' + 10) << 4);
    else
        return false;

    if ( firstCharPtr[1] >= '0' && firstCharPtr[1] <= '9' )
        value += (u8)(firstCharPtr[1] - '0');
    else if ( firstCharPtr[1] >= 'A' && firstCharPtr[1] <= 'F' )
        value += (u8)(firstCharPtr[1] - 'A' + 10);
    else if ( firstCharPtr[1] >= 'a' && firstCharPtr[1] <= 'f' )
        value += (u8)(firstCharPtr[1] - 'a' + 10);
    else
        return false;

    return true;
}

bool getOneCharOctVal(const char character, u8 & value)
{
    if ( character >= '0' && character <= '7' )
    {
        value = (u8)(character - '0');
        return true;
    }
    return false;
}

bool getTwoCharOctVal(const std::string & firstCharPtr, u8 & value)
{
    if ( firstCharPtr[0] >= '0' && firstCharPtr[0] <= '7' &&
        firstCharPtr[1] >= '0' && firstCharPtr[1] <= '7' )
    {
        value = (u8)((firstCharPtr[0] - '0') << 3);
        value += (u8)(firstCharPtr[1] - '0');
        return true;
    }
    return false;
}

bool getThreeCharOctVal(const std::string & firstCharPtr, u8 & value)
{
    if ( firstCharPtr[0] >= '0' && firstCharPtr[0] <= '7' &&
        firstCharPtr[1] >= '0' && firstCharPtr[1] <= '7' &&
        firstCharPtr[2] >= '0' && firstCharPtr[2] <= '7' )
    {
        value = (u8)((firstCharPtr[0] - '0') << 6);
        value += (u8)((firstCharPtr[1] - '0') << 3);
        value += (u8)(firstCharPtr[2] - '0');
        return true;
    }
    return false;
}

RawString::RawString()
{

}

RawString::RawString(const std::string & str) : std::string(str)
{

}

RawString::~RawString()
{

}

EscString::EscString()
{

}

EscString::EscString(const std::string & str) : std::string(str)
{

}

EscString::~EscString()
{

}

ChkdString::ChkdString()
{

}

ChkdString::ChkdString(const std::string & str) : std::string(str)
{

}

ChkdString::~ChkdString()
{

}

bool ChkdString::isOneLine() const
{
    return false;
}

SingleLineChkdString::SingleLineChkdString()
{

}

SingleLineChkdString::SingleLineChkdString(const std::string & str) : ChkdString(str)
{

}

SingleLineChkdString::~SingleLineChkdString()
{

}

bool SingleLineChkdString::isOneLine() const
{
    return true;
}

bool makeEscStr(const std::string & inRawString, size_t inRawStringLength, EscString & outEscString)
{
    outEscString.clear();
    try
    {
        for ( size_t i = 0; i < inRawStringLength; i++ )
        {
            unsigned char currChar = inRawString[i];
            if ( currChar == '\n' )
                outEscString.append("\\n");
            else if ( currChar == '\r' )
                outEscString.append("\\r");
            else if ( currChar == '\t' )
                outEscString.append("\\t");
            else if ( currChar == '\"' )
                outEscString.append("\\\"");
            else if ( currChar == '\\' )
                outEscString.append("\\\\");
            else if ( currChar < 32 || currChar == 127 )
            {
                outEscString.push_back('\\');
                outEscString.push_back('x');

                if ( currChar / 16 > 9 )
                    outEscString.push_back(currChar / 16 + 'A' - 10);
                else
                    outEscString.push_back(currChar / 16 + '0');

                if ( currChar % 16 > 9 )
                    outEscString.push_back(currChar % 16 + 'A' - 10);
                else
                    outEscString.push_back(currChar % 16 + '0');
            }
            else
                outEscString.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // Catch bad_alloc and length_error
    outEscString.clear();
    return false;
}

bool makeEscStr(const RawString & inRawString, EscString & outEscString)
{
    return makeEscStr(inRawString, inRawString.length(), outEscString);
}

bool getSlashEscCodeChar(const std::string & escString, size_t escStringLength, size_t slashPos, char & character, size_t & lastCharPos)
{
    lastCharPos = 0;
    character = '\0';

    if ( slashPos < escStringLength && escString[slashPos] == '\\' )
    {
        if ( slashPos + 1 == escStringLength ) // No characters follow the slash
        {
            lastCharPos = slashPos;
            character = '\\';
        }
        else if ( slashPos + 1 < escStringLength ) // Characters follow the slash
        {
            u8 numValue = 0;
            unsigned char firstEscChar = escString[slashPos + 1];
            if ( firstEscChar == 'X' || firstEscChar == 'x' )
            {
                if ( slashPos + 3 < escStringLength && getTwoCharHexVal(&escString[slashPos + 2], numValue) )
                {
                    lastCharPos = slashPos + 3;
                    character = (char)numValue;
                }
                else if ( slashPos + 2 < escStringLength && getOneCharHexVal(escString[slashPos + 2], numValue) )
                {
                    lastCharPos = slashPos + 2;
                    character = (char)numValue;
                }
                else
                {
                    lastCharPos = slashPos + 1;
                    character = (char)firstEscChar;
                }
            }
            else if ( firstEscChar >= '0' && firstEscChar <= '7' ) // Octal
            {
                if ( slashPos + 3 < escStringLength && getThreeCharOctVal(&escString[slashPos + 1], numValue) )
                {
                    lastCharPos = slashPos + 3;
                    character = (char)numValue;
                }
                else if ( slashPos + 2 < escStringLength && getTwoCharOctVal(&escString[slashPos + 1], numValue) )
                {
                    lastCharPos = slashPos + 2;
                    character = (char)numValue;
                }
                else if ( getOneCharOctVal(firstEscChar, numValue) )
                {
                    lastCharPos = slashPos + 1;
                    character = (char)numValue;
                }
            }
            else
            {
                lastCharPos = slashPos + 1;
                switch ( firstEscChar )
                {
                    case 'a': character = '\a'; break;
                    case 'b': character = '\b'; break;
                    case 'f': character = '\f'; break;
                    case 'n': character = '\n'; break;
                    case 'r': character = '\r'; break;
                    case 't': character = '\t'; break;
                    case 'v': character = '\v'; break;
                    case '\\': character = '\\'; break;
                    case '\'': character = '\''; break;
                    case '\"': character = '\"'; break;
                    case '?': character = '\?'; break;
                    default: character = firstEscChar; break;
                }
            }
        }
        return true;
    }
    return false;
}

bool parseEscStr(const EscString & inEscString, RawString & outRawString)
{
    size_t strLength = inEscString.length(),
        lastEscCharPos = 0;
    char escapedChar = '\0',
        currChar = '\0';
    try
    {
        outRawString.clear();
        outRawString = "";
        for ( size_t i = 0; i < strLength; i++ )
        {
            currChar = inEscString[i];
            if ( currChar == '\\' && // Escape sequence detected
                getSlashEscCodeChar(inEscString, strLength, i, escapedChar, lastEscCharPos) &&
                escapedChar != '\0' )
            {
                outRawString.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else if ( currChar == '\0' )
            {
                outRawString.clear();
                return false;
            }
            else // Not a valid escape sequence
                outRawString.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // catches length_error and bad_alloc
    outRawString.clear();
    return false;
}

bool parseEscBytes(const EscString & inEscString, std::vector<u8> & outRawBytes)
{
    size_t strLength = inEscString.length(),
        lastEscCharPos = 0;
    char escapedChar = '\0',
        currChar = '\0';
    try
    {
        outRawBytes.clear();
        for ( size_t i = 0; i < strLength; i++ )
        {
            currChar = inEscString[i];
            if ( currChar == '\\' && // Escape sequence detected
                getSlashEscCodeChar(inEscString, strLength, i, escapedChar, lastEscCharPos) )
            {
                outRawBytes.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else // Not a '\\' or no characters follow the '\\'
                outRawBytes.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // catches length_error and bad_alloc
    outRawBytes.clear();
    return false;
}

bool MakeOneLineChkdStr(const std::string & inRawString, size_t inRawStringLength, ChkdString & outChkdString)
{
    try
    {
        outChkdString.clear();
        for ( size_t i = 0; i < inRawStringLength; i++ )
        {
            unsigned char currChar = inRawString[i];
            if ( currChar == '\r' && i + 1 < inRawStringLength && inRawString[i + 1] == '\n' )
            {
                outChkdString.append("\\r");
            }
            else if ( currChar == '\n' && i != 0 && inRawString[i - 1] == '\r' )
            {
                outChkdString.append("\\n");
            }
            else if ( currChar == '\t' )
            {
                outChkdString.append("\\t");
            }
            else if ( currChar < 32 || currChar == 127 )
            {
                outChkdString.push_back('<');

                if ( currChar / 16 > 9 )
                    outChkdString.push_back(currChar / 16 + 'A' - 10);
                else
                    outChkdString.push_back(currChar / 16 + '0');

                if ( currChar % 16 > 9 )
                    outChkdString.push_back(currChar % 16 + 'A' - 10);
                else
                    outChkdString.push_back(currChar % 16 + '0');

                outChkdString.push_back('>');
            }
            else if ( currChar == '<' )
                outChkdString.append("\\<");
            else if ( currChar == '\\' )
                outChkdString.append("\\\\");
            else
                outChkdString.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // Catch bad_alloc and length_error
    outChkdString.clear();
    return false;
}

bool makeChkdStr(const std::string & inRawString, size_t inRawStringLength, ChkdString & outChkdString)
{
    if ( outChkdString.isOneLine() )
        return MakeOneLineChkdStr(inRawString, inRawStringLength, outChkdString);

    try
    {
        outChkdString.clear();
        for ( size_t i = 0; i < inRawStringLength; i++ )
        {
            unsigned char currChar = inRawString[i];
            bool partOfNewLine = ((currChar == '\r' && i + 1 < inRawStringLength && inRawString[i+1] == '\n') ||
                (currChar == '\n' && i != 0 && inRawString[i - 1] == '\r'));

            if ( (currChar < 32 || currChar == 127) && currChar != '\t' && !partOfNewLine )
            {
                outChkdString.push_back('<');

                if ( currChar / 16 > 9 )
                    outChkdString.push_back(currChar / 16 + 'A' - 10);
                else
                    outChkdString.push_back(currChar / 16 + '0');

                if ( currChar % 16 > 9 )
                    outChkdString.push_back(currChar % 16 + 'A' - 10);
                else
                    outChkdString.push_back(currChar % 16 + '0');

                outChkdString.push_back('>');
            }
            else if ( currChar == '<' )
                outChkdString.append("\\<");
            else if ( currChar == '\\' )
                outChkdString.append("\\\\");
            else
                outChkdString.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // Catch bad_alloc and length_error
    outChkdString.clear();
    return false;
}

bool makeChkdStr(const RawString & inRawString, ChkdString & outChkdString)
{
    if ( outChkdString.isOneLine() )
        return MakeOneLineChkdStr(inRawString, inRawString.length(), outChkdString);
    else
        return makeChkdStr(inRawString, inRawString.length(), outChkdString);
}

bool getChkdEscCodeChar(const std::string & chkdString, size_t chkdStringLength, size_t lessThanPos, char & character, size_t & lastCharPos)
{
    lastCharPos = 0;
    character = '\0';

    if ( lessThanPos < chkdStringLength && chkdString[lessThanPos] == '<' )
    {
        if ( lessThanPos + 1 == chkdStringLength ) // No characters follow the '<' character
        {
            lastCharPos = lessThanPos;
            character = '<';
            return true;
        }
        else if ( lessThanPos + 1 < chkdStringLength ) // Characters follow the slash
        {
            u8 numValue = 0;
            unsigned char firstEscChar = chkdString[lessThanPos + 1];
            if ( lessThanPos + 3 < chkdStringLength && chkdString[lessThanPos + 3] == '>' &&
                getTwoCharHexVal(&chkdString[lessThanPos + 1], numValue) )
            {
                lastCharPos = lessThanPos + 3;
                character = (char)numValue;
                return true;
            }
            else if ( lessThanPos + 2 < chkdStringLength && chkdString[lessThanPos + 2] == '>' &&
                getOneCharHexVal(chkdString[lessThanPos + 1], numValue) )
            {
                lastCharPos = lessThanPos + 2;
                character = (char)numValue;
                return true;
            }
        }
    }
    return false;
}

bool parseChkdStr(const ChkdString & inChkdString, RawString & outRawString)
{
    size_t strLength = inChkdString.length(),
        lastEscCharPos = 0;
    char escapedChar = '\0',
        currChar = '\0';
    try
    {
        outRawString.clear();
        outRawString = "";
        for ( size_t i = 0; i < strLength; i++ )
        {
            currChar = inChkdString[i];
            if ( currChar == '\\' && // Possible slash escape sequence
                getSlashEscCodeChar(inChkdString, strLength, i, escapedChar, lastEscCharPos) &&
                escapedChar != '\0' )
            {
                outRawString.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else if ( currChar == '<' && /// Possible chkd escape sequence
                getChkdEscCodeChar(inChkdString, strLength, i, escapedChar, lastEscCharPos) &&
                escapedChar != '\0' )
            {
                outRawString.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else if ( currChar == '\0' ) // Is a NUL character
            {
                outRawString.clear();
                return false;
            }
            else // Not a valid escape sequence
                outRawString.push_back(currChar);
        }
        return true;
    }
    catch ( std::exception ) {} // catches length_error and bad_alloc
    outRawString.clear();
    return false;
}

bool parseChkdBytes(const ChkdString & inChkdString, std::vector<u8> & outRawBytes)
{
    size_t strLength = inChkdString.length(),
        lastEscCharPos = 0;
    char escapedChar = '\0',
        currChar = '\0';
    try
    {
        outRawBytes.clear();
        for ( size_t i = 0; i < strLength; i++ )
        {
            currChar = inChkdString[i];
            if ( currChar == '\\' && // Possible slash escape sequence
                getSlashEscCodeChar(inChkdString, strLength, i, escapedChar, lastEscCharPos) )
            {
                outRawBytes.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else if ( currChar == '<' && /// Possible chkd escape sequence
                getChkdEscCodeChar(inChkdString, strLength, i, escapedChar, lastEscCharPos) )
            {
                outRawBytes.push_back(escapedChar);
                i = lastEscCharPos;
            }
            else // Not a valid escape sequence
                outRawBytes.push_back(escapedChar);
        }
        return true;
    }
    catch ( std::exception ) {} // catches length_error and bad_alloc
    outRawBytes.clear();
    return false;
}

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const RawString & inString, RawString & outString)
{
    outString = inString;
}
template void convertStr<RawString, RawString>(const RawString & inString, RawString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const RawString & inString, EscString & outString)
{
    makeEscStr(inString, outString);
}
template void convertStr<RawString, EscString>(const RawString & inString, EscString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const RawString & inString, ChkdString & outString)
{
    makeChkdStr(inString, outString);
}
template void convertStr<RawString, ChkdString>(const RawString & inString, ChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const RawString & inString, SingleLineChkdString & outString)
{
    MakeOneLineChkdStr(inString, inString.length(), outString);
}
template void convertStr<RawString, SingleLineChkdString>(const RawString & inString, SingleLineChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const EscString & inString, RawString & outString)
{
    parseEscStr(inString, outString);
}
template void convertStr<EscString, RawString>(const EscString & inString, RawString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const EscString & inString, EscString & outString)
{
    outString = inString;
}
template void convertStr<EscString, EscString>(const EscString & inString, EscString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const EscString & inString, ChkdString & outString)
{
    RawString rawString;
    parseEscStr(inString, rawString);
    makeChkdStr(rawString, outString);
}
template void convertStr<RawString, ChkdString>(const EscString & inString, ChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const EscString & inString, SingleLineChkdString & outString)
{
    RawString rawString;
    parseEscStr(inString, rawString);
    MakeOneLineChkdStr(rawString, rawString.length(), outString);
}
template void convertStr<RawString, SingleLineChkdString>(const EscString & inString, SingleLineChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const ChkdString & inString, RawString & outString)
{
    parseChkdStr(inString, outString);
}
template void convertStr<ChkdString, RawString>(const ChkdString & inString, RawString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const ChkdString & inString, EscString & outString)
{
    RawString rawString;
    parseChkdStr(inString, rawString);
    makeEscStr(rawString, outString);
}
template void convertStr<ChkdString, EscString>(const ChkdString & inString, EscString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const ChkdString & inString, ChkdString & outString)
{
    outString = inString;
}
template void convertStr<ChkdString, ChkdString>(const ChkdString & inString, ChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const ChkdString & inString, SingleLineChkdString & outString)
{
    RawString rawString;
    parseChkdStr(inString, rawString);
    MakeOneLineChkdStr(rawString, rawString.length(), outString);
}
template void convertStr<ChkdString, SingleLineChkdString>(const ChkdString & inString, SingleLineChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const SingleLineChkdString & inString, RawString & outString)
{
    parseChkdStr(inString, outString);
}
template void convertStr<SingleLineChkdString, RawString>(const SingleLineChkdString & inString, RawString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const SingleLineChkdString & inString, EscString & outString)
{
    RawString rawString;
    parseChkdStr(inString, rawString);
    makeEscStr(rawString, outString);
}
template void convertStr<SingleLineChkdString, EscString>(const SingleLineChkdString & inString, EscString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const SingleLineChkdString & inString, ChkdString & outString)
{
    RawString rawString;
    parseChkdStr(inString, rawString);
    makeChkdStr(rawString, outString);
}
template void convertStr<SingleLineChkdString, ChkdString>(const SingleLineChkdString & inString, ChkdString & outString);

template <typename StringTypeIn, typename StringTypeOut>
void convertStr(const SingleLineChkdString & inString, SingleLineChkdString & outString)
{
    outString = inString;
}
template void convertStr<SingleLineChkdString, SingleLineChkdString>(const SingleLineChkdString & inString, SingleLineChkdString & outString);
