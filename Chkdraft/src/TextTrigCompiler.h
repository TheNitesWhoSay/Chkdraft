#ifndef TEXTTRIGCOMPILER_H
#define TEXTTRIGCOMPILER_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "StaticTrigComponentParser.h"
#include "ScData.h"
#include <unordered_map>

// StringTableNode defined in Basics.h
struct LocationTableNode {
	RawString locationName;
	u8 locationNum;
};
struct UnitTableNode {
	RawString unitName;
	u16 unitID;
};
struct SwitchTableNode {
	RawString switchName;
	u8 switchNum;
};
struct WavTableNode {
	RawString wavName;
	u32 wavID;
};
struct GroupTableNode {
	RawString groupName;
	u32 groupID;
};
struct ScriptTableNode {
	RawString scriptName;
	u32 scriptID;
};

class TextTrigCompiler : public StaticTrigComponentParser
{
	public:

		TextTrigCompiler();
		bool CompileTriggers(std::string trigText, ScenarioPtr chk, ScData &scData); // Compiles text, overwrites TRIG and STR upon success
		bool CompileTriggers(buffer& text, ScenarioPtr chk, ScData &scData); // Compiles text, overwrites TRIG and STR upon success
		bool CompileTrigger(std::string trigText, Trigger* trigger, ScenarioPtr chk, ScData &scData); // Compiles text, fills trigger upon success
		bool CompileTrigger(buffer& text, Trigger* trigger, ScenarioPtr chk, ScData &scData); // Compiles text, fills trigger upon success

		// Attempts to compile the argNum'th condition argument into the given condition
		bool ParseConditionName(std::string text, ConditionId &conditionId);
		bool ParseConditionArg(std::string conditionArgText, u8 argNum, std::vector<u8> &argMap, Condition& condition, ScenarioPtr chk, ScData &scData);
		bool ParseActionName(std::string text, ActionId &id);
		bool ParseActionArg(std::string actionArgText, u8 argNum, std::vector<u8> &argMap, Action &action, ScenarioPtr chk, ScData &scData);
		static u8 defaultConditionFlags(ConditionId CID);
		static u8 defaultActionFlags(ActionId AID);
		static u8 numConditionArgs(ConditionId CID);
		static u8 numActionArgs(ActionId actionId);


	protected:

		bool LoadCompiler(ScenarioPtr chk, ScData &scData); // Sets up all the data needed for a run of the compiler
		void ClearCompiler(); // Clears data loaded for a run of the compiler
		void CleanText(buffer& text); // Remove spacing and standardize line endings

		bool ParseTriggers(buffer& text, buffer& output, char* error); // Parse trigger, generate a trig section in buffer output
		inline bool ParsePartZero(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);
		inline bool ParsePartOne(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting, u32 &playerEnd, u32 &lineEnd, Trigger &currTrig);
		inline bool ParsePartTwo(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);
		inline bool ParsePartThree(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);
		inline bool ParsePartFour(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
			u32 &conditionEnd, u32 &lineEnd, ConditionId &conditionId, u8 &flags, u32 &argsLeft, u32 &numConditions,
			Condition*& currCondition, Trigger &currTrig);
		inline bool ParsePartFive(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting, u32 &argsLeft, u32 &argEnd,
			Condition*& currCondition, ConditionId &conditionId);
		inline bool ParsePartSix(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);
		inline bool ParsePartSeven(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
			u8 &flags, u32 &actionEnd, u32 &lineEnd, ActionId &actionId, u32 &argsLeft, u32 &numActions,
			Action*& currAction, Trigger &currTrig);
		inline bool ParsePartEight(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting,
			u32 &argsLeft, u32 &argEnd, Action*& currAction, ActionId &actionId);
		inline bool ParsePartNine(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);
		inline bool ParsePartTen(buffer& text, buffer& output, char*error, u32 &pos, u32 &line, u32 &expecting,
			u32 &flagsEnd, Trigger& currTrig);
		inline bool ParsePartEleven(buffer& text, buffer& output, char* error, u32 &pos, u32 &line, u32 &expecting);

