#include "TextTrigGenerator.h"
#include "Math.h"
#include <string>

std::vector<std::string> textFlags = { "Don't Always Display", "Always Display" };
std::vector<std::string> scoreTypes = { "Total", "Units", "Buildings", "Units and buildings", "Kills", "Razings", "Kills and razings", "Custom" };
std::vector<std::string> resourceTypes = { "ore", "gas", "ore and gas" };
std::vector<std::string> orderTypes = { "move", "patrol", "attack" };
std::vector<std::string> stateModifiers = { "0", "1", "2", "3", "enabled", "disabled", "toggle" };
std::vector<std::string> switchStates = { "0", "1", "set", "not set" };
std::vector<std::string> switchModifiers = { "0", "1", "2", "3", "set", "clear", "toggle", "7", "8", "9", "10", "randomize" };
std::vector<std::string> allyStates = { "Enemy", "Ally", "Allied Victory" };
std::vector<std::string> numericComparisons = { "At least", "At most", "2", "3", "4", "5", "6", "7", "8", "9", "Exactly" };
std::vector<std::string> numericModifiers = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };

void CollapsableDefines()
{
#define ADD_TEXTTRIG_LOCATION(src) {                     \
        if ( src >= 0 && src < locationTable.size() )    \
            output += (std::string &)locationTable[src]; \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_STRING(src) {                                                                       \
        if ( src == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )                       \
            output += "No String";                                                                       \
        else if ( src >= 0 && (src < stringTable.size() || (65536-src) < extendedStringTable.size() ) )  \
        {                                                                                                \
            if ( src < stringTable.size() )                                                              \
                output += (std::string &)stringTable[src];                                               \
            else                                                                                         \
                output += std::string("k" + extendedStringTable[65536-src]);                             \
        }                                                                                                \
        else { output += std::to_string(src); } }
    
#define ADD_TEXTTRIG_SOUND(src) {                                                                        \
        if ( src == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )                       \
            output += "No WAV";                                                                          \
        else if ( src >= 0 && (src < stringTable.size() || (65536 - src) < extendedStringTable.size()) ) \
        {                                                                                                \
            if ( src < stringTable.size() )                                                              \
                output += (std::string &)stringTable[src];                                               \
            else                                                                                         \
                output += std::string("k" + extendedStringTable[65536 - src]);                           \
        }                                                                                                \
        else { output += std::to_string(src); } }


#define ADD_TEXTTRIG_PLAYER(src) {                                                  \
        if ( src >= 0 && src < groupTable.size() )                                  \
            output += (std::string &)groupTable[src];                               \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_UNIT(src) {                                                    \
        if ( (u16)src >= 0 && (u16)src < unitTable.size() )                         \
            output += (std::string &)unitTable[(u16)src];                           \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_SWITCH(src) {                                                  \
        if ( src >= 0 && src < switchTable.size() )                                 \
            output += (std::string &)switchTable[src];                              \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_SCORE_TYPE(src) {                                              \
        if ( src >= 0 && src < sizeof(scoreTypes)/sizeof(const char*) )             \
            output += scoreTypes[src];                                              \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_RESOURCE_TYPE(src) {                                           \
        if ( src >= 0 && src < sizeof(resourceTypes)/sizeof(const char*) )          \
            output += resourceTypes[src];                                           \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_ORDER(src) {                                                   \
        if ( src >= 0 && src < sizeof(orderTypes)/sizeof(const char*) )             \
            output += orderTypes[src];                                              \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_STATE_MODIFIER(src) {                                          \
        if ( src >= 0 && src < sizeof(stateModifiers)/sizeof(const char*) )         \
            output += stateModifiers[src];                                          \
        else { output += std::to_string(src); } } 

#define ADD_TEXTTRIG_SWITCH_STATE(src) {                                            \
        if ( (u8)src >= 0 && (u8)src < sizeof(switchStates)/sizeof(const char*) )   \
            output += switchStates[(u8)src];                                        \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_SWITCH_MODIFIER(src) {                                         \
        if ( src >= 0 && src < sizeof(switchModifiers)/sizeof(const char*) )        \
            output += switchModifiers[src];                                         \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_ALLY_STATE(src) {                                              \
        if ( src >= 0 && src < sizeof(allyStates)/sizeof(const char*) )             \
            output += allyStates[src];                                              \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_NUMERIC_COMPARISON(src) {                                                  \
        if ( (u8)src >= (u8)0 && (u8)src < (u8)sizeof(numericComparisons)/sizeof(const char*) ) \
            output += numericComparisons[(u8)src];                                              \
        else { output += std::to_string(src); } }

#define ADD_TEXTTRIG_NUMERIC_MODIFIER(src) {                                        \
        if ( src >= 0 && src < sizeof(numericModifiers)/sizeof(const char*) )       \
            output += numericModifiers[src];                                        \
        else { output += std::to_string(src); } }

    #define ADD_TEXTTRIG_SCRIPT(src) {                                              \
        if ( src == 0 )                                                             \
            output += "No Script";                                                  \
        else                                                                        \
        {                                                                           \
            auto it = scriptTable.find(src);                                        \
            if ( it != scriptTable.end() )                                          \
                output += it->second;                                               \
            else { output += '\"'; output += std::string(((char*)&src)[0], ((char*)&src)[4]); output += '\"'; } \
        } }

    #define ADD_TEXTTRIG_NUM_UNITS(src) {                                           \
        if ( src == 0 ) output += "All";                                            \
        else { output += std::to_string(src); } }

    #define ADD_TEXTTRIG_NUMBER(src) {                                              \
        output += std::to_string(src); }

    #define ADD_TEXTTRIG_CND_MASK_FLAG(src) {                                       \
        if ( src == Chk::Condition::MaskFlag::Enabled ) output += "Enabled";        \
        else if ( src == Chk::Condition::MaskFlag::Disabled ) output += "Disabled"; \
        else { output += std::to_string(src); } }

    #define ADD_TEXTTRIG_MASK_FLAG(src) {                                           \
        if ( src == Chk::Action::MaskFlag::Enabled ) output += "Enabled";           \
        else if ( src == Chk::Action::MaskFlag::Disabled ) output += "Disabled";    \
        else { output += std::to_string(src); } }

    #define ADD_TEXTTRIG_MEMORY(src) {                                              \
        if ( useAddressesForMemory )                                                \
            output += to_hex_string(src*4+deathTableOffset);                        \
        else                                                                        \
            output += std::to_string(src); }

    #define ADD_TEXTTRIG_TEXT_FLAGS(src) {                                                       \
        if      ( (src&Chk::Action::Flags::AlwaysDisplay) == 0 )                                 \
            output += textFlags[0];                                                              \
        else if ( (src&Chk::Action::Flags::AlwaysDisplay) == Chk::Action::Flags::AlwaysDisplay ) \
            output += textFlags[1]; }
}

