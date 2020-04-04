#include "TextTrigGenerator.h"
#include "Math.h"
#include <string>
#include <chrono>

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

TextTrigGenerator::TextTrigGenerator(bool useAddressesForMemory, u32 deathTableOffset) :
    goodConditionTable(false), goodActionTable(false), useAddressesForMemory(useAddressesForMemory), deathTableOffset(deathTableOffset)
{

}

TextTrigGenerator::~TextTrigGenerator()
{

}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, std::string & trigString)
{
    logger.info() << "Starting text trig generation..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    if ( map != nullptr &&
        LoadScenario(map, true, false) &&
        BuildTextTrigs(map, trigString) )
    {
        auto finish = std::chrono::high_resolution_clock::now();
        logger.info() << "Text trig generation completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
        return true;
    }
    return false;
}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, size_t trigIndex, std::string & trigString)
{
    if ( map != nullptr )
    {
        Chk::TriggerPtr trig = map->triggers.getTrigger(trigIndex);
        if ( trig != nullptr )
            return LoadScenario(map, true, false) && BuildTextTrig(*trig, trigString);
    }
    return false;
}

bool TextTrigGenerator::LoadScenario(ScenarioPtr map)
{
    return map != nullptr &&
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

std::string TextTrigGenerator::GetConditionName(Chk::Condition::Type conditionType)
{
    if ( size_t(conditionType) < conditionTable.size() )
        return conditionTable[conditionType];
    else
        return std::to_string((int)conditionType);
}

std::string TextTrigGenerator::GetConditionArgument(Chk::Condition & condition, size_t textArgumentIndex)
{
    StringBuffer output;
    appendConditionArgument(output, condition, Chk::Condition::getTextArg(condition.conditionType, textArgumentIndex));
    return output.str();
}

std::string TextTrigGenerator::GetConditionArgument(Chk::Condition & condition, Chk::Condition::Argument argument)
{
    StringBuffer output;
    appendConditionArgument(output, condition, argument);
    return output.str();
}

std::string TextTrigGenerator::GetActionName(Chk::Action::Type actionType)
{
    if ( size_t(actionType) < actionTable.size() )
        return actionTable[actionType];
    else
        return std::to_string((int)actionType);
}

std::string TextTrigGenerator::GetActionArgument(Chk::Action & action, size_t textArgumentIndex)
{
    StringBuffer output;
    appendActionArgument(output, action, Chk::Action::getTextArg(action.actionType, textArgumentIndex));
    return output.str();
}

std::string TextTrigGenerator::GetActionArgument(Chk::Action & action, Chk::Action::Argument argument)
{
    StringBuffer output;
    appendActionArgument(output, action, argument);
    return output.str();
}

ChkdString TextTrigGenerator::GetTrigLocation(size_t locationId)
{
    if ( locationId > 0 && locationId < locationTable.size() )
        return SingleLineChkdString(locationTable[locationId]);
    else
        return ChkdString(std::to_string(locationId));
}

ChkdString TextTrigGenerator::GetTrigString(size_t stringId)
{
    if ( stringId == Chk::StringId::NoString )
        return SingleLineChkdString("No String");
    else if ( stringId > 0 && (stringId < stringTable.size() || (65536 - stringId) < extendedStringTable.size()) )
    {
        if ( stringId < stringTable.size() )
            return (std::string &)stringTable[stringId];
        else
            return SingleLineChkdString("k" + extendedStringTable[65536 - stringId]);
    }
    else
        return ChkdString(std::to_string(stringId));
}

ChkdString TextTrigGenerator::GetTrigWav(size_t stringId)
{
    if ( stringId == 0 )
        return ChkdString("No WAV");
    else
        return GetTrigString(stringId);
}

ChkdString TextTrigGenerator::GetTrigPlayer(size_t playerIndex)
{
    if ( playerIndex < groupTable.size() )
        return groupTable[playerIndex].c_str();
    else
        return ChkdString(std::to_string(playerIndex));
}

ChkdString TextTrigGenerator::GetTrigUnit(Sc::Unit::Type unitType)
{
    if ( unitType < unitTable.size() )
        return unitTable[unitType];
    else
        return ChkdString(std::to_string(unitType));
}

ChkdString TextTrigGenerator::GetTrigSwitch(size_t switchIndex)
{
    if ( switchIndex < switchTable.size() )
        return switchTable[switchIndex];
    else
        return ChkdString(std::to_string(switchIndex));
}

std::string TextTrigGenerator::GetTrigScoreType(Chk::Trigger::ScoreType scoreType)
{
    const char* scoreTypes[] = { "total", "units", "buildings", "units and buildings", "kills", "razings", "kills and razings", "custom" };
    if ( scoreType < sizeof(scoreTypes) / sizeof(const char*) )
        return scoreTypes[scoreType];
    else
        return std::to_string((int)scoreType);
}

std::string TextTrigGenerator::GetTrigResourceType(Chk::Trigger::ResourceType resourceType)
{
    const char* resourceTypes[] = { "ore", "gas", "ore and gas" };
    if ( resourceType < sizeof(resourceTypes) / sizeof(const char*) )
        return resourceTypes[resourceType];
    else
        return std::to_string((int)resourceType);
}

std::string TextTrigGenerator::GetTrigOrder(Chk::Action::Order order)
{
    const char* orderTypes[] = { "move", "patrol", "attack" };
    if ( order < sizeof(orderTypes) / sizeof(const char*) )
        return orderTypes[order];
    else
        return std::to_string((int)order);
}

std::string TextTrigGenerator::GetTrigStateModifier(Chk::Trigger::ValueModifier stateModifier)
{
    const char* stateModifiers[] = { "0", "1", "2", "3", "Enable", "Disable", "Toggle" };
    if ( stateModifier < sizeof(stateModifiers) / sizeof(const char*) )
        return std::string(stateModifiers[stateModifier]);
    else
        return std::to_string((int)stateModifier);
}

std::string TextTrigGenerator::GetTrigSwitchState(Chk::Trigger::ValueModifier switchState)
{
    const char* switchStates[] = { "0", "1", "Set", "Cleared" };
    if ( switchState < sizeof(switchStates) / sizeof(const char*) )
        return std::string(switchStates[switchState]);
    else
        return std::to_string((int)switchState);
}

std::string TextTrigGenerator::GetTrigSwitchModifier(Chk::Trigger::ValueModifier switchModifier)
{
    const char* switchModifiers[] = { "0", "1", "2", "3", "Set", "Clear", "Toggle", "7", "8", "9", "10", "Randomize" };
    if ( switchModifier < sizeof(switchModifiers) / sizeof(const char*) )
        return std::string(switchModifiers[switchModifier]);
    else
        return std::to_string((int)switchModifier);
}

std::string TextTrigGenerator::GetTrigAllyState(Chk::Action::AllianceStatus allyState)
{
    const char* allyStates[] = { "Enemy", "Ally", "Allied Victory" };
    if ( allyState < sizeof(allyStates) / sizeof(const char*) )
        return std::string(allyStates[allyState]);
    else
        return std::to_string(allyState);
}

std::string TextTrigGenerator::GetTrigNumericComparison(Chk::Condition::Comparison numericComparison)
{
    const char* numericComparisons[] = { "at least", "at most", "2", "3", "4", "5", "6", "7", "8", "9", "exactly" };
    if ( numericComparison < sizeof(numericComparisons) / sizeof(const char*) )
        return std::string(numericComparisons[numericComparison]);
    else
        return std::to_string((int)numericComparison);
}

std::string TextTrigGenerator::GetTrigNumericModifier(Chk::Trigger::ValueModifier numericModifier)
{
    const char* numericModifiers[] = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };
    if ( numericModifier < sizeof(numericModifiers) / sizeof(const char*) )
        return std::string(numericModifiers[numericModifier]);
    else
        return std::to_string((int)numericModifier);
}

