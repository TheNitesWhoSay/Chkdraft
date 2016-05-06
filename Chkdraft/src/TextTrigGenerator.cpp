#include "TextTrigGenerator.h"
#include <string>

const char* textFlags[] = { "Don't Always Display", "Always Display" };
const char* scoreTypes[] = { "Total", "Units", "Buildings", "Units and buildings", "Kills", "Razings", "Kills and razings", "Custom" };
const char* resourceTypes[] = { "ore", "gas", "ore and gas" };
const char* orderTypes[] = { "move", "patrol", "attack" };
const char* stateModifiers[] = { "0", "1", "2", "3", "enabled", "disabled", "toggle" };
const char* switchStates[] = { "0", "1", "set", "not set" };
const char* switchModifiers[] = { "0", "1", "2", "3", "set", "clear", "toggle", "7", "8", "9", "10", "randomize" };
const char* allyStates[] = { "Enemy", "Ally", "Allied Victory" };
const char* numericComparisons[] = { "At least", "At most", "2", "3", "4", "5", "6", "7", "8", "9", "Exactly" };
const char* numericModifiers[] = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };

void CollapsableDefines()
{
#define ADD_TEXTTRIG_LOCATION(src) {                                                                            \
        if ( src >= 0 && src < locationTable.size() )                                                           \
            output.addStr(locationTable[src].c_str(), locationTable[src].size());                               \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_STRING(src) {                                                                      \
        if ( src == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )                      \
            output.addStr("No String");                                                                 \
        else if ( src >= 0 && (src < stringTable.size() || (65536-src) < extendedStringTable.size() ) ) \
        {                                                                                               \
            if ( src < stringTable.size() )                                                             \
                output.addStr(stringTable[src].c_str(), stringTable[src].size());                       \
            else                                                                                        \
                output.addStr(std::string("k" + extendedStringTable[65536-src]).c_str(),                \
                    extendedStringTable[65536-src].size()+1);                                           \
        }                                                                                               \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_WAV(src) {                                                                             \
        if ( src == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )                          \
            output.addStr("No WAV");                                                                        \
        else if ( src >= 0 && (src < stringTable.size() || (65536 - src) < extendedStringTable.size()) )    \
        {                                                                                                   \
            if ( src < stringTable.size() )                                                                 \
                output.addStr(stringTable[src].c_str(), stringTable[src].size());                           \
            else                                                                                            \
                output.addStr((std::string("k") + extendedStringTable[65536 - src]).c_str(),                \
                    extendedStringTable[65536 - src].size() + 1);                                           \
        }                                                                                                   \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }


#define ADD_TEXTTRIG_PLAYER(src) {                                                  \
        if ( src >= 0 && src < groupTable.size() )                                  \
            output.addStr(groupTable[src].c_str(), groupTable[src].size());         \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_UNIT(src) {                                                    \
        if ( src >= 0 && src < unitTable.size() )                                   \
            output.addStr(unitTable[src].c_str(), unitTable[src].size());           \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_SWITCH(src) {                                                  \
        if ( src >= 0 && src < switchTable.size() )                                 \
            output.addStr(switchTable[src].c_str(), switchTable[src].size());       \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_SCORE_TYPE(src) {                                              \
        if ( src >= 0 && src < sizeof(scoreTypes)/sizeof(const char*) )             \
            output.addStr(scoreTypes[src], std::strlen(scoreTypes[src]));           \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_RESOURCE_TYPE(src) {                                           \
        if ( src >= 0 && src < sizeof(resourceTypes)/sizeof(const char*) )          \
            output.addStr(resourceTypes[src], std::strlen(resourceTypes[src]));     \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_ORDER(src) {                                                   \
        if ( src >= 0 && src < sizeof(orderTypes)/sizeof(const char*) )             \
            output.addStr(orderTypes[src], std::strlen(orderTypes[src]));           \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_STATE_MODIFIER(src) {                                          \
        if ( src >= 0 && src < sizeof(stateModifiers)/sizeof(const char*) )         \
            output.addStr(stateModifiers[src], std::strlen(stateModifiers[src]));   \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } } 

#define ADD_TEXTTRIG_SWITCH_STATE(src) {                                            \
        if ( src >= 0 && src < sizeof(switchStates)/sizeof(const char*) )           \
            output.addStr(switchStates[src], std::strlen(switchStates[src]));       \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_SWITCH_MODIFIER(src) {                                         \
        if ( src >= 0 && src < sizeof(switchModifiers)/sizeof(const char*) )        \
            output.addStr(switchModifiers[src], std::strlen(switchModifiers[src])); \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_ALLY_STATE(src) {                                              \
        if ( src >= 0 && src < sizeof(allyStates)/sizeof(const char*) )             \
            output.addStr(allyStates[src], std::strlen(allyStates[src]));           \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_NUMERIC_COMPARISON(src) {                                              \
        if ( src >= 0 && src < sizeof(numericComparisons)/sizeof(const char*) )             \
            output.addStr(numericComparisons[src], std::strlen(numericComparisons[src]));   \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

#define ADD_TEXTTRIG_NUMERIC_MODIFIER(src) {                                            \
        if ( src >= 0 && src < sizeof(numericModifiers)/sizeof(const char*) )           \
            output.addStr(numericModifiers[src], std::strlen(numericModifiers[src]));   \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

    #define ADD_TEXTTRIG_SCRIPT(src) {                                              \
        if ( src == 0 )                                                             \
            output.addStr("No Script", 9);                                          \
        else                                                                        \
        {                                                                           \
            auto it = scriptTable.find(src);                                        \
            if ( it != scriptTable.end() )                                          \
                output.addStr(it->second);                                          \
            else { output.add('\"'); output.addStr((char*)&src, 4); output.add('\"'); } \
        } }

    #define ADD_TEXTTRIG_NUM_UNITS(src) {                                           \
        if ( src == 0 ) output.addStr("All", 3);                                    \
        else { std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); } }

    #define ADD_TEXTTRIG_NUMBER(src) {                                              \
        std::strcpy(number, std::to_string(src).c_str()); output.addStr(number, std::strlen(number)); }

    #define ADD_TEXTTRIG_TEXT_FLAGS(src) {                                                      \
        if      ( (src&(u8)Action::Flags::AlwaysDisplay) == 0 )                                 \
            output.addStr(textFlags[0], std::strlen(textFlags[0]));                             \
        else if ( (src&(u8)Action::Flags::AlwaysDisplay) == (u8)Action::Flags::AlwaysDisplay )  \
            output.addStr(textFlags[1], std::strlen(textFlags[1])); }
}

TextTrigGenerator::TextTrigGenerator() : goodConditionTable(false), goodActionTable(false)
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

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, std::string &trigString)
{
    return this != nullptr && map != nullptr && BuildTextTrigs(map, map->GetTrigSection(), trigString);
}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, u32 trigId, std::string &trigString)
{
    Trigger* trig;
    TrigSegment trigBuff(new buffer((u32)SectionId::TRIG));
    return this != nullptr &&
           map != nullptr &&
           map->getTrigger(trig, trigId) &&
           trigBuff->add<Trigger>(*trig) &&
           BuildTextTrigs(map, trigBuff, trigString);
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

std::string TextTrigGenerator::GetConditionArgument(Condition& condition, u8 stdTextTrigArgNum)
{
    buffer output("TEXC");
    AddConditionArgument(output, condition, (ConditionId)condition.condition, stdTextTrigArgNum);
    if ( output.add('\0') )
        return std::string((char*)output.getPtr(0));
    else
        return "";
}

std::string TextTrigGenerator::GetConditionArgument(Condition& condition, u8 argNum, std::vector<u8> &argMap)
{
    if ( argNum < argMap.size() )
    {
        u8 stdTextTrigArgNum = argMap[argNum];
        buffer output("TEXC");
        AddConditionArgument(output, condition, (ConditionId)condition.condition, stdTextTrigArgNum);
        if ( output.add('\0') )
            return std::string((char*)output.getPtr(0));
    }
    return "";
}

std::string TextTrigGenerator::GetActionName(u8 AID)
{
    if ( AID < actionTable.size() )
        return actionTable[AID];
    else
        return std::to_string((int)AID);
}

std::string TextTrigGenerator::GetActionArgument(Action& action, u8 stdTextTrigArgNum)
{
    buffer output("TEXA");
    AddActionArgument(output, action, (ActionId)action.action, stdTextTrigArgNum);
    if ( output.add('\0') )
        return std::string((char*)output.getPtr(0));
    else
        return "";
}

std::string TextTrigGenerator::GetActionArgument(Action& action, u8 argNum, std::vector<u8> &argMap)
{
    if ( argNum < argMap.size() )
    {
        u8 stdTextTrigArgNum = argMap[argNum];
        buffer output("TEXA");
        AddActionArgument(output, action, (ActionId)action.action, stdTextTrigArgNum);
        if ( output.add('\0') )
            return std::string((char*)output.getPtr(0));
    }
    return "";
}

ChkdString TextTrigGenerator::GetTrigLocation(u32 locationNum)
{
    if ( locationNum >= 0 && locationNum < locationTable.size() )
        return ChkdString(locationTable[locationNum], true);
    else
        return ChkdString(std::to_string(locationNum), true);
}

ChkdString TextTrigGenerator::GetTrigString(u32 stringNum)
{
    if ( stringNum == 0 && (stringTable.size() <= 0 || stringTable[0].size() == 0) )
        return ChkdString("No String", true);
    else if ( stringNum >= 0 && (stringNum < stringTable.size() || (65536 - stringNum) < extendedStringTable.size()) )
    {
        if ( stringNum < stringTable.size() )
            return stringTable[stringNum];
        else
            return ChkdString("k" + extendedStringTable[65536 - stringNum], true);
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
    (u32&)script[0] = scriptNum;
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

inline void TextTrigGenerator::AddConditionArgument(buffer &output, Condition& condition, ConditionId conditionId, u8 &stdTextTrigArgNum)
{
    switch ( conditionId )
    {
        case ConditionId::Accumulate: // Player, NumericComparison, Amount, ResourceType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
                case 3: ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex) break;
            }
            break;
        case ConditionId::Bring: // Player, Unit, Location, NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
                case 2: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
                case 3: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 4: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        case ConditionId::Command: // Player, Unit, NumericComparison, Amount
        case ConditionId::Deaths:  // Player, Unit, NumericComparison, Amount
        case ConditionId::Kill:    // Player, Unit, NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
                case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        case ConditionId::CommandTheLeast: // Unit
        case ConditionId::CommandTheMost:  // Unit
        case ConditionId::LeastKills:      // Unit
        case ConditionId::MostKills:       // Unit
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_UNIT(condition.unitID);
            break;
        case ConditionId::CommandTheLeastAt: // Unit, Location
        case ConditionId::CommandTheMostAt:  // Unit, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_UNIT(condition.unitID) break;
                case 1: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
            }
            break;
        case ConditionId::CountdownTimer: // NumericComparison, Amount
        case ConditionId::ElapsedTime:    // NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 1: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        case ConditionId::HighestScore: // ScoreType
        case ConditionId::LowestScore:  // ScoreType
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex);
            break;
        case ConditionId::LeastResources: // ResourceType
        case ConditionId::MostResources:  // ResourceType
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex);
            break;
        case ConditionId::Opponents: // Player, NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        case ConditionId::Score: // Player, ScoreType, NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 1: ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex) break;
                case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        case ConditionId::Switch: // Switch, SwitchState
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_SWITCH(condition.typeIndex) break;
                case 1: ADD_TEXTTRIG_SWITCH_STATE(condition.comparison) break;
            }
            break;
        case ConditionId::Memory: // MemOffset, NumericComparison, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_NUMBER(condition.players) break;
                case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
            }
            break;
        default: // Location, Player, Amount, Unit, NumericComparison, Condition, TypeIndex, Flags, Internal
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
                case 1: ADD_TEXTTRIG_PLAYER(condition.players) break;
                case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
                case 3: ADD_TEXTTRIG_UNIT(condition.unitID) break;
                case 4: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
                case 5: ADD_TEXTTRIG_NUMBER(condition.condition) break;
                case 6: ADD_TEXTTRIG_NUMBER(condition.typeIndex) break;
                case 7: ADD_TEXTTRIG_NUMBER(condition.flags) break;
                case 8: ADD_TEXTTRIG_NUMBER(condition.internalData) break;
            }
    }
}

