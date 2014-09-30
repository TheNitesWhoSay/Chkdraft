#ifndef TEXTTRIG_H
#define TEXTTRIG_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include <unordered_map>
#include <algorithm>

class TextTrigWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);
		void RefreshWindow();

	protected:
		// Takes the text from the edit control given by hWnd and compiles it into the map
		bool CompileEditText(Scenario* map, HWND hWnd);

	private:
		EditControl editControl;

		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// StringTableNode defined in Structs.h
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
		bool ParseScript(char* text, u32 &dest, u32 pos, u32 end); // Find a script by name

		bool ParseResourceType(char* text, u16 &dest, u32 pos, u32 end); // Accelerator for 2-byte resource types
		bool ParseScoreType(char* text, u16 &dest, u32 pos, u32 end); // Accelerator for 2-byte score types

		bool ParseLong(char* text, u32& dest, u32 pos, u32 end); // Grabs a 4-byte int from the given position in the text
		bool ParseTriplet(char* text, u8* dest, u32 pos, u32 end); // Grabs a 3-byte int from the given position in the text
		bool ParseShort(char* text, u16& dest, u32 pos, u32 end); // Grabs a 2-byte int from the given position in the text
		bool ParseByte(char* text, u8& dest, u32 pos, u32 end); // Grabs a 1-byte int from the given position in the text
};

class TextTrigCompiler : public StaticTrigComponentParser
{
	public:

		TextTrigCompiler(Scenario* map, bool& success); // Sets up all the string-based metadata
		bool CompileText(buffer& text, Scenario* chk); // Compiles text, overwrites TRIG and STR upon success


	protected:

		void CleanText(buffer& text); // Remove spacing and standardize line endings

		bool ParseTriggers(buffer& text, buffer& output, char* error); // Parse trigger, generate a trig section in buffer output
		bool ParseExecutingPlayer(buffer &text, Trigger &currTrig, u32 pos, u32 end); // Parse a player that the trigger is executed by
		bool ParseCondition(buffer &text, u32 pos, u32 end, bool disabled, u32 &ID, u8& flags, u32 &argsLeft); // Find the equivilant conditionID
		bool ParseAction(buffer& text, u32 pos, u32 end, bool disabled, u32& ID, u8& flags, u32& argsLeft); // Find the equivilant actionID
		bool ParseConditionArg(buffer& text, Condition& currCondition, u32 pos, u32 end, u32 CID, u32 argsLeft, char* error); // Parse an argument belonging to a condition
		bool ParseActionArg(buffer& text, Action& currAction, u32 pos, u32 end, u32 AID, u32 argsLeft, char* error); // Parse an argument belonging to an action

		bool ParseString(buffer &text, u32& dest, u32 pos, u32 end); // Find a given string in the map, prepare to add it if necessary
		bool ParseLocationName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a location in the map by its string
		bool ParseUnitName(buffer &text, u16 &dest, u32 pos, u32 end); // Get a unitID using a unit name
		bool ParseWavName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a wav in the map by its string
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
		std::unordered_multimap<u32, WavTableNode> wavTable; // Binary tree of the maps wavs
		std::unordered_multimap<u32, GroupTableNode> groupTable; // Binary tree of the maps groups
		std::list<StringTableNode> addedStrings; // Forward list of strings added during compilation
		StringUsageTable strUsage; // Table of strings currently used in the map
		StringUsageTable extendedStrUsage; // Table of extended strings currently used in the map

		bool PrepLocationTable(Scenario* map); // Fills locationTable
		bool PrepUnitTable(Scenario* map); // Fills unitTable
		bool PrepSwitchTable(Scenario* map); // Fills switchTable
		bool PrepWavTable(Scenario* map); // Fills wavTable
		bool PrepGroupTable(Scenario* map); // Fills groupTable
		bool PrepStringTable(Scenario* map); // Fills stringTable

		bool strIsInTable(string str); // Helper for PrepStringTable

		bool BuildNewStringTable(Scenario* chk); // Builds a new STR section using stringTable and addedStrings

		TextTrigCompiler(); // disallow ctor
};

class TextTrigGenerator
{
	public:
		
		TextTrigGenerator(Scenario* map);
		bool GenerateTextTrigs(buffer& TRIG, buffer& output); // Places text trigs representative of the given TRIG section in output if successful

	
	protected:

		bool CorrectLineEndings(buffer& buf); // Corrects any improperly formatted line endings


	private:

		std::vector<string> stringTable; // Array list of map strings
		std::vector<string> extendedStringTable; // Array list of extended map strings
		std::vector<string> locationTable; // Array of map locations
		std::vector<string> unitTable; // Array of map units
		std::vector<string> switchTable; // Array of map switches
		std::vector<string> wavTable; // Array of map wavs
		std::vector<string> groupTable; // Array of map groups
		std::vector<string> conditionTable; // Array of condition names
		std::vector<string> actionTable; // Array of action names

		bool PrepLocationTable(Scenario* map); // Fills locationTable
		bool PrepUnitTable(Scenario* map); // Fills unitTable
		bool PrepSwitchTable(Scenario* map); // Fills switchTable
		bool PrepWavTable(Scenario* map); // Fills wavTable
		bool PrepGroupTable(Scenario* map); // Fills groupTable
		bool PrepStringTable(Scenario* map); // Fills stringTable
		bool PrepConditionTable(); // Fill conditionTable
		bool PrepActionTable(); // Fill actionTable

		TextTrigGenerator(); // disallow ctor
};

#define MAX_ERROR_MESSAGE_SIZE 256

#endif