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
struct StringAssignee {
    size_t trigIndex;
    size_t actionIndex;
    bool isSound;
};
struct StringTableNode {
    bool unused; // If unused, string was only used by triggers being replaced and has yet to be used by new triggers
    ScStr scStr;
    u32 stringId;
    std::vector<StringAssignee> assignees;
};

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
        enum_t(Expecting, u32, {
            Trigger_EndOfText = 0,
            Player_EndOfPlayers = 1,
            OpenTriggerBody = 2,
            SectionStart_EndOfTrigger = 3,
            ConditionStart_NonConditionSectionStart = 4,
            ConditionArg_EndOfCondition = 5,
            NonConditionSectionStart_EndOfTrigger = 6,
            ActionStart_FlagSectionStart_EndOfTrigger = 7,
            ActionArg_EndOfAction = 8,
            FlagSectionStart_EndOfTrigger = 9,
            ExecutionFlags = 10,
            EndOfTrigger = 11,
            Last = 12
        });

        TextTrigCompiler(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigCompiler();
        bool compileTriggers(std::string & trigText, Scenario & chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd); // Compiles text, overwrites TRIG and STR upon success
        bool compileTrigger(std::string & trigText, Scenario & chk, Sc::Data & scData, size_t trigIndex); // Compiles text, fills trigger upon success

        // Attempts to compile the condition argument at argIndex into the given condition
        bool parseConditionName(std::string text, Chk::Condition::Type & conditionType) const;
        bool parseConditionArg(std::string conditionArgText, Chk::Condition::Argument argument, Chk::Condition & condition, const Scenario & chk, Sc::Data & scData, size_t trigIndex, bool silent = false);
        bool parseActionName(std::string text, Chk::Action::Type & actionType) const;
        bool parseActionArg(std::string actionArgText, Chk::Action::Argument argument, Chk::Action & action, const Scenario & chk, Sc::Data & scData, size_t trigIndex, size_t actionIndex, bool silent = false);


    protected:

        bool loadCompiler(const Scenario & chk, Sc::Data & scData, size_t trigIndexBegin, size_t trigIndexEnd, ScenarioDataFlag dataTypes = ScenarioDataFlag::All); // Sets up all the data needed for a run of the compiler
        void clearCompiler(); // Clears data loaded for a run of the compiler
        bool cleanText(std::string & text, std::vector<RawString> & stringContents, std::stringstream & error) const; // Remove spacing and standardize line endings

        bool parseTriggers(std::string & text, std::vector<RawString> & stringContents, std::vector<Chk::Trigger> & output, std::stringstream & error); // Parse trigger, generate a trig section in buffer output
        inline bool parsePartZero(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);
        inline bool parsePartOne(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting, size_t & playerEnd, size_t & lineEnd);
        inline bool parsePartTwo(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);
        inline bool parsePartThree(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);
        inline bool parsePartFour(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting,
            size_t & conditionEnd, size_t & lineEnd, Chk::Condition::VirtualType & conditionType, u8 & flags, u32 & argIndex, u32 & numConditions,
            Chk::Condition* & currCondition);
        inline bool parsePartFive(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting, u32 & argIndex, size_t & argEnd,
            Chk::Condition* & currCondition, Chk::Condition::VirtualType & conditionType);
        inline bool parsePartSix(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);
        inline bool parsePartSeven(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting,
            u8 & flags, size_t & actionEnd, size_t & lineEnd, Chk::Action::VirtualType & actionType, u32 & argIndex, u32 & numActions,
            Chk::Action* & currAction);
        inline bool parsePartEight(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting,
            u32 & argIndex, size_t & argEnd, Chk::Action* & currAction, Chk::Action::VirtualType & actionType, size_t trigIndex, size_t actionIndex);
        inline bool parsePartNine(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);
        inline bool parsePartTen(std::string & text, Chk::Trigger & output, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting,
            size_t & flagsEnd);
        inline bool parsePartEleven(std::string & text, std::stringstream & error, size_t & pos, u32 & line, Expecting & expecting);

        bool parseExecutingPlayer(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Trigger & currTrig, size_t pos, size_t end) const; // Parse a player that the trigger is executed by
        bool parseConditionName(const std::string & arg, Chk::Condition::VirtualType & conditionType) const;
        bool parseCondition(std::string & text, size_t pos, size_t end, Chk::Condition::VirtualType & conditionType, u8 & flags); // Find the equivilant conditionType
        bool parseActionName(const std::string & arg, Chk::Action::VirtualType & actionType) const;
        bool parseAction(std::string & text, size_t pos, size_t end, Chk::Action::VirtualType & actionType, u8 & flags); // Find the equivilant actionType
        bool parseConditionArg(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition & currCondition, size_t pos, size_t end, Chk::Condition::Argument argument, std::stringstream & error); // Parse an argument belonging to a condition
        bool parseActionArg(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Action & currAction, size_t pos, size_t end, Chk::Action::Argument argument, std::stringstream & error, size_t trigIndex, size_t actionIndex); // Parse an argument belonging to an action
        bool parseExecutionFlags(std::string & text, size_t pos, size_t end, u32 & flags) const;

        bool parseString(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end, size_t trigIndex, size_t actionIndex, bool isSound); // Find a given string (not an extended string) in the map, prepare to add it if necessary
        bool parseLocationName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end) const; // Find a location in the map by its string
        bool parseUnitName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Sc::Unit::Type & dest, size_t pos, size_t end) const; // Get a unitID using a unit name
        bool parseSoundName(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end, size_t trigIndex, size_t actionIndex); // Find a sound in the map by its string, redundant? remove me?
        bool parsePlayer(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end) const; // Get a groupID using a group/player name
        bool parseSwitch(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Find a switch in the map by name (or standard name)
        bool parseScript(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end) const; // Find a script by name

        bool parseSwitch(std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u32 & dest, size_t pos, size_t end) const; // Accelerator for 4-byte switches

        Chk::Condition::Type extendedToRegularConditionType(Chk::Condition::VirtualType extendedConditionType) const; // Returns the conditionType the extended condition is based on
        Chk::Action::Type extendedToRegularActionType(Chk::Action::VirtualType extendedActionType) const; // Returns the actionType the extended action is based on

        // Static components
        bool parseNumericComparison(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition::Comparison & dest, size_t pos, size_t end) const; // At least, at most, exactly
        bool parseSwitchState(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, Chk::Condition::Comparison & dest, size_t pos, size_t end) const; // Set, cleared
        bool parseSpecialUnitAmount(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // All
        bool parseAllianceStatus(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end) const; // Ally, Enemy, Allied Victory
        bool parseResourceType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Ore, Gas, Ore and Gas
        bool parseScoreType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Total, Units, Buildings, Units and Buildings, Kill, Razings, Kill and Razing, Custom
        bool parseTextDisplayFlag(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Always Display, Don't Always Display
        bool parseNumericModifier(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Add, subtract, set to
        bool parseSwitchMod(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Set, clear, toggle, randomize
        bool parseStateMod(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Disable, Enable, Toggle
        bool parseOrder(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u8 & dest, size_t pos, size_t end) const; // Attack, move, patrol
        bool parseMemoryAddress(const std::string & text, u32 & dest, size_t pos, size_t end, u32 deathTableOffset) const;

        bool parseResourceType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end) const; // Accelerator for 2-byte resource types
        bool parseScoreType(const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, u16 & dest, size_t pos, size_t end) const; // Accelerator for 2-byte score types

        bool parseBinaryLong(const std::string & text, u32 & dest, size_t pos, size_t end) const; // Grabs a 4-byte binary int from the given position in the 
        bool parseLong(const std::string & text, u32 & dest, size_t pos, size_t end) const; // Grabs a 4-byte int from the given position in the text
        bool parseShort(const std::string & text, u16 & dest, size_t pos, size_t end) const; // Grabs a 2-byte int from the given position in the text
        bool parseByte(const std::string & text, u8 & dest, size_t pos, size_t end) const; // Grabs a 1-byte int from the given position in the text

        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source) const;
        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source, size_t pos, size_t end) const;
        template <size_t N> inline void copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, size_t pos, size_t end) const;

    private:

        bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        u32 deathTableOffset;
        std::hash<std::string> strHash; // A hasher to help generate tables
        std::unordered_multimap<size_t, LocationTableNode> locationTable; // Location hash map
        std::unordered_multimap<size_t, UnitTableNode> unitTable; // Unit hash map
        std::unordered_multimap<size_t, SwitchTableNode> switchTable; // Switch hash map
        std::unordered_multimap<size_t, GroupTableNode> groupTable; // Group/Player hash map
        std::unordered_multimap<size_t, ScriptTableNode> scriptTable; // Script hash map

        std::unordered_multimap<size_t, std::unique_ptr<StringTableNode>> newStringTable; // String hash map
        std::vector<StringTableNode*> unassignedStrings; // Strings in stringTable that have yet to be assigned stringIds

        std::unordered_multimap<size_t, std::unique_ptr<StringTableNode>> newExtendedStringTable; // Extended string hash map
        std::vector<StringTableNode*> unassignedExtendedStrings; // Extended strings in extendedStringTable that have yet to be assigned stringIds

        bool prepLocationTable(const Scenario & map); // Fills locationTable
        bool prepUnitTable(const Scenario & map); // Fills unitTable
        bool prepSwitchTable(const Scenario & map); // Fills switchTable
        bool prepGroupTable(const Scenario & map); // Fills groupTable
        bool prepScriptTable(Sc::Data & scData); // Fills scriptTable
        bool prepStringTable(const Scenario & map, std::unordered_multimap<size_t, std::unique_ptr<StringTableNode>> & stringHashTable, size_t trigIndexBegin, size_t trigIndexEnd, const Chk::StrScope & scope); // Fills stringUsed and stringTable
        void prepTriggerString(const Scenario & scenario, std::unordered_multimap<size_t, std::unique_ptr<StringTableNode>> & stringHashTable, const u32 & stringId, const bool & inReplacedRange, const Chk::StrScope & scope);

        bool buildNewMap(Scenario & scenario, size_t trigIndexBegin, size_t trigIndexEnd, std::vector<Chk::Trigger> & triggers, std::stringstream & error) const; // Builds the new TRIG and STR sections
};

// Returns the position of the next unescaped quote, pos must be greater than the position of the string's open quote, returns npos on failure
size_t findStringEnd(const std::string & str, size_t pos);

/** Returns the position of the next instance of 'character', npos if not found
    if 'terminator' is found before 'character', then npos is returned */
size_t findNext(const std::string & str, size_t pos, char character, char terminator);



template <size_t N>
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source) const
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
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & source, size_t pos, size_t end) const
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
inline void TextTrigCompiler::copyUpperCaseNoSpace(std::array<char, N> & dest, const std::string & text, std::vector<RawString> & stringContents, size_t & nextString, size_t pos, size_t end) const
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