TextTrigGenerator::TextTrigGenerator(bool useAddressesForMemory, u32 deathTableOffset) : goodConditionTable(false), goodActionTable(false), useAddressesForMemory(useAddressesForMemory), deathTableOffset(deathTableOffset)
{
    stringTable.clear();
    extendedStringTable.clear();
    locationTable.clear();
    unitTable.clear();
    switchTable.clear();
    groupTable.clear();
    conditionTable.clear();
    actionTable.clear();
}

TextTrigGenerator::~TextTrigGenerator()
{

}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, std::string & trigString)
{
    return this != nullptr && map != nullptr && BuildTextTrigs(map, map->triggers.trig, trigString);
}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, u32 trigId, std::string & trigString)
{
    if ( this != nullptr && map != nullptr )
    {
        Chk::TriggerPtr trig = map->triggers.getTrigger(trigId);
        if ( trig != nullptr )
        {
            TrigSectionPtr trigSection(new TrigSection());
            trigSection->addTrigger(trig);
            return BuildTextTrigs(map, trigSection, trigString);
        }
    }
    return false;
}

bool TextTrigGenerator::LoadScenario(ScenarioPtr map)
{
    return this != nullptr &&
           map != nullptr &&
           LoadScenario(map, false, true);
}

void TextTrigGenerator::ClearScenario()
{
    stringTable.clear();
    extendedStringTable.clear();
    locationTable.clear();
    unitTable.clear();
    switchTable.clear();
    groupTable.clear();
}

std::string TextTrigGenerator::GetConditionName(u8 CID)
{
    if ( CID < conditionTable.size() )
        return conditionTable[CID];
    else
        return std::to_string((int)CID);
}

std::string TextTrigGenerator::GetConditionArgument(Chk::Condition & condition, u8 stdTextTrigArgNum)
{
    StringBuffer output;
    AddConditionArgument(output, condition, (Chk::Condition::VirtualType)condition.conditionType, Chk::Condition::getTextArg(condition.conditionType, stdTextTrigArgNum));
    return output.str();
}

std::string TextTrigGenerator::GetConditionArgument(Chk::Condition & condition, Chk::Condition::Argument argument)
{
    StringBuffer output;
    AddConditionArgument(output, condition, (Chk::Condition::VirtualType)condition.conditionType, argument);
    return output.str();
}

std::string TextTrigGenerator::GetActionName(u8 AID)
{
    if ( AID < actionTable.size() )
        return actionTable[AID];
    else
        return std::to_string((int)AID);
}

std::string TextTrigGenerator::GetActionArgument(Chk::Action & action, u8 stdTextTrigArgNum)
{
    StringBuffer output;
    AddActionArgument(output, action, (Chk::Action::VirtualType)action.actionType, Chk::Action::getTextArg(action.actionType, stdTextTrigArgNum));
    return output.str();
}

std::string TextTrigGenerator::GetActionArgument(Chk::Action & action, Chk::Action::Argument argument)
{
    StringBuffer output;
    AddActionArgument(output, action, (Chk::Action::VirtualType)action.actionType, argument);
    return output.str();
}

ChkdString TextTrigGenerator::GetTrigLocation(u32 locationNum)
{
    if ( locationNum >= 0 && locationNum < locationTable.size() )
        return SingleLineChkdString(locationTable[locationNum]);
    else
        return ChkdString(std::to_string(locationNum));
}

