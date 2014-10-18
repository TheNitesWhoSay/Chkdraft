#ifndef BASICS_H
#define BASICS_H
#include <string>

#ifdef CHKDRAFT // Globally defined in project properties if this is used in CHKDraft
	void Debug();
	void DebugIf(bool condition);
	void NoDebug();
	void mb(const char* text); // Basic message box message
	void Error(const char* ErrorMessage); // Basic error message box
	bool RetryError(const char* text); // Error box with yes/no confirm
	extern void PrintError(const char* file, unsigned int line, const char* msg, ...); // Prints to LastError and LastErrorLoc
	#define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, __VA_ARGS__)
#else
	void IgnoreErr(const char* file, unsigned int line, const char* msg, ...); // Ignores an error message
	#define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__) /* Would print to error messages if CHKDRAFT was defined */
#endif

#ifndef u64
#define u64 unsigned long long
#endif
#ifndef s64
#define s64 signed long long
#endif
#ifndef u32
#define u32 unsigned long
#endif
#ifndef s32
#define s32 signed long
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef u8
#define u8 unsigned char
#endif
#ifndef s8
#define s8 signed char
#endif

#define BIT_0  0x1
#define BIT_1  0x2
#define BIT_2  0x4
#define BIT_3  0x8
#define BIT_4  0x10
#define BIT_5  0x20
#define BIT_6  0x40
#define BIT_7  0x80

#define BIT_8  0x100
#define BIT_9  0x200
#define BIT_10 0x400
#define BIT_11 0x800
#define BIT_12 0x1000
#define BIT_13 0x2000
#define BIT_14 0x4000
#define BIT_15 0x8000

#define BIT_16 0x10000
#define BIT_17 0x20000
#define BIT_18 0x40000
#define BIT_19 0x80000
#define BIT_20 0x100000
#define BIT_21 0x200000
#define BIT_22 0x400000
#define BIT_23 0x800000

#define BIT_24 0x1000000
#define BIT_25 0x2000000
#define BIT_26 0x4000000
#define BIT_27 0x8000000
#define BIT_28 0x10000000
#define BIT_29 0x20000000
#define BIT_30 0x40000000
#define BIT_31 0x80000000

const u32 u32Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
						BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15,
						BIT_16, BIT_17, BIT_18, BIT_19, BIT_20, BIT_21, BIT_22, BIT_23,
						BIT_24, BIT_25, BIT_26, BIT_27, BIT_28, BIT_29, BIT_30, BIT_31 };

const u16 u16Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
						BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15 };

extern const char* DefaultUnitDisplayName[233];

extern const char* LegacyTextTrigDisplayName[233];

#define NUM_UNIT_NAMES 233

#define NUM_REAL_UNITS 227

struct ChkUnit // A unit as found in a scenario.chk's UNIT section
{
	u32 serial; 
	u16 xc;
	u16 yc;
	u16 id;
	u16 linkType;
	u16 special;
	u16 validFlags;
	u8 owner;
	u8 hitpoints;
	u8 shields;
	u8 energy;
	u32 resources;
	u16 hanger;
	u16 stateFlags;
		#define UNIT_STATE_CLOAKED		BIT_0
		#define UNIT_STATE_BURROWED		BIT_1
		#define UNIT_STATE_LIFTED		BIT_2
		#define UNIT_STATE_HALLUCINATED	BIT_3
		#define UNIT_STATE_INVINCIBLE	BIT_4

	u32 unused;
	u32 link;
};
#define UNIT_STRUCT_SIZE 36
#define NUM_UNIT_FIELDS 16
#define UNIT_FIELD_SERIAL	   0
#define UNIT_FIELD_XC		   1
#define UNIT_FIELD_YC		   2
#define UNIT_FIELD_ID		   3
#define UNIT_FIELD_LINKTYPE	   4
#define UNIT_FIELD_SPECIAL	   5
#define UNIT_FIELD_VALIDFLAGS  6
#define UNIT_FIELD_OWNER	   7
#define UNIT_FIELD_HITPOINTS   8
#define UNIT_FIELD_SHIELDS	   9
#define UNIT_FIELD_ENERGY	  10
#define UNIT_FIELD_RESOURCES  11
#define UNIT_FIELD_HANGER	  12
#define UNIT_FIELD_STATEFLAGS 13
#define UNIT_FIELD_UNUSED	  14
#define UNIT_FIELD_LINK		  15
const u8 unitFieldSize[] = {  4,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  4,  2,  2,  4,  4 };
const u8 unitFieldLoc [] = {  0,  4,  6,  8, 10, 12, 14, 16, 17, 18, 19, 20, 24, 26, 28, 32 };

struct ChkSprite // A sprite as found in a scenario.chk's THG2 section
{
	u16 id;
	u16 xc;
	u16 yc;
	u8 owner;
	u8 unused;
	u16 flags;
		#define FLAG_DRAW_AS_SPRITE		BIT_12
		#define FLAG_SPRITE_DISABLED	BIT_15
};
#define SPRITE_STRUCT_SIZE 10

