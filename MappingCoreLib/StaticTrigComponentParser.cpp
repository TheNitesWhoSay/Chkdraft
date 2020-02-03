#include "StaticTrigComponentParser.h"
#include <exception>
#include <string>

StaticTrigComponentParser::~StaticTrigComponentParser()
{

}

bool StaticTrigComponentParser::ParseNumericComparison(const char* text, Chk::Condition::Comparison & dest, size_t pos, size_t end)
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    text = &text[pos];

    if ( size < 1 || size > 8 )
        return false;

    char comparison[12] = { };

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( text[i] >= 'a' && text[i] <= 'z' )
            comparison[i-numSkipped] = text[i] - 32;
        else if ( text[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = text[i];
        else
            numSkipped ++;
    }
    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'A':
            if      ( strcmp(&comparison[1], "TLEAST") == 0 ) { dest = Chk::Condition::Comparison::AtLeast; success = true; }
            else if ( strcmp(&comparison[1], "TMOST" ) == 0 ) { dest = Chk::Condition::Comparison::AtMost; success = true; }
            break;
        case 'E':
            if ( strcmp(&comparison[1], "XACTLY") == 0 ) { dest = Chk::Condition::Comparison::Exactly; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseSwitchState(const char* text, Chk::Condition::Comparison & dest, size_t pos, size_t end)
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 7 )
        return false;

    char comparison[8] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'C':
            if ( strcmp(&comparison[1], "LEARED") == 0 ) { dest = Chk::Condition::Comparison::NotSet; success = true; }
            break;
        case 'N':
            if ( strcmp(&comparison[1], "OTSET") == 0 ) { dest = Chk::Condition::Comparison::NotSet; success = true; }
            break;
        case 'S':
            if ( strcmp(&comparison[1], "ET") == 0 ) { dest = Chk::Condition::Comparison::Set; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseSpecialUnitAmount(const char* text, u8 & dest, size_t pos, size_t end)
    // All
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 3 )
        return false;

    char comparison[4] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    if ( strcmp(comparison, "ALL") == 0 )
    {
        dest = 0;
        return true;
    }
    else
        return false;
}