ChkdString TextTrigGenerator::GetTrigString(u32 stringNum)
{
    if ( stringNum == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )
        return SingleLineChkdString("No String");
    else if ( stringNum >= 0 && (stringNum < stringTable.size() || (65536 - stringNum) < extendedStringTable.size()) )
    {
        if ( stringNum < stringTable.size() )
            return (std::string &)stringTable[stringNum];
        else
            return SingleLineChkdString("k" + extendedStringTable[65536 - stringNum]);
    }
    else
        return ChkdString(std::to_string(stringNum));
}

ChkdString TextTrigGenerator::GetTrigWav(u32 stringNum)
{
    if ( stringNum == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )
        return ChkdString("No WAV");
    else
        return GetTrigString(stringNum);
}

ChkdString TextTrigGenerator::GetTrigPlayer(u32 groupNum)
{
    if ( groupNum >= 0 && groupNum < groupTable.size() )
        return groupTable[groupNum].c_str();
    else
        return ChkdString(std::to_string(groupNum));
}

ChkdString TextTrigGenerator::GetTrigUnit(u16 unitId)
{
    if ( unitId >= 0 && unitId < unitTable.size() )
        return unitTable[unitId];
    else
        return ChkdString(std::to_string(unitId));
}

ChkdString TextTrigGenerator::GetTrigSwitch(u32 switchNum)
{
    if ( switchNum >= 0 && switchNum < switchTable.size() )
        return switchTable[switchNum];
    else
        return ChkdString(std::to_string(switchNum));
}

std::string TextTrigGenerator::GetTrigScoreType(u16 scoreType)
{
    const char* scoreTypes[] = { "total", "units", "buildings", "units and buildings", "kills", "razings", "kills and razings", "custom" };
    if ( scoreType >= 0 && scoreType < sizeof(scoreTypes) / sizeof(const char*) )
        return scoreTypes[scoreType];
    else
        return std::to_string(scoreType);
}

std::string TextTrigGenerator::GetTrigResourceType(u16 resourceType)
{
    const char* resourceTypes[] = { "ore", "gas", "ore and gas" };
    if ( resourceType >= 0 && resourceType < sizeof(resourceTypes) / sizeof(const char*) )
        return resourceTypes[resourceType];
    else
        return std::to_string(resourceType);
}

std::string TextTrigGenerator::GetTrigOrder(u8 order)
{
    const char* orderTypes[] = { "move", "patrol", "attack" };
    if ( order >= 0 && order < sizeof(orderTypes) / sizeof(const char*) )
        return orderTypes[order];
    else
        return std::to_string(order);
}

std::string TextTrigGenerator::GetTrigStateModifier(u8 stateModifier)
{
    const char* stateModifiers[] = { "0", "1", "2", "3", "Enable", "Disable", "Toggle" };
    if ( stateModifier >= 0 && stateModifier < sizeof(stateModifiers) / sizeof(const char*) )
        return std::string(stateModifiers[stateModifier]);
    else
        return std::to_string(stateModifier);
}

std::string TextTrigGenerator::GetTrigSwitchState(u8 switchState)
{
    const char* switchStates[] = { "0", "1", "Set", "Cleared" };
    if ( switchState >= 0 && switchState < sizeof(switchStates) / sizeof(const char*) )
        return std::string(switchStates[switchState]);
    else
        return std::to_string(switchState);
}

std::string TextTrigGenerator::GetTrigSwitchModifier(u8 switchModifier)
{
    const char* switchModifiers[] = { "0", "1", "2", "3", "Set", "Clear", "Toggle", "7", "8", "9", "10", "Randomize" };
    if ( switchModifier >= 0 && switchModifier < sizeof(switchModifiers) / sizeof(const char*) )
        return std::string(switchModifiers[switchModifier]);
    else
        return std::to_string(switchModifier);
}

std::string TextTrigGenerator::GetTrigAllyState(u16 allyState)
{
    const char* allyStates[] = { "Enemy", "Ally", "Allied Victory" };
    if ( allyState >= 0 && allyState < sizeof(allyStates) / sizeof(const char*) )
        return std::string(allyStates[allyState]);
    else
        return std::to_string(allyState);
}

std::string TextTrigGenerator::GetTrigNumericComparison(u8 numericComparison)
{
    const char* numericComparisons[] = { "at least", "at most", "2", "3", "4", "5", "6", "7", "8", "9", "exactly" };
    if ( numericComparison >= 0 && numericComparison < sizeof(numericComparisons) / sizeof(const char*) )
        return std::string(numericComparisons[numericComparison]);
    else
        return std::to_string(numericComparison);
}

std::string TextTrigGenerator::GetTrigNumericModifier(u8 numericModifier)
{
    const char* numericModifiers[] = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };
    if ( numericModifier >= 0 && numericModifier < sizeof(numericModifiers) / sizeof(const char*) )
        return std::string(numericModifiers[numericModifier]);
    else
        return std::to_string(numericModifier);
}