		bool ParseExecutingPlayer(buffer &text, Trigger &currTrig, u32 pos, u32 end); // Parse a player that the trigger is executed by
		bool ParseConditionName(buffer &arg, ConditionId &conditionId);
		bool ParseCondition(buffer &text, u32 pos, u32 end, bool disabled, ConditionId &conditionId, u8& flags, u32 &argsLeft); // Find the equivilant conditionID
		bool ParseActionName(buffer &arg, ActionId &id);
		bool ParseAction(buffer& text, u32 pos, u32 end, bool disabled, ActionId &id, u8& flags, u32& argsLeft); // Find the equivilant actionID
		bool ParseConditionArg(buffer& text, Condition& currCondition, u32 pos, u32 end, ConditionId conditionId, u32 argsLeft, char* error); // Parse an argument belonging to a condition
		bool ParseActionArg(buffer& text, Action& currAction, u32 pos, u32 end, ActionId actionId, u32 argsLeft, char* error); // Parse an argument belonging to an action
		bool ParseExecutionFlags(buffer& text, u32 pos, u32 end, u32& flags);

		bool ParseString(buffer &text, u32& dest, u32 pos, u32 end); // Find a given string in the map, prepare to add it if necessary
		bool ParseLocationName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a location in the map by its string
		bool ParseUnitName(buffer &text, u16 &dest, u32 pos, u32 end); // Get a unitID using a unit name
		bool ParseWavName(buffer &text, u32 &dest, u32 pos, u32 end); // Find a wav in the map by its string, redundant? remove me?
		bool ParsePlayer(buffer &text, u32 &dest, u32 pos, u32 end); // Get a groupID using a group/player name
		bool ParseSwitch(buffer &text, u8 &dest, u32 pos, u32 end); // Find a switch in the map by name (or standard name)
		bool ParseScript(buffer &text, u32 &dest, u32 pos, u32 end); // Find a script by name

		bool ParseSwitch(buffer &text, u32 &dest, u32 pos, u32 end); // Accelerator for 4-byte switches

		u8 ExtendedToRegularCID(ConditionId extendedCID); // Returns the conditionID the extended condition is based on
		u8 ExtendedToRegularAID(ActionId extendedAID); // Returns the actionID the extended action is based on

		static s32 ExtendedNumConditionArgs(ConditionId extendedCID); // Returns the number of arguments for the extended condition
		static s32 ExtendedNumActionArgs(ActionId extendedAID); // Returns the number of arguments for the extended action


	private:

		std::hash<std::string> strHash; // A hasher to help generate tables
		std::unordered_multimap<u32, StringTableNode> stringTable; // Binary tree of the maps strings
		std::unordered_multimap<u32, LocationTableNode> locationTable; // Binary tree of the maps locations
		std::unordered_multimap<u32, UnitTableNode> unitTable; // Binary tree of the maps untis
		std::unordered_multimap<u32, SwitchTableNode> switchTable; // Binary tree of the maps switches
		std::unordered_multimap<u32, GroupTableNode> groupTable; // Binary tree of the maps groups
		std::unordered_multimap<u32, ScriptTableNode> scriptTable; // Binary tree of map scripts
		std::vector<StringTableNode> addedStrings; // Forward list of strings added during compilation
		StringUsageTable strUsage; // Table of strings currently used in the map
		StringUsageTable extendedStrUsage; // Table of extended strings currently used in the map

		bool PrepLocationTable(ScenarioPtr map); // Fills locationTable
		bool PrepUnitTable(ScenarioPtr map); // Fills unitTable
		bool PrepSwitchTable(ScenarioPtr map); // Fills switchTable
		bool PrepGroupTable(ScenarioPtr map); // Fills groupTable
		bool PrepScriptTable(ScData &scData); // Fills scriptTable
		bool PrepStringTable(ScenarioPtr map); // Fills stringTable

		bool BuildNewStringTable(ScenarioPtr chk); // Builds a new STR section using stringTable and addedStrings
};

#endif