std::string TextTrigGenerator::GetTrigScript(Sc::Ai::ScriptId scriptId)
{
    if ( scriptId == Sc::Ai::ScriptId::NoScript )
        return std::string("No Script");

    char script[5];
    (u32 &)script[0] = (u32)scriptId;
    script[4] = '\0';

    return std::string(script);
}

std::string TextTrigGenerator::GetTrigNumUnits(Chk::Action::NumUnits numUnits)
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

std::string TextTrigGenerator::GetTrigTextFlags(Chk::Action::Flags textFlags)
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
    size_t pos = 0;
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

bool TextTrigGenerator::BuildTextTrigs(ScenarioPtr scenario, std::string & trigString)
{
    StringBuffer output;
    appendTriggers(output, scenario);
    CorrectLineEndings(output);
    trigString = output.str();
    ClearScenario();
    return true;
}

bool TextTrigGenerator::BuildTextTrig(Chk::Trigger & trigger, std::string & trigString)
{
    StringBuffer output;
    appendTrigger(output, trigger);
    CorrectLineEndings(output);
    trigString += output.str();
    ClearScenario();
    return true;
}

inline void TextTrigGenerator::appendTriggers(StringBuffer & output, ScenarioPtr scenario)
{
    Triggers & triggers = scenario->triggers;
    size_t numTrigs = triggers.numTriggers();
    for ( size_t trigIndex=0; trigIndex<numTrigs; trigIndex++ )
    {
        std::shared_ptr<Chk::Trigger> trigger = triggers.getTrigger(trigIndex);
        if ( trigger != nullptr )
            appendTrigger(output, *trigger);
    }
}

