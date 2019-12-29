#include "Triggers.h"
#include "../../../Chkdraft.h"
#include "../../ChkdControls/MoveTo.h"
#include "../../../Mapping/Settings.h"
#include <string>

#undef PlaySound

#define TRIGGER_TOP_PADDING 1
#define TRIGGER_LEFT_PADDING 1
#define TRIGGER_RIGHT_PADDING 1
#define TRIGGER_BOTTOM_PADDING 1

#define STRING_TOP_PADDING 0
#define STRING_LEFT_PADDING 2
#define STRING_RIGHT_PADDING 1
#define STRING_BOTTOM_PADDING 2

#define NO_TRIGGER (u32(-1))

#define TRIGGER_NUM_PREFACE "\x12\x1A#"

enum class Id {
    BUTTON_NEW = ID_FIRST,
    BUTTON_MODIFY,
    BUTTON_DELETE,
    BUTTON_COPY,
    BUTTON_MOVEUP,
    BUTTON_MOVEDOWN,
    BUTTON_MOVETO,
    LIST_GROUPS,
    LIST_TRIGGERS
};

TriggersWindow::TriggersWindow() : currTrigger(NO_TRIGGER), displayAll(true), numVisibleTrigs(0),
    changeGroupHighlightOnly(false), trigListDC(NULL), drawingAll(false), textTrigGenerator(Settings::useAddressesForMemory, Settings::deathTableStart)
{
    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
        groupSelected[i] = false;
}

TriggersWindow::~TriggersWindow()
{

}

bool TriggersWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) == TRUE &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Triggers", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Triggers", WS_VISIBLE|WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TriggersWindow::DestroyThis()
{
    return false;
}

void TriggersWindow::RefreshWindow(bool focus)
{
    this->SetRedraw(false);
    RefreshGroupList();
    RefreshTrigList();
    if ( focus )
        listTriggers.FocusThis();

    if ( trigModifyWindow.getHandle() != NULL )
    {
        if ( numVisibleTrigs > 0 )
            trigModifyWindow.RefreshWindow(currTrigger);
        else
            trigModifyWindow.DestroyThis();
    }
    this->SetRedraw(true);
    this->RedrawThis();
}

void TriggersWindow::DoSize()
{
    RECT rcCli;
    if ( getClientRect(rcCli) )
    {
        long left = rcCli.left,
             top = rcCli.top,
             right = rcCli.right,
             bottom = rcCli.bottom,
             width = right - left,
             height = bottom - top,

             leftPadding = 5,
             topPadding = 5,
             rightPadding = 5,
             bottomPadding = 5,
             buttonPadding = 2,
             listPadding = 4,

             buttonX = right-buttonNew.Width()-rightPadding,
             leftWidth = buttonX-buttonPadding-leftPadding,
             listGroupsBottom = topPadding+listGroups.Height(),

             moveButtonsHeight = buttonMoveUp.Height()+buttonMoveDown.Height()+buttonMoveTo.Height()+buttonPadding*3+bottomPadding,
             buttonsHeight = buttonNew.Height()+buttonModify.Height()+buttonDelete.Height()+buttonCopy.Height()+buttonPadding*4+
                             moveButtonsHeight,

             triggerListHeight = height-listGroupsBottom-bottomPadding-listPadding;

        if ( buttonsHeight+listGroupsBottom > height )
        {
            listGroups.SetPos(leftPadding, topPadding, leftWidth, listGroups.Height());
            listTriggers.SetPos(leftPadding, 117, listGroups.Width(), triggerListHeight);
            buttonNew.MoveTo(buttonX, topPadding);
        }
        else
        {
            buttonNew.MoveTo(buttonX, listGroups.Bottom()+buttonPadding);
            listGroups.SetPos(leftPadding, topPadding, buttonNew.Right()-leftPadding, listGroups.Height());
            listTriggers.SetPos(leftPadding, 117, buttonNew.Left()-buttonPadding-leftPadding, triggerListHeight);
        }

        buttonModify.MoveTo(buttonX, buttonNew.Bottom()+buttonPadding );
        buttonDelete.MoveTo(buttonX, buttonModify.Bottom()+buttonPadding );
        buttonCopy.MoveTo(buttonX, buttonDelete.Bottom()+buttonPadding );
        
        long remainingButtonSpace = height-buttonCopy.Bottom();

        if ( moveButtonsHeight > remainingButtonSpace )
        {
            buttonMoveUp.MoveTo(buttonX, buttonCopy.Bottom()+buttonPadding);
            buttonMoveDown.MoveTo(buttonX, buttonMoveUp.Bottom()+buttonPadding);
            buttonMoveTo.MoveTo(buttonX, buttonMoveDown.Bottom()+buttonPadding);
        }
        else
        {
            buttonMoveTo.MoveTo(buttonX, bottom-bottomPadding-buttonMoveTo.Height());
            buttonMoveDown.MoveTo(buttonX, buttonMoveTo.Top()-buttonPadding-buttonMoveDown.Height());
            buttonMoveUp.MoveTo(buttonX, buttonMoveDown.Top()-buttonPadding-buttonMoveUp.Height());
        }
    }
}

void TriggersWindow::DeleteSelection()
{
    if ( currTrigger != NO_TRIGGER )
    {
        CM->triggers.deleteTrigger(currTrigger);
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        int sel;
        RefreshGroupList();
        if ( listTriggers.GetCurSel(sel) && DeleteTrigListItem(sel) && (SelectTrigListItem(sel) || SelectTrigListItem(sel-1)) )
        {
            listTriggers.RedrawThis();
        }
        else
            RefreshTrigList();
    }
}

void TriggersWindow::CopySelection()
{
    if ( currTrigger != NO_TRIGGER )
    {
        CM->triggers.insertTrigger(currTrigger+1,
            std::shared_ptr<Chk::Trigger>(new Chk::Trigger(*(CM->triggers.getTrigger(currTrigger)))));

        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        int sel;
        if ( listTriggers.GetCurSel(sel) && CopyTrigListItem(sel) && SelectTrigListItem(sel+1) )
            RefreshTrigList();
        else
            RefreshTrigList();
    }
}

