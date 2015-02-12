#include "Trigger.h"

u8 Trigger::numUsedConditions()
{
	u8 total = 0;
	for ( u8 i=0; i<16; i++ )
	{
		if ( conditions[i].condition != CID_NO_CONDITION )
			total ++;
	}
	return total;
}

u8 Trigger::numUsedActions()
{
	u8 total = 0;
	for ( u8 i=0; i<64; i++ )
	{
		if ( actions[i].action != AID_NO_ACTION )
			total ++;
	}
	return total;
}
