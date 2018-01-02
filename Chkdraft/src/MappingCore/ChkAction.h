#ifndef CHKACTION_H
#define CHKACTION_H
#include "Basics.h"

enum class ActionArgType;
enum class ActionId;

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
        u8 internalData[3]; // Number of actions to process next?
        
        enum Flags {
            Unknown_0 = BIT_0, Disabled = BIT_1, AlwaysDisplay = BIT_2, UnitPropertiesUsed = BIT_3,
            UnitTypeUsed = BIT_4, UnitIdUsed = BIT_5 /* Probably Unnecessary*/, Unknown_6 = BIT_6, Unknown_7 = BIT_7,
            xAlwaysDisplay = x8BIT_2, xDisabled = x8BIT_1
        };

        Action();
        Action(u8 action);
        Action(ActionId action);
        void ToggleDisabled();
        bool isDisabled();
        static ActionArgType TextTrigArgType(u8 argNum, ActionId actionId);
};

enum class ActionArgType
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

enum class ActionId : s32
{
    CenterView = 10,
    Comment = 47,
    CreateUnit = 44,
    CreateUnitWithProperties = 11,
    Defeat = 2,
    DisplayTextMessage = 9,
    Draw = 56,
    GiveUnitsToPlayer = 48,
    KillUnit = 22,
    KillUnitAtLocation = 23,
    LeaderboardCtrlAtLoc = 18,
    LeaderboardCtrl = 17,
    LeaderboardGreed = 40,
    LeaderboardKills = 20,
    LeaderboardPoints = 21,
    LeaderboardResources = 19,
    LeaderboardCompPlayers = 32,
    LeaderboardGoalCtrlAtLoc = 34,
    LeaderboardGoalCtrl = 33,
    LeaderboardGoalKills = 36,
    LeaderboardGoalPoints = 37,
    LeaderboardGoalResources = 35,
    MinimapPing = 28,
    ModifyUnitEnergy = 50,
    ModifyUnitHangerCount = 53,
    ModifyUnitHitpoints = 49,
    ModifyUnitResourceAmount = 52,
    ModifyUnitShieldPoints = 51,
    MoveLocation = 38,
    MoveUnit = 39,
    MuteUnitSpeech = 30,
    NoAction = 0,
    Order = 46,
    PauseGame = 5,
    PauseTimer = 54,
    PlayWav = 8,
    PreserveTrigger = 3,
    RemoveUnit = 24,
    RemoveUnitAtLocation = 25,
    RunAiScript = 15,
    RunAiScriptAtLocation = 16,
    SetAllianceStatus = 57,
    SetCountdownTimer = 14,
    SetDeaths = 45,
    SetDoodadState = 42,
    SetInvincibility = 43,
    SetMissionObjectives = 12,
    SetNextScenario = 41,
    SetResources = 26,
    SetScore = 27,
    SetSwitch = 13,
    TalkingPortrait = 29,
    Transmission = 7,
    UnmuteUnitSpeech = 31,
    UnpauseGame = 6,
    UnpauseTimer = 55,
    Victory = 1,
    Wait = 4,

    Custom = -1,
    Memory = -2
};

enum class ExtendedActionBase : s32
{
    Memory = (u8)ActionId::SetDeaths,
};

#endif