void TriggersWindow::MoveUp()
{
    int sel;
    u32 prevTrigIndex;
    if ( currTrigger != NO_TRIGGER &&
         listTriggers.GetCurSel(sel) &&
         sel > 0 &&
         listTriggers.GetItemData(sel-1, prevTrigIndex) )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->triggers.moveTrigger(currTrigger, prevTrigIndex);
        if ( MoveUpTrigListItem(sel, prevTrigIndex) )
        {
            SelectTrigListItem(sel-1);
            listTriggers.RedrawThis();
        }
        else
            RefreshTrigList(); // Restore trigger list to a valid state
    }
}

void TriggersWindow::MoveDown()
{
    int sel;
    u32 nextTrigIndex;
    if ( currTrigger != NO_TRIGGER &&
         listTriggers.GetCurSel(sel) &&
         sel < int(numVisibleTrigs) &&
         listTriggers.GetItemData(sel+1, nextTrigIndex) )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->triggers.moveTrigger(currTrigger, nextTrigIndex);
        if ( MoveDownTrigListItem(sel, nextTrigIndex) )
        {
            SelectTrigListItem(sel+1);
            listTriggers.RedrawThis();
        }
        else
            RefreshTrigList(); // Restore trigger list to a valid state
    }
}

void TriggersWindow::MoveTrigTo()
{
    int sel;
    u32 targetTrigIndex;
    if ( currTrigger != NO_TRIGGER &&
         listTriggers.GetCurSel(sel) &&
         MoveToDialog<u32>::GetIndex(targetTrigIndex, getHandle()) &&
         targetTrigIndex >= 0 &&
         targetTrigIndex != currTrigger &&
         targetTrigIndex < CM->triggers.numTriggers() )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->triggers.moveTrigger(currTrigger, targetTrigIndex);
        int listIndexMovedTo = -1;
        listTriggers.SetRedraw(false);
        if ( MoveTrigListItemTo(sel, currTrigger, targetTrigIndex, listIndexMovedTo) )
        {
            SelectTrigListItem(listIndexMovedTo);
            listTriggers.SetRedraw(true);
            listTriggers.RedrawThis();
        }
        else
        {
            currTrigger = targetTrigIndex;
            RefreshTrigList();
        }
    }
}

void TriggersWindow::ButtonNew()
{
    Chk::TriggerPtr trigger = Chk::TriggerPtr(new Chk::Trigger());
    for ( u8 i=(u8)Sc::Player::Id::Player1; i<=(u8)Sc::Player::Id::Player8; i++ )
    {
        if ( groupSelected[i] )
            trigger->owners[i] = Chk::Trigger::Owned::Yes;
    }

    u32 newTrigId = 0;
    int sel;
    if ( listTriggers.GetCurSel(sel) && sel != numVisibleTrigs-1 )
    {
        if ( listTriggers.GetItemData(sel, newTrigId) )
        {
            newTrigId ++;
            CM->triggers.insertTrigger(newTrigId, trigger);
        }
    }
    else
    {
        CM->triggers.addTrigger(trigger);
        newTrigId = u32(CM->triggers.numTriggers()-1);
    }

    CM->notifyChange(false);
    currTrigger = newTrigId;
    RefreshWindow(true);
    ButtonModify();
}

void TriggersWindow::ButtonModify()
{
    if ( currTrigger != NO_TRIGGER )
    {
        if ( trigModifyWindow.getHandle() == NULL )
        {
            trigModifyWindow.CreateThis(getHandle(), currTrigger);
            SetFocus(trigModifyWindow.getHandle());
        }
        else
        {
            trigModifyWindow.RefreshWindow(currTrigger);
            ShowWindow(trigModifyWindow.getHandle(), SW_SHOW);
            SetFocus(trigModifyWindow.getHandle());
        }
    }
}