std::string TextTrigGenerator::GetTrigScript(u32 scriptNum)
{
    if ( scriptNum == 0 )
        return std::string("No Script");

    char script[5];
    (u32 &)script[0] = scriptNum;
    script[4] = '\0';

    bool hasNonNumericCharacter =
        script[0] < '0' || script[0] > '9' ||
        script[1] < '0' || script[1] > '9' ||
        script[2] < '0' || script[2] > '9' ||
        script[3] < '0' || script[3] > '9';

    if ( hasNonNumericCharacter )
        return std::string(script);
    else
        return std::string("\"" + std::string(script) + "\"");
}

std::string TextTrigGenerator::GetTrigNumUnits(u8 numUnits)
{
    if ( numUnits == 0 )
        return std::string("All");
    else
        return std::to_string(numUnits);
}

std::string TextTrigGenerator::GetTrigNumber(u32 number)
{
    return std::to_string(number);
}

inline void TextTrigGenerator::AddConditionArgument(StringBuffer & output, Chk::Condition & condition, Chk::Condition::VirtualType conditionId, Chk::Condition::Argument argument)
{
    switch ( argument.type )
    {
        case Chk::Condition::ArgType::Unit: ADD_TEXTTRIG_UNIT(condition.unitType) break;
        case Chk::Condition::ArgType::Location: ADD_TEXTTRIG_LOCATION(condition.locationId) break;
        case Chk::Condition::ArgType::Player: ADD_TEXTTRIG_PLAYER(condition.player) break;
        case Chk::Condition::ArgType::Amount: ADD_TEXTTRIG_NUMBER(condition.amount) break;
        case Chk::Condition::ArgType::Comparison:
        case Chk::Condition::ArgType::NumericComparison: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
        case Chk::Condition::ArgType::ResourceType: ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex) break;
        case Chk::Condition::ArgType::ScoreType: ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex) break;
        case Chk::Condition::ArgType::Switch: ADD_TEXTTRIG_SWITCH(condition.typeIndex) break;
        case Chk::Condition::ArgType::SwitchState: ADD_TEXTTRIG_SWITCH_STATE(condition.comparison) break;
        case Chk::Condition::ArgType::ConditionType: ADD_TEXTTRIG_NUMBER(condition.conditionType) break;
        case Chk::Condition::ArgType::TypeIndex: ADD_TEXTTRIG_NUMBER(condition.typeIndex) break;
        case Chk::Condition::ArgType::Flags: ADD_TEXTTRIG_NUMBER(condition.flags) break;
        case Chk::Condition::ArgType::MaskFlag: ADD_TEXTTRIG_CND_MASK_FLAG(condition.maskFlag) break;
        case Chk::Condition::ArgType::MemoryOffset: ADD_TEXTTRIG_MEMORY(condition.player) break;
    }
}

inline void TextTrigGenerator::AddActionArgument(StringBuffer & output, Chk::Action & action, Chk::Action::VirtualType AID, Chk::Action::Argument argument)
{
    switch ( argument.type )
    {
        case Chk::Action::ArgType::Location: ADD_TEXTTRIG_LOCATION(argument.field == Chk::Action::ArgField::Number ? action.number : action.locationId) break;
        case Chk::Action::ArgType::String: ADD_TEXTTRIG_STRING(action.stringId) break;
        case Chk::Action::ArgType::Player: ADD_TEXTTRIG_PLAYER(argument.field == Chk::Action::ArgField::Number ? action.number : action.group) break;
        case Chk::Action::ArgType::Unit: ADD_TEXTTRIG_UNIT(action.type) break;
        case Chk::Action::ArgType::NumUnits: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
        case Chk::Action::ArgType::CUWP: ADD_TEXTTRIG_NUMBER(action.number) break;
        case Chk::Action::ArgType::TextFlags: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
        case Chk::Action::ArgType::Amount: ADD_TEXTTRIG_NUMBER(action.number) break;
        case Chk::Action::ArgType::ScoreType: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
        case Chk::Action::ArgType::ResourceType: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
        case Chk::Action::ArgType::StateMod: ADD_TEXTTRIG_STATE_MODIFIER(action.type2);
        case Chk::Action::ArgType::Percent: ADD_TEXTTRIG_NUMBER(action.number);
        case Chk::Action::ArgType::Order: ADD_TEXTTRIG_ORDER(action.type2) break;
        case Chk::Action::ArgType::Sound: ADD_TEXTTRIG_SOUND(action.soundStringId) break;
        case Chk::Action::ArgType::Duration: ADD_TEXTTRIG_NUMBER(action.time) break;
        case Chk::Action::ArgType::Script: ADD_TEXTTRIG_SCRIPT(action.number) break;
        case Chk::Action::ArgType::AllyState: ADD_TEXTTRIG_ALLY_STATE(action.type) break;
        case Chk::Action::ArgType::NumericMod: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
        case Chk::Action::ArgType::Switch: ADD_TEXTTRIG_SWITCH(action.number) break;
        case Chk::Action::ArgType::SwitchMod: ADD_TEXTTRIG_SWITCH_MODIFIER(action.type2) break;
        case Chk::Action::ArgType::ActionType: ADD_TEXTTRIG_NUMBER(action.actionType) break;
        case Chk::Action::ArgType::Number: ADD_TEXTTRIG_NUMBER(action.number) break;
        case Chk::Action::ArgType::TypeIndex: ADD_TEXTTRIG_NUMBER(action.type) break;
        case Chk::Action::ArgType::SecondaryTypeIndex: ADD_TEXTTRIG_NUMBER(action.type2) break;
        case Chk::Action::ArgType::Flags: ADD_TEXTTRIG_NUMBER(action.flags) break;
        case Chk::Action::ArgType::Padding: ADD_TEXTTRIG_NUMBER(action.padding) break;
        case Chk::Action::ArgType::MaskFlag: ADD_TEXTTRIG_MASK_FLAG(action.maskFlag) break;
        case Chk::Action::ArgType::MemoryOffset: ADD_TEXTTRIG_MEMORY(action.group) break;
    }
}

