#ifndef STATICTRIGCOMPONENTPARSER_H
#define STATICTRIGCOMPONENTPARSER_H
#include "Common Files/CommonFiles.h"

class StaticTrigComponentParser
{
	public:

		bool ParseNumericComparison(char* text, u8 &dest, u32 pos, u32 end); // At least, at most, exactly
		bool ParseSwitchState(char* text, u8 &dest, u32 pos, u32 end); // Set, cleared
		bool ParseSpecialUnitAmount(char* text, u8 &dest, u32 pos, u32 end); // All
		bool ParseAllianceStatus(char* text, u16 &dest, u32 pos, u32 end); // Ally, Enemy, Allied Victory
		bool ParseResourceType(char* text, u8 &dest, u32 pos, u32 end); // Ore, Gas, Ore and Gas
		bool ParseScoreType(char* text, u8 &dest, u32 pos, u32 end); // Total, Units, Buildings, Units and Buildings, Kill, Razings, Kill and Razing, Custom
		bool ParseTextDisplayFlag(char* text, u8 &dest, u32 pos, u32 end); // Always Display, Don't Always Display
		bool ParseNumericModifier(char* text, u8 &dest, u32 pos, u32 end); // Add, subtract, set to
		bool ParseSwitchMod(char* text, u8 &dest, u32 pos, u32 end); // Set, clear, toggle, randomize
		bool ParseStateMod(char* text, u8 &dest, u32 pos, u32 end); // Disable, Enable, Toggle
		bool ParseOrder(char* text, u8 &dest, u32 pos, u32 end); // Attack, move, patrol

		bool ParseResourceType(char* text, u16 &dest, u32 pos, u32 end); // Accelerator for 2-byte resource types
		bool ParseScoreType(char* text, u16 &dest, u32 pos, u32 end); // Accelerator for 2-byte score types

		bool ParseBinaryLong(char* text, u32& dest, u32 pos, u32 end); // Grabs a 4-byte binary int from the given position in the text
		bool ParseLong(char* text, u32& dest, u32 pos, u32 end); // Grabs a 4-byte int from the given position in the text
		bool ParseTriplet(char* text, u8* dest, u32 pos, u32 end); // Grabs a 3-byte int from the given position in the text
		bool ParseShort(char* text, u16& dest, u32 pos, u32 end); // Grabs a 2-byte int from the given position in the text
		bool ParseByte(char* text, u8& dest, u32 pos, u32 end); // Grabs a 1-byte int from the given position in the text
};

#endif