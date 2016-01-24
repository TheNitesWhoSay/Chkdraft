#include "ChkCondition.h"

void Condition::ToggleDisabled()
{
	if ( (flags & CONDITION_FLAG_DISABLED) == CONDITION_FLAG_DISABLED )
		flags &= xCONDITION_FLAG_DISABLED;
	else
		flags |= CONDITION_FLAG_DISABLED;
}

bool Condition::isDisabled()
{
	return (flags & CONDITION_FLAG_DISABLED) == CONDITION_FLAG_DISABLED;
}

ConditionArgType Condition::TextTrigArgType(u8 argNum, u8 condition)
{
	switch ( condition )
	{
		case CID_ACCUMULATE:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndNumericComparison; break;
				case 2: return ConditionArgType::CndAmount; break;
				case 3: return ConditionArgType::CndResourceType; break;
			}
			break;
		case CID_ALWAYS: break;
		case CID_BRING:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndUnit; break;
				case 2: return ConditionArgType::CndLocation; break;
				case 3: return ConditionArgType::CndNumericComparison; break;
				case 4: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_COMMAND:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndUnit; break;
				case 2: return ConditionArgType::CndNumericComparison; break;
				case 3: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_COMMAND_THE_LEAST:
			if ( argNum == 0 )
				return ConditionArgType::CndUnit;
			break;
		case CID_COMMAND_THE_LEAST_AT:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndUnit; break;
				case 1: return ConditionArgType::CndLocation; break;
			}
			break;
		case CID_COMMAND_THE_MOST:
			if ( argNum == 0 )
				return ConditionArgType::CndUnit;
			break;
		case CID_COMMAND_THE_MOST_AT:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndUnit; break;
				case 1: return ConditionArgType::CndLocation; break;
			}
			break;
		case CID_COUNTDOWN_TIMER:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndNumericComparison; break;
				case 1: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_DEATHS:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndUnit; break;
				case 2: return ConditionArgType::CndNumericComparison; break;
				case 3: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_ELAPSED_TIME:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndNumericComparison; break;
				case 1: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_HIGHEST_SCORE:
			if ( argNum == 0 )
				return ConditionArgType::CndScoreType;
			break;
		case CID_KILL:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndUnit; break;
				case 2: return ConditionArgType::CndNumericComparison; break;
				case 3: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_LEAST_KILLS:
			if ( argNum == 0 )
				return ConditionArgType::CndUnit;
			break;
		case CID_LEAST_RESOURCES:
			if ( argNum == 0 )
				return ConditionArgType::CndResourceType;
			break;
		case CID_LOWEST_SCORE:
			if ( argNum == 0 )
				return ConditionArgType::CndScoreType;
			break;
		case CID_MOST_KILLS:
			if ( argNum == 0 )
				return ConditionArgType::CndUnit;
			break;
		case CID_MOST_RESOURCES:
			if ( argNum == 0 )
				return ConditionArgType::CndResourceType;
			break;
		case CID_NEVER: break;
		case CID_NEVER_ALT: break;
		case CID_OPPONENTS:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndNumericComparison; break;
				case 2: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_SCORE:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndPlayer; break;
				case 1: return ConditionArgType::CndScoreType; break;
				case 2: return ConditionArgType::CndNumericComparison; break;
				case 3: return ConditionArgType::CndAmount; break;
			}
			break;
		case CID_SWITCH:
			switch ( argNum ) {
				case 0: return ConditionArgType::CndSwitch; break;
				case 1: return ConditionArgType::CndSwitchState; break;
			}
			break;
		default: // CID_CUSTOM
			switch ( argNum ) {
				case 0: return ConditionArgType::CndLocation; break;
				case 1: return ConditionArgType::CndPlayer; break;
				case 2: return ConditionArgType::CndAmount; break;
				case 3: return ConditionArgType::CndUnit; break;
				case 4: return ConditionArgType::CndComparison; break;
				case 5: return ConditionArgType::CndConditionType; break;
				case 6: return ConditionArgType::CndTypeIndex; break;
				case 7: return ConditionArgType::CndFlags; break;
				case 8: return ConditionArgType::CndInternalData; break;
			}
			break;
	}
	return ConditionArgType::CndNoType;
}

ConditionArgType Condition::TextTrigArgType(u8 argNum)
{
	return TextTrigArgType(argNum, this->condition);
}
