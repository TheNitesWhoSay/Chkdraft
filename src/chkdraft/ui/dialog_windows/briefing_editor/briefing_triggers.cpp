#include "briefing_triggers.h"
#include "chkdraft/chkdraft.h"
#include "ui/chkd_controls/move_to.h"
#include "mapping/settings.h"
#include "mapping/sc_gdi_graphics.h"
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

BriefingTriggersWindow::BriefingTriggersWindow() : currBriefingTrigger(NO_TRIGGER), displayAll(true), numVisibleBriefingTrigs(0),
    changeGroupHighlightOnly(false), briefingTrigListDc(std::nullopt), drawingAll(false), briefingTextTrigGenerator(true)
{
    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
        groupSelected[i] = false;
}

BriefingTriggersWindow::~BriefingTriggersWindow()
{

}

bool BriefingTriggersWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) == TRUE &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "BriefingTriggers", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "BriefingTriggers", WS_VISIBLE|WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool BriefingTriggersWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->currBriefingTrigger = NO_TRIGGER;
    this->displayAll = true;
    this->numVisibleBriefingTrigs = 0;
    this->changeGroupHighlightOnly = false;
    this->briefingTrigListDc = std::nullopt;
    this->drawingAll = false;
    return true;
}

void BriefingTriggersWindow::RefreshWindow(bool focus)
{
    if ( getHandle() == NULL )
        return;

    this->SetRedraw(false);
    RefreshGroupList();
    RefreshTrigList();
    if ( focus )
        listBriefingTriggers.FocusThis();

    if ( briefingTrigModifyWindow.getHandle() != NULL )
    {
        if ( numVisibleBriefingTrigs > 0 )
            briefingTrigModifyWindow.RefreshWindow(currBriefingTrigger);
        else
            briefingTrigModifyWindow.DestroyThis();
    }
    this->SetRedraw(true);
    this->RedrawThis();
}

