#include "ChkTrigger.h"

Condition Trigger::junkCondition;
Action Trigger::junkAction;

Trigger::Trigger() : internalData(0)
{
    memset(players, 0, NUM_TRIG_PLAYERS);
}

Condition& Trigger::condition(u8 index)
{
    if ( index < NUM_TRIG_CONDITIONS )
        return conditions[index];
    else
        return junkCondition;
}

Action& Trigger::action(u8 index)
{
    if ( index < NUM_TRIG_ACTIONS )
        return actions[index];
    else
        return junkAction;
}

u8 Trigger::numUsedConditions()
{
    u8 total = 0;
    for ( u8 i=0; i<16; i++ )
    {
        if ( conditions[i].condition != (u8)ConditionId::NoCondition )
            total ++;
    }
    return total;
}

u8 Trigger::numUsedActions()
{
    u8 total = 0;
    for ( u8 i=0; i<64; i++ )
    {
        if ( actions[i].action != (u8)ActionId::NoAction )
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

bool Trigger::addCondition(const Condition &condition)
{
    for (u8 i = 0; i < NUM_TRIG_CONDITIONS; i++)
    {
        if (conditions[i].condition == (u8)ConditionId::NoCondition)
        {
            conditions[i] = condition;
            return true;
        }
    }
    return false;
}

bool Trigger::addAction(const Action &action)
{
    for (u8 i = 0; i < NUM_TRIG_ACTIONS; i++)
    {
        if (actions[i].action == (u8)ActionId::NoAction)
        {
            actions[i] = action;
            return true;
        }
    }
    return false;
}

void Trigger::deleteCondition(u8 index)
{
    if ( index < NUM_TRIG_CONDITIONS )
    {
        conditions[index].locationNum = 0;
        conditions[index].players = 0;
        conditions[index].amount = 0;
        conditions[index].unitID = 0;
        conditions[index].comparison = 0;
        conditions[index].condition = 0;
        conditions[index].typeIndex = 0;
        conditions[index].flags = 0;
        conditions[index].internalData = 0;
        alignTop();
    }
}

void Trigger::deleteAction(u8 index)
{
    if ( index < NUM_TRIG_ACTIONS )
    {
        actions[index].location = 0;
        actions[index].stringNum = 0;
        actions[index].wavID = 0;
        actions[index].time = 0;
        actions[index].group = 0;
        actions[index].number = 0;
        actions[index].type = 0;
        actions[index].action = 0;
        actions[index].type2 = 0;
        actions[index].flags = 0;
        actions[index].internalData[0] = 0;
        actions[index].internalData[1] = 0;
        actions[index].internalData[2] = 0;
        alignTop();
    }
}

void Trigger::alignTop()
{
    for ( u8 i=0; i<NUM_TRIG_CONDITIONS; i++ )
    {
        if ( conditions[i].condition == (u8)ConditionId::NoCondition )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<NUM_TRIG_CONDITIONS; j++ )
            {
                if ( conditions[j].condition != (u8)ConditionId::NoCondition )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<NUM_TRIG_CONDITIONS; j++ )
                    conditions[j] = conditions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}