inline void TextTrigGenerator::appendTrigger(StringBuffer & output, Chk::Trigger & trigger)
{
    output += "Trigger(";

    // Add players
    bool hasPrevious = false;
    for ( size_t groupNum=0; groupNum<Chk::Trigger::MaxOwners; groupNum++ )
    {
        if ( trigger.owned(groupNum) == Chk::Trigger::Owned::Yes )
        {
            if ( hasPrevious )
                output += ',';
            else
                hasPrevious = true;

            EscString groupName = groupTable[groupNum];
            output += (std::string &)groupName;
        }
        else if ( trigger.owned(groupNum) != Chk::Trigger::Owned::No )
        {
            if ( hasPrevious )
                output += ',';
            else
                hasPrevious = true;

            output += (std::string &)groupTable[groupNum];
            output += ':';
            output += std::to_string((int)trigger.owned(groupNum));
        }
    }

    output += "){\nConditions:";

    // Add conditions
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
    {
        Chk::Condition & condition = trigger.condition(i);
        Chk::Condition::VirtualType conditionType = (Chk::Condition::VirtualType)condition.conditionType;

        if ( conditionType != Chk::Condition::VirtualType::NoCondition )
        {
            if ( (condition.flags & Chk::Condition::Flags::Disabled) == Chk::Condition::Flags::Disabled )
                output += "\n;\t";
            else
                output += "\n\t";

            // Add condition name
            if ( conditionType == Chk::Condition::VirtualType::Deaths && condition.player > 28 ) // Memory condition
                output += "Memory";
            else if ( conditionType >= 0 && (size_t)conditionType < conditionTable.size() )
                output += conditionTable[conditionType];
            else
                output += "Custom";

            output += '(';
            // Add condition args
            if ( conditionType == Chk::Condition::VirtualType::Deaths && condition.player > 28 ) // Memory condition
                conditionType = Chk::Condition::VirtualType::Memory;

            for ( size_t argumentIndex=0; argumentIndex<Chk::Condition::MaxArguments; argumentIndex++ )
            {
                const Chk::Condition::Argument & argument = Chk::Condition::getTextArg(conditionType, argumentIndex);
                if ( argument.type == Chk::Condition::ArgType::NoType )
                    break;
                else
                {
                    if ( argumentIndex > 0 )
                        output += ", ";

                    appendConditionArgument(output, condition, argument);
                }
            }

            output += ");";
        }
    }

    output += "\n\nActions:";

    // Add actions
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        Chk::Action & action = trigger.action(i);
        Chk::Action::VirtualType actionType = (Chk::Action::VirtualType)action.actionType;

        if ( actionType != Chk::Action::VirtualType::NoAction )
        {
            if ( (action.flags&Chk::Action::Flags::Disabled) == Chk::Action::Flags::Disabled )
                output += "\n;\t";
            else
                output += "\n\t";

            // Add action name
            if ( actionType == Chk::Action::VirtualType::SetDeaths && action.group > 28 ) // Memory action
                output += "Set Memory";
            else if ( actionType >= 0 && (size_t)actionType < actionTable.size() )
                output += actionTable[actionType];
            else
                output += "Custom";

            output += '(';
            // Add action args
            if ( actionType == Chk::Action::VirtualType::SetDeaths && action.group > 28 ) // Memory action
                actionType = Chk::Action::VirtualType::SetMemory;

            for ( size_t argumentIndex=0; argumentIndex<Chk::Action::MaxArguments; argumentIndex++ )
            {
                const Chk::Action::Argument & argument = Chk::Action::getTextArg(actionType, argumentIndex);
                if ( argument.type == Chk::Action::ArgType::NoType )
                    break;
                else
                {
                    if ( argumentIndex > 0 )
                        output += ", ";

                    appendActionArgument(output, action, argument);
                }
            }

            output += ");";
        }
    }

    // Add Flags
    if ( trigger.flags > 0 )
    {
        output += "\n\nFlags:\n";
        char number[36];
        _itoa_s(trigger.flags, number, 36, 2); // TODO: FIXME
        size_t length = std::strlen(number);
        output += std::string(32-length, '0');
        output += std::string(number);
        output += ';';
    }

    output += "\n}\n\n//-----------------------------------------------------------------//\n\n";
}

