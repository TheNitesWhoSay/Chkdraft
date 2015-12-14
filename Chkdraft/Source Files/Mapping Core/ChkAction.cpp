#include "ChkAction.h"

void Action::ToggleDisabled()
{
	if ( (flags & ACTION_FLAG_DISABLED) == ACTION_FLAG_DISABLED )
		flags &= xACTION_FLAG_DISABLED;
	else
		flags |= ACTION_FLAG_DISABLED;
}

bool Action::isDisabled()
{
	return (flags & ACTION_FLAG_DISABLED) == ACTION_FLAG_DISABLED;
}

ActionArgType Action::TextTrigArgType(u8 argNum, u8 action)
{
	switch ( action )
	{
		case AID_CENTER_VIEW:
			if ( argNum == 0 )
				return ActionArgType::ActLocation;
			break;
		case AID_COMMENT:
			if ( argNum == 0 )
				return ActionArgType::ActString;
			break;
		case AID_CREATE_UNIT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_CREATE_UNIT_WITH_PROPERTIES:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
				case 4: return ActionArgType::ActCUWP; break;
			}
			break;
		case AID_DEFEAT: break;
		case AID_DISPLAY_TEXT_MESSAGE:
			switch ( argNum ) {
				case 0: return ActionArgType::ActTextFlags; break;
				case 1: return ActionArgType::ActString; break;
			}
			break;
		case AID_DRAW: break;
		case AID_GIVE_UNITS_TO_PLAYER:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActPlayer; break;
				case 2: return ActionArgType::ActUnit; break;
				case 3: return ActionArgType::ActNumUnits; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_KILL_UNIT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
			}
			break;
		case AID_KILL_UNIT_AT_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_LEADERBOARD_CONTROL_AT_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_LEADERBOARD_CONTROL:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
			}
			break;
		case AID_LEADERBOARD_GREED:
			if ( argNum == 0 )
				return ActionArgType::ActAmount;
			break;
		case AID_LEADERBOARD_KILLS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
			}
			break;
		case AID_LEADERBOARD_POINTS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActScoreType; break;
			}
			break;
		case AID_LEADERBOARD_RESOURCES:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActResourceType; break;
			}
			break;
		case AID_LEADERBOARD_COMPUTER_PLAYERS:
			if ( argNum == 0 )
				return ActionArgType::ActStateMod;
			break;
		case AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActAmount; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_CONTROL:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActAmount; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_KILLS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActAmount; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_POINTS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActScoreType; break;
				case 2: return ActionArgType::ActAmount; break;
			}
			break;
		case AID_LEADERBOARD_GOAL_RESOURCES:
			switch ( argNum ) {
				case 0: return ActionArgType::ActString; break;
				case 1: return ActionArgType::ActAmount; break;
				case 2: return ActionArgType::ActResourceType; break;
			}
			break;
		case AID_MINIMAP_PING:
			if ( argNum == 0 )
				return ActionArgType::ActLocation;
			break;
		case AID_MODIFY_UNIT_ENERGY:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActPercent; break;
				case 3: return ActionArgType::ActNumUnits; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MODIFY_UNIT_HANGER_COUNT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActAmount; break;
				case 3: return ActionArgType::ActNumUnits; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MODIFY_UNIT_HITPOINTS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActPercent; break;
				case 3: return ActionArgType::ActNumUnits; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MODIFY_UNIT_RESOURCE_AMOUNT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActAmount; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MODIFY_UNIT_SHIELD_POINTS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActPercent; break;
				case 3: return ActionArgType::ActNumUnits; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MOVE_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActLocation; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MOVE_UNIT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
				case 4: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_MUTE_UNIT_SPEECH: break;
		case AID_NO_ACTION: break;
		case AID_ORDER:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActLocation; break;
				case 3: return ActionArgType::ActLocation; break;
				case 4: return ActionArgType::ActOrder; break;
			}
			break;
		case AID_PAUSE_GAME: break;
		case AID_PAUSE_TIMER: break;
		case AID_PLAY_WAV:
			switch ( argNum ) {
				case 0: return ActionArgType::ActWav; break;
				case 1: return ActionArgType::ActDuration; break;
			}
			break;
		case AID_PRESERVE_TRIGGER: break;
		case AID_REMOVE_UNIT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
			}
			break;
		case AID_REMOVE_UNIT_AT_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumUnits; break;
				case 3: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_RUN_AI_SCRIPT:
			if ( argNum == 0 )
				return ActionArgType::ActScript; break;
			break;
		case AID_RUN_AI_SCRIPT_AT_LOCATION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActScript; break;
				case 1: return ActionArgType::ActLocation; break;
			}
			break;
		case AID_SET_ALLIANCE_STATUS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActAllyState; break;
			}
			break;
		case AID_SET_COUNTDOWN_TIMER:
			switch ( argNum ) {
				case 0: return ActionArgType::ActNumericMod; break;
				case 1: return ActionArgType::ActAmount; break;
			}
			break;
		case AID_SET_DEATHS:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActNumericMod; break;
				case 3: return ActionArgType::ActAmount; break;
			}
			break;
		case AID_SET_DOODAD_STATE:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActLocation; break;
				case 3: return ActionArgType::ActStateMod; break;
			}
			break;
		case AID_SET_INVINCIBILITY:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActUnit; break;
				case 2: return ActionArgType::ActLocation; break;
				case 3: return ActionArgType::ActStateMod; break;
			}
			break;
		case AID_SET_MISSION_OBJECTIVES:
			if ( argNum == 0 )
				return ActionArgType::ActString;
			break;
		case AID_SET_NEXT_SCENARIO:
			if ( argNum == 0 )
				return ActionArgType::ActString;
			break;
		case AID_SET_RESOURCES:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActNumericMod; break;
				case 2: return ActionArgType::ActAmount; break;
				case 3: return ActionArgType::ActResourceType; break;
			}
			break;
		case AID_SET_SCORE:
			switch ( argNum ) {
				case 0: return ActionArgType::ActPlayer; break;
				case 1: return ActionArgType::ActNumericMod; break;
				case 2: return ActionArgType::ActAmount; break;
				case 3: return ActionArgType::ActScoreType; break;
			}
			break;
		case AID_SET_SWITCH:
			switch ( argNum ) {
				case 0: return ActionArgType::ActSwitch; break;
				case 1: return ActionArgType::ActSwitchMod; break;
			}
			break;
		case AID_TALKING_PORTRAIT:
			switch ( argNum ) {
				case 0: return ActionArgType::ActUnit; break;
				case 1: return ActionArgType::ActDuration; break;
			}
			break;
		case AID_TRANSMISSION:
			switch ( argNum ) {
				case 0: return ActionArgType::ActTextFlags; break;
				case 1: return ActionArgType::ActString; break;
				case 2: return ActionArgType::ActUnit; break;
				case 3: return ActionArgType::ActLocation; break;
				case 4: return ActionArgType::ActNumericMod; break;
				case 5: return ActionArgType::ActDuration; break;
				case 6: return ActionArgType::ActWav; break;
			}
			break;
		case AID_UNMUTE_UNIT_SPEECH: break;
		case AID_UNPAUSE_GAME: break;
		case AID_UNPAUSE_TIMER: break;
		case AID_VICTORY: break;
		case AID_WAIT:
			if ( argNum == 0 )
				return ActionArgType::ActDuration;
			break;
		default: // AID_CUSTOM
			switch ( argNum ) {
				case 0: return ActionArgType::ActLocation; break;
				case 1: return ActionArgType::ActString; break;
				case 2: return ActionArgType::ActWav; break;
				case 3: return ActionArgType::ActDuration; break;
				case 4: return ActionArgType::ActPlayer; break;
				case 5: return ActionArgType::ActNumber; break;
				case 6: return ActionArgType::ActTypeIndex; break;
				case 7: return ActionArgType::ActActionType; break;
				case 8: return ActionArgType::ActSecondaryType; break;
				case 9: return ActionArgType::ActFlags; break;
				case 10: return ActionArgType::ActInternalData; break;
			}
			break;
	}
	return ActionArgType::ActNoType;
}
