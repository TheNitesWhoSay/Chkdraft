#ifndef TEXTTRIGCOMPILER_H
#define TEXTTRIGCOMPILER_H
#include "Basics.h"
#include "StaticTrigComponentParser.h"
#include "ScData.h"
#include "Scenario.h"
#include "StringUsage.h"
#include "StringTableNode.h"
#include <unordered_map>
#include <sstream>
#include <string>

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

constexpr u32 maxErrorMessageSize = 512;

class TextTrigCompiler : public StaticTrigComponentParser
{
    public:

        TextTrigCompiler(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigCompiler();
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

        bool ParseTriggers(buffer& text, buffer& output, std::stringstream &error); // Parse trigger, generate a trig section in buffer output
        inline bool ParsePartZero(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);
        inline bool ParsePartOne(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting, s64 &playerEnd, s64 &lineEnd, Trigger &currTrig);
        inline bool ParsePartTwo(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);
        inline bool ParsePartThree(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);
        inline bool ParsePartFour(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting,
            s64 &conditionEnd, s64 &lineEnd, ConditionId &conditionId, u8 &flags, u32 &argsLeft, u32 &numConditions,
            Condition*& currCondition, Trigger &currTrig);
        inline bool ParsePartFive(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting, u32 &argsLeft, s64 &argEnd,
            Condition*& currCondition, ConditionId &conditionId);
        inline bool ParsePartSix(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);
        inline bool ParsePartSeven(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting,
            u8 &flags, s64 &actionEnd, s64 &lineEnd, ActionId &actionId, u32 &argsLeft, u32 &numActions,
            Action*& currAction, Trigger &currTrig);
        inline bool ParsePartEight(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting,
            u32 &argsLeft, s64 &argEnd, Action*& currAction, ActionId &actionId);
        inline bool ParsePartNine(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);
        inline bool ParsePartTen(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting,
            s64 &flagsEnd, Trigger& currTrig);
        inline bool ParsePartEleven(buffer& text, buffer& output, std::stringstream &error, s64 &pos, u32 &line, u32 &expecting);

        bool ParseExecutingPlayer(buffer &text, Trigger &currTrig, s64 pos, s64 end); // Parse a player that the trigger is executed by
        bool ParseConditionName(buffer &arg, ConditionId &conditionId);
        bool ParseCondition(buffer &text, s64 pos, s64 end, bool disabled, ConditionId &conditionId, u8& flags, u32 &argsLeft); // Find the equivilant conditionID
        bool ParseActionName(buffer &arg, ActionId &id);
        bool ParseAction(buffer& text, s64 pos, s64 end, bool disabled, ActionId &id, u8& flags, u32& argsLeft); // Find the equivilant actionID
        bool ParseConditionArg(buffer& text, Condition& currCondition, s64 pos, s64 end, ConditionId conditionId, u32 argsLeft, std::stringstream &error); // Parse an argument belonging to a condition
        bool ParseActionArg(buffer& text, Action& currAction, s64 pos, s64 end, ActionId actionId, u32 argsLeft, std::stringstream &error); // Parse an argument belonging to an action
        bool ParseExecutionFlags(buffer& text, s64 pos, s64 end, u32& flags);

        bool ParseString(buffer &text, u32& dest, s64 pos, s64 end); // Find a given string in the map, prepare to add it if necessary
        bool ParseLocationName(buffer &text, u32 &dest, s64 pos, s64 end); // Find a location in the map by its string
        bool ParseUnitName(buffer &text, u16 &dest, s64 pos, s64 end); // Get a unitID using a unit name
        bool ParseWavName(buffer &text, u32 &dest, s64 pos, s64 end); // Find a wav in the map by its string, redundant? remove me?
        bool ParsePlayer(buffer &text, u32 &dest, s64 pos, s64 end); // Get a groupID using a group/player name
        bool ParseSwitch(buffer &text, u8 &dest, s64 pos, s64 end); // Find a switch in the map by name (or standard name)
        bool ParseScript(buffer &text, u32 &dest, s64 pos, s64 end); // Find a script by name

        bool ParseSwitch(buffer &text, u32 &dest, s64 pos, s64 end); // Accelerator for 4-byte switches

        u8 ExtendedToRegularCID(ConditionId extendedCID); // Returns the conditionID the extended condition is based on
        u8 ExtendedToRegularAID(ActionId extendedAID); // Returns the actionID the extended action is based on

        static s32 ExtendedNumConditionArgs(ConditionId extendedCID); // Returns the number of arguments for the extended condition
        static s32 ExtendedNumActionArgs(ActionId extendedAID); // Returns the number of arguments for the extended action


    private:

        bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        u32 deathTableOffset;
        std::hash<std::string> strHash; // A hasher to help generate tables
        std::unordered_multimap<size_t, StringTableNode> stringTable; // Binary tree of the maps strings
        std::unordered_multimap<size_t, LocationTableNode> locationTable; // Binary tree of the maps locations
        std::unordered_multimap<size_t, UnitTableNode> unitTable; // Binary tree of the maps untis
        std::unordered_multimap<size_t, SwitchTableNode> switchTable; // Binary tree of the maps switches
        std::unordered_multimap<size_t, GroupTableNode> groupTable; // Binary tree of the maps groups
        std::unordered_multimap<size_t, ScriptTableNode> scriptTable; // Binary tree of map scripts
        std::vector<StringTableNode> addedStrings; // Forward list of strings added during compilation
        StringUsageTable strUsage; // Table of strings currently used in the map
        StringUsageTable extendedStrUsage; // Table of extended strings currently used in the map

        bool PrepLocationTable(ScenarioPtr map); // Fills locationTable
        bool PrepUnitTable(ScenarioPtr map); // Fills unitTable
        bool PrepSwitchTable(ScenarioPtr map); // Fills switchTable
        bool PrepGroupTable(ScenarioPtr map); // Fills groupTable
        bool PrepScriptTable(ScData &scData); // Fills scriptTable
        bool PrepStringTable(ScenarioPtr map); // Fills stringTable

        bool BuildNewStringTable(ScenarioPtr chk, std::stringstream &error); // Builds a new STR section using stringTable and addedStrings
};

#endif