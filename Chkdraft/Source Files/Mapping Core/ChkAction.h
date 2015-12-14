#ifndef CHKACTION_H
#define CHKACTION_H
#include "Basics.h"

enum ActionArgType
{
	ActNoType,
	ActLocation,
	ActString,
	ActPlayer,
	ActUnit,
	ActNumUnits,
	ActCUWP,
	ActTextFlags,
	ActAmount,
	ActScoreType,
	ActResourceType,
	ActStateMod,
	ActPercent,
	ActOrder,
	ActWav,
	ActDuration,
	ActScript,
	ActAllyState,
	ActNumericMod,
	ActSwitch,
	ActSwitchMod,
	ActType,
	ActActionType,
	ActSecondaryType,
	ActFlags,
	ActNumber, // Amount, Group2, LocDest, UnitPropNum, ScriptNum
	ActTypeIndex, // Unit, ScoreType, ResourceType, AllianceStatus
	ActSecondaryTypeIndex, // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
	ActInternalData
};

class Action // A trigger action, as found in sets of 64 in a Trigger
{
	public:
		u32 location; // 1 based
		u32 stringNum;
		u32 wavID;
		u32 time;
		u32 group;
		u32 number; // Amount/Group2/LocDest/UnitPropNum/ScriptNum
		u16 type; // Unit/score/resource type/alliance status
		u8 action;
		u8 type2; // Num units (0 for all)/switch action/unit order/modify type
		u8 flags;
			#define ACTION_FLAG_UKNOWN_0			 BIT_0
			#define ACTION_FLAG_DISABLED			 BIT_1
			#define ACTION_FLAG_ALWAYS_DISPLAY		 BIT_2
			#define ACTION_FLAG_UNIT_PROPERTIES_USED BIT_3
			#define ACTION_FLAG_UNIT_TYPE_USED		 BIT_4
			#define ACTION_FLAG_UNIT_ID_USED		 BIT_5 // Probably unnecessary
			#define ACTION_FLAG_UNKNOWN_6			 BIT_6
			#define ACTION_FLAG_UNKNOWN_7			 BIT_7

			#define ACTION_FLAG_DONT_ALWAYS_DISPLAY (~ACTION_FLAG_ALWAYS_DISPLAY)

			#define xACTION_FLAG_DISABLED x8BIT_1

		u8 internalData[3]; // Number of actions to process next?
		
		void ToggleDisabled();
		bool isDisabled();
		static ActionArgType TextTrigArgType(u8 argNum, u8 action);
};

	  //AID = Action ID
#define AID_CENTER_VIEW 10
#define AID_COMMENT 47
#define AID_CREATE_UNIT 44
#define AID_CREATE_UNIT_WITH_PROPERTIES 11
#define AID_DEFEAT 2
#define AID_DISPLAY_TEXT_MESSAGE 9
#define AID_DRAW 56
#define AID_GIVE_UNITS_TO_PLAYER 48
#define AID_KILL_UNIT 22
#define AID_KILL_UNIT_AT_LOCATION 23
#define AID_LEADERBOARD_CONTROL_AT_LOCATION 18
#define AID_LEADERBOARD_CONTROL 17
#define AID_LEADERBOARD_GREED 40
#define AID_LEADERBOARD_KILLS 20
#define AID_LEADERBOARD_POINTS 21
#define AID_LEADERBOARD_RESOURCES 19
#define AID_LEADERBOARD_COMPUTER_PLAYERS 32
#define AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION 34
#define AID_LEADERBOARD_GOAL_CONTROL 33
#define AID_LEADERBOARD_GOAL_KILLS 36
#define AID_LEADERBOARD_GOAL_POINTS 37
#define AID_LEADERBOARD_GOAL_RESOURCES 35
#define AID_MINIMAP_PING 28
#define AID_MODIFY_UNIT_ENERGY 50
#define AID_MODIFY_UNIT_HANGER_COUNT 53
#define AID_MODIFY_UNIT_HITPOINTS 49
#define AID_MODIFY_UNIT_RESOURCE_AMOUNT 52
#define AID_MODIFY_UNIT_SHIELD_POINTS 51
#define AID_MOVE_LOCATION 38
#define AID_MOVE_UNIT 39
#define AID_MUTE_UNIT_SPEECH 30
#define AID_NO_ACTION 0
#define AID_ORDER 46
#define AID_PAUSE_GAME 5
#define AID_PAUSE_TIMER 54
#define AID_PLAY_WAV 8
#define AID_PRESERVE_TRIGGER 3
#define AID_REMOVE_UNIT 24
#define AID_REMOVE_UNIT_AT_LOCATION 25
#define AID_RUN_AI_SCRIPT 15
#define AID_RUN_AI_SCRIPT_AT_LOCATION 16
#define AID_SET_ALLIANCE_STATUS 57
#define AID_SET_COUNTDOWN_TIMER 14
#define AID_SET_DEATHS 45
#define AID_SET_DOODAD_STATE 42
#define AID_SET_INVINCIBILITY 43
#define AID_SET_MISSION_OBJECTIVES 12
#define AID_SET_NEXT_SCENARIO 41
#define AID_SET_RESOURCES 26
#define AID_SET_SCORE 27
#define AID_SET_SWITCH 13
#define AID_TALKING_PORTRAIT 29
#define AID_TRANSMISSION 7
#define AID_UNMUTE_UNIT_SPEECH 31
#define AID_UNPAUSE_GAME 6
#define AID_UNPAUSE_TIMER 55
#define AID_VICTORY 1
#define AID_WAIT 4

const int AID_CUSTOM = -1;
const int AID_MEMORY = -2;
#define AID_MEMORY_BASE AID_SET_DEATHS

#endif