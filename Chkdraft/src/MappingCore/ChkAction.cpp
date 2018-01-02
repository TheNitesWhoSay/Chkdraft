#include "ChkAction.h"

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
u8 internalData[3]; // Number of actions to process next?

Action::Action() : location(0), stringNum(0), wavID(0), time(0), group(0), number(0), type(0), action(0), type2(0), flags(0)
{
    memset(internalData, 0, 3);
}

Action::Action(u8 action) : location(0), stringNum(0), wavID(0), time(0), group(0), number(0), type(0), action(action), type2(0), flags(0)
{
    memset(internalData, 0, 3);
}

Action::Action(ActionId action) : location(0), stringNum(0), wavID(0), time(0), group(0), number(0), type(0), action((u8)action), type2(0), flags(0)
{
    memset(internalData, 0, 3);
}

void Action::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

bool Action::isDisabled()
{
    return (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled;
}

ActionArgType Action::TextTrigArgType(u8 argNum, ActionId actionId)
{
    switch ( actionId )
    {
        case ActionId::CenterView:
            if ( argNum == 0 )
                return ActionArgType::ActLocation;
            break;
        case ActionId::Comment:
            if ( argNum == 0 )
                return ActionArgType::ActString;
            break;
        case ActionId::CreateUnit:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::CreateUnitWithProperties:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
                case 4: return ActionArgType::ActCUWP; break;
            }
            break;
        case ActionId::Defeat: break;
        case ActionId::DisplayTextMessage:
            switch ( argNum ) {
                case 0: return ActionArgType::ActTextFlags; break;
                case 1: return ActionArgType::ActString; break;
            }
            break;
        case ActionId::Draw: break;
        case ActionId::GiveUnitsToPlayer:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActPlayer; break;
                case 2: return ActionArgType::ActUnit; break;
                case 3: return ActionArgType::ActNumUnits; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::KillUnit:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
            }
            break;
        case ActionId::KillUnitAtLocation:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::LeaderboardCtrlAtLoc:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::LeaderboardCtrl:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
            }
            break;
        case ActionId::LeaderboardGreed:
            if ( argNum == 0 )
                return ActionArgType::ActAmount;
            break;
        case ActionId::LeaderboardKills:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
            }
            break;
        case ActionId::LeaderboardPoints:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActScoreType; break;
            }
            break;
        case ActionId::LeaderboardResources:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActResourceType; break;
            }
            break;
        case ActionId::LeaderboardCompPlayers:
            if ( argNum == 0 )
                return ActionArgType::ActStateMod;
            break;
        case ActionId::LeaderboardGoalCtrlAtLoc:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActAmount; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::LeaderboardGoalCtrl:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActAmount; break;
            }
            break;
        case ActionId::LeaderboardGoalKills:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActAmount; break;
            }
            break;
        case ActionId::LeaderboardGoalPoints:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActScoreType; break;
                case 2: return ActionArgType::ActAmount; break;
            }
            break;
        case ActionId::LeaderboardGoalResources:
            switch ( argNum ) {
                case 0: return ActionArgType::ActString; break;
                case 1: return ActionArgType::ActAmount; break;
                case 2: return ActionArgType::ActResourceType; break;
            }
            break;
        case ActionId::MinimapPing:
            if ( argNum == 0 )
                return ActionArgType::ActLocation;
            break;
        case ActionId::ModifyUnitEnergy:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActPercent; break;
                case 3: return ActionArgType::ActNumUnits; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::ModifyUnitHangerCount:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActAmount; break;
                case 3: return ActionArgType::ActNumUnits; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::ModifyUnitHitpoints:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActPercent; break;
                case 3: return ActionArgType::ActNumUnits; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::ModifyUnitResourceAmount:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActAmount; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::ModifyUnitShieldPoints:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActPercent; break;
                case 3: return ActionArgType::ActNumUnits; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::MoveLocation:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActLocation; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::MoveUnit:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
                case 4: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::MuteUnitSpeech: break;
        case ActionId::NoAction: break;
        case ActionId::Order:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActLocation; break;
                case 3: return ActionArgType::ActLocation; break;
                case 4: return ActionArgType::ActOrder; break;
            }
            break;
        case ActionId::PauseGame: break;
        case ActionId::PauseTimer: break;
        case ActionId::PlayWav:
            switch ( argNum ) {
                case 0: return ActionArgType::ActWav; break;
                case 1: return ActionArgType::ActDuration; break;
            }
            break;
        case ActionId::PreserveTrigger: break;
        case ActionId::RemoveUnit:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
            }
            break;
        case ActionId::RemoveUnitAtLocation:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumUnits; break;
                case 3: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::RunAiScript:
            if ( argNum == 0 )
                return ActionArgType::ActScript; break;
            break;
        case ActionId::RunAiScriptAtLocation:
            switch ( argNum ) {
                case 0: return ActionArgType::ActScript; break;
                case 1: return ActionArgType::ActLocation; break;
            }
            break;
        case ActionId::SetAllianceStatus:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActAllyState; break;
            }
            break;
        case ActionId::SetCountdownTimer:
            switch ( argNum ) {
                case 0: return ActionArgType::ActNumericMod; break;
                case 1: return ActionArgType::ActAmount; break;
            }
            break;
        case ActionId::SetDeaths:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActNumericMod; break;
                case 3: return ActionArgType::ActAmount; break;
            }
            break;
        case ActionId::SetDoodadState:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActLocation; break;
                case 3: return ActionArgType::ActStateMod; break;
            }
            break;
        case ActionId::SetInvincibility:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActUnit; break;
                case 2: return ActionArgType::ActLocation; break;
                case 3: return ActionArgType::ActStateMod; break;
            }
            break;
        case ActionId::SetMissionObjectives:
            if ( argNum == 0 )
                return ActionArgType::ActString;
            break;
        case ActionId::SetNextScenario:
            if ( argNum == 0 )
                return ActionArgType::ActString;
            break;
        case ActionId::SetResources:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActNumericMod; break;
                case 2: return ActionArgType::ActAmount; break;
                case 3: return ActionArgType::ActResourceType; break;
            }
            break;
        case ActionId::SetScore:
            switch ( argNum ) {
                case 0: return ActionArgType::ActPlayer; break;
                case 1: return ActionArgType::ActNumericMod; break;
                case 2: return ActionArgType::ActAmount; break;
                case 3: return ActionArgType::ActScoreType; break;
            }
            break;
        case ActionId::SetSwitch:
            switch ( argNum ) {
                case 0: return ActionArgType::ActSwitch; break;
                case 1: return ActionArgType::ActSwitchMod; break;
            }
            break;
        case ActionId::TalkingPortrait:
            switch ( argNum ) {
                case 0: return ActionArgType::ActUnit; break;
                case 1: return ActionArgType::ActDuration; break;
            }
            break;
        case ActionId::Transmission:
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
        case ActionId::UnmuteUnitSpeech: break;
        case ActionId::UnpauseGame: break;
        case ActionId::UnpauseTimer: break;
        case ActionId::Victory: break;
        case ActionId::Wait:
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