inline void TextTrigGenerator::appendConditionArgument(StringBuffer & output, Chk::Condition & condition, Chk::Condition::Argument argument)
{
    switch ( argument.type )
    {
        case Chk::Condition::ArgType::Unit: appendUnit(output, condition.unitType); break;
        case Chk::Condition::ArgType::Location: appendLocation(output, condition.locationId); break;
        case Chk::Condition::ArgType::Player: appendPlayer(output, condition.player); break;
        case Chk::Condition::ArgType::Amount: appendAmount(output, condition.amount); break;
        case Chk::Condition::ArgType::Comparison:
        case Chk::Condition::ArgType::NumericComparison: appendNumericComparison(output, condition.comparison); break;
        case Chk::Condition::ArgType::ResourceType: appendResourceType(output, (Chk::Trigger::ResourceType)condition.typeIndex); break;
        case Chk::Condition::ArgType::ScoreType: appendScoreType(output, (Chk::Trigger::ScoreType)condition.typeIndex); break;
        case Chk::Condition::ArgType::Switch: appendSwitch(output, condition.typeIndex); break;
        case Chk::Condition::ArgType::SwitchState: appendSwitchState(output, (Chk::Trigger::ValueModifier)condition.comparison); break;
        case Chk::Condition::ArgType::ConditionType: appendNumber(output, condition.conditionType); break;
        case Chk::Condition::ArgType::TypeIndex: appendNumber(output, condition.typeIndex); break;
        case Chk::Condition::ArgType::Flags: appendNumber(output, condition.flags); break;
        case Chk::Condition::ArgType::MaskFlag: appendConditionMaskFlag(output, condition.maskFlag); break;
        case Chk::Condition::ArgType::MemoryOffset: appendMemory(output, condition.player); break;
    }
}