std::string TriggersWindow::GetConditionString(u8 conditionNum, Chk::Trigger* trigger, TextTrigGenerator& tt)
{
    std::stringstream ssCondition;
    Chk::Condition &condition = trigger->condition(conditionNum);
    Chk::Condition::Type conditionType = (Chk::Condition::Type)condition.conditionType;
    if ( condition.isDisabled() )
        ssCondition << "(disabled) ";

    switch ( conditionType )
    {
        case Chk::Condition::Type::Accumulate: // Players, Comparison, Amount, TypeIndex
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C accumulates \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
                << "\x0C \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::Always:
            ssCondition << "Always";
            break;
        case Chk::Condition::Type::Bring: // Players, Comparison, Amount, UnitID, Location
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C brings \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C to \'\x08"
                << tt.GetTrigLocation(condition.locationId) << "\x0C\'.";
            break;
        case Chk::Condition::Type::Command: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C commands \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheLeast: // UnitID
            ssCondition << "Current player commands the least \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheLeastAt: // UnitID, Location
            ssCondition << "Current player commands the least \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C at \x08"
                << tt.GetTrigLocation(condition.locationId) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheMost: // UnitID
            ssCondition << "Current player commands the most \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheMostAt: // UnitID, Location
            ssCondition << "Current player commands the most \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C at \x08"
                << tt.GetTrigLocation(condition.locationId) << "\x0C.";
            break;
        case Chk::Condition::Type::CountdownTimer: // Comparison, Amount
            ssCondition << "Countdown timer is \x08" << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C game seconds.";
            break;
        case Chk::Condition::Type::Deaths: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C has suffered \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
                << "\x0C deaths of \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::ElapsedTime: // Comparison, Amount
            ssCondition << "Elapsed scenario time is \x08" << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C game seconds.";
            break;
        case Chk::Condition::Type::HighestScore: // TypeIndex
            ssCondition << "Current player has highest score \x08" << tt.GetTrigScoreType(condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::Kill: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C kills \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::LeastKills: // UnitID
            ssCondition << "Current player has least kills of \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::LeastResources: // TypeIndex
            ssCondition << "Current player has least \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::LowestScore: // TypeIndex
            ssCondition << "Current player has lowest score \x08" << tt.GetTrigScoreType(condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::MostKills: // UnitID
            ssCondition << "Current player has the most kills of \x08" << tt.GetTrigUnit((u16)condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::MostResources: // TypeIndex
            ssCondition << "Current player has most \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::Never:
            ssCondition << "Never";
            break;
        case Chk::Condition::Type::IsBriefing:
            ssCondition << "Never (alt)";
            break;
        case Chk::Condition::Type::NoCondition:
            ssCondition << "(No condition)";
            break;
        case Chk::Condition::Type::Opponents: // Players, Comparison, Amount
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C has \x08"
                << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
                << "\x0C opponents remaining in the game.";
            break;
        case Chk::Condition::Type::Score: // Players, TypeIndex, Comparison, Amount
            ssCondition << '\x08' << tt.GetTrigPlayer(condition.player) << "\x0C \x08" << tt.GetTrigScoreType(condition.typeIndex)
                << "\x0C score is \x08" << tt.GetTrigNumericComparison((u8)condition.comparison) << "\x0C \x08"
                << tt.GetTrigNumber(condition.amount) << "\x0C.";
            break;
        case Chk::Condition::Type::Switch: // TypeIndex, Comparison
            ssCondition << "\'\x08" << tt.GetTrigSwitch(condition.typeIndex) << "\x0C\' is \x08"
                << tt.GetTrigSwitchState((u8)condition.comparison) << "\x0C.";
            break;
        default: // Location, Player, Amount, Unit, NumericComparison, Condition, TypeIndex, Flags, Internal
            ssCondition << "Condition: \x08" << condition.locationId << "\x0C, \x08" << condition.player << "\x0C, \x08"
                << condition.amount << "\x0C, \x08" << (u16)condition.unitType << "\x0C, \x08" << u16((u8)condition.comparison)
                << "\x0C, \x08" << u16(condition.conditionType) << "\x0C, \x08" << u16(condition.typeIndex) << "\x0C, \x08"
                << u16(condition.flags) << "\x0C, \x08" << (u16)condition.maskFlag << '\x0C';
            break;
    }
    return ssCondition.str();
}

std::string TriggersWindow::GetActionString(u8 actionNum, Chk::Trigger* trigger, TextTrigGenerator& tt)
{
    std::stringstream ssAction;
    Chk::Action&action = trigger->action(actionNum);
    Chk::Action::Type actionType = action.actionType;
    if ( action.isDisabled() )
        ssAction << "(disabled) ";

    switch ( actionType )
    {
        case Chk::Action::Type::CenterView: // Location
            ssAction << "Center view for current player at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::Comment: // String
            ssAction << "Comment:\x08" << tt.GetTrigString(action.stringId) << "\x0C";
            break;
        case Chk::Action::Type::CreateUnit: // Type2, Type, Location, Players
            ssAction << "Create \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\' for \x08"
                << tt.GetTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::CreateUnitWithProperties: // Type2, Type, Location, Players, Number
            ssAction << "Create \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\' for \x08"
                << tt.GetTrigPlayer(action.group) << "\x0C. Apply \x08properties\x0C.";
            break;
        case Chk::Action::Type::Defeat:
            ssAction << "End scenario in defeat for current player.";
            break;
        case Chk::Action::Type::DisplayTextMessage: // String
            ssAction << "Display for current player:\x08" << tt.GetTrigString(action.stringId) << '\x0C';
            break;
        case Chk::Action::Type::Draw:
            ssAction << "End the scenario in a draw for all players.";
            break;
        case Chk::Action::Type::GiveUnitsToPlayer: // Type2, Type, Players, Location, Number
            ssAction << "Give \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.GetTrigPlayer(action.number) << "\x0C.";
            break;
        case Chk::Action::Type::KillUnit: // Type, Players
            ssAction << "Kill all \x08" << tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::KillUnitAtLocation: // Type2, Type, Players, Location
            ssAction << "Kill \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\'.";
            break;
        case Chk::Action::Type::LeaderboardCtrlAtLoc: // Type, Location, String
            ssAction << "Show Leader Board for most control of \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
                << tt.GetTrigLocation(action.locationId) << "\x0C\'. Display label: \'\x08" << tt.GetTrigString(action.stringId)
                << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardCtrl: // Type, String
            ssAction << "Show Leader Board for most control of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
                << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGreed: // Number
            ssAction << "Show Greed Leader Board for player closest to accumulation of \x08" << tt.GetTrigNumber(action.number)
                << "\x0C ore and gas.";
            break;
        case Chk::Action::Type::LeaderboardKills: // Type, String
            ssAction << "Show Leader Board for most kills of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
                << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardPoints: // Type, String
            ssAction << "Show Leader Board for most \x08" << tt.GetTrigScoreType(action.type) << "\x0C. Display label: \'\x08"
                << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardResources: // Type, String
            ssAction << "Show Leader board for accumulation of most \x08" << tt.GetTrigResourceType(action.type)
                << "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardCompPlayers: // Type2
            ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C use of computer players in leaderboard calculations.";
            break;
        case Chk::Action::Type::LeaderboardGoalCtrlAtLoc: // Number, Type, Location, String
            ssAction << "Show Leader Board for player closest to control of \x08" << tt.GetTrigNumber(action.number) << "\x0C of \x08"
                << tt.GetTrigUnit(action.type) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\'. Display label: \'\x08" << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalCtrl: // Number, Type, String
            ssAction << "Show Leader Board for player closest to control of \x08" << tt.GetTrigNumber(action.number)
                << "\x0C of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
                << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalKills: // Number, Type, String
            ssAction << "Show Leader Board for player closest to \x08" << tt.GetTrigNumber(action.number) << "\x0C kills of \x08"
                << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalPoints: // Number, Type, String
            ssAction << "Show Leader Board for player closest to \x08" << tt.GetTrigNumber(action.number) << "\x0C \x08"
                << tt.GetTrigScoreType(action.type) << "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalResources: // Number, Type, String
            ssAction << "Show Leader Board for player closest to accumulation of \x08" << tt.GetTrigNumber(action.number)
                << "\x0C \x08" << tt.GetTrigResourceType(action.type) << "\x0C. Display label: \'\x08"
                << tt.GetTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::MinimapPing: // Location
            ssAction << "Show minimap ping for current player at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\'";
            break;
        case Chk::Action::Type::ModifyUnitEnergy: // Type2, Type, Players, Location, Number
            ssAction << "Set energy points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.GetTrigNumber(action.number) << "\x0C%.";
            break;
        case Chk::Action::Type::ModifyUnitHangerCount: // Number, Type2, Type, Location, Players
            ssAction << "Add at most \x08" << tt.GetTrigNumber(action.number) << "\x0C to hanger for \x08"
                << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
                << tt.GetTrigLocation(action.locationId) << "\x0C\' owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::ModifyUnitHitpoints: // Type2, Type, Players, Location, Number
            ssAction << "Set hit points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08"
                << tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group)
                << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\' to \x08"
                << tt.GetTrigNumber(action.number) << "\x0C%";
            break;
        case Chk::Action::Type::ModifyUnitResourceAmount: // Type2, Players, Location, Number
            ssAction << "Set resource amount for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C resource sources owned by \x08"
                << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\' to \x08"
                << tt.GetTrigNumber(action.number) << "\x0C.";
            break;
        case Chk::Action::Type::ModifyUnitShieldPoints: // Type2, Type, Players, Location, Number
            ssAction << "Set shield points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.GetTrigNumber(action.number) << "\x0C%.";
            break;
        case Chk::Action::Type::MoveLocation: // Number, Type, Players, Location
            ssAction << "Center location labeled \'\x08" << tt.GetTrigLocation(action.number) << "\x0C\' on \x08"
                << tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08"
                << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::MoveUnit: // Type2, Type, Players, Location, Number
            ssAction << "Move \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.number)
                << "\x0C\' to \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::MuteUnitSpeech:
            ssAction << "Mute all non-trigger unit sounds for current player.";
            break;
        case Chk::Action::Type::NoAction:
            ssAction << "(No action)";
            break;
        case Chk::Action::Type::Order: // Type, Players, Location, Type2, Number
            ssAction << "Issue order to all \x08" << tt.GetTrigUnit(action.type) << "\x0C owned by \x08"
                << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\':\x08"
                << tt.GetTrigOrder(action.type2) << "\x0C to \'\x08" << tt.GetTrigLocation(action.number) << "\x0C\'.";
            break;
        case Chk::Action::Type::PauseGame:
            ssAction << "Pause the game.";
            break;
        case Chk::Action::Type::PauseTimer:
            ssAction << "Pause the countdown timer.";
            break;
        case Chk::Action::Type::PlaySound: // Wav, Duration
            ssAction << "Play \'\x08" << tt.GetTrigWav(action.soundStringId) << "\x0C\'.";
            break;
        case Chk::Action::Type::PreserveTrigger:
            ssAction << "Preserve trigger.";
            break;
        case Chk::Action::Type::RemoveUnit: // Type, Players
            ssAction << "Remove all \x08" << tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::RemoveUnitAtLocation: // Type2, Type, Players, Location
            ssAction << "Remove \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
                << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId)
                << "\x0C\'.";
            break;
        case Chk::Action::Type::RunAiScript: // Number
            {
                std::string aiName = "";
                if ( chkd.scData.aiScripts.GetAiName(action.number, aiName) )
                    ssAction << "Execute AI script \'\x08" << aiName << " (" << tt.GetTrigScript(action.number) << ")\x0C\'.";
                else
                    ssAction << "Execute AI script \'\x08" << tt.GetTrigScript(action.number) << "\x0C\'.";
            }
            break;
        case Chk::Action::Type::RunAiScriptAtLocation: // Number, Location
            {
                std::string aiName = "";
                if ( chkd.scData.aiScripts.GetAiName(action.number, aiName) )
                {
                    ssAction << "Execute AI script \'\x08" << aiName << " (" << tt.GetTrigScript(action.number) << ")\x0C\' at \'\x08"
                        << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
                }
                else
                {
                    ssAction << "Execute AI script \'\x08" << tt.GetTrigScript(action.number) << "\x0C\' at \'\x08"
                        << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
                }
            }
            break;
        case Chk::Action::Type::SetAllianceStatus: // Players, Type
            ssAction << "Set \x08" << tt.GetTrigPlayer(action.group) << "\x0C to \x08" << tt.GetTrigAllyState(action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetCountdownTimer: // Type2, Duration
            ssAction << "Modify Countdown Timer: \x08" << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08"
                << tt.GetTrigNumber(action.time) << "\x0C seconds.";
            break;
        case Chk::Action::Type::SetDeaths: // Players, Type2, Number, Type
            ssAction << "Modify death counts for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
                << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
                << "\x0C for \x08" << tt.GetTrigUnit(action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetDoodadState: // Type2, Type, Players, Location
            ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C doodad state for \x08"
                << tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08"
                << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::SetInvincibility: // Type2, Type, Players, Location
            ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C invincibility for \x08"
                << tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group)
                << "\x0C at \'\x08" << tt.GetTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::SetMissionObjectives: // String
            ssAction << "Set Mission Objectives to:\x08" << tt.GetTrigString(action.stringId) << '\x0C';
            break;
        case Chk::Action::Type::SetNextScenario: // String
            ssAction << "Load \'\x08" << tt.GetTrigString(action.stringId) << "\x0C\' after completion of current game.";
            break;
        case Chk::Action::Type::SetResources: // Players, Type2, Number, Type
            ssAction << "Modify resources for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
                << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
                << "\x0C \x08" << tt.GetTrigResourceType(action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetScore: // Players, Type2, Number, Type
            ssAction << "Modify score for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
                << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
                << "\x0C \x08" << tt.GetTrigScoreType(action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetSwitch: // Type2, Number
            ssAction << '\x08' << tt.GetTrigSwitchModifier(action.type2) << "\x0C \'\x08" << tt.GetTrigSwitch(action.number) << "\x0C\'.";
            break;
        case Chk::Action::Type::TalkingPortrait: // Type, Duration
            ssAction << "Show \x08" << tt.GetTrigUnit(action.type) << "\x0C talking to current player for \x08"
                << tt.GetTrigNumber(action.time) << "\x0C miliseconds.";
            break;
        case Chk::Action::Type::Transmission: // Type, Location, Wav, Type2, Number, String
            ssAction << "Send transmission to current player from \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
                << tt.GetTrigLocation(action.locationId) << "\x0C\'. Play '\x08" << tt.GetTrigWav(action.soundStringId)
                << "\x0C\'. Modify duration: \x08" << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08"
                << tt.GetTrigNumber(action.number) << "\x0C miliseconds. Display text:\x08"
                << tt.GetTrigString(action.stringId) << '\x0C';
            break;
        case Chk::Action::Type::UnmuteUnitSpeech:
            ssAction << "Unmute all non-trigger unit sounds for current player.";
            break;
        case Chk::Action::Type::UnpauseGame:
            ssAction << "Unpause the game.";
            break;
        case Chk::Action::Type::UnpauseTimer:
            ssAction << "Unpause the countdown timer.";
            break;
        case Chk::Action::Type::Victory:
            ssAction << "End scenario in victory for current player.";
            break;
        case Chk::Action::Type::Wait: // Duration
            ssAction << "Wait for \x08" << tt.GetTrigNumber(action.time) << "\x0C miliseconds.";
            break;//*/
        default: // Location, String, Wav, Duration, Player, Number, Type, Action, Type2, Flags, Internal
            ssAction << "Action: \x08" << action.locationId << "\x0C, \x08" << action.stringId << "\x0C, \x08" << action.soundStringId
                << "\x0C, \x08" << action.time << "\x0C, \x08" << action.group << "\x0C, \x08" << action.number
                << "\x0C, \x08" << action.type << "\x0C, \x08" << u16(action.type2) << "\x0C, \x08" << u16(action.flags)
                << "\x0C, \x08" << (u16)action.padding << "\x0C, \x08" << (u16)action.maskFlag << "\x0C.";
            break;
    }
    return ssAction.str();
}

std::string TriggersWindow::GetTriggerString(u32 trigNum, Chk::Trigger* trigger, TextTrigGenerator& tt)
{
    bool more = false;
    std::stringstream ssTrigger;
    u8 remainingLines = 13;
    size_t numConditions = trigger->numUsedConditions();
    size_t numActions = trigger->numUsedActions();
    if ( numConditions == 0 && numActions == 0 )
        ssTrigger << "<EMPTY>" << TRIGGER_NUM_PREFACE << trigNum << "\x0C\r\n";
    else
    {
        if ( numConditions > 0 )
        {
            ssTrigger << "CONDITIONS:" << TRIGGER_NUM_PREFACE << trigNum << "\x0C\r\n";
            u8 i=0;
            while ( numConditions > 0 && i < 16 )
            {
                if ( trigger->condition(i).conditionType != Chk::Condition::Type::NoCondition )
                {
                    ssTrigger << ' ' << GetConditionString(i, trigger, textTrigGenerator) << "\r\n";

                    remainingLines --;
                    if ( remainingLines == 0 )
                    {
                        if ( numConditions > 1 )
                            more = true;
                        break;
                    }
                    numConditions --;
                }
                i++;
            }
        }
        if ( numActions > 0 && remainingLines > 0 )
        {
            if ( remainingLines == 13 )
                ssTrigger << "ACTIONS:" << TRIGGER_NUM_PREFACE << trigNum << "\x0C\r\n";
            else
                ssTrigger << "ACTIONS:\r\n";

            u8 i=0;
            while ( numActions > 0 && i < 64 )
            {
                if ( trigger->action(i).actionType != Chk::Action::Type::NoAction )
                {
                    ssTrigger << ' ' << GetActionString(i, trigger, textTrigGenerator) << "\r\n";

                    remainingLines --;
                    if ( remainingLines == 0 )
                    {
                        if ( numActions > 1 )
                            more = true;
                        break;
                    }
                    numActions --;
                }
                i++;
            }
        }
        if ( more )
            ssTrigger << "\x13(MORE)" << std::endl;
    }
    return ssTrigger.str();
}

void TriggersWindow::CreateSubWindows(HWND hWnd)
{
    buttonNew.CreateThis(hWnd, 0, 0, 75, 23, "New", (u64)Id::BUTTON_NEW);
    buttonModify.CreateThis(hWnd, 0, 25, 75, 23, "Modify", (u64)Id::BUTTON_MODIFY);
    buttonDelete.CreateThis(hWnd, 0, 50, 75, 23, "Delete", (u64)Id::BUTTON_DELETE);
    buttonCopy.CreateThis(hWnd, 0, 75, 75, 23, "Copy", (u64)Id::BUTTON_COPY);
    
    buttonMoveUp.CreateThis(hWnd, 0, 100, 75, 23, "Move Up", (u64)Id::BUTTON_MOVEUP);
    buttonMoveDown.CreateThis(hWnd, 0, 125, 75, 23, "Move Down", (u64)Id::BUTTON_MOVEDOWN);
    buttonMoveTo.CreateThis(hWnd, 0, 150, 75, 23, "Move To", (u64)Id::BUTTON_MOVETO);

    listGroups.CreateThis(hWnd, 0, 0, 200, 116, true, true, false, false, (u64)Id::LIST_GROUPS);
    if ( listTriggers.CreateThis(hWnd, 0, 120, 200, 150, true, false, false, false, (u64)Id::LIST_TRIGGERS) )
        listGroups.SetPeer(listTriggers.getHandle());

    DoSize();
    RefreshWindow(true);
}

void TriggersWindow::RefreshGroupList()
{
    listGroups.SetRedraw(false);
    listGroups.ClearItems();

    u8 firstNotFound = 0;
    bool addedPlayer[Chk::Trigger::MaxOwners];
    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
        addedPlayer[i] = false;

    if ( CM != nullptr )
    {
        size_t numTriggers = CM->triggers.numTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            Chk::TriggerPtr trigger = CM->triggers.getTrigger(i);
            if ( trigger != nullptr )
            {
                for ( u8 player=firstNotFound; player<Chk::Trigger::MaxOwners; player++ )
                {
                    if ( !addedPlayer[player] && trigger->owners[player] != Chk::Trigger::Owned::No )
                    {
                        addedPlayer[player] = true;
                        if ( player == firstNotFound ) // Skip already-found players
                            firstNotFound ++;
                    }
                }
            }
        }
        int selectAllIndex = -1;
        for ( u8 i=0; i<12; i++ ) // Players
        {
            if ( addedPlayer[i] )
                listGroups.AddItem(i);
        }
        for ( u8 i=18; i<22; i++ ) // Forces
        {
            if ( addedPlayer[i] )
                listGroups.AddItem(i);
        }
        if ( addedPlayer[17] ) // All Players
            listGroups.AddItem(17);
        if ( numTriggers > 0 ) // Show All
            selectAllIndex = listGroups.AddItem(Chk::Trigger::MaxOwners);
        for ( u8 i=12; i<17; i++ ) // Lower unused groups
        {
            if ( addedPlayer[i] )
                listGroups.AddItem(i);
        }
        for ( u8 i=22; i<Chk::Trigger::MaxOwners; i++ ) // Upper unused groups
        {
            if ( addedPlayer[i] )
                listGroups.AddItem(i);
        }
        changeGroupHighlightOnly = true; // Begin selection
        if ( displayAll )
            listGroups.SelectItem(selectAllIndex);
        for ( u8 i=0; i<28; i++ )
        {
            if ( groupSelected[i] )
                listGroups.SelectItem(i);
        }
        changeGroupHighlightOnly = false; // End selection
    }

    listGroups.SetRedraw(true);
}

void TriggersWindow::RefreshTrigList()
{
    listTriggers.SetRedraw(false);
    listTriggers.ClearItems();
    numVisibleTrigs = 0;
    int toSelect = -1;

    if ( CM != nullptr )
    {
        size_t numTriggers = CM->triggers.numTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            Chk::TriggerPtr trigger = CM->triggers.getTrigger(i);
            if ( trigger != nullptr && ShowTrigger(&(*trigger)) )
            {
                int newListIndex = listTriggers.AddItem((u32)i);
                if ( newListIndex != -1 ) // Only consider the trigger if it could be added to the ListBox
                {
                    numVisibleTrigs ++;
                    if ( currTrigger == i ) // This trigger is the currTrigger
                        toSelect = newListIndex; // Mark position for selection
                }
            }
        }
    }

    listTriggers.SetRedraw(true);
    if ( toSelect == -1 || !listTriggers.SetCurSel(toSelect) ) // Attempt selection
    {
        currTrigger = NO_TRIGGER; // Clear currTrigger if selection fails
        if ( trigModifyWindow.getHandle() != NULL )
            trigModifyWindow.DestroyThis();
    }
}

bool TriggersWindow::SelectTrigListItem(int item)
{
    if ( numVisibleTrigs > 0 && listTriggers.SetCurSel(item) && listTriggers.GetItemData(item, currTrigger) )
    {
        trigModifyWindow.RefreshWindow(currTrigger);
        return true;
    }
    else
    {
        listTriggers.ClearSel();
        currTrigger = NO_TRIGGER;
        trigModifyWindow.DestroyThis();
        return false;
    }
}

bool TriggersWindow::DeleteTrigListItem(int item)
{
    bool removedItem = false;
    u32 data;
    bool success = true;
    int totalItems = listTriggers.GetNumItems();
    for ( int i=item+1; i<totalItems; i++ ) // Attempt to decrement the trigger index of items with higher list indexes
    {
        if ( !( listTriggers.GetItemData(i, data) && listTriggers.SetItemData(i, data-1) ) ) // Decrement trigger index
        {
            success = false; // Decrementing failed
            break;
        }
    }

    if ( success && listTriggers.RemoveItem(item) ) // Decrement succeeded, delete the target item
    {
        currTrigger = NO_TRIGGER; // Ensure currTrigger is cleared (until selection is updated)
        numVisibleTrigs --; // Update amount of visible triggers
        return true;
    }
    else
        return false;
}

bool TriggersWindow::CopyTrigListItem(int item)
{
    u32 data;
    bool success = true;
    int totalItems = listTriggers.GetNumItems();
    for ( int i=item+1; i<totalItems; i++ ) // Attempt to increment the trigger index of items with higher list indexes
    {
        if ( !( listTriggers.GetItemData(i, data) && listTriggers.SetItemData(i, data+1) ) ) // Increment trigger index
        {
            success = false; // Incrementing failed
            break;
        }
    }

    if ( success && listTriggers.InsertItem(item+1, currTrigger+1) ) // Increment succeeded, insert the copy item
    {
        currTrigger = NO_TRIGGER; // Ensure currTrigger is cleared (until selection is updated)
        numVisibleTrigs ++; // Update amount of visible triggers
        return true;
    }
    else
        return false;
}

bool TriggersWindow::MoveUpTrigListItem(int listIndex, u32 prevTrigIndex)
{
    currTrigger = prevTrigIndex;
    int prevItemHeight, currItemHeight;
    return listTriggers.GetItemHeight(listIndex-1, prevItemHeight) && // Get heights
           listTriggers.GetItemHeight(listIndex, currItemHeight) &&
           listTriggers.SetItemData(listIndex, prevTrigIndex+1) && // Adjust stored indexes
           listTriggers.SetItemHeight(listIndex-1, currItemHeight) && // Swap heights
           listTriggers.SetItemHeight(listIndex, prevItemHeight);
}

bool TriggersWindow::MoveDownTrigListItem(int listIndex, u32 nextTrigIndex)
{
    currTrigger = nextTrigIndex;
    int nextItemHeight, currItemHeight;
    return listTriggers.GetItemHeight(listIndex+1, nextItemHeight) && // Get heights
           listTriggers.GetItemHeight(listIndex, currItemHeight) &&
           listTriggers.SetItemData(listIndex, nextTrigIndex-1) && // Adjust stored indexes
           listTriggers.SetItemHeight(listIndex+1, currItemHeight) && // Swap heights
           listTriggers.SetItemHeight(listIndex, nextItemHeight);
}

bool TriggersWindow::MoveTrigListItemTo(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int &listIndexMovedTo)
{
    listIndexMovedTo = -1;
    int targetListIndex = -1;
    if ( FindTargetListIndex(currListIndex, currTrigIndex, targetTrigIndex, targetListIndex) )
    {
        u32 listItemData;
        int listItemHeight,
            preservedHeight;

        if ( targetListIndex < currListIndex )
        {
            if ( !listTriggers.GetItemHeight(currListIndex, preservedHeight) )
                return false;

            for ( int i=currListIndex; i>=targetListIndex; i-- )
            {
                if ( !( listTriggers.GetItemData(i-1, listItemData) &&
                        listTriggers.SetItemData(i, listItemData+1) &&
                        listTriggers.GetItemHeight(i-1, listItemHeight) &&
                        listTriggers.SetItemHeight(i, listItemHeight) ) )
                {
                    return false;
                }
            }

            if ( listTriggers.SetItemData(targetListIndex, targetTrigIndex) &&
                 listTriggers.SetItemHeight(targetListIndex, preservedHeight) )
            {
                listIndexMovedTo = targetListIndex;
                return true;
            }
            else
                return false;
        }
        else if ( targetListIndex > currListIndex )
        {
            if ( !listTriggers.GetItemHeight(currListIndex, preservedHeight) )
                return false;

            for ( int i=currListIndex; i<=targetListIndex; i++ )
            {
                if ( !( listTriggers.GetItemData(i+1, listItemData) &&
                        listTriggers.SetItemData(i, listItemData-1) &&
                        listTriggers.GetItemHeight(i+1, listItemHeight) &&
                        listTriggers.SetItemHeight(i, listItemHeight) ) )
                {
                    return false;
                }
            }

            if ( listTriggers.SetItemData(targetListIndex, targetTrigIndex) &&
                 listTriggers.SetItemHeight(targetListIndex, preservedHeight) )
            {
                listIndexMovedTo = targetListIndex;
                return true;
            }
        }
        else // targetListIndex == currListIndex
        {
            listIndexMovedTo = targetListIndex;
            return currTrigIndex == targetTrigIndex ||
                   listTriggers.SetItemData(targetListIndex, targetTrigIndex);
        }
    }
    return false;
}

bool TriggersWindow::FindTargetListIndex(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int &targetListIndex)
{
    u32 listItemData;
    if ( targetTrigIndex < currTrigIndex )
    {
        for ( int i=currListIndex; i>0; i-- )
        {
            if ( !listTriggers.GetItemData(i, listItemData) )
                return false;
            else if ( listItemData <= targetTrigIndex )
            {
                targetListIndex = i;
                return true;
            }
        }
        targetListIndex = 0;
        return true;
    }
    else if ( targetTrigIndex > currTrigIndex )
    {
        int numListIndexes = listTriggers.GetNumItems();
        for ( int i=currListIndex; i<numListIndexes; i++ )
        {
            if ( !listTriggers.GetItemData(i, listItemData) )
                return false;
            if ( listItemData == targetTrigIndex )
            {
                targetListIndex = i;
                return true;
            }
            else if ( listItemData > targetTrigIndex )
            {
                targetListIndex = i-1;
                return true;
            }
        }
        targetListIndex = numListIndexes-1;
        return true;
    }
    else // targetTrigIndex == currTrigIndex
    {
        targetListIndex = currListIndex;
        return true;
    }
}

bool TriggersWindow::ShowTrigger(Chk::Trigger* trigger)
{
    if ( displayAll )
        return true;

    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
    {
        if ( groupSelected[i] && trigger->owners[i] != Chk::Trigger::Owned::No )
            return true;
    }

    return false;
}

void TriggersWindow::ClearGroups()
{
    for ( u8 i=0; i<28; i++ )
        groupSelected[i] = false;
}

bool TriggersWindow::GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, ScenarioPtr chk, u32 triggerNum, Chk::Trigger* trigger)
{
    size_t commentStringId = trigger->getComment();
    if ( commentStringId != Chk::StringId::NoString )
    {
        RawStringPtr str = chk->strings.getString<RawString>(commentStringId);
        if ( str != nullptr )
        {
            size_t endOfLine = str->find("\r\n");
            if ( endOfLine != std::string::npos )
                str->insert(endOfLine, (std::string(TRIGGER_NUM_PREFACE) + std::to_string(triggerNum) + '\x0C'));
            else
                str->append((std::string(TRIGGER_NUM_PREFACE) + std::to_string(triggerNum) + '\x0C'));

            if ( GetStringDrawSize(hDC, width, height, *str) )
            {
                width += TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
                height += TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;
            }
        }
    }
    else
    {
        RawString str = GetTriggerString(triggerNum, trigger, textTrigGenerator);
        HGDIOBJ sel = SelectObject(hDC, defaultFont);
        if ( sel != NULL && sel != HGDI_ERROR )
        {
            width = 0;
            height = TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;

            UINT strWidth, strHeight;
            if ( GetStringDrawSize(hDC, strWidth, strHeight, str) )
            {
                UINT newWidth = strWidth+TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
                if ( newWidth > width )
                    width = newWidth;

                height += strHeight;
                return true;
            }
        }
    }
    return false;
}

void TriggersWindow::DrawGroup(HDC hDC, RECT &rcItem, bool isSelected, u8 groupNum)
{
    HBRUSH hBackground = NULL;
    if ( isSelected )
        hBackground = CreateSolidBrush(RGB(51, 153, 255));
    else
        hBackground = CreateSolidBrush(RGB(255, 255, 255));

    if ( hBackground != NULL )
    {
        FillRect(hDC, &rcItem, hBackground);
        DeleteObject(hBackground);
    }

    SetBkMode(hDC, TRANSPARENT);
    if ( groupNum < 29 )
    {
        std::string groupNames[] = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6",
                                     "Player 7", "Player 8", "Player 9 (unused)", "Player 10 (unused)",
                                     "Player 11 (unused)", "Player 12 (unused)",
                                     "unknown/unused", "Current Player (unused)", "Foes (unused)", "Allies (unused)", "Neutral Players (unused)",
                                     "All players", "Force 1", "Force 2", "Force 3", "Force 4",
                                     "unknown/unused", "unknown/unused", "unknown/unused", "unknown/unused",
                                     "Non AV Players (unused)", "unknown/unused", "Show All" };

        DrawString(hDC, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
            rcItem.right-rcItem.left, RGB(0, 0, 0), groupNames[groupNum]);
    }
    else
    {
        std::stringstream ssGroup;
        ssGroup << "Group: " << u16(groupNum);
        DrawString(hDC, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
            rcItem.right-rcItem.left, RGB(0, 0, 0), ssGroup.str());
    }
}