void BriefingTriggersWindow::DoSize()
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
            listBriefingTriggers.SetPos(leftPadding, 117, listGroups.Width(), triggerListHeight);
            buttonNew.MoveTo(buttonX, topPadding);
        }
        else
        {
            buttonNew.MoveTo(buttonX, listGroups.Bottom()+buttonPadding);
            listGroups.SetPos(leftPadding, topPadding, buttonNew.Right()-leftPadding, listGroups.Height());
            listBriefingTriggers.SetPos(leftPadding, 117, buttonNew.Left()-buttonPadding-leftPadding, triggerListHeight);
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

void BriefingTriggersWindow::DeleteSelection()
{
    if ( currBriefingTrigger != NO_TRIGGER )
    {
        CM->deleteBriefingTrigger(currBriefingTrigger);
        CM->deleteUnusedStrings(Chk::Scope::Both);
        briefingTrigModifyWindow.DestroyThis();
        int sel;
        RefreshGroupList();
        if ( listBriefingTriggers.GetCurSel(sel) && DeleteTrigListItem(sel) && (SelectTrigListItem(sel) || SelectTrigListItem(sel-1)) )
        {
            listBriefingTriggers.RedrawThis();
        }
        else
            RefreshTrigList();
    }
}

void BriefingTriggersWindow::CopySelection()
{
    if ( currBriefingTrigger != NO_TRIGGER )
    {
        CM->insertBriefingTrigger(currBriefingTrigger+1, CM->getBriefingTrigger(currBriefingTrigger));
        briefingTrigModifyWindow.DestroyThis();
        int sel;
        if ( listBriefingTriggers.GetCurSel(sel) && CopyTrigListItem(sel) && SelectTrigListItem(sel+1) )
            RefreshTrigList();
        else
            RefreshTrigList();
    }
}

void BriefingTriggersWindow::MoveUp()
{
    int sel;
    LPARAM prevTrigIndex;
    if ( currBriefingTrigger != NO_TRIGGER &&
         listBriefingTriggers.GetCurSel(sel) &&
         sel > 0 &&
         listBriefingTriggers.GetItemData(sel-1, prevTrigIndex) )
    {
        briefingTrigModifyWindow.DestroyThis();
        CM->moveBriefingTrigger(currBriefingTrigger, size_t(prevTrigIndex));
        if ( MoveUpTrigListItem(sel, u32(prevTrigIndex)) )
        {
            SelectTrigListItem(sel-1);
            listBriefingTriggers.RedrawThis();
        }
        else
            RefreshTrigList(); // Restore trigger list to a valid state
    }
}

void BriefingTriggersWindow::MoveDown()
{
    int sel;
    LPARAM nextTrigIndex;
    if ( currBriefingTrigger != NO_TRIGGER &&
         listBriefingTriggers.GetCurSel(sel) &&
         sel < int(numVisibleBriefingTrigs) &&
         listBriefingTriggers.GetItemData(sel+1, nextTrigIndex) )
    {
        briefingTrigModifyWindow.DestroyThis();
        CM->moveBriefingTrigger(currBriefingTrigger, size_t(nextTrigIndex));
        if ( MoveDownTrigListItem(sel, u32(nextTrigIndex)) )
        {
            SelectTrigListItem(sel+1);
            listBriefingTriggers.RedrawThis();
        }
        else
            RefreshTrigList(); // Restore trigger list to a valid state
    }
}

void BriefingTriggersWindow::MoveTrigTo()
{
    int sel;
    u32 targetTrigIndex;
    if ( currBriefingTrigger != NO_TRIGGER &&
         listBriefingTriggers.GetCurSel(sel) &&
         MoveToDialog<u32>::GetIndex(targetTrigIndex, getHandle()) &&
         targetTrigIndex >= 0 &&
         targetTrigIndex != currBriefingTrigger &&
         targetTrigIndex < CM->numBriefingTriggers() )
    {
        briefingTrigModifyWindow.DestroyThis();
        CM->moveBriefingTrigger(currBriefingTrigger, targetTrigIndex);
        int listIndexMovedTo = -1;
        listBriefingTriggers.SetRedraw(false);
        if ( MoveTrigListItemTo(sel, currBriefingTrigger, targetTrigIndex, listIndexMovedTo) )
        {
            SelectTrigListItem(listIndexMovedTo);
            listBriefingTriggers.SetRedraw(true);
            listBriefingTriggers.RedrawThis();
        }
        else
        {
            currBriefingTrigger = targetTrigIndex;
            RefreshTrigList();
        }
    }
}

void BriefingTriggersWindow::ButtonNew()
{
    Chk::Trigger briefingTrigger {};
    for ( u8 i=Sc::Player::Id::Player1; i<=Sc::Player::Id::Player8; i++ )
    {
        if ( groupSelected[i] )
            briefingTrigger.owners[i] = Chk::Trigger::Owned::Yes;
    }

    briefingTrigger.conditions[0].conditionType = Chk::Condition::Type::IsBriefing;

    LPARAM newTrigId = 0;
    int sel;
    if ( listBriefingTriggers.GetCurSel(sel) && sel != numVisibleBriefingTrigs-1 )
    {
        if ( listBriefingTriggers.GetItemData(sel, newTrigId) )
        {
            newTrigId ++;
            CM->insertBriefingTrigger(size_t(newTrigId), briefingTrigger);
        }
    }
    else
    {
        CM->addBriefingTrigger(briefingTrigger);
        newTrigId = LPARAM(CM->numBriefingTriggers()-1);
    }

    currBriefingTrigger = u32(newTrigId);
    RefreshWindow(true);
    ButtonModify();
}

void BriefingTriggersWindow::ButtonModify()
{
    if ( currBriefingTrigger != NO_TRIGGER )
    {
        if ( briefingTrigModifyWindow.getHandle() == NULL )
        {
            briefingTrigModifyWindow.CreateThis(getHandle(), currBriefingTrigger);
            SetFocus(briefingTrigModifyWindow.getHandle());
        }
        else
        {
            briefingTrigModifyWindow.RefreshWindow(currBriefingTrigger);
            ShowWindow(briefingTrigModifyWindow.getHandle(), SW_SHOW);
            SetFocus(briefingTrigModifyWindow.getHandle());
        }
    }
}

std::string BriefingTriggersWindow::GetActionString(u8 actionNum, const Chk::Trigger & briefingTrigger, BriefingTextTrigGenerator & tt)
{
    std::stringstream ssAction;
    const Chk::Action & action = briefingTrigger.action(actionNum);
    Chk::Action::Type actionType = action.actionType;
    if ( action.isDisabled() )
        ssAction << "(disabled) ";

    switch ( actionType )
    {
        case Chk::Action::Type::BriefingDisplaySpeakingPortrait:
            ssAction << "Show speaking animation for \x08" << tt.getBriefingSlot(action.group) << "\x0C for \x08" << tt.getBriefingTrigNumber(action.time) << "\x0C milliseconds.";
            break;
        case Chk::Action::Type::BriefingHidePortrait:
            ssAction << "Hide portrait in slot \x08" << tt.getBriefingSlot(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::BriefingMissionObjectives:
            ssAction << "Set Mission Objectives for current player to: '\x08" << tt.getBriefingTrigString(action.stringId) << "\x0C'";
            break;
        case Chk::Action::Type::BriefingPlaySound:
            ssAction << "Play '\x08" << tt.getBriefingTrigSound(action.soundStringId) << "\x0C' for current player.";
            break;
        case Chk::Action::Type::BriefingShowPortrait:
            ssAction << "Show portrait of \x08" << tt.getBriefingTrigUnit(Sc::Unit::Type(action.type)) << "\x0C in slot \x08" << tt.getBriefingSlot(action.group) << "\x0C.";
            break;
        case Chk::Action::Type::BriefingSkipTutorialEnabled:
            ssAction << "Enable skip tutorial button.";
            break;
        case Chk::Action::Type::BriefingTextMessage:
            ssAction << "Display the following text for \x08" << tt.getBriefingTrigNumber(action.time) << "\x0C milliseconds: '\x08" << tt.getBriefingTrigString(action.stringId) << "\x0C'";
            break;
        case Chk::Action::Type::BriefingTransmission:
            ssAction << "Show speaking animation for \x08" << tt.getBriefingSlot(action.group) << "\x0C. Play '\x08"
                << tt.getBriefingTrigSound(action.soundStringId) << "\x0C'. Modify transmission duration: \x08"
                << tt.getBriefingTrigNumericModifier(Chk::Trigger::ValueModifier(action.type2)) << "\x0C \x08" << tt.getBriefingTrigNumber(action.number)
                << "\x0C milliseconds. Display the following text: '\x08" << tt.getBriefingTrigString(action.stringId) << "\x0C'";
            break;
        case Chk::Action::Type::BriefingWait:
            ssAction << "Pause for \x08" << action.time << "\x0C milliseconds.";
            break;
        case Chk::Action::Type::NoAction:
            ssAction << "(No action)";
            break;
        default: // Location, String, Sound, Duration, Player, Number, Type, Action, Type2, Flags, Internal
            ssAction << "Action: \x08" << action.locationId << "\x0C, \x08" << action.stringId << "\x0C, \x08" << action.soundStringId
                << "\x0C, \x08" << action.time << "\x0C, \x08" << action.group << "\x0C, \x08" << action.number
                << "\x0C, \x08" << action.type << "\x0C, \x08" << u16(action.type2) << "\x0C, \x08" << u16(action.flags)
                << "\x0C, \x08" << (u16)action.padding << "\x0C, \x08" << action.maskFlag << "\x0C.";
            break;
    }
    return ssAction.str();
}

std::string BriefingTriggersWindow::GetBriefingTriggerString(u32 briefingTrigNum, const Chk::Trigger & briefingTrigger, BriefingTextTrigGenerator & tt)
{
    bool more = false;
    std::stringstream ssTrigger;
    u8 remainingLines = 13;
    size_t numConditions = briefingTrigger.numUsedConditions();
    size_t numActions = briefingTrigger.numUsedActions();
    if ( numActions == 0 )
        ssTrigger << "<EMPTY>" << TRIGGER_NUM_PREFACE << briefingTrigNum << "\x0C\r\n";
    else
    {
        if ( numActions > 0 && remainingLines > 0 )
        {
            if ( remainingLines == 13 )
                ssTrigger << "ACTIONS:" << TRIGGER_NUM_PREFACE << briefingTrigNum << "\x0C\r\n";
            else
                ssTrigger << "ACTIONS:\r\n";

            u8 i=0;
            while ( numActions > 0 && i < 64 )
            {
                if ( briefingTrigger.action(i).actionType != Chk::Action::Type::NoAction )
                {
                    ssTrigger << ' ' << GetActionString(i, briefingTrigger, tt) << "\r\n";

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

void BriefingTriggersWindow::CreateSubWindows(HWND hWnd)
{
    buttonNew.CreateThis(hWnd, 0, 0, 75, 23, "New", Id::BUTTON_NEW);
    buttonModify.CreateThis(hWnd, 0, 25, 75, 23, "Modify", Id::BUTTON_MODIFY);
    buttonDelete.CreateThis(hWnd, 0, 50, 75, 23, "Delete", Id::BUTTON_DELETE);
    buttonCopy.CreateThis(hWnd, 0, 75, 75, 23, "Copy", Id::BUTTON_COPY);
    
    buttonMoveUp.CreateThis(hWnd, 0, 100, 75, 23, "Move Up", Id::BUTTON_MOVEUP);
    buttonMoveDown.CreateThis(hWnd, 0, 125, 75, 23, "Move Down", Id::BUTTON_MOVEDOWN);
    buttonMoveTo.CreateThis(hWnd, 0, 150, 75, 23, "Move To", Id::BUTTON_MOVETO);

    listGroups.CreateThis(hWnd, 0, 0, 200, 116, true, true, false, false, false, Id::LIST_GROUPS);
    if ( listBriefingTriggers.CreateThis(hWnd, 0, 120, 200, 150, true, false, false, false, false, Id::LIST_TRIGGERS) )
        listGroups.SetPeer(listBriefingTriggers.getHandle());

    DoSize();
    RefreshWindow(true);
}

void BriefingTriggersWindow::RefreshGroupList()
{
    listGroups.SetRedraw(false);
    listGroups.ClearItems();

    u8 firstNotFound = 0;
    bool addedPlayer[Chk::Trigger::MaxOwners];
    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
        addedPlayer[i] = false;

    if ( CM != nullptr )
    {
        size_t numTriggers = CM->numBriefingTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            const Chk::Trigger & trigger = CM->getBriefingTrigger(i);
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

void BriefingTriggersWindow::RefreshTrigList()
{
    auto start = std::chrono::high_resolution_clock::now();
    listBriefingTriggers.SetRedraw(false);
    listBriefingTriggers.ClearItems();
    numVisibleBriefingTrigs = 0;
    int toSelect = -1;

    if ( CM != nullptr )
    {
        size_t numTriggers = CM->numBriefingTriggers();
        for ( size_t i=0; i<numTriggers; i++ )
        {
            const Chk::Trigger & briefingTrigger = CM->getBriefingTrigger(i);
            if ( ShowTrigger(briefingTrigger) )
            {
                int newListIndex = listBriefingTriggers.AddItem((u32)i);
                if ( newListIndex >= 0 ) // Only consider the trigger if it could be added to the ListBox
                {
                    numVisibleBriefingTrigs ++;
                    if ( currBriefingTrigger == i ) // This trigger is the currBriefingTrigger
                        toSelect = newListIndex; // Mark position for selection
                }
            }
        }
    }
    
    listBriefingTriggers.SetRedraw(true);
    if ( toSelect == -1 || !listBriefingTriggers.SetCurSel(toSelect) ) // Attempt selection
    {
        currBriefingTrigger = NO_TRIGGER; // Clear currBriefingTrigger if selection fails
        if ( briefingTrigModifyWindow.getHandle() != NULL )
            briefingTrigModifyWindow.DestroyThis();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    logger.trace() << "Briefing trigger list refresh completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
}

bool BriefingTriggersWindow::SelectTrigListItem(int item)
{
    LPARAM selIndex = 0;
    if ( numVisibleBriefingTrigs > 0 && listBriefingTriggers.SetCurSel(item) && listBriefingTriggers.GetItemData(item, selIndex) )
    {
        this->currBriefingTrigger = u32(selIndex);
        briefingTrigModifyWindow.RefreshWindow(currBriefingTrigger);
        return true;
    }
    else
    {
        listBriefingTriggers.ClearSel();
        currBriefingTrigger = NO_TRIGGER;
        briefingTrigModifyWindow.DestroyThis();
        return false;
    }
}

bool BriefingTriggersWindow::DeleteTrigListItem(int item)
{
    bool removedItem = false;
    LPARAM data = 0;
    bool success = true;
    int totalItems = listBriefingTriggers.GetNumItems();
    for ( int i=item+1; i<totalItems; i++ ) // Attempt to decrement the trigger index of items with higher list indexes
    {
        if ( !( listBriefingTriggers.GetItemData(i, data) && listBriefingTriggers.SetItemData(i, data-1) ) ) // Decrement trigger index
        {
            success = false; // Decrementing failed
            break;
        }
    }

    if ( success && listBriefingTriggers.RemoveItem(item) ) // Decrement succeeded, delete the target item
    {
        currBriefingTrigger = NO_TRIGGER; // Ensure currTrigger is cleared (until selection is updated)
        numVisibleBriefingTrigs --; // Update amount of visible triggers
        return true;
    }
    else
        return false;
}

bool BriefingTriggersWindow::CopyTrigListItem(int item)
{
    LPARAM data = 0;
    bool success = true;
    int totalItems = listBriefingTriggers.GetNumItems();
    for ( int i=item+1; i<totalItems; i++ ) // Attempt to increment the trigger index of items with higher list indexes
    {
        if ( !( listBriefingTriggers.GetItemData(i, data) && listBriefingTriggers.SetItemData(i, data+1) ) ) // Increment trigger index
        {
            success = false; // Incrementing failed
            break;
        }
    }

    if ( success && listBriefingTriggers.InsertItem(item+1, currBriefingTrigger+1) ) // Increment succeeded, insert the copy item
    {
        currBriefingTrigger = NO_TRIGGER; // Ensure currTrigger is cleared (until selection is updated)
        numVisibleBriefingTrigs ++; // Update amount of visible triggers
        return true;
    }
    else
        return false;
}

bool BriefingTriggersWindow::MoveUpTrigListItem(int listIndex, u32 prevBriefingTrigIndex)
{
    currBriefingTrigger = prevBriefingTrigIndex;
    int prevItemHeight, currItemHeight;
    return listBriefingTriggers.GetItemHeight(listIndex-1, prevItemHeight) && // Get heights
           listBriefingTriggers.GetItemHeight(listIndex, currItemHeight) &&
           listBriefingTriggers.SetItemData(listIndex, prevBriefingTrigIndex+1) && // Adjust stored indexes
           listBriefingTriggers.SetItemHeight(listIndex-1, currItemHeight) && // Swap heights
           listBriefingTriggers.SetItemHeight(listIndex, prevItemHeight);
}

bool BriefingTriggersWindow::MoveDownTrigListItem(int listIndex, u32 nextBriefingTrigIndex)
{
    currBriefingTrigger = nextBriefingTrigIndex;
    int nextItemHeight, currItemHeight;
    return listBriefingTriggers.GetItemHeight(listIndex+1, nextItemHeight) && // Get heights
           listBriefingTriggers.GetItemHeight(listIndex, currItemHeight) &&
           listBriefingTriggers.SetItemData(listIndex, nextBriefingTrigIndex-1) && // Adjust stored indexes
           listBriefingTriggers.SetItemHeight(listIndex+1, currItemHeight) && // Swap heights
           listBriefingTriggers.SetItemHeight(listIndex, nextItemHeight);
}

bool BriefingTriggersWindow::MoveTrigListItemTo(int currListIndex, u32 currBriefingTrigIndex, u32 targetBriefingTrigIndex, int & listIndexMovedTo)
{
    listIndexMovedTo = -1;
    int targetListIndex = -1;
    if ( FindTargetListIndex(currListIndex, currBriefingTrigIndex, targetBriefingTrigIndex, targetListIndex) )
    {
        LPARAM listItemData = 0;
        int listItemHeight,
            preservedHeight;

        if ( targetListIndex < currListIndex )
        {
            if ( !listBriefingTriggers.GetItemHeight(currListIndex, preservedHeight) )
                return false;

            for ( int i=currListIndex; i>=targetListIndex; i-- )
            {
                if ( !( listBriefingTriggers.GetItemData(i-1, listItemData) &&
                        listBriefingTriggers.SetItemData(i, listItemData+1) &&
                        listBriefingTriggers.GetItemHeight(i-1, listItemHeight) &&
                        listBriefingTriggers.SetItemHeight(i, listItemHeight) ) )
                {
                    return false;
                }
            }

            if ( listBriefingTriggers.SetItemData(targetListIndex, targetBriefingTrigIndex) &&
                 listBriefingTriggers.SetItemHeight(targetListIndex, preservedHeight) )
            {
                listIndexMovedTo = targetListIndex;
                return true;
            }
            else
                return false;
        }
        else if ( targetListIndex > currListIndex )
        {
            if ( !listBriefingTriggers.GetItemHeight(currListIndex, preservedHeight) )
                return false;

            for ( int i=currListIndex; i<=targetListIndex; i++ )
            {
                if ( !( listBriefingTriggers.GetItemData(i+1, listItemData) &&
                        listBriefingTriggers.SetItemData(i, listItemData-1) &&
                        listBriefingTriggers.GetItemHeight(i+1, listItemHeight) &&
                        listBriefingTriggers.SetItemHeight(i, listItemHeight) ) )
                {
                    return false;
                }
            }

            if ( listBriefingTriggers.SetItemData(targetListIndex, targetBriefingTrigIndex) &&
                 listBriefingTriggers.SetItemHeight(targetListIndex, preservedHeight) )
            {
                listIndexMovedTo = targetListIndex;
                return true;
            }
        }
        else // targetListIndex == currListIndex
        {
            listIndexMovedTo = targetListIndex;
            return currBriefingTrigIndex == targetBriefingTrigIndex ||
                   listBriefingTriggers.SetItemData(targetListIndex, targetBriefingTrigIndex);
        }
    }
    return false;
}

bool BriefingTriggersWindow::FindTargetListIndex(int currListIndex, u32 currBriefingTrigIndex, u32 targetBriefingTrigIndex, int & targetListIndex)
{
    LPARAM listItemData = 0;
    if ( targetBriefingTrigIndex < currBriefingTrigIndex )
    {
        for ( int i=currListIndex; i>0; i-- )
        {
            if ( !listBriefingTriggers.GetItemData(i, listItemData) )
                return false;
            else if ( listItemData <= LPARAM(targetBriefingTrigIndex) )
            {
                targetListIndex = i;
                return true;
            }
        }
        targetListIndex = 0;
        return true;
    }
    else if ( targetBriefingTrigIndex > currBriefingTrigIndex )
    {
        int numListIndexes = listBriefingTriggers.GetNumItems();
        for ( int i=currListIndex; i<numListIndexes; i++ )
        {
            if ( !listBriefingTriggers.GetItemData(i, listItemData) )
                return false;
            if ( listItemData == LPARAM(targetBriefingTrigIndex) )
            {
                targetListIndex = i;
                return true;
            }
            else if ( listItemData > LPARAM(targetBriefingTrigIndex) )
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

bool BriefingTriggersWindow::ShowTrigger(const Chk::Trigger & briefingTrigger)
{
    if ( displayAll )
        return true;

    for ( u8 i=0; i<Chk::Trigger::MaxOwners; i++ )
    {
        if ( groupSelected[i] && briefingTrigger.owners[i] != Chk::Trigger::Owned::No )
            return true;
    }

    return false;
}

void BriefingTriggersWindow::ClearGroups()
{
    for ( u8 i=0; i<28; i++ )
        groupSelected[i] = false;
}

bool BriefingTriggersWindow::GetTriggerDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, Scenario & chk, u32 briefingTriggerNum, const Chk::Trigger & briefingTrigger)
{
    RawString str = GetBriefingTriggerString(briefingTriggerNum, briefingTrigger, briefingTextTrigGenerator);
    width = 0;
    height = TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;

    UINT strWidth, strHeight;
    if ( GetStringDrawSize(dc, strWidth, strHeight, str, briefingTrigLineSizeTable) )
    {
        UINT newWidth = strWidth+TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
        if ( newWidth > width )
            width = newWidth;

        height += strHeight;
        return true;
    }
    return false;
}

void BriefingTriggersWindow::DrawGroup(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, u8 groupNum)
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

void BriefingTriggersWindow::DrawTrigger(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, Scenario & chk, u32 briefingTriggerNum, const Chk::Trigger & briefingTrigger)
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
    RawString str = GetBriefingTriggerString(briefingTriggerNum, briefingTrigger, briefingTextTrigGenerator);
    dc.setBkMode(TRANSPARENT);
    DrawString(dc, left, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
        rcItem.right-left-TRIGGER_RIGHT_PADDING-STRING_RIGHT_PADDING, RGB(0, 0, 0), str);
}

LRESULT BriefingTriggersWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_SELCHANGE:
        if ( LOWORD(wParam) == Id::LIST_TRIGGERS ) // Change selection, update info boxes and so fourth
        {
            int sel;
            LPARAM currIndex = 0;
            if ( listBriefingTriggers.GetCurSel(sel) && sel != -1 && listBriefingTriggers.GetItemData(sel, currIndex) )
            {
                currBriefingTrigger = u32(currIndex);
                if ( briefingTrigModifyWindow.getHandle() != NULL )
                    briefingTrigModifyWindow.RefreshWindow(currBriefingTrigger);
            }
            else
                currBriefingTrigger = NO_TRIGGER;
        }
        else if ( LOWORD(wParam) == Id::LIST_GROUPS )
        {
            currBriefingTrigger = NO_TRIGGER;
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

LRESULT BriefingTriggersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
            if ( listBriefingTriggers == (HWND)lParam )
                ButtonModify();
            break;

        case WM_MOUSEWHEEL:
            SendMessage(listBriefingTriggers.getHandle(), WM_MOUSEWHEEL, wParam, lParam);
            break;

        case WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all triggers once
            briefingTextTrigGenerator.loadScenario((Scenario &)*CM);
            briefingTrigListDc.emplace(listBriefingTriggers.getHandle());
            briefingTrigListDc->setDefaultFont();
            briefingTrigLineSizeTable.clear();
            break;

        case WM_MEASUREITEM:
            if ( wParam == Id::LIST_TRIGGERS )
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                u32 triggerNum = (u32)mis->itemData;
                
                const Chk::Trigger & briefingTrigger = CM->getBriefingTrigger(triggerNum);
                if ( briefingTrigListDc )
                    GetTriggerDrawSize(*briefingTrigListDc, mis->itemWidth, mis->itemHeight, *CM, triggerNum, briefingTrigger);
                else
                    GetTriggerDrawSize(listBriefingTriggers.getDeviceContext(), mis->itemWidth, mis->itemHeight, *CM, triggerNum, briefingTrigger);

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
            briefingTrigListDc = std::nullopt;
            briefingTextTrigGenerator.clearScenario();
            break;

        case WM_CTLCOLORLISTBOX:
            if ( listBriefingTriggers == (HWND)lParam )
            {
                HBRUSH hBrush = WinLib::ResourceManager::getSolidBrush(RGB(171, 171, 171));
                if ( hBrush != NULL )
                    return (LPARAM)hBrush;
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WinLib::LB::WM_PREDRAWITEMS:
            briefingTextTrigGenerator.loadScenario((Scenario &)*CM);
            drawingAll = true;
            break;

        case WM_DRAWITEM:
            if ( wParam == Id::LIST_TRIGGERS )
            {
                if ( !drawingAll )
                    briefingTextTrigGenerator.loadScenario((Scenario &)*CM);

                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                {
                    u32 triggerNum = (u32)pdis->itemData;
                    
                    const Chk::Trigger & trigger = CM->getBriefingTrigger(triggerNum);
                    if ( CM != nullptr )
                        DrawTrigger(pdis->hDC, pdis->rcItem, isSelected, *CM, triggerNum, trigger);
                }

                if ( !drawingAll )
                    briefingTextTrigGenerator.clearScenario();

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
            briefingTextTrigGenerator.clearScenario();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