bool TextTrigGenerator::BuildTextTrigs(ScenarioPtr map, TrigSectionPtr trigData, std::string & trigString)
{
    if ( !LoadScenario(map, true, false) )
        return false;

    StringBuffer output;

    size_t numTrigs = trigData->numTriggers();
    Chk::Condition::VirtualType CID = Chk::Condition::VirtualType::NoCondition;
    Chk::Action::VirtualType AID = Chk::Action::VirtualType::NoAction;

    int numArgs;

    const u8 conditionNumArgs[] = { 0, 2, 4, 5, 4, 4, 1, 2, 1, 1,
                                    1, 2, 2, 0, 3, 4, 1, 2, 1, 1,
                                    1, 4, 0, 0 };

    const u8 actionNumArgs[] = { 0, 0, 0, 0, 1, 0, 0, 8, 2, 2,
                                 1, 5, 1, 2, 2, 1, 2, 2, 3, 2,
                                 2, 2, 2, 4, 2, 4, 4, 4, 1, 2,
                                 0, 0, 1, 3, 4, 3, 3, 3, 4, 5,
                                 1, 1, 4, 4, 4, 4, 5, 1, 5, 5,
                                 5, 5, 4, 5, 0, 0, 0, 2, 0, 0 };

    for ( u32 trigNum=0; trigNum<numTrigs; trigNum++ )
    {
        std::shared_ptr<Chk::Trigger> trigger = trigData->getTrigger(trigNum);
        if ( trigger != nullptr )
        {
            output += "Trigger(";

            // Add players
            bool hasPrevious = false;
            for ( int groupNum=0; groupNum<Chk::Trigger::MaxOwners; groupNum++ )
            {
                if ( trigger->owned(groupNum) == Chk::Trigger::Owned::Yes )
                {
                    if ( hasPrevious )
                        output += ',';
                    else
                        hasPrevious = true;

                    ChkdString groupName = groupTable[groupNum];
                    output += (std::string &)groupName;
                }
                else if ( trigger->owned(groupNum) != Chk::Trigger::Owned::No )
                {
                    if ( hasPrevious )
                        output += ',';
                    else
                        hasPrevious = true;

                    output += (std::string &)groupTable[groupNum];
                    output += ':';
                    output += std::to_string((u8)trigger->owned(groupNum));
                }
            }

            output += "){\nConditions:";

            // Add conditions
            for ( int i=0; i<Chk::Trigger::MaxConditions; i++ )
            {
                Chk::Condition & condition = trigger->condition(i);
                CID = (Chk::Condition::VirtualType)condition.conditionType;

                if ( CID != Chk::Condition::VirtualType::NoCondition )
                {
                    if ( (condition.flags & Chk::Condition::Flags::Disabled) == Chk::Condition::Flags::Disabled )
                        output += "\n;\t";
                    else
                        output += "\n\t";

                    // Add condition name
                    if ( CID == Chk::Condition::VirtualType::Deaths && condition.player > 28 ) // Memory condition
                        output += "Memory";
                    else if ( CID >= 0 && (size_t)CID < conditionTable.size() )
                        output += conditionTable[CID];
                    else
                        output += "Custom";

                    output += '(';
                    // Add condition args
                    if ( CID == Chk::Condition::VirtualType::Deaths && condition.player > 28 ) // Memory condition
                    {
                        CID = Chk::Condition::VirtualType::Memory;
                        numArgs = 3;
                    }
                    else if ( CID < sizeof(conditionNumArgs) )
                        numArgs = conditionNumArgs[CID];
                    else
                        numArgs = 9; // custom

                    for ( u8 i=0; i<numArgs; i++ )
                    {
                        if ( i > 0 )
                            output += ", ";

                        AddConditionArgument(output, condition, CID, Chk::Condition::getTextArg(CID, i));
                    }

                    output += ");";
                }
            }

            output += "\n\nActions:";

            // Add actions
            for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
            {
                Chk::Action & action = trigger->action(i);
                AID = (Chk::Action::VirtualType)action.actionType;
                if ( AID != Chk::Action::VirtualType::NoAction )
                {
                    if ( (action.flags&Chk::Action::Flags::Disabled) == Chk::Action::Flags::Disabled )
                        output += "\n;\t";
                    else
                        output += "\n\t";

                    // Add action name
                    if ( AID == Chk::Action::VirtualType::SetDeaths && action.group > 28 ) // Memory action
                        output += "Set Memory";
                    else if ( AID >= 0 && (size_t)AID < actionTable.size() )
                        output += actionTable[AID];
                    else
                        output += "Custom";

                    output += '(';
                    // Add action args
                    if ( AID == Chk::Action::VirtualType::SetDeaths && action.group > 28 ) // Memory action
                    {
                        AID = Chk::Action::VirtualType::SetMemory;
                        numArgs = 3;
                    }
                    else if ( AID < sizeof(actionNumArgs) )
                        numArgs = actionNumArgs[AID];
                    else
                        numArgs = 11; // custom

                    for ( u8 i=0; i<numArgs; i++ )
                    {
                        if ( i > 0 )
                            output += ", ";

                        AddActionArgument(output, action, AID, Chk::Action::getTextArg(AID, i));
                    }

                    output += ");";
                }
            }

            // Add Flags
            if ( trigger->flags > 0 )
            {
                output += "\n\nFlags:\n";
                char number[36];
                _itoa_s(trigger->flags, number, 36, 2); // TODO: FIXME
                size_t length = std::strlen(number);
                output += std::string(32-length, '0');
                output += std::string(number);
                output += ';';
            }

            output += "\n}\n\n//-----------------------------------------------------------------//\n\n";
        }
    }
    // Add NUL
    output += '\0';
    CorrectLineEndings(output);

    trigString = output.str();
    ClearScenario();
    return true;
}