void TriggersWindow::DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, ScenarioPtr chk, u32 triggerNum, Chk::Trigger* trigger)
{
    HBRUSH hBackground = CreateSolidBrush(RGB(171, 171, 171)); // Same color as in WM_CTLCOLORLISTBOX
    if ( hBackground != NULL )
    {
        FillRect(hDC, &rcItem, hBackground); // Draw far background
        DeleteObject(hBackground);
        hBackground = NULL;
    }

    if ( isSelected )
        hBackground = CreateSolidBrush(RGB(51, 153, 255));
    else
        hBackground = CreateSolidBrush(RGB(253, 246, 208));
    
    if ( hBackground != NULL )
    {
        RECT rcNote;
        rcNote.left = rcItem.left+TRIGGER_LEFT_PADDING;
        rcNote.top = rcItem.top+TRIGGER_TOP_PADDING;
        rcNote.right = rcItem.right-TRIGGER_RIGHT_PADDING;
        rcNote.bottom = rcItem.bottom-TRIGGER_BOTTOM_PADDING;

        FillRect(hDC, &rcNote, hBackground); // Draw specific trigger's background
        if ( isSelected )
            DrawFocusRect(hDC, &rcNote);

        DeleteObject(hBackground);
    }

    LONG left = rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING;
    size_t commentStringId = trigger->getComment();
    if ( commentStringId != Chk::StringId::NoString )
    {
        RawStringPtr str = chk->strings.getString<RawString>(commentStringId);
        if ( str != nullptr )
        {
            std::string num(std::to_string(triggerNum));
            size_t endOfLine = str->find("\r\n");
            if ( endOfLine != std::string::npos )
                str->insert(endOfLine, std::string(TRIGGER_NUM_PREFACE) + num + '\x0C');
            else
                str->append(std::string(TRIGGER_NUM_PREFACE) + num + '\x0C');

            SetBkMode(hDC, TRANSPARENT);
            DrawString(hDC, left, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
                rcItem.right-left-TRIGGER_RIGHT_PADDING-STRING_RIGHT_PADDING, RGB(0, 0, 0), *str);
        }
    }
    else
    {
        RawString str = GetTriggerString(triggerNum, trigger, textTrigGenerator);
        SetBkMode(hDC, TRANSPARENT);
        DrawString(hDC, left, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
            rcItem.right-left-TRIGGER_RIGHT_PADDING-STRING_RIGHT_PADDING, RGB(0, 0, 0), str);
    }
}