inline void TextTrigGenerator::AddActionArgument(buffer &output, Action &action, ActionId AID, u8 &stdTextTrigArgNum)
{
    switch ( AID )
    {
        case ActionId::CenterView:  // Location
        case ActionId::MinimapPing: // Location
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_LOCATION(action.location)
            break;
        case ActionId::Comment:              // String
        case ActionId::SetMissionObjectives: // String
        case ActionId::SetNextScenario:      // String
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_STRING(action.stringNum)
            break;
        case ActionId::KillUnitAtLocation: // Player, Unit, NumUnits, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::CreateUnit: // Player, Unit, Number (NumUnits w/o 'All'), Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::CreateUnitWithProperties: // Player, Unit, Number (NumUnits w/o 'All'), Location, Properties
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 4: ADD_TEXTTRIG_NUMBER(action.number) break;
            }
            break;
        case ActionId::DisplayTextMessage: // TextFlags, String
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
                case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
            }
            break;
        case ActionId::GiveUnitsToPlayer: // Player, SecondPlayer, Unit, NumUnits, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_PLAYER(action.number) break;
                case 2: ADD_TEXTTRIG_UNIT(action.type) break;
                case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::KillUnit:   // Player, Unit
        case ActionId::RemoveUnit: // Player, Unit
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
            }
            break;
        case ActionId::LeaderboardCtrlAtLoc: // String, Unit, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::LeaderboardCtrl: // String, Unit
        case ActionId::LeaderboardKills: // String, Unit
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
            }
            break;
        case ActionId::LeaderboardGreed: // Amount
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_NUMBER(action.number);
            break;
        case ActionId::LeaderboardPoints: // String, ScoreType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
            }
            break;
        case ActionId::LeaderboardResources: // String, ResourceType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
            }
            break;
        case ActionId::LeaderboardCompPlayers: // StateModifier
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_STATE_MODIFIER(action.type2);
            break;
        case ActionId::LeaderboardGoalCtrlAtLoc: // String, Unit, Amount, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::LeaderboardGoalCtrl: // String, Unit, Amount
        case ActionId::LeaderboardGoalKills: // String, Unit, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
            }
            break;
        case ActionId::LeaderboardGoalPoints: // String, ScoreType, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
            }
            break;
        case ActionId::LeaderboardGoalResources: // String, Amount, ResourceType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 2: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
            }
            break;
        case ActionId::ModifyUnitEnergy: // Player, Unit, Amount, NumUnits, Location
        case ActionId::ModifyUnitHangerCount: // Player, Unit, Amount, NumUnits, Location
        case ActionId::ModifyUnitHitpoints: // Player, Unit, Amount, NumUnits, Location
        case ActionId::ModifyUnitShieldPoints: // Player, Unit, Amount, NumUnits, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::ModifyUnitResourceAmount: // Player, Amount, NumUnits, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::MoveLocation: // Player, Unit, LocDest, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
            }
            break;
        case ActionId::MoveUnit: // Player, Unit, NumUnits, Location, LocDest
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 4: ADD_TEXTTRIG_LOCATION(action.number) break;
            }
            break;
        case ActionId::Order: // Player, Unit, Location, LocDest, OrderType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
                case 4: ADD_TEXTTRIG_ORDER(action.type2) break;
            }
            break;
        case ActionId::PlayWav: // Wav, Duration
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_WAV(action.wavID) break;
                case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
            }
            break;
        case ActionId::RemoveUnitAtLocation: // Player, Unit, NumUnits, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::RunAiScript: // Script
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_SCRIPT(action.number)
            break;
        case ActionId::RunAiScriptAtLocation: // Script, Location
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_SCRIPT(action.number) break;
                case 1: ADD_TEXTTRIG_LOCATION(action.location) break;
            }
            break;
        case ActionId::SetAllianceStatus: // Player, AllyState
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_ALLY_STATE(action.type) break;
            }
            break;
        case ActionId::SetCountdownTimer: // NumericModifier, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
                case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
            }
            break;
        case ActionId::SetDeaths: // Player, Unit, NumericModifier, Amount
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
                case 3: ADD_TEXTTRIG_NUMBER(action.number) break;
            }
            break;
        case ActionId::SetDoodadState:   // Player, Unit, Location, StateMod
        case ActionId::SetInvincibility: // Player, Unit, Location, StateMod
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_UNIT(action.type) break;
                case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 3: ADD_TEXTTRIG_STATE_MODIFIER(action.type2) break;
            }
            break;
        case ActionId::SetResources: // Player, NumericModifier, Amount, ResourceType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 3: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
            }
            break;
        case ActionId::SetScore: // Player, NumericModifier, Amount, ScoreType
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
                case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 3: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
            }
            break;
        case ActionId::SetSwitch: // Switch, SwitchMod
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_SWITCH(action.number) break;
                case 1: ADD_TEXTTRIG_SWITCH_MODIFIER(action.type2) break;
            }
            break;
        case ActionId::TalkingPortrait: // Unit, Duration
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_UNIT(action.type) break;
                case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
            }
            break;
        case ActionId::Transmission: // TextFlags, String, Unit, Location, NumericModifier, DurationMod, Wav, Duration
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
                case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 2: ADD_TEXTTRIG_UNIT(action.type) break;
                case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 4: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
                case 5: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 6: ADD_TEXTTRIG_WAV(action.wavID) break;
                case 7: ADD_TEXTTRIG_NUMBER(action.time) break;
            }
            break;
        case ActionId::Wait: // Duration
            if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_NUMBER(action.time);
            break;
        default: // Location, String, Wav, Duration, Player, Number, Type, Action, Type2, Flags, Internal
            switch ( stdTextTrigArgNum ) {
                case 0: ADD_TEXTTRIG_LOCATION(action.location) break;
                case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
                case 2: ADD_TEXTTRIG_WAV(action.wavID) break;
                case 3: ADD_TEXTTRIG_NUMBER(action.time) break;
                case 4: ADD_TEXTTRIG_PLAYER(action.group) break;
                case 5: ADD_TEXTTRIG_NUMBER(action.number) break;
                case 6: ADD_TEXTTRIG_NUMBER(action.type) break;
                case 7: ADD_TEXTTRIG_NUMBER(action.action) break;
                case 8: ADD_TEXTTRIG_NUMBER(action.type2) break;
                case 9: ADD_TEXTTRIG_NUMBER(action.flags) break;
                case 10: ADD_TEXTTRIG_NUMBER(TripletToInt(&action.internalData[0])) break;
            }
            break;
    }
}