std::string TextTrigGenerator::GetTrigTextFlags(u8 textFlags)
{
    const char* cTextFlags[] = { "Don't Always Display", "Always Display" };
    if      ( (textFlags&Chk::Action::Flags::AlwaysDisplay) == 0 )
        return std::string(cTextFlags[0]);
    else if ( (textFlags&Chk::Action::Flags::AlwaysDisplay) == Chk::Action::Flags::AlwaysDisplay )
        return std::string(cTextFlags[1]);
    else
        return std::to_string(textFlags);
}

// protected

bool TextTrigGenerator::LoadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames)
{
    return map != nullptr &&
           PrepConditionTable() &&
           PrepActionTable() &&
           PrepLocationTable(map, quoteArgs) &&
           PrepUnitTable(map, quoteArgs, useCustomNames) &&
           PrepSwitchTable(map, quoteArgs) &&
           PrepGroupTable(map, quoteArgs) &&
           PrepScriptTable(map, quoteArgs) &&
           PrepStringTable(map, quoteArgs);
}

bool TextTrigGenerator::CorrectLineEndings(StringBuffer & buf)
{
    char curr;
    u32 pos = 0;
    StringBuffer dest;

    while ( pos < buf.size() ) 
    {
        curr = buf[pos];
        switch ( curr )
        {
            case '\15': // CR (line ending)
                if ( buf[pos+1] == '\12' ) // Has LF
                    pos ++;
            case '\12': // LF (line ending)
            case '\13': // VT (line ending)
            case '\14': // FF (line ending)
                dest += '\15';
                dest += '\12';
                pos ++;
                break;
            default:
                dest += curr;
                pos ++;
                break;
        }

    }

    buf.swap(dest);
    return true;
}

// private

bool TextTrigGenerator::PrepConditionTable()
{
    if ( goodConditionTable )
        return true;

    const char* legacyConditionNames[] = { "", "Countdown Timer", "Command", "Bring", "Accumulate", "Kill", "Command the Most", 
                                           "Commands the Most At", "Most Kills", "Highest Score", "Most Resources", "Switch", "Elapsed Time", 
                                           "Never", "Opponents", "Deaths", "Command the Least", "Command the Least At", "Least Kills", 
                                           "Lowest Score", "Least Resources", "Score", "Always", "Never" };

    const char** conditionNames = legacyConditionNames;

    for ( int i=0; i<24; i++ )
        conditionTable.push_back(std::string(conditionNames[i]));

    std::string custom("Custom");
    for ( int i=24; i<256; i++ )
        conditionTable.push_back(custom);

    goodConditionTable = true;
    return true;
}

bool TextTrigGenerator::PrepActionTable()
{
    if ( goodActionTable )
        return true;

    const char* legacyActionNames[] = { "No Action", "Victory", "Defeat", "Preserve Trigger", "Wait", "Pause Game", "Unpause Game", "Transmission", 
                                        "Play WAV", "Display Text Message", "Center View", "Create Unit with Properties", "Set Mission Objectives", 
                                        "Set Switch", "Set Countdown Timer", "Run AI Script", "Run AI Script At Location", "Leader Board Control", 
                                        "Leader Board Control At Location","Leader Board Resources", "Leader Board Kills", "Leader Board Points", 
                                        "Kill Unit", "Kill Unit At Location", "Remove Unit", "Remove Unit At Location", "Set Resources", "Set Score", 
                                        "Minimap Ping", "Talking Portrait", "Mute Unit Speech", "Unmute Unit Speech", "Leaderboard Computer Players", 
                                        "Leaderboard Goal Control", "Leaderboard Goal Control At Location", "Leaderboard Goal Resources", 
                                        "Leaderboard Goal Kills", "Leaderboard Goal Points", "Move Location", "Move Unit", "Leaderboard Greed", 
                                        "Set Next Scenario", "Set Doodad State", "Set Invincibility", "Create Unit", "Set Deaths", "Order", "Comment", 
                                        "Give Units to Player", "Modify Unit Hit Points", "Modify Unit Energy", "Modify Unit Shield Points", 
                                        "Modify Unit Resource Amount", "Modify Unit Hanger Count", "Pause Timer", "Unpause Timer", "Draw", 
                                        "Set Alliance Status", "Disable Debug Mode", "Enable Debug Mode" };

    const char** actionNames = legacyActionNames;

    for ( int i=0; i<60; i++ )
        actionTable.push_back(std::string(actionNames[i]));

    std::string custom("Custom");
    for ( int i=60; i<256; i++ )
        actionTable.push_back(custom);

    goodActionTable = true;
    return true;
}