LRESULT TriggersWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_SELCHANGE:
        if ( (Id)LOWORD(wParam) == Id::LIST_TRIGGERS ) // Change selection, update info boxes and so fourth
        {
            int sel;
            if ( !(listTriggers.GetCurSel(sel) && sel != -1 && listTriggers.GetItemData(sel, currTrigger)) )
                currTrigger = NO_TRIGGER;
            else if ( trigModifyWindow.getHandle() != NULL )
                trigModifyWindow.RefreshWindow(currTrigger);
        }
        else if ( (Id)LOWORD(wParam) == Id::LIST_GROUPS )
        {
            currTrigger = NO_TRIGGER;
            if ( changeGroupHighlightOnly )
                return ClassWindow::Command(hWnd, wParam, lParam);

            int numSel = listGroups.GetNumSel();
            displayAll = false;
            ClearGroups();
            for ( int i = 0; i<numSel; i++ )
            {
                int selItem;
                if ( listGroups.GetSelItem(i, selItem) )
                {
                    if ( selItem == 28 )
                    {
                        ClearGroups();
                        displayAll = true;
                    }
                    else
                        groupSelected[selItem] = true;
                }
            }
            RefreshTrigList();
        }
    case LBN_KILLFOCUS: // List box item may have lost focus, check if anything should be updated
        if ( (Id)LOWORD(wParam) == Id::LIST_TRIGGERS )
        {

        }
        else if ( (Id)LOWORD(wParam) == Id::LIST_GROUPS )
        {

        }
    case BN_CLICKED:
        if ( (Id)LOWORD(wParam) == Id::BUTTON_NEW )
            ButtonNew();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_MODIFY )
            ButtonModify();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_DELETE )
            DeleteSelection();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_COPY )
            CopySelection();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_MOVEUP )
            MoveUp();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_MOVEDOWN )
            MoveDown();
        else if ( (Id)LOWORD(wParam) == Id::BUTTON_MOVETO )
            MoveTrigTo();
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT TriggersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
                RefreshWindow(true);
            else if ( wParam == FALSE )
            {
                // Perhaps update details of the current trigger
            }
            break;

        case (UINT)WinLib::LB::WM_DBLCLKITEM:
            if ( listTriggers == (HWND)lParam )
                ButtonModify();
            break;

        case WM_MOUSEWHEEL:
            SendMessage(listTriggers.getHandle(), WM_MOUSEWHEEL, wParam, lParam);
            break;

        case (UINT)WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all triggers once
            if ( this != nullptr )
            {
                textTrigGenerator.LoadScenario(CM);
                trigListDC = listTriggers.getDC();
            }
            break;

        case WM_MEASUREITEM:
            if ( (Id)wParam == Id::LIST_TRIGGERS )
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                u32 triggerNum = (u32)mis->itemData;
                
                Chk::TriggerPtr trigger = CM->triggers.getTrigger(triggerNum);
                if ( trigger != nullptr )
                    GetTriggerDrawSize(trigListDC, mis->itemWidth, mis->itemHeight, CM, triggerNum, &(*trigger));
                
                return TRUE;
            }
            else if ( (Id)wParam == Id::LIST_GROUPS )
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                u32 listIndex = (u32)mis->itemData;
                mis->itemWidth = 120;
                mis->itemHeight = 13;
                return TRUE;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case (UINT)WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listTriggers.ReleaseDC(trigListDC);
            trigListDC = NULL;
            textTrigGenerator.ClearScenario();
            break;

        case WM_CTLCOLORLISTBOX:
            if ( listTriggers == (HWND)lParam )
            {
                HBRUSH hBrush = CreateSolidBrush(RGB(171, 171, 171));
                if ( hBrush != NULL )
                    return (LPARAM)hBrush;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case (UINT)WinLib::LB::WM_PREDRAWITEMS:
            textTrigGenerator.LoadScenario(CM);
            drawingAll = true;
            break;

        case WM_DRAWITEM:
            if ( (Id)wParam == Id::LIST_TRIGGERS )
            {
                if ( !drawingAll )
                    textTrigGenerator.LoadScenario(CM);

                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                {
                    u32 triggerNum = (u32)pdis->itemData;
                    
                    Chk::TriggerPtr trigger = CM->triggers.getTrigger(triggerNum);
                    if ( CM != nullptr && trigger != nullptr )
                        DrawTrigger(pdis->hDC, pdis->rcItem, isSelected, CM, triggerNum, &(*trigger));
                }

                if ( !drawingAll )
                    textTrigGenerator.ClearScenario();

                return TRUE;
            }
            else if ( (Id)wParam == Id::LIST_GROUPS )
            {
                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                    DrawGroup(pdis->hDC, pdis->rcItem, isSelected, u8(u32(pdis->itemData)));

                return TRUE;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case (UINT)WinLib::LB::WM_POSTDRAWITEMS:
            drawingAll = false;
            textTrigGenerator.ClearScenario();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