struct ChkDoodad // A doodad as found in a scenario.chk's DD2 section
{
	u16 id;
	u16 xc;
	u16 yc;
	u8 owner;
	u8 enabled; // 0 enabled, 1 disabled
};
#define DOODAD_STRUCT_SIZE 8

class Condition // A trigger condition, as found in sets of 16 in a Trigger
{
	public:
		u32 locationNum; // 1 based
		u32 players;
		u32 amount;
		u16 unitID;
		u8 comparison;
		u8 condition;
		u8 typeIndex; // Resource type/score type/switch num
		u8 flags;
			#define CONDITION_FLAG_UNKNOWN_0			BIT_0
			#define CONDITION_FLAG_DISABLED				BIT_1
			#define CONDITION_FLAG_ALWAYS_DISPLAY		BIT_2
			#define CONDITION_FLAG_UNIT_PROPERTIES_USED BIT_3
			#define CONDITION_FLAG_UNIT_TYPE_USED		BIT_4 // Possibly unnecessary
			#define CONDITION_FLAG_UNITID_USED			BIT_5 // Probably unnecessary
			#define CONDITION_FLAG_UKNOWN_6				BIT_6
			#define CONDITION_FLAG_UNNOWN_7				BIT_7
		u16 internalData; // Number of which condition to process next?
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
		u8 internalData[3]; // Number of actions to process next?
};

#define NUM_TRIG_CONDITIONS 16
#define NUM_TRIG_ACTIONS 64
#define NUM_TRIG_PLAYERS 28
class Trigger // A trigger as found in a scenario.chk's TRIG and MBRF sections
{
	public:
		Condition conditions[NUM_TRIG_CONDITIONS];
		Action actions[NUM_TRIG_ACTIONS];
		u32 internalData; // ie: check trigger
		u8 players[NUM_TRIG_PLAYERS];
			#define EXECUTED_BY_PLAYER	   1
			#define NOT_EXECUTED_BY_PLAYER 0
};
#define TRIG_STRUCT_SIZE 2400

struct ChkLocation // A location as found in a scenario.chk's MRGN section
{
	s32 xc1;
	s32 yc1;
	s32 xc2;
	s32 yc2;
	u16 stringNum;
	u16 elevation;
		#define LOC_ELEVATION_LOWGROUND		BIT_0
		#define LOC_ELEVATION_MEDGROUND		BIT_1
		#define LOC_ELEVATION_HIGHGROUND	BIT_2
		#define LOC_ELEVATION_LOWAIR		BIT_3
		#define LOC_ELEVATION_MEDAIR		BIT_4
		#define LOC_ELEVATION_HIGHAIR		BIT_5
};
#define CHK_LOCATION_SIZE 20
#define NUM_LOCATION_FIELDS 6
#define LOC_FIELD_XC1		  0
#define LOC_FIELD_YC1		  1
#define LOC_FIELD_XC2		  2
#define LOC_FIELD_YC2		  3
#define LOC_FIELD_STRING_NUM  4
#define LOC_FIELD_ELEVATION	  5
const u8 locationFieldSize[] = {  4,  4,  4,  4,  2,  2  };
const u8 locationFieldLoc [] = {  0,  4,  8,  12, 16, 18 };

class StringTableNode // An object comprehensively representing a StarCraft string
{
	public:

		std::string string; // A given string from the map
		u32 stringNum; // The ID associated with this string
		bool isExtended; // Whether or not the given string is extended
		u32 propStruct; // propStruct (extended strings only)

		StringTableNode(); // ctor
		bool operator== (StringTableNode other); // Check for complete equivalency
		bool operator< (StringTableNode other); // Check if one string num is less than another
};

bool CompareStrTblNode(StringTableNode first, StringTableNode second); // Returns whether the first should be sorted below the second

	  //CID = Condition ID
#define CID_ACCUMULATE 4
#define CID_ALWAYS 22
#define CID_BRING 3
#define CID_COMMAND 2
#define CID_COMMAND_THE_LEAST 16
#define CID_COMMAND_THE_LEAST_AT 17
#define CID_COMMAND_THE_MOST 6
#define CID_COMMAND_THE_MOST_AT 7
#define CID_COUNTDOWN_TIMER 1
#define CID_DEATHS 15
#define CID_ELAPSED_TIME 12
#define CID_HIGHEST_SCORE 9
#define CID_KILL 5
#define CID_LEAST_KILLS 18
#define CID_LEAST_RESOURCES 20
#define CID_LOWEST_SCORE 19
#define CID_MOST_KILLS 8
#define CID_MOST_RESOURCES 10
#define CID_NEVER 23
#define CID_NEVER_ALT 13
#define CID_NO_CONDITION 0
#define CID_OPPONENTS 14
#define CID_SCORE 21
#define CID_SWITCH 11

const int CID_CUSTOM = -1;
const int CID_MEMORY = -2;
#define CID_MEMORY_BASE CID_DEATHS

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