bool TextTrigGenerator::BuildTextTrigs(ScenarioPtr map, TrigSegment trigData, std::string &trigString)
{
    if ( !LoadScenario(map, true, false) )
        return false;

    buffer output("TeOu");

    u32 numTrigs = trigData->size()/TRIG_STRUCT_SIZE;
    Trigger* currTrig;
    Condition* conditions;
    Action* actions;
    u8* players;
    ConditionId CID = ConditionId::NoCondition;
    ActionId AID = ActionId::NoAction;

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
        if ( trigData->getPtr<Trigger>(currTrig, trigNum*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE) )
        {
            output.addStr("Trigger(", 8);

            // Add players
            players = currTrig->players;
            bool hasPrevious = false;
            for ( int groupNum=0; groupNum<NUM_TRIG_PLAYERS; groupNum++ )
            {
                if ( players[groupNum] == EXECUTED_BY_PLAYER )
                {
                    if ( hasPrevious )
                        output.add<char>(',');
                    else
                        hasPrevious = true;

                    ChkdString groupName = groupTable[groupNum];
                    output.addStr(groupName.c_str(), groupName.size());
                }
                else if ( players[groupNum] > 0 )
                {
                    if ( hasPrevious )
                        output.add<char>(',');
                    else
                        hasPrevious = true;

                    output.addStr(groupTable[groupNum]);
                    output.add<char>(':');
                    output.addStr(std::to_string(players[groupNum]));
                }
            }

            output.addStr("){\nConditions:", 14);

            // Add conditions
            conditions = currTrig->conditions;
            for ( int i=0; i<NUM_TRIG_CONDITIONS; i++ )
            {
                Condition& condition = conditions[i];
                CID = (ConditionId)condition.condition;

                if ( CID != (ConditionId)ConditionId::NoCondition )
                {
                    if ( (condition.flags&(u8)Condition::Flags::Disabled) == (u8)Condition::Flags::Disabled )
                        output.addStr("\n;\t", 3);
                    else
                        output.addStr("\n\t", 2);

                    // Add condition name
                    if ( CID == ConditionId::Deaths && condition.players > 28 ) // Memory condition
                        output.addStr("Memory", 6);
                    else if ( (s32)CID < (s32)conditionTable.size() )
                        output.addStr(conditionTable[(s32)CID].c_str(), conditionTable[(s32)CID].size());
                    else
                        output.addStr("Custom", 6);

                    output.add<char>('(');
                    // Add condition args
                    if ( CID == ConditionId::Deaths && condition.players > 28 ) // Memory condition
                    {
                        CID = ConditionId::Memory;
                        numArgs = 3;
                    }
                    else if ( (s32)CID < sizeof(conditionNumArgs) )
                        numArgs = conditionNumArgs[(s32)CID];
                    else
                        numArgs = 9; // custom

                    for ( u8 i=0; i<numArgs; i++ )
                    {
                        if ( i > 0 )
                            output.addStr(", ", 2);

                        AddConditionArgument(output, condition, (ConditionId)CID, i);
                    }

                    output.addStr(");", 2);
                }
            }

            output.addStr("\n\nActions:", 10);

            // Add actions
            actions = currTrig->actions;
            for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
            {
                Action& action = actions[i];
                AID = (ActionId)action.action;
                if ( AID != ActionId::NoAction )
                {
                    if ( (action.flags&(u8)Action::Flags::Disabled) == (u8)Action::Flags::Disabled )
                        output.addStr("\n;\t", 3);
                    else
                        output.addStr("\n\t", 2);

                    // Add action name
                    if ( (s32)AID < (s32)actionTable.size() )
                        output.addStr(actionTable[(s32)AID].c_str(), actionTable[(s32)AID].size());
                    else
                        output.addStr("Custom", 6);

                    output.add<char>('(');
                    // Add action args
                    if ( (s32)AID < sizeof(actionNumArgs) )
                        numArgs = actionNumArgs[(s32)AID];
                    else
                        numArgs = 11; // custom

                    for ( u8 i=0; i<numArgs; i++ )
                    {
                        if ( i > 0 )
                            output.addStr(", ", 2);

                        AddActionArgument(output, action, AID, i);
                    }

                    output.addStr(");", 2);
                }
            }

            // Add Flags
            if ( currTrig->internalData > 0 )
            {
                output.addStr("\n\nFlags:\n", 9);
                _itoa_s(currTrig->internalData, number, 36, 2); //FIXME
                size_t length = std::strlen(number);
                output.addStr("00000000000000000000000000000000", (32-length));
                output.addStr(number, length);
                output.add<char>(';');
            }

            output.addStr("\n}\n\n//-----------------------------------------------------------------//\n\n", 75);
        }
    }
    // Add NUL
    output.add<char>('\0');
    CorrectLineEndings(output);

    trigString = (const char*)output.getPtr(0);
    ClearScenario();
    return true;
}

