#include "triggers.h"
#include "chkdraft/chkdraft.h"
#include "ui/chkd_controls/move_to.h"
#include "mapping/settings.h"
#include "mapping/sc_graphics.h"
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

enum_t(Id, u32, {
    BUTTON_NEW = ID_FIRST,
    BUTTON_MODIFY,
    BUTTON_DELETE,
    BUTTON_COPY,
    BUTTON_MOVEUP,
    BUTTON_MOVEDOWN,
    BUTTON_MOVETO,
    LIST_GROUPS,
    LIST_TRIGGERS
});

TriggersWindow::TriggersWindow() : currTrigger(NO_TRIGGER), displayAll(true), numVisibleTrigs(0),
    changeGroupHighlightOnly(false), trigListDc(std::nullopt), drawingAll(false), textTrigGenerator(Settings::useAddressesForMemory, Settings::deathTableStart, true),
    countTillCachePurge(0)
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
    ClassWindow::DestroyThis();
    this->currTrigger = NO_TRIGGER;
    this->displayAll = true;
    this->numVisibleTrigs = 0;
    this->changeGroupHighlightOnly = false;
    this->trigListDc = std::nullopt;
    this->drawingAll = false;
    return true;
}

void TriggersWindow::RefreshWindow(bool focus)
{
    if ( getHandle() == NULL )
        return;

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
        size_t numTriggers = CM->numTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            const Chk::Trigger & trigger = CM->getTrigger(i);
            for ( u8 player=firstNotFound; player<Chk::Trigger::MaxOwners; player++ )
            {
                if ( !addedPlayer[player] && trigger.owners[player] == Chk::Trigger::Owned::Yes )
                {
                    addedPlayer[player] = true;
                    if ( player == firstNotFound ) // Skip already-found players
                        firstNotFound ++;
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
        CM->deleteTrigger(currTrigger);
        CM->deleteUnusedStrings(Chk::Scope::Both);
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
        CM->insertTrigger(currTrigger+1, CM->getTrigger(currTrigger));
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
    LPARAM prevTrigIndex = 0;
    if ( currTrigger != NO_TRIGGER &&
         listTriggers.GetCurSel(sel) &&
         sel > 0 &&
         listTriggers.GetItemData(sel-1, prevTrigIndex) )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->moveTrigger(currTrigger, size_t(prevTrigIndex));
        if ( MoveUpTrigListItem(sel, u32(prevTrigIndex)) )
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
    LPARAM nextTrigIndex = 0;
    if ( currTrigger != NO_TRIGGER &&
         listTriggers.GetCurSel(sel) &&
         sel < int(numVisibleTrigs) &&
         listTriggers.GetItemData(sel+1, nextTrigIndex) )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->moveTrigger(currTrigger, size_t(nextTrigIndex));
        if ( MoveDownTrigListItem(sel, u32(nextTrigIndex)) )
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
         targetTrigIndex < CM->numTriggers() )
    {
        trigModifyWindow.DestroyThis();
        CM->notifyChange(false);
        CM->moveTrigger(currTrigger, targetTrigIndex);
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
    Chk::Trigger trigger {};
    for ( u8 i=Sc::Player::Id::Player1; i<=Sc::Player::Id::Player8; i++ )
    {
        if ( groupSelected[i] )
            trigger.owners[i] = Chk::Trigger::Owned::Yes;
    }

    LPARAM newTrigId = 0;
    int sel;
    if ( listTriggers.GetCurSel(sel) && sel != numVisibleTrigs-1 )
    {
        if ( listTriggers.GetItemData(sel, newTrigId) )
        {
            newTrigId ++;
            CM->insertTrigger(size_t(newTrigId), trigger);
        }
    }
    else
    {
        CM->addTrigger(trigger);
        newTrigId = LPARAM(CM->numTriggers()-1);
    }

    CM->notifyChange(false);
    currTrigger = u32(newTrigId);
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

std::string TriggersWindow::GetConditionString(u8 conditionNum, const Chk::Trigger & trigger, TextTrigGenerator & tt)
{
    std::stringstream ssCondition;
    const Chk::Condition & condition = trigger.condition(conditionNum);
    Chk::Condition::Type conditionType = condition.conditionType;
    if ( condition.isDisabled() )
        ssCondition << "(disabled) ";

    switch ( conditionType )
    {
        case Chk::Condition::Type::Accumulate: // Players, Comparison, Amount, TypeIndex
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C accumulates \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.getTrigNumber(condition.amount)
                << "\x0C \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::Always:
            ssCondition << "Always";
            break;
        case Chk::Condition::Type::Bring: // Players, Comparison, Amount, UnitID, Location
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C brings \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C \x08" << tt.getTrigUnit(condition.unitType) << "\x0C to \'\x08"
                << tt.getTrigLocation(condition.locationId) << "\x0C\'.";
            break;
        case Chk::Condition::Type::Command: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C commands \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheLeast: // UnitID
            ssCondition << "Current player commands the least \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheLeastAt: // UnitID, Location
            ssCondition << "Current player commands the least \x08" << tt.getTrigUnit(condition.unitType) << "\x0C at \x08"
                << tt.getTrigLocation(condition.locationId) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheMost: // UnitID
            ssCondition << "Current player commands the most \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::CommandTheMostAt: // UnitID, Location
            ssCondition << "Current player commands the most \x08" << tt.getTrigUnit(condition.unitType) << "\x0C at \x08"
                << tt.getTrigLocation(condition.locationId) << "\x0C.";
            break;
        case Chk::Condition::Type::CountdownTimer: // Comparison, Amount
            ssCondition << "Countdown timer is \x08" << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C game seconds.";
            break;
        case Chk::Condition::Type::Deaths: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C has suffered \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.getTrigNumber(condition.amount)
                << "\x0C deaths of \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::ElapsedTime: // Comparison, Amount
            ssCondition << "Elapsed scenario time is \x08" << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C game seconds.";
            break;
        case Chk::Condition::Type::HighestScore: // TypeIndex
            ssCondition << "Current player has highest score \x08" << tt.getTrigScoreType((Chk::Trigger::ScoreType)condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::Kill: // Players, Comparison, Amount, UnitID
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C kills \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::LeastKills: // UnitID
            ssCondition << "Current player has least kills of \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::LeastResources: // TypeIndex
            ssCondition << "Current player has least \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::LowestScore: // TypeIndex
            ssCondition << "Current player has lowest score \x08" << tt.getTrigScoreType((Chk::Trigger::ScoreType)condition.typeIndex) << "\x0C.";
            break;
        case Chk::Condition::Type::MostKills: // UnitID
            ssCondition << "Current player has the most kills of \x08" << tt.getTrigUnit(condition.unitType) << "\x0C.";
            break;
        case Chk::Condition::Type::MostResources: // TypeIndex
            ssCondition << "Current player has most \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)condition.typeIndex) << "\x0C.";
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
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C has \x08"
                << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.getTrigNumber(condition.amount)
                << "\x0C opponents remaining in the game.";
            break;
        case Chk::Condition::Type::Score: // Players, TypeIndex, Comparison, Amount
            ssCondition << '\x08' << tt.getTrigPlayer(condition.player) << "\x0C \x08" << tt.getTrigScoreType((Chk::Trigger::ScoreType)condition.typeIndex)
                << "\x0C score is \x08" << tt.getTrigNumericComparison(condition.comparison) << "\x0C \x08"
                << tt.getTrigNumber(condition.amount) << "\x0C.";
            break;
        case Chk::Condition::Type::Switch: // TypeIndex, Comparison
            ssCondition << "\'\x08" << tt.getTrigSwitch(condition.typeIndex) << "\x0C\' is \x08"
                << tt.getTrigSwitchState((Chk::Trigger::ValueModifier)condition.comparison) << "\x0C.";
            break;
        default: // Location, Player, Amount, Unit, NumericComparison, Condition, TypeIndex, Flags, Internal
            ssCondition << "Condition: \x08" << condition.locationId << "\x0C, \x08" << condition.player << "\x0C, \x08"
                << condition.amount << "\x0C, \x08" << condition.unitType << "\x0C, \x08" << u16(condition.comparison)
                << "\x0C, \x08" << u16(condition.conditionType) << "\x0C, \x08" << u16(condition.typeIndex) << "\x0C, \x08"
                << u16(condition.flags) << "\x0C, \x08" << condition.maskFlag << '\x0C';
            break;
    }
    return ssCondition.str();
}

std::string TriggersWindow::GetActionString(u8 actionNum, const Chk::Trigger & trigger, TextTrigGenerator & tt)
{
    std::stringstream ssAction;
    const Chk::Action & action = trigger.action(actionNum);
    Chk::Action::Type actionType = action.actionType;
    if ( action.isDisabled() )
        ssAction << "(disabled) ";

    switch ( actionType )
    {
        case Chk::Action::Type::CenterView: // Location
            ssAction << "Center view for current player at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::Comment: // String
            ssAction << "Comment:\x08" << tt.getTrigString(action.stringId) << "\x0C";
            break;
        case Chk::Action::Type::CreateUnit: // Type2, Type, Location, Players
            ssAction << "Create \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\' for \x08"
                << tt.getTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::CreateUnitWithProperties: // Type2, Type, Location, Players, Number
            ssAction << "Create \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\' for \x08"
                << tt.getTrigPlayer(action.group) << "\x0C. Apply \x08properties\x0C.";
            break;
        case Chk::Action::Type::Defeat:
            ssAction << "End scenario in defeat for current player.";
            break;
        case Chk::Action::Type::DisableDebugMode:
            ssAction << "Disable debug mode if in single-player.";
            break;
        case Chk::Action::Type::DisplayTextMessage: // String
            ssAction << "Display for current player:\x08" << tt.getTrigString(action.stringId) << '\x0C';
            break;
        case Chk::Action::Type::Draw:
            ssAction << "End the scenario in a draw for all players.";
            break;
        case Chk::Action::Type::EnableDebugMode:
            ssAction << "Enable debug mode if in single-player.";
            break;
        case Chk::Action::Type::GiveUnitsToPlayer: // Type2, Type, Players, Location, Number
            ssAction << "Give \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C owned by \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.getTrigPlayer(action.number) << "\x0C.";
            break;
        case Chk::Action::Type::KillUnit: // Type, Players
            ssAction << "Kill all \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::KillUnitAtLocation: // Type2, Type, Players, Location
            ssAction << "Kill \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\'.";
            break;
        case Chk::Action::Type::LeaderboardCtrlAtLoc: // Type, Location, String
            ssAction << "Show Leader Board for most control of \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C at \'\x08"
                << tt.getTrigLocation(action.locationId) << "\x0C\'. Display label: \'\x08" << tt.getTrigString(action.stringId)
                << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardCtrl: // Type, String
            ssAction << "Show Leader Board for most control of \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C. Display label: \'\x08"
                << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGreed: // Number
            ssAction << "Show Greed Leader Board for player closest to accumulation of \x08" << tt.getTrigNumber(action.number)
                << "\x0C ore and gas.";
            break;
        case Chk::Action::Type::LeaderboardKills: // Type, String
            ssAction << "Show Leader Board for most kills of \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C. Display label: \'\x08"
                << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardPoints: // Type, String
            ssAction << "Show Leader Board for most \x08" << tt.getTrigScoreType((Chk::Trigger::ScoreType)action.type) << "\x0C. Display label: \'\x08"
                << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardResources: // Type, String
            ssAction << "Show Leader board for accumulation of most \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)action.type)
                << "\x0C. Display label: \'\x08" << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardCompPlayers: // Type2
            ssAction << '\x08' << tt.getTrigStateModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C use of computer players in leaderboard calculations.";
            break;
        case Chk::Action::Type::LeaderboardGoalCtrlAtLoc: // Number, Type, Location, String
            ssAction << "Show Leader Board for player closest to control of \x08" << tt.getTrigNumber(action.number) << "\x0C of \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\'. Display label: \'\x08" << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalCtrl: // Number, Type, String
            ssAction << "Show Leader Board for player closest to control of \x08" << tt.getTrigNumber(action.number)
                << "\x0C of \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C. Display label: \'\x08"
                << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalKills: // Number, Type, String
            ssAction << "Show Leader Board for player closest to \x08" << tt.getTrigNumber(action.number) << "\x0C kills of \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C. Display label: \'\x08" << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalPoints: // Number, Type, String
            ssAction << "Show Leader Board for player closest to \x08" << tt.getTrigNumber(action.number) << "\x0C \x08"
                << tt.getTrigScoreType((Chk::Trigger::ScoreType)action.type) << "\x0C. Display label: \'\x08" << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::LeaderboardGoalResources: // Number, Type, String
            ssAction << "Show Leader Board for player closest to accumulation of \x08" << tt.getTrigNumber(action.number)
                << "\x0C \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)action.type) << "\x0C. Display label: \'\x08"
                << tt.getTrigString(action.stringId) << "\x0C\'";
            break;
        case Chk::Action::Type::MinimapPing: // Location
            ssAction << "Show minimap ping for current player at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\'";
            break;
        case Chk::Action::Type::ModifyUnitEnergy: // Type2, Type, Players, Location, Number
            ssAction << "Set energy points for \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C owned by \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.getTrigNumber(action.number) << "\x0C%.";
            break;
        case Chk::Action::Type::ModifyUnitHangarCount: // Number, Type2, Type, Location, Players
            ssAction << "Add at most \x08" << tt.getTrigNumber(action.number) << "\x0C to hangar for \x08"
                << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C at \'\x08"
                << tt.getTrigLocation(action.locationId) << "\x0C\' owned by \x08" << tt.getTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::ModifyUnitHitpoints: // Type2, Type, Players, Location, Number
            ssAction << "Set hit points for \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C owned by \x08" << tt.getTrigPlayer(action.group)
                << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\' to \x08"
                << tt.getTrigNumber(action.number) << "\x0C%";
            break;
        case Chk::Action::Type::ModifyUnitResourceAmount: // Type2, Players, Location, Number
            ssAction << "Set resource amount for \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C resource sources owned by \x08"
                << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\' to \x08"
                << tt.getTrigNumber(action.number) << "\x0C.";
            break;
        case Chk::Action::Type::ModifyUnitShieldPoints: // Type2, Type, Players, Location, Number
            ssAction << "Set shield points for \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C owned by \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\' to \x08" << tt.getTrigNumber(action.number) << "\x0C%.";
            break;
        case Chk::Action::Type::MoveLocation: // Number, Type, Players, Location
            ssAction << "Center location labeled \'\x08" << tt.getTrigLocation(action.number) << "\x0C\' on \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C owned by \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08"
                << tt.getTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::MoveUnit: // Type2, Type, Players, Location, Number
            ssAction << "Move \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.number)
                << "\x0C\' to \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::MuteUnitSpeech:
            ssAction << "Mute all non-trigger unit sounds for current player.";
            break;
        case Chk::Action::Type::NoAction:
            ssAction << "(No action)";
            break;
        case Chk::Action::Type::Order: // Type, Players, Location, Type2, Number
            ssAction << "Issue order to all \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C owned by \x08"
                << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\':\x08"
                << tt.getTrigOrder((Chk::Action::Order)action.type2) << "\x0C to \'\x08" << tt.getTrigLocation(action.number) << "\x0C\'.";
            break;
        case Chk::Action::Type::PauseGame:
            ssAction << "Pause the game.";
            break;
        case Chk::Action::Type::PauseTimer:
            ssAction << "Pause the countdown timer.";
            break;
        case Chk::Action::Type::PlaySound: // Sound, Duration
            ssAction << "Play \'\x08" << tt.getTrigSound(action.soundStringId) << "\x0C\'.";
            break;
        case Chk::Action::Type::PreserveTrigger:
            ssAction << "Preserve trigger.";
            break;
        case Chk::Action::Type::RemoveUnit: // Type, Players
            ssAction << "Remove all \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::RemoveUnitAtLocation: // Type2, Type, Players, Location
            ssAction << "Remove \x08" << tt.getTrigNumUnits((Chk::Action::NumUnits)action.type2) << "\x0C \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type)
                << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId)
                << "\x0C\'.";
            break;
        case Chk::Action::Type::RunAiScript: // Number
            {
                std::string aiName;
                if ( chkd.scData.ai.getNameById(action.number, aiName) )
                    ssAction << "Execute AI script \'\x08" << aiName << " (" << tt.getTrigScript((Sc::Ai::ScriptId)action.number) << ")\x0C\'.";
                else
                    ssAction << "Execute AI script \'\x08" << tt.getTrigScript((Sc::Ai::ScriptId)action.number) << "\x0C\'.";
            }
            break;
        case Chk::Action::Type::RunAiScriptAtLocation: // Number, Location
            {
                std::string aiName;
                if ( chkd.scData.ai.getNameById(action.number, aiName) )
                {
                    ssAction << "Execute AI script \'\x08" << aiName << " (" << tt.getTrigScript((Sc::Ai::ScriptId)action.number) << ")\x0C\' at \'\x08"
                        << tt.getTrigLocation(action.locationId) << "\x0C\'.";
                }
                else
                {
                    ssAction << "Execute AI script \'\x08" << tt.getTrigScript((Sc::Ai::ScriptId)action.number) << "\x0C\' at \'\x08"
                        << tt.getTrigLocation(action.locationId) << "\x0C\'.";
                }
            }
            break;
        case Chk::Action::Type::SetAllianceStatus: // Players, Type
            ssAction << "Set \x08" << tt.getTrigPlayer(action.group) << "\x0C to \x08" << tt.getTrigAllyState((Chk::Action::AllianceStatus)action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetCountdownTimer: // Type2, Duration
            ssAction << "Modify Countdown Timer: \x08" << tt.getTrigNumericModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \x08"
                << tt.getTrigNumber(action.time) << "\x0C seconds.";
            break;
        case Chk::Action::Type::SetDeaths: // Players, Type2, Number, Type
            ssAction << "Modify death counts for \x08" << tt.getTrigPlayer(action.group) << "\x0C: \x08"
                << tt.getTrigNumericModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \x08" << tt.getTrigNumber(action.number)
                << "\x0C for \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetDoodadState: // Type2, Type, Players, Location
            ssAction << '\x08' << tt.getTrigStateModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C doodad state for \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C for \x08" << tt.getTrigPlayer(action.group) << "\x0C at \'\x08"
                << tt.getTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::SetInvincibility: // Type2, Type, Players, Location
            ssAction << '\x08' << tt.getTrigStateModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C invincibility for \x08"
                << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C owned by \x08" << tt.getTrigPlayer(action.group)
                << "\x0C at \'\x08" << tt.getTrigLocation(action.locationId) << "\x0C\'.";
            break;
        case Chk::Action::Type::SetMissionObjectives: // String
            ssAction << "Set Mission Objectives to:\x08" << tt.getTrigString(action.stringId) << '\x0C';
            break;
        case Chk::Action::Type::SetNextScenario: // String
            ssAction << "Load \'\x08" << tt.getTrigString(action.stringId) << "\x0C\' after completion of current game.";
            break;
        case Chk::Action::Type::SetResources: // Players, Type2, Number, Type
            ssAction << "Modify resources for \x08" << tt.getTrigPlayer(action.group) << "\x0C: \x08"
                << tt.getTrigNumericModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \x08" << tt.getTrigNumber(action.number)
                << "\x0C \x08" << tt.getTrigResourceType((Chk::Trigger::ResourceType)action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetScore: // Players, Type2, Number, Type
            ssAction << "Modify score for \x08" << tt.getTrigPlayer(action.group) << "\x0C: \x08"
                << tt.getTrigNumericModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \x08" << tt.getTrigNumber(action.number)
                << "\x0C \x08" << tt.getTrigScoreType((Chk::Trigger::ScoreType)action.type) << "\x0C.";
            break;
        case Chk::Action::Type::SetSwitch: // Type2, Number
            ssAction << '\x08' << tt.getTrigSwitchModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \'\x08" << tt.getTrigSwitch(action.number) << "\x0C\'.";
            break;
        case Chk::Action::Type::TalkingPortrait: // Type, Duration
            ssAction << "Show \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C talking to current player for \x08"
                << tt.getTrigNumber(action.time) << "\x0C miliseconds.";
            break;
        case Chk::Action::Type::Transmission: // Type, Location, Wav, Type2, Number, String
            ssAction << "Send transmission to current player from \x08" << tt.getTrigUnit((Sc::Unit::Type)action.type) << "\x0C at \'\x08"
                << tt.getTrigLocation(action.locationId) << "\x0C\'. Play '\x08" << tt.getTrigSound(action.soundStringId)
                << "\x0C\'. Modify duration: \x08" << tt.getTrigNumericModifier((Chk::Trigger::ValueModifier)action.type2) << "\x0C \x08"
                << tt.getTrigNumber(action.number) << "\x0C miliseconds. Display text:\x08"
                << tt.getTrigString(action.stringId) << '\x0C';
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
            ssAction << "Wait for \x08" << tt.getTrigNumber(action.time) << "\x0C miliseconds.";
            break;
        default: // Location, String, Sound, Duration, Player, Number, Type, Action, Type2, Flags, Internal
            ssAction << "Action: \x08" << action.locationId << "\x0C, \x08" << action.stringId << "\x0C, \x08" << action.soundStringId
                << "\x0C, \x08" << action.time << "\x0C, \x08" << action.group << "\x0C, \x08" << action.number
                << "\x0C, \x08" << action.type << "\x0C, \x08" << u16(action.actionType) << "\x0C, \x08" << u16(action.type2)
                << "\x0C, \x08" << u16(action.flags) << "\x0C, \x08" << (u16)action.padding << "\x0C, \x08" << action.maskFlag << "\x0C.";
            break;
    }
    return ssAction.str();
}

std::string TriggersWindow::GetTriggerString(u32 trigNum, const Chk::Trigger & trigger, TextTrigGenerator & tt)
{
    bool more = false;
    std::stringstream ssTrigger;
    u8 remainingLines = 13;
    size_t numConditions = trigger.numUsedConditions();
    size_t numActions = trigger.numUsedActions();
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
                if ( trigger.condition(i).conditionType != Chk::Condition::Type::NoCondition )
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
                if ( trigger.action(i).actionType != Chk::Action::Type::NoAction )
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
    buttonNew.CreateThis(hWnd, 0, 0, 75, 23, "New", Id::BUTTON_NEW);
    buttonModify.CreateThis(hWnd, 0, 25, 75, 23, "Modify", Id::BUTTON_MODIFY);
    buttonDelete.CreateThis(hWnd, 0, 50, 75, 23, "Delete", Id::BUTTON_DELETE);
    buttonCopy.CreateThis(hWnd, 0, 75, 75, 23, "Copy", Id::BUTTON_COPY);
    
    buttonMoveUp.CreateThis(hWnd, 0, 100, 75, 23, "Move Up", Id::BUTTON_MOVEUP);
    buttonMoveDown.CreateThis(hWnd, 0, 125, 75, 23, "Move Down", Id::BUTTON_MOVEDOWN);
    buttonMoveTo.CreateThis(hWnd, 0, 150, 75, 23, "Move To", Id::BUTTON_MOVETO);

    listGroups.CreateThis(hWnd, 0, 0, 200, 116, true, true, false, false, false, Id::LIST_GROUPS);
    if ( listTriggers.CreateThis(hWnd, 0, 120, 200, 150, true, false, false, false, false, Id::LIST_TRIGGERS) )
        listGroups.SetPeer(listTriggers.getHandle());

    DoSize();
    RefreshWindow(true);
}

void TriggersWindow::RefreshTrigList()
{
    auto start = std::chrono::high_resolution_clock::now();
    listTriggers.SetRedraw(false);
    listTriggers.ClearItems();
    numVisibleTrigs = 0;
    int toSelect = -1;

    if ( CM != nullptr )
    {
        size_t numTriggers = CM->numTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            const Chk::Trigger & trigger = CM->getTrigger(i);
            if ( ShowTrigger(trigger) )
            {
                int newListIndex = listTriggers.AddItem((u32)i);
                if ( newListIndex >= 0 ) // Only consider the trigger if it could be added to the ListBox
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
    auto finish = std::chrono::high_resolution_clock::now();
    logger.trace() << "Trigger list refresh completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
}

bool TriggersWindow::SelectTrigListItem(int item)
{
    LPARAM selIndex = 0;
    if ( numVisibleTrigs > 0 && listTriggers.SetCurSel(item) && listTriggers.GetItemData(item, selIndex) )
    {
        this->currTrigger = u32(selIndex);
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
    LPARAM data = 0;
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
    LPARAM data = 0;
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

bool TriggersWindow::MoveTrigListItemTo(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int & listIndexMovedTo)
{
    listIndexMovedTo = -1;
    int targetListIndex = -1;
    if ( FindTargetListIndex(currListIndex, currTrigIndex, targetTrigIndex, targetListIndex) )
    {
        LPARAM listItemData = 0;
        int listItemHeight,
            preservedHeight;

        if ( targetListIndex < currListIndex )
        {
            if ( !listTriggers.GetItemHeight(currListIndex, preservedHeight) )
                return false;

            for ( int i=currListIndex; i>=targetListIndex; i-- )
            {
                if ( !( listTriggers.GetItemData(i-1, listItemData) &&
                        listTriggers.SetItemData(i, listItemData) &&
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
                        listTriggers.SetItemData(i, listItemData) &&
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

bool TriggersWindow::FindTargetListIndex(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int & targetListIndex)
{
    LPARAM listItemData = 0;
    if ( targetTrigIndex < currTrigIndex )
    {
        for ( int i=currListIndex; i>0; i-- )
        {
            if ( !listTriggers.GetItemData(i, listItemData) )
                return false;
            else if ( listItemData <= LPARAM(targetTrigIndex) )
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
            if ( listItemData == LPARAM(targetTrigIndex) )
            {
                targetListIndex = i;
                return true;
            }
            else if ( listItemData > LPARAM(targetTrigIndex) )
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

bool TriggersWindow::ShowTrigger(const Chk::Trigger & trigger)
{
    if ( displayAll )
        return true;

    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
    {
        if ( groupSelected[i] && trigger.owners[i] != Chk::Trigger::Owned::No )
            return true;
    }

    return false;
}

void TriggersWindow::ClearGroups()
{
    for ( u8 i=0; i<28; i++ )
        groupSelected[i] = false;
}

bool TriggersWindow::GetTriggerDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, Scenario & chk, u32 triggerNum, const Chk::Trigger & trigger)
{
    auto commentString = CM->getExtendedComment<RawString>(triggerNum);
    if ( !commentString )
        commentString = CM->getComment<RawString>(triggerNum);

    if ( commentString )
    {
        size_t hash = strHash(*commentString);
        auto matches = commentSizeTable.equal_range(hash);
        if ( matches.first != commentSizeTable.end() && matches.first->first == hash )
        {
            for ( auto it = matches.first; it != matches.second; ++it )
            {
                CommentSize & commentSize = it->second;
                if ( commentSize.str.compare(*commentString) == 0 )
                {
                    width = commentSize.width;
                    height = commentSize.height;
                    return false;
                }
            }
        }

        CommentSize commentSize = { *commentString, 0, 0 };

        size_t endOfLine = commentString->find("\r\n");
        if ( endOfLine != std::string::npos )
            commentString->insert(endOfLine, (std::string(TRIGGER_NUM_PREFACE) + std::to_string(triggerNum) + '\x0C'));
        else
            commentString->append((std::string(TRIGGER_NUM_PREFACE) + std::to_string(triggerNum) + '\x0C'));

        if ( GetStringDrawSize(dc, width, height, *commentString) )
        {
            width += TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
            height += TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;
            commentSize.width = width;
            commentSize.height = height;
            commentSizeTable.insert(std::pair<size_t, CommentSize>(hash, commentSize));
            return true;
        }
    }
    else
    {
        RawString str = GetTriggerString(triggerNum, trigger, textTrigGenerator);
        width = 0;
        height = TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;

        UINT strWidth, strHeight;
        if ( GetStringDrawSize(dc, strWidth, strHeight, str, trigLineSizeTable) )
        {
            UINT newWidth = strWidth+TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
            if ( newWidth > width )
                width = newWidth;

            height += strHeight;
            return true;
        }
    }
    return false;
}

void TriggersWindow::DrawGroup(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, u8 groupNum)
{
    dc.fillRect(rcItem, isSelected ? RGB(51, 153, 255) : RGB(255, 255, 255));

    dc.setBkMode(TRANSPARENT);
    if ( groupNum < 29 )
    {
        std::string groupNames[] = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6",
                                     "Player 7", "Player 8", "Player 9 (unused)", "Player 10 (unused)",
                                     "Player 11 (unused)", "Player 12 (unused)",
                                     "unknown/unused", "Current Player (unused)", "Foes (unused)", "Allies (unused)", "Neutral Players (unused)",
                                     "All players", "Force 1", "Force 2", "Force 3", "Force 4",
                                     "22", "23", "24", "25",
                                     "Non AV Players (unused)", "Show All" };

        DrawString(dc, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
            rcItem.right-rcItem.left, RGB(0, 0, 0), groupNames[groupNum]);
    }
    else
    {
        std::stringstream ssGroup;
        ssGroup << "Group: " << u16(groupNum);
        DrawString(dc, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
            rcItem.right-rcItem.left, RGB(0, 0, 0), ssGroup.str());
    }
}

void TriggersWindow::DrawTrigger(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, Scenario & chk, u32 triggerNum, const Chk::Trigger & trigger)
{
    dc.fillRect(rcItem, RGB(171, 171, 171)); // Draw far background, same color as in WM_CTLCOLORLISTBOX

    RECT rcNote;
    rcNote.left = rcItem.left+TRIGGER_LEFT_PADDING;
    rcNote.top = rcItem.top+TRIGGER_TOP_PADDING;
    rcNote.right = rcItem.right-TRIGGER_RIGHT_PADDING;
    rcNote.bottom = rcItem.bottom-TRIGGER_BOTTOM_PADDING;

    dc.fillRect(rcNote, isSelected ? RGB(51, 153, 255) : RGB(253, 246, 208)); // Draw specific trigger's background
    if ( isSelected )
        dc.drawFocusRect(rcNote);

    LONG left = rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING;
    size_t commentStringId = trigger.getComment();
    
    auto commentString = CM->getExtendedComment<RawString>(triggerNum);
    if ( !commentString )
        commentString = CM->getComment<RawString>(triggerNum);

    if ( commentString )
    {
        std::string num(std::to_string(triggerNum));
        size_t endOfLine = commentString->find("\r\n");
        if ( endOfLine != std::string::npos )
            commentString->insert(endOfLine, std::string(TRIGGER_NUM_PREFACE) + num + '\x0C');
        else
            commentString->append(std::string(TRIGGER_NUM_PREFACE) + num + '\x0C');

        dc.setBkMode(TRANSPARENT);
        DrawString(dc, left, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
            rcItem.right-left-TRIGGER_RIGHT_PADDING-STRING_RIGHT_PADDING, RGB(0, 0, 0), *commentString);
    }
    else
    {
        RawString str = GetTriggerString(triggerNum, trigger, textTrigGenerator);
        dc.setBkMode(TRANSPARENT);
        DrawString(dc, left, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
            rcItem.right-left-TRIGGER_RIGHT_PADDING-STRING_RIGHT_PADDING, RGB(0, 0, 0), str);
    }
}

LRESULT TriggersWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_SELCHANGE:
        if ( LOWORD(wParam) == Id::LIST_TRIGGERS ) // Change selection, update info boxes and so fourth
        {
            int sel;
            LPARAM currIndex = 0;
            if ( listTriggers.GetCurSel(sel) && sel != -1 && listTriggers.GetItemData(sel, currIndex) )
            {
                currTrigger = u32(currIndex);
                if ( trigModifyWindow.getHandle() != NULL )
                    trigModifyWindow.RefreshWindow(currTrigger);
            }
            else
                currTrigger = NO_TRIGGER;
        }
        else if ( LOWORD(wParam) == Id::LIST_GROUPS )
        {
            currTrigger = NO_TRIGGER;
            if ( changeGroupHighlightOnly )
                return ClassWindow::Command(hWnd, wParam, lParam);

            int numSel = listGroups.GetNumSel();
            displayAll = false;
            ClearGroups();
            for ( int i = 0; i<numSel; i++ )
            {
                LPARAM selItem = 0;
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
        break;
    case LBN_KILLFOCUS: // List box item may have lost focus, check if anything should be updated
        if ( LOWORD(wParam) == Id::LIST_TRIGGERS )
        {

        }
        else if ( LOWORD(wParam) == Id::LIST_GROUPS )
        {

        }
        break;
    case BN_CLICKED:
        if ( LOWORD(wParam) == Id::BUTTON_NEW )
            ButtonNew();
        else if ( LOWORD(wParam) == Id::BUTTON_MODIFY )
            ButtonModify();
        else if ( LOWORD(wParam) == Id::BUTTON_DELETE )
            DeleteSelection();
        else if ( LOWORD(wParam) == Id::BUTTON_COPY )
            CopySelection();
        else if ( LOWORD(wParam) == Id::BUTTON_MOVEUP )
            MoveUp();
        else if ( LOWORD(wParam) == Id::BUTTON_MOVEDOWN )
            MoveDown();
        else if ( LOWORD(wParam) == Id::BUTTON_MOVETO )
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

        case WinLib::LB::WM_DBLCLKITEM:
            if ( listTriggers == (HWND)lParam )
                ButtonModify();
            break;

        case WM_MOUSEWHEEL:
            SendMessage(listTriggers.getHandle(), WM_MOUSEWHEEL, wParam, lParam);
            break;

        case WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all triggers once
            textTrigGenerator.loadScenario((Scenario &)*CM);
            trigListDc.emplace(listTriggers.getHandle());
            trigListDc->setDefaultFont();
            if ( countTillCachePurge == 0 )
            {
                commentSizeTable.clear();
                trigLineSizeTable.clear();
                countTillCachePurge = 500;
            }
            else
                --countTillCachePurge;
            break;

        case WM_MEASUREITEM:
            if ( wParam == Id::LIST_TRIGGERS )
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                u32 triggerNum = (u32)mis->itemData;
                
                const Chk::Trigger & trigger = CM->getTrigger(triggerNum);
                if ( trigListDc )
                    GetTriggerDrawSize(*trigListDc, mis->itemWidth, mis->itemHeight, *CM, triggerNum, trigger);
                else
                    GetTriggerDrawSize(listTriggers.getDeviceContext(), mis->itemWidth, mis->itemHeight, *CM, triggerNum, trigger);

                if ( mis->itemHeight > 255 )
                    mis->itemHeight = 255;

                return TRUE;
            }
            else if ( wParam == Id::LIST_GROUPS )
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                u32 listIndex = (u32)mis->itemData;
                mis->itemWidth = 120;
                mis->itemHeight = 13;
                return TRUE;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            trigListDc = std::nullopt;
            textTrigGenerator.clearScenario();
            break;

        case WM_CTLCOLORLISTBOX:
            if ( listTriggers == (HWND)lParam )
            {
                HBRUSH hBrush = WinLib::ResourceManager::getSolidBrush(RGB(171, 171, 171));
                if ( hBrush != NULL )
                    return (LPARAM)hBrush;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WinLib::LB::WM_PREDRAWITEMS:
            textTrigGenerator.loadScenario((Scenario &)*CM);
            drawingAll = true;
            break;

        case WM_DRAWITEM:
            if ( wParam == Id::LIST_TRIGGERS )
            {
                if ( !drawingAll )
                    textTrigGenerator.loadScenario((Scenario &)*CM);

                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                {
                    u32 triggerNum = (u32)pdis->itemData;
                    
                    const Chk::Trigger & trigger = CM->getTrigger(triggerNum);
                    if ( CM != nullptr )
                        DrawTrigger(pdis->hDC, pdis->rcItem, isSelected, *CM, triggerNum, trigger);
                }

                if ( !drawingAll )
                    textTrigGenerator.clearScenario();

                return TRUE;
            }
            else if ( wParam == Id::LIST_GROUPS )
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

        case WinLib::LB::WM_POSTDRAWITEMS:
            drawingAll = false;
            textTrigGenerator.clearScenario();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