bool TextTrigGenerator::PrepLocationTable(ScenarioPtr map, bool quoteArgs)
{
    locationTable.clear();
    
    SingleLineChkdString locationName;

    locationTable.push_back(SingleLineChkdString("No Location"));

    size_t numLocations = map->layers.numLocations();
    for ( size_t i=1; i<=numLocations; i++ )
    {
        Chk::LocationPtr loc = map->layers.getLocation(i);
        if ( loc != nullptr )
        {
            locationName = "";

            if ( i == Chk::LocationId::Anywhere )
            {
                if ( quoteArgs )
                    locationName = "\"Anywhere\"";
                else
                    locationName = "Anywhere";

                locationTable.push_back( locationName );
            }
            else if ( loc->stringId > 0 )
            {
                std::shared_ptr<EscString> locationName = map->strings.getString<EscString>((size_t)loc->stringId, Chk::Scope::Game);
                if ( locationName != nullptr )
                {
                    if ( quoteArgs )
                        locationTable.push_back( "\"" + *locationName + "\"" );
                    else
                        locationTable.push_back(*locationName);
                }
            }
        }

        if ( locationTable.size() == i )
            locationTable.push_back(std::to_string(i));
    }

    return true;
}

bool TextTrigGenerator::PrepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames)
{
    unitTable.clear();

    SingleLineChkdStringPtr unitName;
    for ( int unitID=0; unitID<=232; unitID++ )
    {
        if ( quoteArgs )
        {
            if ( useCustomNames && unitID < 228 )
            {
                SingleLineChkdStringPtr unquotedName = map->strings.getUnitName<SingleLineChkdString>((Sc::Unit::Type)unitID, true);
                unitName = SingleLineChkdStringPtr(new SingleLineChkdString("\"" + *unquotedName + "\""));
            }
            else
                unitName = SingleLineChkdStringPtr(new SingleLineChkdString("\"" + std::string(Sc::Unit::legacyTextTrigDisplayNames[unitID]) + "\""));
        }
        else
        {
            if ( useCustomNames && unitID < 228 )
                unitName = map->strings.getUnitName<SingleLineChkdString>((Sc::Unit::Type)unitID, true);
            else
                unitName = SingleLineChkdStringPtr(new SingleLineChkdString(Sc::Unit::legacyTextTrigDisplayNames[unitID]));
        }

        unitTable.push_back(*unitName);
    }
    return true;
}

bool TextTrigGenerator::PrepSwitchTable(ScenarioPtr map, bool quoteArgs)
{
    switchTable.clear();

    for ( u32 switchIndex=0; switchIndex<Chk::TotalSwitches; switchIndex++ )
    {
        RawStringPtr switchName = map->strings.getSwitchName<RawString>(switchIndex);
        if ( switchName != nullptr )
        {
            if ( quoteArgs )
                switchTable.push_back( "\"" + *switchName + "\"" );
            else
                switchTable.push_back(*switchName);
        }
        else
        {
            if ( quoteArgs )
                switchTable.push_back(RawString("\"Switch" + std::to_string(switchIndex + 1) + "\""));
            else
                switchTable.push_back(RawString("Switch" + std::to_string(switchIndex + 1)));
        }
    }
    return true;
}

