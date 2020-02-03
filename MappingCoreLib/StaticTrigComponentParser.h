#ifndef STATICTRIGCOMPONENTPARSER_H
#define STATICTRIGCOMPONENTPARSER_H
#include "Basics.h"
#include "Chk.h"

class StaticTrigComponentParser
{
    public:

        virtual ~StaticTrigComponentParser();
        bool ParseNumericComparison(const char* text, Chk::Condition::Comparison & dest, size_t pos, size_t end); // At least, at most, exactly
        bool ParseSwitchState(const char* text, Chk::Condition::Comparison & dest, size_t pos, size_t end); // Set, cleared
        bool ParseSpecialUnitAmount(const char* text, u8 & dest, size_t pos, size_t end); // All
        bool ParseAllianceStatus(const char* text, u16 & dest, size_t pos, size_t end); // Ally, Enemy, Allied Victory
        bool ParseResourceType(const std::string & text, u8 & dest, size_t pos, size_t end); // Ore, Gas, Ore and Gas
        bool ParseScoreType(const std::string & text, u8 & dest, size_t pos, size_t end); // Total, Units, Buildings, Units and Buildings, Kill, Razings, Kill and Razing, Custom
        bool ParseTextDisplayFlag(const char* text, u8 & dest, size_t pos, size_t end); // Always Display, Don't Always Display
        bool ParseNumericModifier(const char* text, u8 & dest, size_t pos, size_t end); // Add, subtract, set to
        bool ParseSwitchMod(const char* text, u8 & dest, size_t pos, size_t end); // Set, clear, toggle, randomize
        bool ParseStateMod(const char* text, u8 & dest, size_t pos, size_t end); // Disable, Enable, Toggle
        bool ParseOrder(const char* text, u8 & dest, size_t pos, size_t end); // Attack, move, patrol
        bool ParseMemoryAddress(const char* text, u32 & dest, size_t pos, size_t end, u32 deathTableOffset);

        bool ParseResourceType(const std::string & text, u16 & dest, size_t pos, size_t end); // Accelerator for 2-byte resource types
        bool ParseScoreType(const std::string & text, u16 & dest, size_t pos, size_t end); // Accelerator for 2-byte score types

        bool ParseBinaryLong(const char* text, u32 & dest, size_t pos, size_t end); // Grabs a 4-byte binary int from the given position in the 
        bool ParseLong(const char* text, u32 & dest, size_t pos, size_t end); // Grabs a 4-byte int from the given position in the text
        bool ParseTriplet(const char* text, u8* dest, size_t pos, size_t end); // Grabs a 3-byte int from the given position in the text
        bool ParseShort(const char* text, u16 & dest, size_t pos, size_t end); // Grabs a 2-byte int from the given position in the text
        bool ParseByte(const char* text, u8 & dest, size_t pos, size_t end); // Grabs a 1-byte int from the given position in the text
};

#endif