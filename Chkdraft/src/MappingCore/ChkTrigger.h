#ifndef TRIGGER_H
#define TRIGGER_H
#include "Basics.h"
#include "ChkCondition.h"
#include "ChkAction.h"

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
            #define EXECUTED_BY_PLAYER     1
            #define NOT_EXECUTED_BY_PLAYER 0

        Trigger();
        Condition& condition(u8 index);
        Action& action(u8 index);

        u8 numUsedConditions();
        u8 numUsedActions();
        bool preserveTriggerFlagged();
        bool disabled();
        bool ignoreConditionsOnce();
        bool ignoreWaitSkipOnce();
        bool ignoreMiscActionsOnce();
        bool ignoreDefeatDraw();
        bool flagPaused();

        void setPreserveTriggerFlagged(bool preserved);
        void setDisabled(bool disabled);
        void setIgnoreConditionsOnce(bool ignoreConditionsOnce);
        void setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce);
        void setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce);
        void setIgnoreDefeatDraw(bool ignoreDefeatDraw);
        void setFlagPaused(bool flagPaused);
        bool addCondition(const Condition &condition);
        bool addAction(const Action &action);
        void deleteCondition(u8 index);
        void deleteAction(u8 index);
        void alignTop(); // Moves all non-CID_NO_CONDITION conditions to top of array

        static Condition junkCondition;
        static Action junkAction;
};
#define TRIG_STRUCT_SIZE 2400

enum TrigPlayers
{
    Player1,
    Player2,
    Player3,
    Player4,
    Player5,
    Player6,
    Player7,
    Player8,
    Player9,
    Player10,
    Player11,
    Player12,
    None,
    CurrentPlayer,
    Foes,
    Allies,
    NeutralPlayers,
    AllPlayers,
    Force1,
    Force2,
    Force3,
    Force4,
    Unused1,
    Unused2,
    Unused3,
    Unused4,
    NonAvPlayers,
    ID27
};

#endif