bool TextTrigGenerator::PrepGroupTable(ScenarioPtr map, bool quoteArgs)
{
    groupTable.clear();

    SingleLineChkdString groupName;

    const char** legacyLowerGroupNames;
    const char** legacyUpperGroupNames;

    if ( quoteArgs )
    {
        const char* legacyLowerGroups[] = { "\"Player 1\"", "\"Player 2\"", "\"Player 3\"", "\"Player 4\"", "\"Player 5\"", "\"Player 6\"",
                                            "\"Player 7\"", "\"Player 8\"", "\"Player 9\"", "\"Player 10\"", "\"Player 11\"", "\"Player 12\"",
                                            "\"unknown/unused\"", "\"Current Player\"", "\"Foes\"", "\"Allies\"", "\"Neutral Players\"",
                                            "\"All players\"" };
        const char* legacyUpperGroups[] = { "\"unknown/unused\"", "\"unknown/unused\"", "\"unknown/unused\"", "\"unknown/unused\"",
                                            "\"Non Allied Victory Players\"", "\"unknown/unused\"" };

        legacyLowerGroupNames = legacyLowerGroups;
        legacyUpperGroupNames = legacyUpperGroups;
    }
    else
    {
        const char* legacyLowerGroups[] = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6",
                                            "Player 7", "Player 8", "Player 9", "Player 10", "Player 11", "Player 12",
                                            "unknown/unused", "Current Player", "Foes", "Allies", "Neutral Players",
                                            "All players" };
        const char* legacyUpperGroups[] = { "unknown/unused", "unknown/unused", "unknown/unused", "unknown/unused",
                                            "Non Allied Victory Players", "unknown/unused" };

        legacyLowerGroupNames = legacyLowerGroups;
        legacyUpperGroupNames = legacyUpperGroups;
    }

    const char** lowerGroups = legacyLowerGroupNames;
    const char** upperGroups = legacyUpperGroupNames;

    for ( u32 i=0; i<18; i++ )
    {
        groupName = lowerGroups[i];
        groupTable.push_back( groupName );
    }

    for ( u32 i=0; i<4; i++ )
    {
        ChkdStringPtr forceName = map->strings.getForceName<ChkdString>((Chk::Force)i);
        if ( forceName != nullptr )
        {
            if ( quoteArgs )
                groupName = "\"" + *forceName + "\"";

            groupTable.push_back( groupName );
        }
        else
        {
            char forceName[12];
            if ( quoteArgs )
                groupName = "\"Force " + std::to_string(i) + "\"";
            else
                groupName = "Force " + std::to_string(i);

            groupName = forceName;
            groupTable.push_back( groupName );
        }
    }

    for ( u32 i=22; i<28; i++ )
    {
        groupName = upperGroups[i-22];
        groupTable.push_back( groupName );
    }

    return true;
}

bool TextTrigGenerator::PrepScriptTable(ScenarioPtr map, bool quoteArgs)
{
    scriptTable.clear();

    scriptTable.insert(std::pair<u32, std::string>(0, "No Script"));

    Chk::Trigger* trigPtr = nullptr;
    size_t numTrigs = map->triggers.numTriggers();
    for ( u32 i = 0; i < numTrigs; i++ )
    {
        Chk::TriggerPtr trigPtr = map->triggers.getTrigger(i);
        if ( trigPtr != nullptr )
        {
            for ( u8 actionNum = 0; actionNum < Chk::Trigger::MaxActions; actionNum++ )
            {
                Chk::Action & action = trigPtr->action(actionNum);
                Chk::Action::Type actionId = action.actionType;
                bool isScriptAction = (actionId == Chk::Action::Type::RunAiScript || actionId == Chk::Action::Type::RunAiScriptAtLocation);
                if ( isScriptAction && action.number != 0 )
                {
                    char numberString[5] = {};
                    (u32 &)numberString = action.number;
                    for ( u8 i = 0; i < 4; i++ )
                    {
                        if ( numberString[i] == '\0' )
                            numberString[i] = ' ';
                    }

                    if ( quoteArgs )
                    {
                        scriptTable.insert(std::pair<u32, std::string>(action.number,
                            std::string("\"" + std::string(numberString) + "\"")));
                    }
                    else
                        scriptTable.insert(std::pair<u32, std::string>(action.number, std::string(numberString)));
                }
            }
        }
    }
    return true;
}

bool TextTrigGenerator::PrepStringTable(ScenarioPtr map, bool quoteArgs)
{
    stringTable.clear();
    extendedStringTable.clear();

    std::bitset<Chk::MaxStrings> stringUsed, extendedStringUsed;
    map->strings.markValidUsedStrings(stringUsed, Chk::Scope::Either, Chk::Scope::Game);
    map->strings.markValidUsedStrings(extendedStringUsed, Chk::Scope::Either, Chk::Scope::Editor);
    SingleLineChkdStringPtr str;

    size_t numStrings = map->strings.getCapacity(Chk::Scope::Game);
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( stringUsed[i] )
            str = map->strings.getString<SingleLineChkdString>(i);

        if ( str != nullptr )
        {
            SingleLineChkdString newString;
            for ( auto & character : *str )
            {
                if ( character == '\"' )
                {
                    newString.push_back('\\');
                    newString.push_back('\"');
                }
                else
                    newString.push_back(character);
            }

            if ( quoteArgs )
                stringTable.push_back( "\"" + newString + "\"" );
            else
                stringTable.push_back(newString);
        }
    }

    numStrings = map->strings.getCapacity(Chk::Scope::Editor);
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( extendedStringUsed[i] )
            str = map->strings.getString<SingleLineChkdString>(i);
        
        if ( str != nullptr )
        {
            SingleLineChkdString newString;
            for ( auto & character : *str )
            {
                if ( character == '\"' )
                {
                    newString.push_back('\\');
                    newString.push_back('\"');
                }
                else
                    newString.push_back(character);
            }

            if ( quoteArgs )
                extendedStringTable.push_back( "\"" + newString + "\"" );
            else
                extendedStringTable.push_back(newString);
        }
    }

    return true;
}