inline void TextTrigGenerator::appendActionArgument(StringBuffer & output, Chk::Action & action, Chk::Action::Argument argument)
{
    switch ( argument.type )
    {
        case Chk::Action::ArgType::Location:
            if ( argument.field == Chk::Action::ArgField::Number ) {
                appendLocation(output, action.number);
            } else {
                appendLocation(output, action.locationId);
            } break;
        case Chk::Action::ArgType::String: appendString(output, action.stringId); break;
        case Chk::Action::ArgType::Player:
            if ( argument.field == Chk::Action::ArgField::Number ) {
                appendPlayer(output, action.number);
            } else {
                appendPlayer(output, action.group);
            } break;
        case Chk::Action::ArgType::Unit: appendUnit(output, (Sc::Unit::Type)action.type); break;
        case Chk::Action::ArgType::NumUnits: appendNumUnits(output, (Chk::Action::NumUnits)action.type2); break;
        case Chk::Action::ArgType::CUWP: appendNumber(output, action.number); break;
        case Chk::Action::ArgType::TextFlags: appendTextFlags(output, (Chk::Action::Flags)action.flags); break;
        case Chk::Action::ArgType::Amount:
            if ( argument.field == Chk::Action::ArgField::Type2 ) {
                appendNumber(output, action.type2);
            } else {
                appendNumber(output, action.number);
            } break;
        case Chk::Action::ArgType::ScoreType: appendScoreType(output, (Chk::Trigger::ScoreType)action.type); break;
        case Chk::Action::ArgType::ResourceType: appendResourceType(output, (Chk::Trigger::ResourceType)action.type); break;
        case Chk::Action::ArgType::StateMod: appendStateModifier(output, (Chk::Trigger::ValueModifier)action.type2); break;
        case Chk::Action::ArgType::Percent: appendNumber(output, action.number); break;
        case Chk::Action::ArgType::Order: appendOrder(output, (Chk::Action::Order)action.type2); break;
        case Chk::Action::ArgType::Sound: appendSound(output, action.soundStringId); break;
        case Chk::Action::ArgType::Duration: appendNumber(output, action.time); break;
        case Chk::Action::ArgType::Script: appendScript(output, (Sc::Ai::ScriptId)action.number); break;
        case Chk::Action::ArgType::AllyState: appendAllyState(output, (Chk::Action::AllianceStatus)action.type); break;
        case Chk::Action::ArgType::NumericMod: appendNumericModifier(output, (Chk::Trigger::ValueModifier)action.type2); break;
        case Chk::Action::ArgType::Switch: appendSwitch(output, action.number); break;
        case Chk::Action::ArgType::SwitchMod: appendSwitchModifier(output, (Chk::Trigger::ValueModifier)action.type2); break;
        case Chk::Action::ArgType::ActionType: appendNumber(output, action.actionType); break;
        case Chk::Action::ArgType::Number: appendNumber(output, action.number); break;
        case Chk::Action::ArgType::TypeIndex: appendNumber(output, action.type); break;
        case Chk::Action::ArgType::SecondaryTypeIndex: appendNumber(output, action.type2); break;
        case Chk::Action::ArgType::Flags: appendNumber(output, action.flags); break;
        case Chk::Action::ArgType::Padding: appendNumber(output, action.padding); break;
        case Chk::Action::ArgType::MaskFlag: appendActionMaskFlag(output, action.maskFlag); break;
        case Chk::Action::ArgType::MemoryOffset: appendMemory(output, action.group); break;
    }
}

inline void TextTrigGenerator::appendLocation(StringBuffer & output, const size_t & locationId)
{
    if ( locationId < locationTable.size() )
        output += (std::string &)locationTable[locationId];
    else
        output += std::to_string(locationId);
}

inline void TextTrigGenerator::appendString(StringBuffer & output, const size_t & stringId)
{
    if ( stringId == 0 )
        output += "No String";
    else if ( stringId < stringTable.size() || (65536-stringId) < extendedStringTable.size() )
    {
        if ( stringId < stringTable.size() )
            output += (std::string &)stringTable[stringId];
        else
            output += std::string("k" + extendedStringTable[65536-stringId]);
    }
    else
        output += std::to_string(stringId);
}

inline void TextTrigGenerator::appendSound(StringBuffer & output, const size_t & stringId)
{
    if ( stringId == 0 )
        output += "No WAV";
    else if ( stringId < stringTable.size() || (65536 - stringId) < extendedStringTable.size() )
    {
        if ( stringId < stringTable.size() )
            output += (std::string &)stringTable[stringId];
        else
            output += std::string("k" + extendedStringTable[65536 - stringId]);
    }
    else
        output += std::to_string(stringId);
}

