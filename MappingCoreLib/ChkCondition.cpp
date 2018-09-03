#include "ChkCondition.h"

Condition::Condition() : locationNum(0), players(0), amount(0), unitID(0), comparison(0), condition(0), typeIndex(0), flags(0), internalData(0)
{

}

Condition::Condition(u8 condition) : locationNum(0), players(0), amount(0), unitID(0), comparison(0), condition(condition), typeIndex(0), flags(0), internalData(0)
{

}

Condition::Condition(ConditionId condition) : locationNum(0), players(0), amount(0), unitID(0), comparison(0), condition((u8)condition), typeIndex(0), flags(0), internalData(0)
{

}

void Condition::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

bool Condition::isDisabled()
{
    return (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled;
}

ConditionArgType Condition::TextTrigArgType(u8 argNum, ConditionId condition)
{
    switch ( condition )
    {
        case ConditionId::Accumulate:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndNumericComparison; break;
                case 2: return ConditionArgType::CndAmount; break;
                case 3: return ConditionArgType::CndResourceType; break;
            }
            break;
        case ConditionId::Always: break;
        case ConditionId::Bring:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndUnit; break;
                case 2: return ConditionArgType::CndLocation; break;
                case 3: return ConditionArgType::CndNumericComparison; break;
                case 4: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::Command:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndUnit; break;
                case 2: return ConditionArgType::CndNumericComparison; break;
                case 3: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::CommandTheLeast:
            if ( argNum == 0 )
                return ConditionArgType::CndUnit;
            break;
        case ConditionId::CommandTheLeastAt:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndUnit; break;
                case 1: return ConditionArgType::CndLocation; break;
            }
            break;
        case ConditionId::CommandTheMost:
            if ( argNum == 0 )
                return ConditionArgType::CndUnit;
            break;
        case ConditionId::CommandTheMostAt:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndUnit; break;
                case 1: return ConditionArgType::CndLocation; break;
            }
            break;
        case ConditionId::CountdownTimer:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndNumericComparison; break;
                case 1: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::Deaths:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndUnit; break;
                case 2: return ConditionArgType::CndNumericComparison; break;
                case 3: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::ElapsedTime:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndNumericComparison; break;
                case 1: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::HighestScore:
            if ( argNum == 0 )
                return ConditionArgType::CndScoreType;
            break;
        case ConditionId::Kill:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndUnit; break;
                case 2: return ConditionArgType::CndNumericComparison; break;
                case 3: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::LeastKills:
            if ( argNum == 0 )
                return ConditionArgType::CndUnit;
            break;
        case ConditionId::LeastResources:
            if ( argNum == 0 )
                return ConditionArgType::CndResourceType;
            break;
        case ConditionId::LowestScore:
            if ( argNum == 0 )
                return ConditionArgType::CndScoreType;
            break;
        case ConditionId::MostKills:
            if ( argNum == 0 )
                return ConditionArgType::CndUnit;
            break;
        case ConditionId::MostResources:
            if ( argNum == 0 )
                return ConditionArgType::CndResourceType;
            break;
        case ConditionId::Never: break;
        case ConditionId::NeverAlt: break;
        case ConditionId::Opponents:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndNumericComparison; break;
                case 2: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::Score:
            switch ( argNum ) {
                case 0: return ConditionArgType::CndPlayer; break;
                case 1: return ConditionArgType::CndScoreType; break;
                case 2: return ConditionArgType::CndNumericComparison; break;
                case 3: return ConditionArgType::CndAmount; break;
            }
            break;
        case ConditionId::Switch:
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
    return TextTrigArgType(argNum, (ConditionId)condition);
}