bool StaticTrigComponentParser::ParseAllianceStatus(const char* text, u16 & dest, size_t pos, size_t end)
    // Ally, Enemy, Allied Victory
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 14 )
        return false;

    char comparison[16] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'A':
            if      ( strcmp(&comparison[1], "LLIEDVICTORY") == 0 ) { dest = 2; success = true; }
            else if ( strcmp(&comparison[1], "LLY"         ) == 0 ) { dest = 1; success = true; }
            break;
        case 'E':
            if ( strcmp(&comparison[1], "NEMY") == 0 ) { dest = 0; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseResourceType(const std::string & text, u8 & dest, size_t pos, size_t end)
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }
    
    if ( size < 1 || size > 11 )
        return false;

    char resource[12] = { };
    const char* srcStr = &text.c_str()[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            resource[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            resource[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    resource[size] = '\0';
    bool success = false;
    switch ( resource[0] )
    {
        case 'O':
            if      ( strcmp(&resource[1], "RE"      ) == 0 ) { dest = 0; success = true; }
            else if ( strcmp(&resource[1], "REANDGAS") == 0 ) { dest = 2; success = true; }
            break;
        case 'G':
            if ( strcmp(&resource[1], "AS") == 0 ) { dest = 1; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseScoreType(const std::string & text, u8 & dest, size_t pos, size_t end)
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }
    
    if ( size < 1 || size > 19 )
        return false;

    char score[20] = { };
    const char* srcStr = &text.c_str()[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<20; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            score[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            score[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    score[size] = '\0';
    bool success = false;
    switch ( score[0] )
    {
        case 'B':
            if ( strcmp(&score[1], "UILDINGS") == 0 ) { dest = 2; success = true; }
            break;
        case 'C':
            if ( strcmp(&score[1], "USTOM") == 0 ) { dest = 7; success = true; }
            break;
        case 'K':
            if      ( strcmp(&score[1], "ILLS"          ) == 0 ) { dest = 4; success = true; }
            else if ( strcmp(&score[1], "ILLSANDRAZINGS") == 0 ) { dest = 6; success = true; }
            break;
        case 'R':
            if ( strcmp(&score[1], "AZINGS") == 0 ) { dest = 5; success = true; }
            break;
        case 'T':
            if ( strcmp(&score[1], "OTAL") == 0 ) { dest = 0; success = true; }
            break;
        case 'U':
            if      ( strcmp(&score[1], "NITS"            ) == 0 ) { dest = 1; success = true; }
            else if ( strcmp(&score[1], "NITSANDBUILDINGS") == 0 ) { dest = 3; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseTextDisplayFlag(const char* text, u8 & dest, size_t pos, size_t end)
    // Always Display, Don't Always Display
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 20 )
        return false;

    char comparison[24] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'A':
            if ( strcmp(&comparison[1], "LWAYSDISPLAY") == 0 ) { dest |= Chk::Action::Flags::AlwaysDisplay; success = true; }
            break;
        case 'D':
            if ( strcmp(&comparison[1], "ON'TALWAYSDISPLAY") == 0 ) { dest &= Chk::Action::Flags::xAlwaysDisplay; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseNumericModifier(const char* text, u8 & dest, size_t pos, size_t end)
    // Add, subtract, set to
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 8 )
    {
        return false;
    }

    char comparison[12] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'A':
            if ( strcmp(&comparison[1], "DD") == 0 ) { dest = 8; success = true; }
            break;
        case 'S':
            if      ( strcmp(&comparison[1], "ETTO"   ) == 0 ) { dest = 7; success = true; }
            else if ( strcmp(&comparison[1], "UBTRACT") == 0 ) { dest = 9; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseSwitchMod(const char* text, u8 & dest, size_t pos, size_t end)
    // Set, clear, toggle, randomize
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 9 )
        return false;

    char comparison[10] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'C':
            if ( strcmp(&comparison[1], "LEAR") == 0 ) { dest = 5; success = true; }
            break;
        case 'R':
            if ( strcmp(&comparison[1], "ANDOMIZE") == 0 ) { dest = 11; success = true; }
            break;
        case 'S':
            if ( strcmp(&comparison[1], "ET") == 0 ) { dest = 4; success = true; }
            break;
        case 'T':
            if ( strcmp(&comparison[1], "OGGLE") == 0 ) { dest = 6; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseStateMod(const char* text, u8 & dest, size_t pos, size_t end)
    // Disable, Disabled, Enable, Enabled, Toggle
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 8 )
        return false;

    char comparison[12] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'D':
            if      ( strcmp(&comparison[1], "ISABLE" ) == 0 ) { dest = 5; success = true; }
            else if ( strcmp(&comparison[1], "ISABLED") == 0 ) { dest = 5; success = true; }
            break;
        case 'E':
            if      ( strcmp(&comparison[1], "NABLE" ) == 0 ) { dest = 4; success = true; }
            else if ( strcmp(&comparison[1], "NABLED") == 0 ) { dest = 4; success = true; }
            break;
        case 'T':
            if ( strcmp(&comparison[1], "OGGLE") == 0 ) { dest = 6; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseOrder(const char* text, u8 & dest, size_t pos, size_t end)
    // Attack, move, patrol
{
    size_t size = end-pos;
    if ( text[pos] == '\"' ) // Quoted argument, ignore the quotes
    {
        if ( size < 2 )
            return false;

        pos ++;
        end --;
        size -= 2;
    }

    if ( size < 1 || size > 6 )
        return false;

    char comparison[8] = { };
    const char* srcStr = &text[pos];

    // Take uppercase copy of argument
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        if ( srcStr[i] >= 'a' && srcStr[i] <= 'z' )
            comparison[i-numSkipped] = srcStr[i] - 32;
        else if ( srcStr[i] != ' ' ) // also ignore spacing
            comparison[i-numSkipped] = srcStr[i];
        else
            numSkipped ++;
    }

    comparison[size] = '\0';
    bool success = false;
    switch ( comparison[0] )
    {
        case 'A':
            if ( strcmp(&comparison[1], "TTACK") == 0 ) { dest = 2; success = true; }
            break;
        case 'M':
            if ( strcmp(&comparison[1], "OVE") == 0 ) { dest = 0; success = true; }
            break;
        case 'P':
            if ( strcmp(&comparison[1], "ATROL") == 0 ) { dest = 1; success = true; }
            break;
    }
    return success;
}

bool StaticTrigComponentParser::ParseMemoryAddress(const char* text, u32 & dest, size_t pos, size_t end, u32 deathTableOffset)
{
    u32 temp = 0;
    if ( ParseLong(text, temp, pos, end) )
    {
        dest = (temp/4*4-deathTableOffset)/4;
        return true;
    }
    return false;
}

bool StaticTrigComponentParser::ParseResourceType(const std::string & text, u16 & dest, size_t pos, size_t end)
{
    u8 temp = 0;
    bool success = ParseResourceType(text, temp, pos, end);
    dest = temp;
    return success;
}

bool StaticTrigComponentParser::ParseScoreType(const std::string & text, u16 & dest, size_t pos, size_t end)
{
    u8 temp = 0;
    bool success = ParseScoreType(text, temp, pos, end);
    dest = temp;
    return success;
}

bool StaticTrigComponentParser::ParseBinaryLong(const char* text, u32 & dest, size_t pos, size_t end)
{
    size_t size = end - pos;
    if ( size < 33 )
    {
        if ( size == 0 )
        {
            dest = 0;
            return true;
        }
        else
        {
            bool allZero = true;
            for ( size_t i=0; i<size; i++ )
            {
                if ( text[pos+i] != '0' )
                    allZero = false;
            }

            if ( allZero )
            {
                dest = 0;
                return true;
            }
            else
            {
                char potentialLong[36] = { };
                std::memcpy(potentialLong, &text[pos], (size_t)size);
                potentialLong[size] = '\0';
                dest = (u32)strtoll(potentialLong, nullptr, 2);
                return dest > 0;
            }
        }
    }
    return false;
}

bool StaticTrigComponentParser::ParseLong(const char* text, u32 & dest, size_t pos, size_t end)
{
    size_t size = end - pos;
    if ( size < 12 )
    {
        if ( size == 1 && text[pos] == '0' )
        {
            dest = 0;
            return true;
        }
        else if ( size > 2 && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X') )
        {
            char potentialLong[12] = {};
            std::memcpy(potentialLong, &text[pos + 2], size_t(size - 2));
            potentialLong[size - 2] = '\0';
            try {
                dest = (u32)std::stoll(potentialLong, nullptr, 16);
                return true;
            }
            catch (std::exception e) {}
        }
        else
        {
            char potentialLong[12] = { };
            std::memcpy(potentialLong, &text[pos], (size_t)size);
            potentialLong[size] = '\0';
            return ( (dest = (u32)std::atoll(potentialLong)) > 0 );
        }
    }
    return false;
}

bool StaticTrigComponentParser::ParseTriplet(const char* text, u8* dest, size_t pos, size_t end)
{
    u32 val;
    bool isNegative = false;

    if ( text[pos] == '-' ) // isNegative
    {
        pos ++;
        isNegative = true;
    }

    if ( ParseLong(text, val, pos, end) )
    {
        if ( isNegative )
            val = 0x1000000 - val;
        
        dest[2] = u8(val/0x10000);
        val /= 0x10000;
        dest[1] = u8(val/0x100);
        val /= 0x100;
        dest[0] = u8(val);
        return true;
    }
    return false;
}

bool StaticTrigComponentParser::ParseShort(const char* text, u16 & dest, size_t pos, size_t end)
{
    size_t size = end - pos;
    if ( size < 7 )
    {
        if ( size == 1 && text[pos] == '0' )
        {
            dest = 0;
            return true;
        }
        else if ( size > 2 && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X') )
        {
            char potentialShort[7] = {};
            std::memcpy(potentialShort, &text[pos + 2], size_t(size - 2));
            potentialShort[size - 2] = '\0';
            try {
                dest = (u32)std::stol(potentialShort, nullptr, 16);
                return true;
            }
            catch ( std::exception e ) { return false; }
        }
        else
        {
            char potentialShort[7] = {};
            std::memcpy(potentialShort, &text[pos], (size_t)size);
            potentialShort[size] = '\0';
            return ((dest = std::atoi(potentialShort)) > 0);
        }
    }
    return false;
}

bool StaticTrigComponentParser::ParseByte(const char* text, u8 & dest, size_t pos, size_t end)
{
    size_t size = end - pos;
    if ( size < 5 )
    {
        if ( size == 1 && text[pos] == '0' )
        {
            dest = 0;
            return true;
        }
        else if ( size > 2 && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X') )
        {
            char potentialByte[5] = {};
            std::memcpy(potentialByte, &text[pos + 2], size_t(size - 2));
            potentialByte[size - 2] = '\0';
            try {
                dest = (u32)std::stol(potentialByte, nullptr, 16);
                return true;
            }
            catch ( std::exception e ) { return false; }
        }
        else
        {
            char potentialByte[5] = {};
            std::memcpy(potentialByte, &text[pos], (size_t)size);
            potentialByte[size] = '\0';
            return ((dest = std::atoi(potentialByte)) > 0);
        }
    }
    return false;
}