inline void TextTrigGenerator::appendPlayer(StringBuffer & output, const size_t & playerIndex)
{
    if ( playerIndex < groupTable.size() )
        output += (std::string &)groupTable[playerIndex];
    else
        output += std::to_string(playerIndex);
}

inline void TextTrigGenerator::appendAmount(StringBuffer & output, const u32 & amount)
{
    output << amount;
}

inline void TextTrigGenerator::appendUnit(StringBuffer & output, const Sc::Unit::Type & unitType)
{
    if ( size_t(unitType) < unitTable.size() )
        output += (std::string &)unitTable[unitType];
    else
        output += std::to_string(unitType);
}

inline void TextTrigGenerator::appendSwitch(StringBuffer & output, const size_t & switchIndex)
{
    if ( switchIndex < switchTable.size() )
        output += (std::string &)switchTable[switchIndex];
    else
        output += std::to_string(switchIndex);
}

inline void TextTrigGenerator::appendScoreType(StringBuffer & output, const Chk::Trigger::ScoreType & scoreType)
{
    if ( size_t(scoreType) < scoreTypes.size() )
        output += scoreTypes[scoreType];
    else
        output += std::to_string((int)scoreType);
}

inline void TextTrigGenerator::appendResourceType(StringBuffer & output, const Chk::Trigger::ResourceType & resourceType)
{
    if ( size_t(resourceType) < resourceTypes.size() )
        output += resourceTypes[resourceType];
    else
        output += std::to_string((int)resourceType);
}

inline void TextTrigGenerator::appendOrder(StringBuffer & output, const Chk::Action::Order & order)
{
    if ( size_t(order) < orderTypes.size() )
        output += orderTypes[order];
    else
        output += std::to_string((int)order);
}

inline void TextTrigGenerator::appendStateModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & stateModifier)
{
    if ( size_t(stateModifier) < stateModifiers.size() )
        output += stateModifiers[stateModifier];
    else
        output += std::to_string((int)stateModifier);
}

inline void TextTrigGenerator::appendSwitchState(StringBuffer & output, const Chk::Trigger::ValueModifier & switchState)
{
    if ( size_t(switchState) < switchStates.size() )
        output += switchStates[switchState];
    else
        output += std::to_string((int)switchState);
}

inline void TextTrigGenerator::appendSwitchModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & switchModifier)
{
    if ( size_t(switchModifier) < switchModifiers.size() )
        output += switchModifiers[switchModifier];
    else
        output += std::to_string((int)switchModifier);
}

inline void TextTrigGenerator::appendAllyState(StringBuffer & output, const Chk::Action::AllianceStatus & allyState)
{
    if ( size_t(allyState) < allyStates.size() )
        output += allyStates[allyState];
    else
        output += std::to_string(allyState);
}

inline void TextTrigGenerator::appendNumericComparison(StringBuffer & output, const Chk::Condition::Comparison & numericComparison)
{
    if ( size_t(numericComparison) < numericComparisons.size() )
        output += numericComparisons[numericComparison];
    else
        output += std::to_string((int)numericComparison);
}

inline void TextTrigGenerator::appendNumericModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & numericModifier)
{
    if ( size_t(numericModifier) < numericModifiers.size() )
        output += numericModifiers[numericModifier];
    else
        output += std::to_string((int)numericModifier);
}

inline void TextTrigGenerator::appendScript(StringBuffer & output, const Sc::Ai::ScriptId & scriptId)
{
    if ( scriptId == Sc::Ai::ScriptId::NoScript )
        output += "No Script";
    else
    {
        auto it = scriptTable.find(scriptId);
        if ( it != scriptTable.end() )
            output += it->second;
        else
        {
            output += '\"';
            output += std::string(((char*)&scriptId)[0], ((char*)&scriptId)[4]);
            output += '\"';
        }
    }
}

inline void TextTrigGenerator::appendNumUnits(StringBuffer & output, const Chk::Action::NumUnits & numUnits)
{
    if ( numUnits == 0 )
        output += "All";
    else
        output += std::to_string((int)numUnits);
}

