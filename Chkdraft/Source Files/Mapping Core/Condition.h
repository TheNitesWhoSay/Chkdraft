#ifndef CONDITION_H
#define CONDITION_H
#include "Basics.h"

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

#endif