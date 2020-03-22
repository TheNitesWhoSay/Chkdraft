#ifndef TEXTTRIGCOMPILER_H
#define TEXTTRIGCOMPILER_H
#include "Basics.h"
#include "StaticTrigComponentParser.h"
#include "Sc.h"
#include "Scenario.h"
#include <unordered_map>
#include <sstream>
#include <string>

struct LocationTableNode {
    RawString locationName;
    u8 locationId;
};
struct UnitTableNode {
    RawString unitName;
    Sc::Unit::Type unitType;
};
struct SwitchTableNode {
    RawString switchName;
    u8 switchId;
};
struct GroupTableNode {
    RawString groupName;
    u32 groupId;
};
struct ScriptTableNode {
    RawString scriptName;
    u32 scriptId;
};
struct StringTableNode {
    bool unused; // If unused, string was only used by triggers being replaced and has yet to be used by new triggers
    ScStrPtr scStr;
    u32 stringId;
    std::vector<u32*> assignees;
};
using StringTableNodePtr = std::shared_ptr<StringTableNode>;

class TextTrigCompiler : public StaticTrigComponentParser
{
    public:

        TextTrigCompiler(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigCompiler();
        bool CompileTriggers(std::string & trigText, ScenarioPtr chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd); // Compiles text, overwrites TRIG and STR upon success
        bool CompileTrigger(std::string & trigText, Chk::TriggerPtr trigger, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex); // Compiles text, fills trigger upon success

        // Attempts to compile the condition argument at argIndex into the given condition
        bool ParseConditionName(std::string text, Chk::Condition::Type & conditionType);
        bool ParseConditionArg(std::string conditionArgText, Chk::Condition::Argument argument, Chk::Condition & condition, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex);
        bool ParseActionName(std::string text, Chk::Action::Type & actionType);
        bool ParseActionArg(std::string actionArgText, Chk::Action::Argument argument, Chk::Action & action, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex);


    protected:

        bool LoadCompiler(ScenarioPtr chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd); // Sets up all the data needed for a run of the compiler
        void ClearCompiler(); // Clears data loaded for a run of the compiler
        void CleanText(std::string & text); // Remove spacing and standardize line endings

        bool ParseTriggers(std::string & text, std::deque<std::shared_ptr<Chk::Trigger>> & output, std::stringstream & error); // Parse trigger, generate a trig section in buffer output
        inline bool ParsePartZero(std::string & text, Chk::TriggerPtr & currTrig, Chk::Condition* & currCondition, Chk::Action* & currAction, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartOne(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting, size_t & playerEnd, size_t & lineEnd);
        inline bool ParsePartTwo(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartThree(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartFour(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            size_t & conditionEnd, size_t & lineEnd, Chk::Condition::VirtualType & conditionType, u8 & flags, u32 & argIndex, u32 & numConditions,
            Chk::Condition* & currCondition);
        inline bool ParsePartFive(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting, u32 & argIndex, size_t & argEnd,
            Chk::Condition* & currCondition, Chk::Condition::VirtualType & conditionType);
        inline bool ParsePartSix(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartSeven(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            u8 & flags, size_t & actionEnd, size_t & lineEnd, Chk::Action::VirtualType & actionType, u32 & argIndex, u32 & numActions,
            Chk::Action* & currAction);
        inline bool ParsePartEight(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            u32 & argIndex, size_t & argEnd, Chk::Action* & currAction, Chk::Action::VirtualType & actionType);
        inline bool ParsePartNine(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartTen(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            size_t & flagsEnd);
        inline bool ParsePartEleven(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);

        bool ParseExecutingPlayer(std::string & text, Chk::Trigger & currTrig, size_t pos, size_t end); // Parse a player that the trigger is executed by
        bool ParseConditionName(std::string & arg, Chk::Condition::VirtualType & conditionType);
        bool ParseCondition(std::string & text, size_t pos, size_t end, bool disabled, Chk::Condition::VirtualType & conditionType, u8 & flags); // Find the equivilant conditionType
        bool ParseActionName(std::string & arg, Chk::Action::VirtualType & actionType);
        bool ParseAction(std::string & text, size_t pos, size_t end, bool disabled, Chk::Action::VirtualType & actionType, u8 & flags); // Find the equivilant actionType
        bool ParseConditionArg(std::string & text, Chk::Condition & currCondition, size_t pos, size_t end, Chk::Condition::VirtualType conditionType, Chk::Condition::Argument argument, std::stringstream & error); // Parse an argument belonging to a condition
        bool ParseActionArg(std::string & text, Chk::Action & currAction, size_t pos, size_t end, Chk::Action::VirtualType actionType, Chk::Action::Argument argument, std::stringstream & error); // Parse an argument belonging to an action
        bool ParseExecutionFlags(std::string & text, size_t pos, size_t end, u32 & flags);

        bool ParseString(std::string & text, u32 & dest, size_t pos, size_t end); // Find a given string (not an extended string) in the map, prepare to add it if necessary
        bool ParseLocationName(std::string & text, u32 & dest, size_t pos, size_t end); // Find a location in the map by its string
        bool ParseUnitName(std::string & text, Sc::Unit::Type & dest, size_t pos, size_t end); // Get a unitID using a unit name
        bool ParseWavName(std::string & text, u32 & dest, size_t pos, size_t end); // Find a wav in the map by its string, redundant? remove me?
        bool ParsePlayer(std::string & text, u32 & dest, size_t pos, size_t end); // Get a groupID using a group/player name
        bool ParseSwitch(std::string & text, u8 & dest, size_t pos, size_t end); // Find a switch in the map by name (or standard name)
        bool ParseScript(std::string & text, u32 & dest, size_t pos, size_t end); // Find a script by name

        bool ParseSwitch(std::string & text, u32 & dest, size_t pos, size_t end); // Accelerator for 4-byte switches

        Chk::Condition::Type ExtendedToRegularCID(Chk::Condition::VirtualType extendedConditionType); // Returns the conditionType the extended condition is based on
        Chk::Action::Type ExtendedToRegularAID(Chk::Action::VirtualType extendedActionType); // Returns the actionType the extended action is based on


    private:

        bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        u32 deathTableOffset;
        std::hash<std::string> strHash; // A hasher to help generate tables
        std::unordered_multimap<size_t, LocationTableNode> locationTable; // Location hash map
        std::unordered_multimap<size_t, UnitTableNode> unitTable; // Unit hash map
        std::unordered_multimap<size_t, SwitchTableNode> switchTable; // Switch hash map
        std::unordered_multimap<size_t, GroupTableNode> groupTable; // Group/Player hash map
        std::unordered_multimap<size_t, ScriptTableNode> scriptTable; // Script hash map

        std::unordered_multimap<size_t, StringTableNodePtr> newStringTable; // String hash map
        std::vector<StringTableNodePtr> unassignedStrings; // Strings in stringTable that have yet to be assigned stringIds

        std::unordered_multimap<size_t, StringTableNodePtr> newExtendedStringTable; // Extended string hash map
        std::vector<StringTableNodePtr> unassignedExtendedStrings; // Extended strings in extendedStringTable that have yet to be assigned stringIds

        bool PrepLocationTable(ScenarioPtr map); // Fills locationTable
        bool PrepUnitTable(ScenarioPtr map); // Fills unitTable
        bool PrepSwitchTable(ScenarioPtr map); // Fills switchTable
        bool PrepGroupTable(ScenarioPtr map); // Fills groupTable
        bool PrepScriptTable(Sc::Data & scData); // Fills scriptTable
        bool PrepStringTable(ScenarioPtr map, std::unordered_multimap<size_t, StringTableNodePtr> & stringHashTable, size_t trigIndexBegin, size_t trigIndexEnd, const Chk::Scope & scope); // Fills stringUsed and stringTable
        void PrepTriggerString(Scenario & scenario, std::unordered_multimap<size_t, StringTableNodePtr> & stringHashTable, const u32 & stringId, const bool & inReplacedRange, const Chk::Scope & scope);
        bool PrepExtendedStringTable(ScenarioPtr map); // Fills extendedStringUsed and extendedStringTable

        bool BuildNewMap(ScenarioPtr scenario, size_t trigIndexBegin, size_t trigIndexEnd, std::deque<Chk::TriggerPtr> triggers, std::stringstream & error); // Builds the new TRIG and STR sections
};

// Returns the position of the next unescaped quote, pos must be greater than the position of the string's open quote, returns npos on failure
size_t findStringEnd(const std::string & str, size_t pos);

/** Returns the position of the next instance of 'character' outside quotes, npos if not found
    pos must be the position of a character that is not quoted,
    pos may be the position of an open quote, and may not be the position of a close quote
    'character' must not be a quote (U+0022) */
size_t findNextUnquoted(const std::string & str, size_t pos, char character);

/** Returns the position of the next instance of 'character' outside quotes, npos if not found
    pos must be the position of a character that is not quoted,
    pos may be the position of an open quote, and may not be the position of a close quote
    'character' must not be a quote (U+0022)
    if an unquoted 'terminator' is found before 'character', then npos is returned */
size_t findNextUnquoted(const std::string & str, size_t pos, char character, char terminator);

/** Returns whether str spanning from [pos, pos+count) equals the string pointed to by "other" */
bool compareCaseless(const std::string & str, size_t pos, size_t count, const char* other);

#endif