inline void TextTrigGenerator::appendConditionMaskFlag(StringBuffer & output, const Chk::Condition::MaskFlag & maskFlag)
{
    if ( maskFlag == Chk::Condition::MaskFlag::Enabled )
        output += "Enabled";
    else if ( maskFlag == Chk::Condition::MaskFlag::Disabled )
        output += "Disabled";
    else
        output += std::to_string(maskFlag);
}

inline void TextTrigGenerator::appendActionMaskFlag(StringBuffer & output, const Chk::Action::MaskFlag & maskFlag)
{
    if ( maskFlag == Chk::Action::MaskFlag::Enabled )
        output += "Enabled";
    else if ( maskFlag == Chk::Action::MaskFlag::Disabled )
        output += "Disabled";
    else
        output += std::to_string(maskFlag);
}

inline void TextTrigGenerator::appendMemory(StringBuffer & output, const u32 & memory)
{
    if ( useAddressesForMemory )
        output += to_hex_string(memory*4+deathTableOffset);
    else
        output += std::to_string(memory);
}

inline void TextTrigGenerator::appendTextFlags(StringBuffer & output, const Chk::Action::Flags & textFlags)
{
    if ( (textFlags&Chk::Action::Flags::AlwaysDisplay) == 0 )
        output += "Don't Always Display";
    else if ( (textFlags&Chk::Action::Flags::AlwaysDisplay) == Chk::Action::Flags::AlwaysDisplay )
        output += "Always Display";
    else
        output += std::to_string((int)textFlags);
}

// private

bool TextTrigGenerator::PrepConditionTable()
{
    if ( goodConditionTable )
        return true;

    const char* legacyConditionNames[] = { "No Condition", "Countdown Timer", "Command", "Bring", "Accumulate", "Kill", "Command the Most", 
                                           "Commands the Most At", "Most Kills", "Highest Score", "Most Resources", "Switch", "Elapsed Time", 
                                           "Never", "Opponents", "Deaths", "Command the Least", "Command the Least At", "Least Kills", 
                                           "Lowest Score", "Least Resources", "Score", "Always", "Never" };

    const char** conditionNames = legacyConditionNames;

    for ( size_t i=0; i<sizeof(legacyConditionNames)/sizeof(const char*); i++ )
        conditionTable.push_back(std::string(conditionNames[i]));

    std::string custom("Custom");
    for ( size_t i=sizeof(legacyConditionNames)/sizeof(const char*); i<256; i++ )
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

    for ( size_t i=0; i<sizeof(legacyActionNames)/sizeof(const char*); i++ )
        actionTable.push_back(std::string(actionNames[i]));

    std::string custom("Custom");
    for ( size_t i=sizeof(legacyActionNames)/sizeof(const char*); i<256; i++ )
        actionTable.push_back(custom);

    goodActionTable = true;
    return true;
}

