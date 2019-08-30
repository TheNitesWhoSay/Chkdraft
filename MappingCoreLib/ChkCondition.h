#ifndef CONDITION_H
#define CONDITION_H
#include "Basics.h"
/*
enum class ConditionArgType;
enum class ConditionId;

#pragma pack(push, 1)
__declspec(align(1))
class Condition // 20 bytes, A trigger condition, as found in sets of 16 in a Trigger
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
        u16 internalData; // Number of which condition to process next?

        enum class Flags : u8
        {
            Disabled = BIT_1, // If set, the trigger condition/action is disabled/ignored
            AlwaysDisplay = BIT_2,
            UnitPropertiesUsed = BIT_3, // If set, unit properties is used
            UnitTypeUsed = BIT_4, // If set the unit type is used (may be unnecessary)
            UnitIdUsed = BIT_5, // If set, the unitId is used (unnecessary)

            xDisabled = x8BIT_1,
            xAlwaysDisplay = x8BIT_2,
            xUnitPropertiesUsed = x8BIT_3,
            xUnitTypeUsed = x8BIT_4,
            xUnitIdUsed = x8BIT_5
        };

        Condition();
        Condition(u8 condition);
        Condition(ConditionId condition);
        void ToggleDisabled();
        bool isDisabled();
        static ConditionArgType TextTrigArgType(u8 argNum, ConditionId condition);
        ConditionArgType TextTrigArgType(u8 argNum);
};
#pragma pack(pop)

enum class ConditionArgType
{
    CndNoType,
    CndUnit,
    CndLocation,
    CndPlayer,
    CndAmount,
    CndNumericComparison,
    CndResourceType,
    CndScoreType,
    CndSwitch,
    CndSwitchState,
    CndComparison, // NumericComparison, SwitchState
    CndConditionType,
    CndTypeIndex, // ResourceType, ScoreType, Switch
    CndFlags,
    CndInternalData
};

enum class ConditionId : s32
{
    Accumulate = 4,
    Always = 22,
    Bring = 3,
    Command = 2,
    CommandTheLeast = 16,
    CommandTheLeastAt = 17,
    CommandTheMost = 6,
    CommandTheMostAt = 7,
    CountdownTimer = 1,
    Deaths = 15,
    ElapsedTime = 12,
    HighestScore = 9,
    Kill = 5,
    LeastKills = 18,
    LeastResources = 20,
    LowestScore = 19,
    MostKills = 8,
    MostResources = 10,
    Never = 23,
    NeverAlt = 13,
    NoCondition = 0,
    Opponents = 14,
    Score = 21,
    Switch = 11,

    Custom = -1,
    Memory = -2,
};

enum class ExtendedConditionBase : u8
{
    Memory = (u8)ConditionId::Deaths,
};
*/
#endif