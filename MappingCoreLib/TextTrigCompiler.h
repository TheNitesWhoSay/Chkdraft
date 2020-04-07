#ifndef TEXTTRIGCOMPILER_H
#define TEXTTRIGCOMPILER_H
#include "Basics.h"
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

class TextTrigCompiler
{
    public:

        enum_t(ScenarioDataFlag, u32, {
            None = 0,
            Locations = BIT_0,
            Units = BIT_1,
            Switches = BIT_2,
            Groups = BIT_3,
            Scripts = BIT_4,
            StandardStrings = BIT_5,
            ExtendedStrings = BIT_6,
            Strings = StandardStrings | ExtendedStrings,
            All = u32_max
        });

        TextTrigCompiler(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigCompiler();
        bool CompileTriggers(std::string & trigText, ScenarioPtr chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd); // Compiles text, overwrites TRIG and STR upon success
        bool CompileTrigger(std::string & trigText, Chk::TriggerPtr trigger, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex); // Compiles text, fills trigger upon success

        // Attempts to compile the condition argument at argIndex into the given condition
        bool ParseConditionName(std::string text, Chk::Condition::Type & conditionType);
        bool ParseConditionArg(std::string conditionArgText, Chk::Condition::Argument argument, Chk::Condition & condition, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex, bool silent = false);
        bool ParseActionName(std::string text, Chk::Action::Type & actionType);
        bool ParseActionArg(std::string actionArgText, Chk::Action::Argument argument, Chk::Action & action, ScenarioPtr chk, Sc::Data & scData, size_t trigIndex, bool silent = false);


    protected:

        bool LoadCompiler(ScenarioPtr chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd, ScenarioDataFlag dataTypes = ScenarioDataFlag::All); // Sets up all the data needed for a run of the compiler
        void ClearCompiler(); // Clears data loaded for a run of the compiler
        void CleanText(std::string & text, std::vector<RawString> & stringContents); // Remove spacing and standardize line endings

        bool ParseTriggers(std::string & text, std::vector<RawString> & stringContents, std::deque<std::shared_ptr<Chk::Trigger>> & output, std::stringstream & error); // Parse trigger, generate a trig section in buffer output
        inline bool ParsePartZero(std::string & text, Chk::TriggerPtr & currTrig, Chk::Condition* & currCondition, Chk::Action* & currAction, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartOne(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting, size_t & playerEnd, size_t & lineEnd);
        inline bool ParsePartTwo(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartThree(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartFour(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            size_t & conditionEnd, size_t & lineEnd, Chk::Condition::VirtualType & conditionType, u8 & flags, u32 & argIndex, u32 & numConditions,
            Chk::Condition* & currCondition);
        inline bool ParsePartFive(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting, u32 & argIndex, size_t & argEnd,
            Chk::Condition* & currCondition, Chk::Condition::VirtualType & conditionType);
        inline bool ParsePartSix(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartSeven(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            u8 & flags, size_t & actionEnd, size_t & lineEnd, Chk::Action::VirtualType & actionType, u32 & argIndex, u32 & numActions,
            Chk::Action* & currAction);
        inline bool ParsePartEight(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            u32 & argIndex, size_t & argEnd, Chk::Action* & currAction, Chk::Action::VirtualType & actionType);
        inline bool ParsePartNine(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);
        inline bool ParsePartTen(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting,
            size_t & flagsEnd);
        inline bool ParsePartEleven(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, u32 & expecting);

        bool ParseExecutingPlayer(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & currTrig, size_t pos, size_t end); // Parse a player that the trigger is executed by
        bool ParseConditionName(const std::string & arg, Chk::Condition::VirtualType & conditionType);
        bool ParseCondition(std::string & text, size_t pos, size_t end, bool disabled, Chk::Condition::VirtualType & conditionType, u8 & flags); // Find the equivilant conditionType
        bool ParseActionName(const std::string & arg, Chk::Action::VirtualType & actionType);
        bool ParseAction(std::string & text, size_t pos, size_t end, bool disabled, Chk::Action::VirtualType & actionType, u8 & flags); // Find the equivilant actionType
        bool ParseConditionArg(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition & currCondition, size_t pos, size_t end, Chk::Condition::VirtualType conditionType, Chk::Condition::Argument argument, std::stringstream & error); // Parse an argument belonging to a condition
        bool ParseActionArg(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Action & currAction, size_t pos, size_t end, Chk::Action::VirtualType actionType, Chk::Action::Argument argument, std::stringstream & error); // Parse an argument belonging to an action
        bool ParseExecutionFlags(std::string & text, size_t pos, size_t end, u32 & flags);

        bool ParseString(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Find a given string (not an extended string) in the map, prepare to add it if necessary
        bool ParseLocationName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Find a location in the map by its string
        bool ParseUnitName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Sc::Unit::Type & dest, size_t pos, size_t end); // Get a unitID using a unit name
        bool ParseWavName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Find a wav in the map by its string, redundant? remove me?
        bool ParsePlayer(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Get a groupID using a group/player name
        bool ParseSwitch(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Find a switch in the map by name (or standard name)
        bool ParseScript(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Find a script by name

        bool ParseSwitch(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end); // Accelerator for 4-byte switches

        Chk::Condition::Type ExtendedToRegularCID(Chk::Condition::VirtualType extendedConditionType); // Returns the conditionType the extended condition is based on
        Chk::Action::Type ExtendedToRegularAID(Chk::Action::VirtualType extendedActionType); // Returns the actionType the extended action is based on

        // Static components
        bool ParseNumericComparison(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition::Comparison & dest, size_t pos, size_t end); // At least, at most, exactly
        bool ParseSwitchState(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition::Comparison & dest, size_t pos, size_t end); // Set, cleared
        bool ParseSpecialUnitAmount(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // All
        bool ParseAllianceStatus(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end); // Ally, Enemy, Allied Victory
        bool ParseResourceType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Ore, Gas, Ore and Gas
        bool ParseScoreType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Total, Units, Buildings, Units and Buildings, Kill, Razings, Kill and Razing, Custom
        bool ParseTextDisplayFlag(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Always Display, Don't Always Display
        bool ParseNumericModifier(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Add, subtract, set to
        bool ParseSwitchMod(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Set, clear, toggle, randomize
        bool ParseStateMod(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Disable, Enable, Toggle
        bool ParseOrder(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end); // Attack, move, patrol
        bool ParseMemoryAddress(const std::string & text, u32 & dest, size_t pos, size_t end, u32 deathTableOffset);

        bool ParseResourceType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end); // Accelerator for 2-byte resource types
        bool ParseScoreType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end); // Accelerator for 2-byte score types

        bool ParseBinaryLong(const std::string & text, u32 & dest, size_t pos, size_t end); // Grabs a 4-byte binary int from the given position in the 
        bool ParseLong(const std::string & text, u32 & dest, size_t pos, size_t end); // Grabs a 4-byte int from the given position in the text
        bool ParseShort(const std::string & text, u16 & dest, size_t pos, size_t end); // Grabs a 2-byte int from the given position in the text
        bool ParseByte(const std::string & text, u8 & dest, size_t pos, size_t end); // Grabs a 1-byte int from the given position in the text

        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source);
        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source, size_t pos, size_t end);
        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, size_t pos, size_t end);

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

/** Returns the position of the next instance of 'character', npos if not found
    if 'terminator' is found before 'character', then npos is returned */
size_t findNext(const std::string & str, size_t pos, char character, char terminator);

/** Returns whether str spanning from [pos, pos+count) equals the string pointed to by "other" */
bool compareCaseless(const std::string & str, size_t pos, size_t count, const char* other);



template <size_t N>
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source)
{
    const char* src = source.c_str();
    size_t size = source.size();
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        const char & currChar = src[i];
        if ( currChar >= 'a' && currChar <= 'z' )
            dest[i-numSkipped] = currChar - 32;
        else if ( currChar != ' ' )
            dest[i-numSkipped] = currChar;
        else
            numSkipped ++;
    }
    dest[size] = '\0';
}

template <size_t N>
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source, size_t pos, size_t end)
{
    const char* src = &source.c_str()[pos];
    size_t size = end-pos;
    size_t numSkipped = 0;
    for ( size_t i=0; i<size; i++ )
    {
        const char & currChar = src[i];
        if ( currChar >= 'a' && currChar <= 'z' )
            dest[i-numSkipped] = currChar - 32;
        else if ( currChar != ' ' )
            dest[i-numSkipped] = currChar;
        else
            numSkipped ++;
    }
    dest[size] = '\0';
}

template <size_t N>
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, size_t pos, size_t end)
{
    if ( text[pos] == '\"' ) // Quoted argument, take off stringContents
    {
        RawString & rawString = stringContents[nextString];
        nextString ++;
        copyUpperCaseNoSpace(dest, rawString);
    }
    else
        copyUpperCaseNoSpace(dest, text, pos, end);
}

#endif