bool TextTrigGenerator::PrepLocationTable(ScenarioPtr map, bool quoteArgs)
{
    locationTable.clear();
    locationTable.push_back(EscString("No Location"));

    size_t numLocations = map->layers.numLocations();
    for ( size_t i=1; i<=numLocations; i++ )
    {
        Chk::LocationPtr loc = map->layers.getLocation(i);
        if ( loc != nullptr )
        {
            if ( i == Chk::LocationId::Anywhere )
            {
                EscString locationName = quoteArgs ? "\"Anywhere\"" : "Anywhere";
                locationTable.push_back(locationName);
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

    for ( size_t unitType=0; unitType<Sc::Unit::TotalReferenceTypes; unitType++ )
    {
        EscStringPtr unitName = nullptr;
        if ( quoteArgs )
        {
            if ( useCustomNames && unitType < 228 )
            {
                EscStringPtr unquotedName = map->strings.getUnitName<EscString>((Sc::Unit::Type)unitType, false);
                unitName = EscStringPtr(new EscString("\"" + *unquotedName + "\""));
            }
            if ( unitName == nullptr )
                unitName = EscStringPtr(new EscString("\"" + std::string(Sc::Unit::legacyTextTrigDisplayNames[unitType]) + "\""));
        }
        else
        {
            if ( useCustomNames && unitType < 228 )
                unitName = map->strings.getUnitName<EscString>((Sc::Unit::Type)unitType, false);
            if ( unitName == nullptr )
                unitName = EscStringPtr(new EscString(Sc::Unit::legacyTextTrigDisplayNames[unitType]));
        }

        unitTable.push_back(*unitName);
    }
    return true;
}

bool TextTrigGenerator::PrepSwitchTable(ScenarioPtr map, bool quoteArgs)
{
    switchTable.clear();

    for ( size_t switchIndex=0; switchIndex<Chk::TotalSwitches; switchIndex++ )
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

    EscString groupName;

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

    for ( size_t i=0; i<18; i++ )
    {
        groupName = lowerGroups[i];
        groupTable.push_back( groupName );
    }

    for ( size_t i=0; i<4; i++ )
    {
        EscStringPtr forceName = map->strings.getForceName<EscString>((Chk::Force)i);
        if ( forceName != nullptr )
        {
            if ( quoteArgs )
                groupTable.push_back("\"" + *forceName + "\"");
            else
                groupTable.push_back(*forceName);

        }
        else
        {
            if ( quoteArgs )
                groupTable.push_back("\"Force " + std::to_string(i) + "\"");
            else
                groupTable.push_back("Force " + std::to_string(i));
        }
    }

    for ( size_t i=22; i<28; i++ )
    {
        groupName = upperGroups[i-22];
        groupTable.push_back( groupName );
    }

    return true;
}

bool TextTrigGenerator::PrepScriptTable(ScenarioPtr map, bool quoteArgs)
{
    scriptTable.clear();

    scriptTable.insert(std::pair<Sc::Ai::ScriptId, std::string>(Sc::Ai::ScriptId::NoScript, "No Script"));

    Chk::Trigger* trigPtr = nullptr;
    size_t numTrigs = map->triggers.numTriggers();
    for ( size_t i = 0; i < numTrigs; i++ )
    {
        Chk::TriggerPtr trigPtr = map->triggers.getTrigger(i);
        if ( trigPtr != nullptr )
        {
            for ( size_t actionNum = 0; actionNum < Chk::Trigger::MaxActions; actionNum++ )
            {
                Chk::Action & action = trigPtr->action(actionNum);
                Chk::Action::Type actionId = action.actionType;
                bool isScriptAction = (actionId == Chk::Action::Type::RunAiScript || actionId == Chk::Action::Type::RunAiScriptAtLocation);
                if ( isScriptAction && action.number != 0 )
                {
                    char numberString[5] = {};
                    (u32 &)numberString = action.number;
                    for ( size_t i = 0; i < 4; i++ )
                    {
                        if ( numberString[i] == '\0' )
                            numberString[i] = ' ';
                    }

                    if ( quoteArgs )
                    {
                        scriptTable.insert(std::pair<Sc::Ai::ScriptId, std::string>((Sc::Ai::ScriptId)action.number,
                            std::string("\"" + std::string(numberString) + "\"")));
                    }
                    else
                        scriptTable.insert(std::pair<Sc::Ai::ScriptId, std::string>((Sc::Ai::ScriptId)action.number, std::string(numberString)));
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
    EscStringPtr str;

    size_t numStrings = map->strings.getCapacity(Chk::Scope::Game);
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( stringUsed[i] )
            str = map->strings.getString<EscString>(i);

        if ( str != nullptr )
        {
            EscString newString;
            for ( auto & character : *str )
                newString.push_back(character);

            if ( quoteArgs )
                stringTable.push_back( "\"" + newString + "\"" );
            else
                stringTable.push_back(newString);
        }
        else
            stringTable.push_back("");
    }

    numStrings = map->strings.getCapacity(Chk::Scope::Editor);
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( extendedStringUsed[i] )
            str = map->strings.getString<EscString>(i);
        
        if ( str != nullptr )
        {
            EscString newString;
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
        else
            stringTable.push_back("");
    }

    return true;
}