std::string TextTrigGenerator::GetTrigTextFlags(u8 textFlags)
{
    const char* cTextFlags[] = { "Don't Always Display", "Always Display" };
    if      ( (textFlags&(u8)Action::Flags::AlwaysDisplay) == 0 )
        return std::string(cTextFlags[0]);
    else if ( (textFlags&(u8)Action::Flags::AlwaysDisplay) == (u8)Action::Flags::AlwaysDisplay )
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

bool TextTrigGenerator::CorrectLineEndings(buffer& buf)
{
    u8 curr;
    u32 pos = 0;
    buffer dest(buf.title());
    dest.setSize(buf.size());

    while ( pos < buf.size() ) 
    {
        curr = buf.get<u8>(pos);
        switch ( curr )
        {
            case '\15': // CR (line ending)
                if ( buf.get<u8>(pos+1) == '\12' ) // Has LF
                    pos ++;
            case '\12': // LF (line ending)
            case '\13': // VT (line ending)
            case '\14': // FF (line ending)
                dest.add<u8>('\15');
                dest.add<u8>('\12');
                pos ++;
                break;
            default:
                dest.add<u8>(curr);
                pos ++;
                break;
        }

    }

    buf.takeAllData(dest);
    return true;
}

// private

bool TextTrigGenerator::PrepConditionTable()
{
    if ( goodConditionTable )
        return true;

    const char* legacyConditionNames[] = { "Always", "Countdown Timer", "Command", "Bring", "Accumulate", "Kill", "Command the Most", 
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
    
    ChkLocation* loc;
    ChkdString locationName(true);

    locationTable.push_back(ChkdString("No Location", true));

    if ( map->HasLocationSection() && map->hasStrSection(false) )
    {
        int numLocations = map->locationCapacity();
        for ( int i=0; i<numLocations; i++ )
        {
            if ( map->getLocation(loc, i) )
            {
                locationName = "";

                if ( i == 63 )
                {
                    if ( quoteArgs )
                        locationName = "\"Anywhere\"";
                    else
                        locationName = "Anywhere";

                    locationTable.push_back( locationName );
                }
                else if ( loc->stringNum > 0 && map->GetString(locationName, loc->stringNum) )
                {
                    if ( quoteArgs )
                        locationTable.push_back( "\"" + locationName + "\"" );
                    else
                        locationTable.push_back(locationName);
                }
            }

            if ( locationTable.size() == i+1 )
                locationTable.push_back(std::to_string(i + 1));
        }
    }
    return true;
}

bool TextTrigGenerator::PrepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames)
{
    unitTable.clear();

    ChkdString unitName(true);
    if ( map->hasUnitSettingsSection() && map->hasStrSection(false) )
    {
        for ( int unitID=0; unitID<=232; unitID++ )
        {
            if ( quoteArgs )
            {
                if ( useCustomNames && unitID < 228 )
                {
                    ChkdString unquotedName;
                    map->getUnitName(unquotedName, unitID);
                    unitName = "\"" + unquotedName + "\"";
                }
                else
                    unitName = ChkdString("\"" + std::string(LegacyTextTrigDisplayName[unitID]) + "\"");
            }
            else
            {
                if ( useCustomNames && unitID < 228 )
                    map->getUnitName(unitName, unitID);
                else
                    unitName = ChkdString(LegacyTextTrigDisplayName[unitID]);
            }

            unitTable.push_back( unitName );
        }
    }
    return true;
}

bool TextTrigGenerator::PrepSwitchTable(ScenarioPtr map, bool quoteArgs)
{
    switchTable.clear();

    ChkdString switchName(true);
    if ( map->hasSwitchSection() && map->hasStrSection(false) )
    {
        u32 stringID = 0;
        for ( u32 switchID=0; switchID<256; switchID++ )
        {
            if ( map->getSwitchStrId((u8)switchID, stringID) &&
                 stringID > 0 &&
                 map->GetString(switchName, stringID) )
            {
                if ( quoteArgs )
                    switchTable.push_back( "\"" + switchName + "\"" );
                else
                    switchTable.push_back(switchName);
            }
            else
            {
                if ( quoteArgs )
                    switchName = "\"Switch" + std::to_string(switchID + 1) + "\"";
                else
                    switchName = "Switch" + std::to_string(switchID + 1);

                switchTable.push_back( switchName );
            }
        }
    }
    return true;
}

bool TextTrigGenerator::PrepGroupTable(ScenarioPtr map, bool quoteArgs)
{
    groupTable.clear();

    ChkdString groupName(true);
    bool hasForcStrings = map->hasForceSection() && map->hasStrSection(false);

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
        DebugIf( i == 3 );
        if ( hasForcStrings && map->getForceString(groupName, i) )
        {
            if ( quoteArgs )
                groupName = "\"" + groupName + "\"";

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

    Trigger* trigPtr = nullptr;
    u32 numTrigs = map->numTriggers();
    for ( u32 i = 0; i < numTrigs; i++ )
    {
        if ( map->getTrigger(trigPtr, i) )
        {
            for ( u8 actionNum = 0; actionNum < NUM_TRIG_ACTIONS; actionNum++ )
            {
                Action &action = trigPtr->action(actionNum);
                ActionId actionId = (ActionId)action.action;
                bool isScriptAction = (actionId == ActionId::RunAiScript || actionId == ActionId::RunAiScriptAtLocation);
                if ( isScriptAction && action.number != 0 )
                {
                    char numberString[5] = {};
                    (u32&)numberString = action.number;
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

    if ( map->hasStrSection(false) || map->hasStrSection(true) )
    {
        StringUsageTable standardStringUsage;
        StringUsageTable extendedStringUsage;
        if ( standardStringUsage.populateTable(map.get(), false) || extendedStringUsage.populateTable(map.get(), true) )
        {
            ChkdString str(true);

            u32 numStrings = standardStringUsage.numStrings();
            for ( u32 i=0; i<numStrings; i++ )
            {
                if ( standardStringUsage.isUsed(i) )
                    map->GetString(str, i);

                ChkdString newString(true);
                for ( auto &character : str )
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

            numStrings = extendedStringUsage.numStrings();
            for ( u32 i=0; i<numStrings; i++ )
            {
                if ( extendedStringUsage.isUsed(i) )
                    map->GetString(str, (65536-i));

                ChkdString newString(true);
                for ( auto &character : str )
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

            return true;
        }
        else
            return false;
    }
    else
        return false;
}
