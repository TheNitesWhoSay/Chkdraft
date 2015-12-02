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
	#define ADD_TEXTTRIG_LOCATION(src) {											\
		if ( src >= 0 && src < locationTable.size() )								\
			output.addStr(locationTable[src].c_str(), locationTable[src].size());	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_STRING(src) {																							\
		if ( src >= 0 && (src < stringTable.size() || (65536-src) < extendedStringTable.size() ) )								\
		{																														\
			if ( src < stringTable.size() )																						\
				output.addStr(stringTable[src].c_str(), stringTable[src].size());												\
			else																												\
				output.addStr(std::string("k" + extendedStringTable[65536-src]).c_str(), extendedStringTable[65536-src].size()+1);	\
		}																														\
		else { _itoa_s(src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_PLAYER(src) {												\
		if ( src >= 0 && src < groupTable.size() )									\
			output.addStr(groupTable[src].c_str(), groupTable[src].size());			\
		else { _itoa_s(src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_UNIT(src) {												\
		if ( src >= 0 && src < unitTable.size() )									\
			output.addStr(unitTable[src].c_str(), unitTable[src].size());			\
		else { _itoa_s(src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_SWITCH(src) {												\
		if ( src >= 0 && src < switchTable.size() )									\
			output.addStr(switchTable[src].c_str(), switchTable[src].size());		\
		else { _itoa_s( src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_SCORE_TYPE(src) {											\
		if ( src >= 0 && src < sizeof(scoreTypes)/sizeof(const char*) )				\
			output.addStr(scoreTypes[src], std::strlen(scoreTypes[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_RESOURCE_TYPE(src) {										\
		if ( src >= 0 && src < sizeof(resourceTypes)/sizeof(const char*) )			\
			output.addStr(resourceTypes[src], std::strlen(resourceTypes[src]));			\
		else { _itoa_s(src, number, 10); output.addStr(number, std::strlen(number)); } }

	#define ADD_TEXTTRIG_ORDER(src) {												\
		if ( src >= 0 && src < sizeof(orderTypes)/sizeof(const char*) )				\
			output.addStr(orderTypes[src], strlen(orderTypes[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_STATE_MODIFIER(src) {										\
		if ( src >= 0 && src < sizeof(stateModifiers)/sizeof(const char*) )			\
			output.addStr(stateModifiers[src], strlen(stateModifiers[src]));		\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } } 

	#define ADD_TEXTTRIG_SWITCH_STATE(src) {										\
		if ( src >= 0 && src < sizeof(switchStates)/sizeof(const char*) )			\
			output.addStr(switchStates[src], strlen(switchStates[src]));			\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SWITCH_MODIFIER(src) {										\
		if ( src >= 0 && src < sizeof(switchModifiers)/sizeof(const char*) )		\
			output.addStr(switchModifiers[src], strlen(switchModifiers[src]));		\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_ALLY_STATE(src) {											\
		if ( src >= 0 && src < sizeof(allyStates)/sizeof(const char*) )				\
			output.addStr(allyStates[src], strlen(allyStates[src]));				\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMERIC_COMPARISON(src) {										\
		if ( src >= 0 && src < sizeof(numericComparisons)/sizeof(const char*) )			\
			output.addStr(numericComparisons[src], strlen(numericComparisons[src]));	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMERIC_MODIFIER(src) {									\
		if ( src >= 0 && src < sizeof(numericModifiers)/sizeof(const char*) )		\
			output.addStr(numericModifiers[src], strlen(numericModifiers[src]));	\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_SCRIPT(src) {												\
		output.add('\"'); output.addStr(src, 4); output.add('\"'); }

	#define ADD_TEXTTRIG_NUM_UNITS(src) {											\
		if ( src == 0 ) output.addStr("All", 3);									\
		else { _itoa_s(src, number, 10); output.addStr(number, strlen(number)); } }

	#define ADD_TEXTTRIG_NUMBER(src) {												\
		_itoa_s((int)src, number, 10); output.addStr(number, strlen(number)); }

	#define ADD_TEXTTRIG_TEXT_FLAGS(src) {											\
		if		( (src&ACTION_FLAG_ALWAYS_DISPLAY) == 0 )							\
			output.addStr(textFlags[0], strlen(textFlags[0]));						\
		else if ( (src&ACTION_FLAG_ALWAYS_DISPLAY) == ACTION_FLAG_ALWAYS_DISPLAY )	\
			output.addStr(textFlags[1], strlen(textFlags[1])); }
}

TextTrigGenerator::TextTrigGenerator() : goodConditionTable(false), goodActionTable(false)
{
	stringTable.clear();
	extendedStringTable.clear();
	locationTable.clear();
	unitTable.clear();
	switchTable.clear();
	wavTable.clear();
	groupTable.clear();
	conditionTable.clear();
	actionTable.clear();
}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, std::string &trigString)
{
	return this != nullptr && map != nullptr && GenerateTextTrigs(map, map->TRIG(), trigString);
}

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, u32 trigId, std::string &trigString)
{
	Trigger* trig;
	buffer trigBuff("TRIG");
	return this != nullptr &&
		   map != nullptr &&
		   map->getTrigger(trig, trigId) &&
		   trigBuff.add<Trigger>(*trig) &&
		   GenerateTextTrigs(map, trigBuff, trigString);
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
	wavTable.clear();
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
	AddConditionArgument(output, condition, condition.condition, stdTextTrigArgNum);
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
		AddConditionArgument(output, condition, condition.condition, stdTextTrigArgNum);
		if ( output.add('\0') )
			return std::string((char*)output.getPtr(0));
	}
	return "";
}

std::string TextTrigGenerator::GetActionArgument(Action& action, u8 stdTextTrigArgNum)
{
	buffer output("TEXA");
	AddActionArgument(output, action, action.action, stdTextTrigArgNum);
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
		AddActionArgument(output, action, action.action, stdTextTrigArgNum);
		if ( output.add('\0') )
			return std::string((char*)output.getPtr(0));
	}
	return "";
}

std::string TextTrigGenerator::GetTrigLocation(u32 locationNum)
{
	char number[12];
	if ( locationNum >= 0 && locationNum < locationTable.size() )
		return std::string(locationTable[locationNum]);
	else
	{
		_itoa_s(locationNum, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigString(u32 stringNum)
{
	char number[12];
	if ( stringNum >= 0 && (stringNum < stringTable.size() || (65536-stringNum) < extendedStringTable.size() ) )
	{
		if ( stringNum < stringTable.size() )
			return stringTable[stringNum];
		else
			return std::string("k" + extendedStringTable[65536-stringNum]);
	}
	else
	{
		_itoa_s(stringNum, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigPlayer(u32 groupNum)
{
	char number[12];
	if ( groupNum >= 0 && groupNum < groupTable.size() )
		return groupTable[groupNum].c_str();
	else
	{
		_itoa_s(groupNum, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigUnit(u16 unitId)
{
	char number[12];
	if ( unitId >= 0 && unitId < unitTable.size() )
		return unitTable[unitId];
	else
	{
		_itoa_s(unitId, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigSwitch(u32 switchNum)
{
	char number[12];
	if ( switchNum >= 0 && switchNum < switchTable.size() )
		return switchTable[switchNum];
	else
	{
		_itoa_s(switchNum, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigScoreType(u16 scoreType)
{
	char number[12];
	const char* scoreTypes[] = { "total", "units", "buildings", "units and buildings", "kills", "razings", "kills and razings", "custom" };
	if ( scoreType >= 0 && scoreType < sizeof(scoreTypes)/sizeof(const char*) )
		return scoreTypes[scoreType];
	else
	{
		_itoa_s(scoreType, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigResourceType(u16 resourceType)
{
	char number[12];
	const char* resourceTypes[] = { "ore", "gas", "ore and gas" };
	if ( resourceType >= 0 && resourceType < sizeof(resourceTypes)/sizeof(const char*) )
		return resourceTypes[resourceType];
	else
	{
		_itoa_s(resourceType, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigOrder(u8 order)
{
	char number[12];
	const char* orderTypes[] = { "move", "patrol", "attack" };
	if ( order >= 0 && order < sizeof(orderTypes)/sizeof(const char*) )
		return orderTypes[order];
	else
	{
		_itoa_s(order, number, 10);
		return number;
	}
}

std::string TextTrigGenerator::GetTrigStateModifier(u8 stateModifier)
{
	char number[12];
	const char* stateModifiers[] = { "0", "1", "2", "3", "Enable", "Disable", "Toggle" };
	if ( stateModifier >= 0 && stateModifier < sizeof(stateModifiers)/sizeof(const char*) )
		return std::string(stateModifiers[stateModifier]);
	else
	{
		_itoa_s(stateModifier, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigSwitchState(u8 switchState)
{
	char number[12];
	const char* switchStates[] = { "0", "1", "Set", "Cleared" };
	if ( switchState >= 0 && switchState < sizeof(switchStates)/sizeof(const char*) )
		return std::string(switchStates[switchState]);
	else
	{
		_itoa_s(switchState, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigSwitchModifier(u8 switchModifier)
{
	char number[12];
	const char* switchModifiers[] = { "0", "1", "2", "3", "Set", "Clear", "Toggle", "7", "8", "9", "10", "Randomize" };
	if ( switchModifier >= 0 && switchModifier < sizeof(switchModifiers)/sizeof(const char*) )
		return std::string(switchModifiers[switchModifier]);
	else
	{
		_itoa_s(switchModifier, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigAllyState(u16 allyState)
{
	char number[12];
	const char* allyStates[] = { "Enemy", "Ally", "Allied Victory" };
	if ( allyState >= 0 && allyState < sizeof(allyStates)/sizeof(const char*) )
		return std::string(allyStates[allyState]);
	else
	{
		_itoa_s(allyState, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigNumericComparison(u8 numericComparison)
{
	char number[12];
	const char* numericComparisons[] = { "at least", "at most", "2", "3", "4", "5", "6", "7", "8", "9", "exactly" };
	if ( numericComparison >= 0 && numericComparison < sizeof(numericComparisons)/sizeof(const char*) )
		return std::string(numericComparisons[numericComparison]);
	else
	{
		_itoa_s(numericComparison, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigNumericModifier(u8 numericModifier)
{
	char number[12];
	const char* numericModifiers[] = { "0", "1", "2", "3", "4", "5", "6", "Set To", "Add", "Subtract" };
	if ( numericModifier >= 0 && numericModifier < sizeof(numericModifiers)/sizeof(const char*) )
		return std::string(numericModifiers[numericModifier]);
	else
	{
		_itoa_s(numericModifier, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigScript(u32 scriptNum)
{
	char script[7];
	script[0] = '\"';
	(u32&)script[1] = scriptNum;
	script[5] = '\"';
	script[6] = '\0';
	return std::string(script);
}

std::string TextTrigGenerator::GetTrigNumUnits(u8 numUnits)
{
	char number[12];
	if ( numUnits == 0 )
		return std::string("All");
	else
	{
		_itoa_s(numUnits, number, 10);
		return std::string(number);
	}
}

std::string TextTrigGenerator::GetTrigNumber(u32 number)
{
	char cNumber[12];
	_itoa_s((int)number, cNumber, 10);
	return std::string(cNumber);
}

inline void TextTrigGenerator::AddConditionArgument(buffer &output, Condition& condition, u8 &CID, u8 &stdTextTrigArgNum)
{
	switch ( CID )
	{
		case CID_ACCUMULATE: // Player, NumericComparison, Amount, ResourceType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
				case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
				case 3: ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex) break;
			}
			break;
		case CID_BRING: // Player, Unit, Location, NumericComparison, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
				case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
				case 2: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
				case 3: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 4: ADD_TEXTTRIG_NUMBER(condition.amount) break;
			}
			break;
		case CID_COMMAND: // Player, Unit, NumericComparison, Amount
		case CID_DEATHS:  // Player, Unit, NumericComparison, Amount
		case CID_KILL:	  // Player, Unit, NumericComparison, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
				case 1: ADD_TEXTTRIG_UNIT(condition.unitID) break;
				case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
			}
			break;
		case CID_COMMAND_THE_LEAST: // Unit
		case CID_COMMAND_THE_MOST:	// Unit
		case CID_LEAST_KILLS:		// Unit
		case CID_MOST_KILLS:		// Unit
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_UNIT(condition.unitID);
			break;
		case CID_COMMAND_THE_LEAST_AT: // Unit, Location
		case CID_COMMAND_THE_MOST_AT: // Unit, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_UNIT(condition.unitID) break;
				case 1: ADD_TEXTTRIG_LOCATION(condition.locationNum) break;
			}
			break;
		case CID_COUNTDOWN_TIMER: // NumericComparison, Amount
		case CID_ELAPSED_TIME: // NumericComparison, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 1: ADD_TEXTTRIG_NUMBER(condition.amount) break;
			}
			break;
		case CID_HIGHEST_SCORE: // ScoreType
		case CID_LOWEST_SCORE: // ScoreType
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex);
			break;
		case CID_LEAST_RESOURCES: // ResourceType
		case CID_MOST_RESOURCES: // ResourceType
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_RESOURCE_TYPE(condition.typeIndex);
			break;
		case CID_OPPONENTS: // Player, NumericComparison, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
				case 1: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 2: ADD_TEXTTRIG_NUMBER(condition.amount) break;
			}
			break;
		case CID_SCORE: // Player, ScoreType, NumericComparison, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(condition.players) break;
				case 1: ADD_TEXTTRIG_SCORE_TYPE(condition.typeIndex) break;
				case 2: ADD_TEXTTRIG_NUMERIC_COMPARISON(condition.comparison) break;
				case 3: ADD_TEXTTRIG_NUMBER(condition.amount) break;
			}
			break;
		case CID_SWITCH: // Switch, SwitchState
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_SWITCH(condition.typeIndex) break;
				case 1: ADD_TEXTTRIG_SWITCH_STATE(condition.comparison) break;
			}
			break;
		case (u8)CID_MEMORY: // MemOffset, NumericComparison, Amount
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

inline void TextTrigGenerator::AddActionArgument(buffer &output, Action &action, u8 &AID, u8 &stdTextTrigArgNum)
{
	switch ( AID )
	{
		case AID_CENTER_VIEW:  // Location
		case AID_MINIMAP_PING: // Location
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_LOCATION(action.location)
			break;
		case AID_COMMENT:				 // String
		case AID_SET_MISSION_OBJECTIVES: // String
		case AID_SET_NEXT_SCENARIO:		 // String
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_STRING(action.stringNum)
			break;
		case AID_CREATE_UNIT:			// Player, Unit, NumUnits, Location
		case AID_KILL_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_CREATE_UNIT_WITH_PROPERTIES: // Player, Unit, NumUnits, Location, Properties
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 4: ADD_TEXTTRIG_NUMBER(action.number) break;
			}
			break;
		case AID_DISPLAY_TEXT_MESSAGE: // TextFlags, String
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
				case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
			}
			break;
		case AID_GIVE_UNITS_TO_PLAYER: // Player, SecondPlayer, Unit, NumUnits, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_PLAYER(action.number) break;
				case 2: ADD_TEXTTRIG_UNIT(action.type) break;
				case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_KILL_UNIT:	  // Player, Unit
		case AID_REMOVE_UNIT: // Player, Unit
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
			}
			break;
		case AID_LEADERBOARD_CONTROL_AT_LOCATION: // String, Unit, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_LEADERBOARD_CONTROL: // String, Unit
		case AID_LEADERBOARD_KILLS: // String, Unit
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
			}
			break;
		case AID_LEADERBOARD_GREED: // Amount
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_NUMBER(action.number);
			break;
		case AID_LEADERBOARD_POINTS: // String, ScoreType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
			}
			break;
		case AID_LEADERBOARD_RESOURCES: // String, ResourceType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
			}
			break;
		case AID_LEADERBOARD_COMPUTER_PLAYERS: // StateModifier
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_STATE_MODIFIER(action.type2);
			break;
		case AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION: // String, Unit, Amount, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_LEADERBOARD_GOAL_CONTROL: // String, Unit, Amount
		case AID_LEADERBOARD_GOAL_KILLS: // String, Unit, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
			}
			break;
		case AID_LEADERBOARD_GOAL_POINTS: // String, ScoreType, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
			}
			break;
		case AID_LEADERBOARD_GOAL_RESOURCES: // String, Amount, ResourceType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 2: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
			}
			break;
		case AID_MODIFY_UNIT_ENERGY: // Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_HANGER_COUNT: // Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_HITPOINTS: // Player, Unit, Amount, NumUnits, Location
		case AID_MODIFY_UNIT_SHIELD_POINTS: // Player, Unit, Amount, NumUnits, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 3: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 4: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_MODIFY_UNIT_RESOURCE_AMOUNT: // Player, Amount, NumUnits, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_MOVE_LOCATION: // Player, Unit, LocDest, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
			}
			break;
		case AID_MOVE_UNIT: // Player, Unit, NumUnits, Location, LocDest
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 4: ADD_TEXTTRIG_LOCATION(action.number) break;
			}
			break;
		case AID_ORDER: // Player, Unit, Location, LocDest, OrderType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.number) break;
				case 4: ADD_TEXTTRIG_ORDER(action.type2) break;
			}
			break;
		case AID_PLAY_WAV: // Wav, Duration
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_STRING(action.wavID) break;
				case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
			}
			break;
		case AID_REMOVE_UNIT_AT_LOCATION: // Player, Unit, NumUnits, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUM_UNITS(action.type2) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_RUN_AI_SCRIPT: // Script
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_SCRIPT((char*)&action.number);
			break;
		case AID_RUN_AI_SCRIPT_AT_LOCATION: // Script, Location
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_SCRIPT((char*)&action.number) break;
				case 1: ADD_TEXTTRIG_LOCATION(action.location) break;
			}
			break;
		case AID_SET_ALLIANCE_STATUS: // Player, AllyState
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_ALLY_STATE(action.type) break;
			}
			break;
		case AID_SET_COUNTDOWN_TIMER: // NumericModifier, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
				case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
			}
			break;
		case AID_SET_DEATHS: // Player, Unit, NumericModifier, Amount
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
				case 3: ADD_TEXTTRIG_NUMBER(action.number) break;
			}
			break;
		case AID_SET_DOODAD_STATE:	// Player, Unit, Location, StateMod
		case AID_SET_INVINCIBILITY: // Player, Unit, Location, StateMod
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_UNIT(action.type) break;
				case 2: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 3: ADD_TEXTTRIG_STATE_MODIFIER(action.type2) break;
			}
			break;
		case AID_SET_RESOURCES: // Player, NumericModifier, Amount, ResourceType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 3: ADD_TEXTTRIG_RESOURCE_TYPE(action.type) break;
			}
			break;
		case AID_SET_SCORE: // Player, NumericModifier, Amount, ScoreType
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_PLAYER(action.group) break;
				case 1: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
				case 2: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 3: ADD_TEXTTRIG_SCORE_TYPE(action.type) break;
			}
			break;
		case AID_SET_SWITCH: // Switch, SwitchMod
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_SWITCH(action.number) break;
				case 1: ADD_TEXTTRIG_SWITCH_MODIFIER(action.type2) break;
			}
			break;
		case AID_TALKING_PORTRAIT: // Unit, Duration
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_UNIT(action.type) break;
				case 1: ADD_TEXTTRIG_NUMBER(action.time) break;
			}
			break;
		case AID_TRANSMISSION: // TextFlags, String, Unit, Location, NumericModifier, DurationMod, Wav, Duration
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_TEXT_FLAGS(action.flags) break;
				case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 2: ADD_TEXTTRIG_UNIT(action.type) break;
				case 3: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 4: ADD_TEXTTRIG_NUMERIC_MODIFIER(action.type2) break;
				case 5: ADD_TEXTTRIG_NUMBER(action.number) break;
				case 6: ADD_TEXTTRIG_STRING(action.wavID) break;
				case 7: ADD_TEXTTRIG_NUMBER(action.time) break;
			}
			break;
		case AID_WAIT: // Duration
			if ( stdTextTrigArgNum == 0 ) ADD_TEXTTRIG_NUMBER(action.time);
			break;
		default: // Location, String, Wav, Duration, Player, Number, Type, Action, Type2, Flags, Internal
			switch ( stdTextTrigArgNum ) {
				case 0: ADD_TEXTTRIG_LOCATION(action.location) break;
				case 1: ADD_TEXTTRIG_STRING(action.stringNum) break;
				case 2: ADD_TEXTTRIG_STRING(action.wavID) break;
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

bool TextTrigGenerator::GenerateTextTrigs(ScenarioPtr map, buffer &triggers, std::string &trigString)
{
	if ( !LoadScenario(map, true, false) )
		return false;

	buffer& TRIG = triggers;
	buffer output("TeOu");

	u32 numTrigs = TRIG.size()/TRIG_STRUCT_SIZE;
	Trigger* currTrig;
	Condition* conditions;
	Action* actions;
	u8* players;
	u8 CID;
	u8 AID;

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
		if ( TRIG.getPtr<Trigger>(currTrig, trigNum*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE) )
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

					std::string groupName = groupTable[groupNum];
					output.addStr(groupName.c_str(), groupName.size());
				}
				else if ( players[groupNum] > 0 )
				{
					if ( hasPrevious )
						output.add<char>(',');
					else
						hasPrevious = true;

					std::string groupName = groupTable[groupNum];
					output.addStr(groupName.c_str(), groupName.size());
					output.add<char>(':');
					_itoa_s(players[groupNum], number, 10);
					output.addStr(number, strlen(number));
				}
			}

			output.addStr("){\nConditions:", 14);

			// Add conditions
			conditions = currTrig->conditions;
			for ( int i=0; i<NUM_TRIG_CONDITIONS; i++ )
			{
				Condition& condition = conditions[i];
				CID = condition.condition;

				if ( CID > CID_NO_CONDITION )
				{
					if ( (condition.flags&CONDITION_FLAG_DISABLED) == CONDITION_FLAG_DISABLED )
						output.addStr("\n;	", 3);
					else
						output.addStr("\n	", 2);

					// Add condition name
					if ( CID == CID_DEATHS && condition.players > 28 ) // Memory condition
						output.addStr("Memory", 6);
					else if ( CID < conditionTable.size() )
						output.addStr(conditionTable[CID].c_str(), conditionTable[CID].size());
					else
						output.addStr("Custom", 6);

					output.add<char>('(');
					// Add condition args
					if ( CID == CID_DEATHS && condition.players > 28 ) // Memory condition
					{
						CID = CID_MEMORY;
						numArgs = 3;
					}
					else if ( CID < sizeof(conditionNumArgs) )
						numArgs = conditionNumArgs[CID];
					else
						numArgs = 9; // custom

					for ( u8 i=0; i<numArgs; i++ )
					{
						if ( i > 0 )
							output.addStr(", ", 2);

						AddConditionArgument(output, condition, CID, i);
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
				AID = action.action;
				if ( AID > AID_NO_ACTION )
				{
					if ( (action.flags&ACTION_FLAG_DISABLED) == ACTION_FLAG_DISABLED )
						output.addStr("\n;	", 3);
					else
						output.addStr("\n	", 2);

					// Add action name
					if ( AID < actionTable.size() )
						output.addStr(actionTable[AID].c_str(), actionTable[AID].size());
					else
						output.addStr("Custom", 6);

					output.add<char>('(');
					// Add action args
					if ( AID < sizeof(actionNumArgs) )
						numArgs = actionNumArgs[AID];
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
				_itoa_s(currTrig->internalData, number, 2);
				size_t length = strlen(number);
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
	char number[12];
	const char* cTextFlags[] = { "Don't Always Display", "Always Display" };
	if		( (textFlags&ACTION_FLAG_ALWAYS_DISPLAY) == 0 )
		return std::string(cTextFlags[0]);
	else if ( (textFlags&ACTION_FLAG_ALWAYS_DISPLAY) == ACTION_FLAG_ALWAYS_DISPLAY )
		return std::string(cTextFlags[1]);
	else
	{
		_itoa_s(textFlags, number, 10);
		return std::string(number);
	}
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
		   PrepWavTable(map, quoteArgs) &&
		   PrepGroupTable(map, quoteArgs) &&
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
	u16 stringNum;
	std::string locationName;
	buffer& MRGN = map->MRGN();

	locationTable.push_back(std::string("0") );

	if ( MRGN.exists() && map->STR().exists() )
	{
		int numLocations = MRGN.size()/sizeof(ChkLocation);
		for ( int i=0; i<numLocations; i++ )
		{
			Invariant( locationTable.size() == i+1 );
			if ( MRGN.getPtr(loc, i*sizeof(ChkLocation), sizeof(ChkLocation)) )
			{
				locationName = "";
				stringNum = loc->stringNum;

				if ( i == 63 )
				{
					if ( quoteArgs )
						locationName = "\"Anywhere\"";
					else
						locationName = "Anywhere";

					locationTable.push_back( locationName );
				}
				else if ( loc->stringNum > 0 && map->getEscapedString(locationName, loc->stringNum) )
				{
					if ( quoteArgs )
						locationTable.push_back( "\"" + locationName + "\"" );
					else
						locationTable.push_back(locationName);
				}
			}

			if ( locationTable.size() == i+1 )
			{
				char locNum[12];
				_itoa_s(i+1, locNum, 10);
				locationName = locNum;
				locationTable.push_back( locationName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames)
{
	unitTable.clear();

	std::string unitName;
	buffer& unitSettings = map->unitSettings();
	if ( unitSettings.exists() && map->STR().exists() )
	{
		for ( int unitID=0; unitID<=232; unitID++ )
		{
			Invariant( unitTable.size() == unitID );
			if ( quoteArgs )
			{
				if ( useCustomNames && unitID < 228 )
				{
					std::string unquotedName;
					map->getUnitName(unquotedName, unitID);
					unitName = "\"" + unquotedName + "\"";
				}
				else
					unitName = "\"" + std::string(LegacyTextTrigDisplayName[unitID]) + "\"";
			}
			else
			{
				if ( useCustomNames && unitID < 228 )
					map->getUnitName(unitName, unitID);
				else
					unitName = std::string(LegacyTextTrigDisplayName[unitID]);
			}

			unitTable.push_back( unitName );
		}
	}
	return true;
}

bool TextTrigGenerator::PrepSwitchTable(ScenarioPtr map, bool quoteArgs)
{
	switchTable.clear();

	std::string switchName;
	buffer& SWNM = map->SWNM();
	if ( SWNM.exists() && map->STR().exists() )
	{
		u32 stringID;
		for ( u32 switchID=0; switchID<256; switchID++ )
		{
			Invariant( switchTable.size() == switchID );
			if ( SWNM.get<u32>(stringID, switchID*4) &&
				 stringID > 0 &&
				 map->getEscapedString(switchName, stringID) )
			{
				if ( quoteArgs )
					switchTable.push_back( "\"" + switchName + "\"" );
				else
					switchTable.push_back(switchName);
			}
			else
			{
				char swDefault[20];
				if ( quoteArgs )
					sprintf_s(swDefault, 20, "\"Switch%i\"", switchID+1);
				else
					sprintf_s(swDefault, 20, "Switch%i", switchID+1);

				switchName = swDefault;
				switchTable.push_back( switchName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepWavTable(ScenarioPtr map, bool quoteArgs)
{
	wavTable.clear();

	std::string wavName;
	buffer& WAV = map->WAV();
	if ( WAV.exists() && map->STR().exists() )
	{
		for ( u32 i=0; i<512; i++ )
		{
			Invariant( wavTable.size() == i );
			u32 stringID;
			if ( WAV.get<u32>(stringID, i*4) &&
				 stringID > 0 &&
				 map->getEscapedString(wavName, stringID) )
			{
				if ( quoteArgs )
					wavTable.push_back( "\"" + wavName + "\"" );
				else
					wavTable.push_back(wavName);
			}
			else
			{
				char wavNum[12];
				_itoa_s(i, wavNum, 10);
				wavName = wavNum;
				wavTable.push_back( wavName );
			}
		}
	}
	return true;
}

bool TextTrigGenerator::PrepGroupTable(ScenarioPtr map, bool quoteArgs)
{
	groupTable.clear();

	std::string groupName;
	buffer& FORC = map->FORC();
	bool hasForcStrings = FORC.exists() && map->STR().exists();

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
		Invariant( groupTable.size() == i+18 );
		u16 stringID;
		if ( hasForcStrings &&
			 FORC.get<u16>(stringID, 8+i*2) &&
			 stringID > 0 &&
			 map->getEscapedString(groupName, stringID) )
		{
			if ( quoteArgs )
				groupName = "\"" + groupName + "\"";

			groupTable.push_back( groupName );
		}
		else
		{
			char forceName[12];
			if ( quoteArgs )
				sprintf_s(forceName, 12, "\"Force %i\"", i);
			else
				sprintf_s(forceName, 12, "Force %i", i);

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

bool TextTrigGenerator::PrepStringTable(ScenarioPtr map, bool quoteArgs)
{
	stringTable.clear();
	extendedStringTable.clear();

	if ( map->STR().exists() || map->KSTR().exists() )
	{
		StringUsageTable standardStringUsage;
		StringUsageTable extendedStringUsage;
		if ( standardStringUsage.populateTable(map.get(), false) || extendedStringUsage.populateTable(map.get(), true) )
		{
			std::string str;

			u32 numStrings = standardStringUsage.numStrings();
			for ( u32 i=0; i<numStrings; i++ )
			{
				str = "";

				if ( standardStringUsage.isUsed(i) )
					map->getEscapedString(str, i);

				if ( quoteArgs )
					stringTable.push_back( "\"" + str + "\"" );
				else
					stringTable.push_back(str);
			}

			numStrings = extendedStringUsage.numStrings();
			for ( u32 i=0; i<numStrings; i++ )
			{
				str = "";

				if ( extendedStringUsage.isUsed(i) )
					map->getEscapedString(str, (65536-i));

				if ( quoteArgs )
					extendedStringTable.push_back( "\"" + str + "\"" );
				else
					extendedStringTable.push_back(str);
			}

			return true;
		}
		else
			return false;
	}
	else
		return false;
}
