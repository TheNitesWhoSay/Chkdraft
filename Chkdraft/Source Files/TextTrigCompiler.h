#ifndef TEXTTRIGCOMPILER_H
#define TEXTTRIGCOMPILER_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "StaticTrigComponentParser.h"
#include <unordered_map>

// StringTableNode defined in Basics.h
struct LocationTableNode {
	std::string locationName;
	u8 locationNum;
};
struct UnitTableNode {
	std::string unitName;
	u16 unitID;
};
struct SwitchTableNode {
	std::string switchName;
	u8 switchNum;
};
struct WavTableNode {
	std::string wavName;
	u32 wavID;
};
struct GroupTableNode {
	std::string groupName;
	u32 groupID;
};

class TextTrigCompiler : public StaticTrigComponentParser
{
	public:

		TextTrigCompiler();
		bool CompileTriggers(std::string trigText, Scenario* chk); // Compiles text, overwrites TRIG and STR upon success
		bool CompileTriggers(buffer& text, Scenario* chk); // Compiles text, overwrites TRIG and STR upon success
		bool CompileTrigger(std::string trigText, Trigger* trigger, Scenario* chk); // Compiles text, fills trigger upon success
		bool CompileTrigger(buffer& text, Trigger* trigger, Scenario* chk); // Compiles text, fills trigger upon success


	protected:

		bool LoadScenario(Scenario* chk); // Sets up all the string-based metadata
		void ClearScenario(); // Clears data loaded about a scenario
		void CleanText(buffer& text); // Remove spacing and standardize line endings

		bool ParseTriggers(buffer& text, buffer& output, char* error); // Parse trigger, generate a trig section in buffer output
		bool ParseExecutingPlayer(buffer &text, Trigger &currTrig, u32 pos, u32 end); // Parse a player that the trigger is executed by
		bool ParseCondition(buffer &text, u32 pos, u32 end, bool disabled, u32 &ID, u8& flags, u32 &argsLeft); // Find the equivilant conditionID
		bool ParseAction(buffer& text, u32 pos, u32 end, bool disabled, u32& ID, u8& flags, u32& argsLeft); // Find the equivilant actionID
		bool ParseConditionArg(buffer& text, Condition& currCondition, u32 pos, u32 end, u32 CID, u32 argsLeft, char* error); // Parse an argument belonging to a condition
		bool ParseActionArg(buffer& text, Action& currAction, u32 pos, u32 end, u32 AID, u32 argsLeft, char* error); // Parse an argument belonging to an action
		bool ParseExecutionFlags(buffer& text, u32 pos, u32 end, u32& flags);

		bool ParseString(buffer &text, u32& dest, u32 pos, u32 end); // Find a given string in the map, prepare to add it if necessary
		bool ParseLocationName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a location in the map by its string
		bool ParseUnitName(buffer &text, u16 &dest, u32 pos, u32 end); // Get a unitID using a unit name
		bool ParseWavName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a wav in the map by its string, redundant? remove me?
		bool ParsePlayer(buffer &text, u32 &dest, u32 pos, u32 end); // Get a groupID using a group/player name
		bool ParseSwitch(buffer &text, u8 &dest, u32 pos, u32 end); // Find a switch in the map by name (or standard name)

		bool ParseSwitch(buffer &text, u32 &dest, u32 pos, u32 end); // Accelerator for 4-byte switches

		u8 ExtendedToRegularCID(s32 extendedCID); // Returns the conditionID the extended condition is based on
		u8 ExtendedToRegularAID(s32 extendedAID); // Returns the actionID the extended action is based on

		s32 ExtendedNumConditionArgs(s32 extendedCID); // Returns the number of arguments for the extended condition
		s32 ExtendedNumActionArgs(s32 extendedAID); // Returns the number of arguments for the extended action


	private:

		std::hash<std::string> strHash; // A hasher to help generate tables
		std::unordered_multimap<u32, StringTableNode> stringTable; // Binary tree of the maps strings
		std::unordered_multimap<u32, LocationTableNode> locationTable; // Binary tree of the maps locations
		std::unordered_multimap<u32, UnitTableNode> unitTable; // Binary tree of the maps untis
		std::unordered_multimap<u32, SwitchTableNode> switchTable; // Binary tree of the maps switches
		std::unordered_multimap<u32, WavTableNode> wavTable; // Binary tree of the maps wavs, redundant? remove me?
		std::unordered_multimap<u32, GroupTableNode> groupTable; // Binary tree of the maps groups
		std::list<StringTableNode> addedStrings; // Forward list of strings added during compilation
		StringUsageTable strUsage; // Table of strings currently used in the map
		StringUsageTable extendedStrUsage; // Table of extended strings currently used in the map

		bool PrepLocationTable(Scenario* map); // Fills locationTable
		bool PrepUnitTable(Scenario* map); // Fills unitTable
		bool PrepSwitchTable(Scenario* map); // Fills switchTable
		bool PrepWavTable(Scenario* map); // Fills wavTable, redundant? remove me?
		bool PrepGroupTable(Scenario* map); // Fills groupTable
		bool PrepStringTable(Scenario* map); // Fills stringTable

		bool strIsInTable(string str); // Helper for PrepStringTable

		bool BuildNewStringTable(Scenario* chk); // Builds a new STR section using stringTable and addedStrings
};

#endif