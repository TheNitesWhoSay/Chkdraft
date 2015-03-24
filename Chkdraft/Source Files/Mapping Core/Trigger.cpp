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

bool Trigger::preserveTriggerFlagged()
{
	return (internalData & BIT_2) == BIT_2;
}

bool Trigger::disabled()
{
	return (internalData & BIT_3) == BIT_3;
}

bool Trigger::ignoreConditionsOnce()
{
	return (internalData & BIT_0) == BIT_0;
}

bool Trigger::ignoreWaitSkipOnce()
{
	return (internalData & BIT_6) == BIT_6;
}

bool Trigger::ignoreMiscActionsOnce()
{
	return (internalData & BIT_4) == BIT_4;
}

bool Trigger::ignoreDefeatDraw()
{
	return (internalData & BIT_1) == BIT_1;
}

bool Trigger::flagPaused()
{
	return (internalData & BIT_5) == BIT_5;
}

void Trigger::setPreserveTriggerFlagged(bool preserved)
{
	if ( preserved )
		internalData |= BIT_2;
	else
		internalData &= x32BIT_2;
}

void Trigger::setDisabled(bool disabled)
{
	if ( disabled )
		internalData |= BIT_3;
	else
		internalData &= x32BIT_3;
}

void Trigger::setIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
	if ( ignoreConditionsOnce )
		internalData |= BIT_0;
	else
		internalData &= x32BIT_0;
}

void Trigger::setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
	if ( ignoreWaitSkipOnce )
		internalData |= BIT_6;
	else
		internalData &= x32BIT_6;
}

void Trigger::setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
	if ( ignoreMiscActionsOnce )
		internalData |= BIT_4;
	else
		internalData &= x32BIT_4;
}

void Trigger::setIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
	if ( ignoreDefeatDraw )
		internalData |= BIT_1;
	else
		internalData &= x32BIT_1;
}

void Trigger::setFlagPaused(bool flagPaused)
{
	if ( flagPaused )
		internalData |= BIT_5;
	else
		internalData &= x32BIT_5;
}
