#include "trig_general.h"
#include "chkdraft/chkdraft.h"
#include "ui/chkd_controls/chkd_string_input.h"

enum_t(Id, u32, {
    GROUP_COMMENT = ID_FIRST,
    GROUP_NOTES,
    GROUP_FLAGS,
    EDIT_COMMENT,
    EDIT_NOTES,
    BUTTON_COMMENT_PROPERTIES,
    BUTTON_NOTES_PROPERTIES,

    CHECK_PRESERVE_TRIGGER,
    CHECK_DISABLED,
    CHECK_IGNORE_CONDITIONS_ONCE,
    CHECK_IGNORE_WAITSKIP_ONCE,
    CHECK_IGNORE_MISCACTIONS_ONCE,
    CHECK_IGNORE_DEFEAT_DRAW,
    CHECK_IS_PAUSED,
    TEXT_RAW_FLAGS,
    EDIT_RAW_FLAGS,

    BUTTON_ADVANCED
});

TrigGeneralWindow::TrigGeneralWindow() : trigIndex(0), refreshing(true), advancedMode(false)
{

}

TrigGeneralWindow::~TrigGeneralWindow()
{

}

bool TrigGeneralWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TrigMeta", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "TrigMeta", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TrigGeneralWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->trigIndex = 0;
    this->refreshing = true;
    this->advancedMode = false;
    return true;
}

void TrigGeneralWindow::RefreshWindow(u32 trigIndex)
{
    refreshing = true;
    this->trigIndex = trigIndex;
    const Chk::Trigger & trigger = CM->getTrigger(trigIndex);
    editRawFlags.SetEditBinaryNum<s32>(trigger.flags);
    checkPreservedFlag.SetCheck(trigger.preserveTriggerFlagged());
    checkDisabled.SetCheck(trigger.disabled());
    checkIgnoreConditionsOnce.SetCheck(trigger.ignoreConditionsOnce());
    checkIgnoreWaitSkipOnce.SetCheck(trigger.ignoreWaitSkipOnce());
    checkIgnoreManyActions.SetCheck(trigger.ignoreMiscActionsOnce());
    checkIgnoreDefeatDraw.SetCheck(trigger.ignoreDefeatDraw());
    checkFlagPaused.SetCheck(trigger.pauseFlagged());
        
    auto extendedComment = CM->getExtendedComment<ChkdString>(trigIndex);
    auto extendedNotes = CM->getExtendedNotes<ChkdString>(trigIndex);
    editComment.SetText(extendedComment ? *extendedComment : "");
    editNotes.SetText(extendedNotes ? *extendedNotes : "");
    refreshing = false;
}

void TrigGeneralWindow::DoSize()
{
    RECT rect;
    if ( getClientRect(rect) )
    {
        long cliWidth = rect.right-rect.left,
             cliHeight = rect.bottom-rect.top,
             padding = 4,
             groupFlagsHeight = 114,
             groupFlagsTop = cliHeight-groupFlagsHeight-padding,
             remainder = cliHeight-groupFlagsHeight-padding,
             halfRemainder = remainder/2,
             groupTopPadding = padding+10,
             groupLeft = 0,
             groupTop = 0,
             groupWidth = 0,
             groupHeight = 0;

        groupLeft = padding;
        groupTop = padding;
        groupWidth = cliWidth-padding*2;
        groupHeight = halfRemainder-padding*2;
        buttonCommentProperties.MoveTo(groupLeft+groupWidth-buttonCommentProperties.Width()-padding, groupTop+groupTopPadding);
        groupComment.SetPos(groupLeft, groupTop, groupWidth, groupHeight);
        editComment.SetPos(groupLeft+padding, groupTop+groupTopPadding,
            buttonCommentProperties.Left()-padding-1-groupLeft, groupHeight-groupTopPadding-padding);

        groupLeft = padding;
        groupTop = halfRemainder+padding;
        groupWidth = cliWidth-padding*2;
        groupHeight = halfRemainder-padding*2;
        buttonNotesProperties.MoveTo(groupLeft+groupWidth-buttonNotesProperties.Width()-padding, groupTop+groupTopPadding);
        groupNotes.SetPos(groupLeft, groupTop, groupWidth, groupHeight);
        editNotes.SetPos(groupLeft+padding, groupTop+groupTopPadding,
            buttonNotesProperties.Left()-padding-1-groupLeft, groupHeight-groupTopPadding-padding);

        groupLeft = padding;
        groupTop = groupFlagsTop;
        groupWidth = cliWidth-padding*2;
        groupHeight = groupFlagsHeight;
        groupExecutionFlags.SetPos(groupLeft, groupTop, groupWidth, groupHeight);
        checkPreservedFlag.MoveTo(groupLeft+padding, groupTop+groupTopPadding);
        checkDisabled.MoveTo(groupLeft+padding, checkPreservedFlag.Bottom()+1);
        checkIgnoreConditionsOnce.MoveTo(groupLeft+padding, checkDisabled.Bottom()+1);
        checkIgnoreWaitSkipOnce.MoveTo(checkPreservedFlag.Right()+padding, groupTop+groupTopPadding);
        checkIgnoreManyActions.MoveTo(checkIgnoreWaitSkipOnce.Left(), checkIgnoreWaitSkipOnce.Bottom()+1);
        checkIgnoreDefeatDraw.MoveTo(checkIgnoreWaitSkipOnce.Left(), checkIgnoreManyActions.Bottom()+1);
        checkFlagPaused.MoveTo(checkIgnoreWaitSkipOnce.Left(), checkIgnoreDefeatDraw.Bottom()+1);

        textRawFlags.MoveTo(checkFlagPaused.Right()+padding*4, groupTop+groupTopPadding);
        editRawFlags.MoveTo(textRawFlags.Left(), textRawFlags.Bottom()+1);

        buttonAdvanced.MoveTo(rect.right - buttonAdvanced.Width() - 8, rect.bottom - buttonAdvanced.Height() - 8);
    }
}

void TrigGeneralWindow::CreateSubWindows(HWND hWnd)
{
    groupComment.CreateThis(hWnd, 5, 5, 75, 20, "Comment: ", Id::GROUP_COMMENT);
    editComment.CreateThis(hWnd, 85, 5, 100, 40, true, Id::EDIT_COMMENT);
    buttonCommentProperties.CreateThis(hWnd, 5, 5, 23, 23, "", Id::BUTTON_COMMENT_PROPERTIES, true);
    buttonCommentProperties.SetImageFromResourceId(IDB_PROPERTIES);

    groupNotes.CreateThis(hWnd, 5, 50, 75, 20, "Notes: ", Id::GROUP_NOTES);
    editNotes.CreateThis(hWnd, 85, 50, 100, 80, true, Id::EDIT_NOTES);
    buttonNotesProperties.CreateThis(hWnd, 5, 5, 23, 23, "", Id::BUTTON_NOTES_PROPERTIES, true);
    buttonNotesProperties.SetImageFromResourceId(IDB_PROPERTIES);

    groupExecutionFlags.CreateThis(hWnd, 5, 100, 75, 20, "Execution Flags: ", Id::GROUP_FLAGS);
    checkPreservedFlag.CreateThis(hWnd, 5, 5, 150, 20, false, "Preserve Trigger", Id::CHECK_PRESERVE_TRIGGER);
    checkDisabled.CreateThis(hWnd, 5, 5, 150, 20, false, "Disabled", Id::CHECK_DISABLED);
    checkIgnoreConditionsOnce.CreateThis(hWnd, 5, 5, 150, 20, false, "Ignore Conditions Once", Id::CHECK_IGNORE_CONDITIONS_ONCE);
    checkIgnoreWaitSkipOnce.CreateThis(hWnd, 5, 5, 150, 20, false, "Ignore Wait Skip Once", Id::CHECK_IGNORE_WAITSKIP_ONCE);
    checkIgnoreManyActions.CreateThis(hWnd, 5, 5, 150, 20, false, "Ignore Misc Actions Once", Id::CHECK_IGNORE_MISCACTIONS_ONCE);
    checkIgnoreDefeatDraw.CreateThis(hWnd, 5, 5, 150, 20, false, "Ignore Defeat/Draw", Id::CHECK_IGNORE_DEFEAT_DRAW);
    checkFlagPaused.CreateThis(hWnd, 5, 5, 150, 20, false, "Flag Paused", Id::CHECK_IS_PAUSED);

    textRawFlags.CreateThis(hWnd, 5, 5, 150, 13, "Raw Flags:", Id::TEXT_RAW_FLAGS);
    editRawFlags.CreateThis(hWnd, 5, 5, 200, 20, false, Id::EDIT_RAW_FLAGS);

    buttonAdvanced.CreateThis(hWnd, 12, 310, 75, 20, "Advanced", Id::BUTTON_ADVANCED);

    checkIgnoreWaitSkipOnce.Hide();
    checkIgnoreManyActions.Hide();
    checkIgnoreDefeatDraw.Hide();
    checkFlagPaused.Hide();
    textRawFlags.Hide();
    editRawFlags.Hide();
}

void TrigGeneralWindow::OnLeave()
{
    ParseRawFlagsText();
}

void TrigGeneralWindow::SetPreserveTrigger(bool preserve)
{
    CM->editTrigger(trigIndex).setPreserveTriggerFlagged(preserve);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetDisabledTrigger(bool disabled)
{
    CM->editTrigger(trigIndex).setDisabled(disabled);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
    CM->editTrigger(trigIndex).setIgnoreConditionsOnce(ignoreConditionsOnce);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
    CM->editTrigger(trigIndex).setIgnoreWaitSkipOnce(ignoreWaitSkipOnce);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
    CM->editTrigger(trigIndex).setIgnoreMiscActionsOnce(ignoreMiscActionsOnce);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
    CM->editTrigger(trigIndex).setIgnoreDefeatDraw(ignoreDefeatDraw);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::SetPausedTrigger(bool paused)
{
    CM->editTrigger(trigIndex).setPauseFlagged(paused);
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
}

void TrigGeneralWindow::ParseRawFlagsText()
{
    if ( trigIndex < CM->numTriggers() )
    {
        u32 triggerFlags = CM->getTrigger(trigIndex).flags;
        if ( editRawFlags.GetEditBinaryNum(triggerFlags) )
        {
            CM->operator()()->triggers[trigIndex].flags = triggerFlags;
            CM->notifyChange(false);
        }
        
        RefreshWindow(trigIndex);
    }
}

void TrigGeneralWindow::ToggleAdvancedMode()
{
    advancedMode = !advancedMode;
    if ( advancedMode ) // Now in advanced mode
    {
        buttonAdvanced.SetText("Standard");
        checkIgnoreWaitSkipOnce.Show();
        checkIgnoreManyActions.Show();
        checkIgnoreDefeatDraw.Show();
        checkFlagPaused.Show();
        textRawFlags.Show();
        editRawFlags.Show();
    }
    else // Now in standard mode
    {
        buttonAdvanced.SetText("Advanced");
        checkIgnoreWaitSkipOnce.Hide();
        checkIgnoreManyActions.Hide();
        checkIgnoreDefeatDraw.Hide();
        checkFlagPaused.Hide();
        textRawFlags.Hide();
        editRawFlags.Hide();
    }
    Hide(); Show(); // Dirty fix to redraw issues
    RedrawThis(); // Apparently a call to RedrawWindow is insufficient
}

void TrigGeneralWindow::EditCommentFocusLost()
{
    auto newCommentText = editComment.GetWinText();
    bool addIfNotFound = newCommentText && !newCommentText->empty();
    if ( addIfNotFound || CM->hasTriggerExtension(trigIndex) )
    {
        auto edit = CM->operator()();
        auto extensionIndex = CM->getTriggerExtension(trigIndex, addIfNotFound);
        size_t newCommentStringId = CM->addString<ChkdString>(ChkdString(*newCommentText), Chk::Scope::Editor);
        if ( newCommentStringId != Chk::StringId::NoString )
        {
            edit->triggerExtensions[extensionIndex].commentStringId = (u32)newCommentStringId;
            CM->deleteUnusedStrings(Chk::Scope::Editor);
            CM->refreshScenario();
        }
    }
}

void TrigGeneralWindow::EditNotesFocusLost()
{
    auto newNotesText = editNotes.GetWinText();
    bool addIfNotFound = newNotesText && !newNotesText->empty();
    if ( addIfNotFound ||  CM->hasTriggerExtension(trigIndex) )
    {
        auto edit = CM->operator()();
        auto extensionIndex = CM->getTriggerExtension(trigIndex, addIfNotFound);
        size_t newNotesStringId = CM->addString<ChkdString>(ChkdString(*newNotesText), Chk::Scope::Editor);
        if ( newNotesStringId != Chk::StringId::NoString )
        {
            edit->triggerExtensions[extensionIndex].notesStringId = (u32)newNotesStringId;
            CM->deleteUnusedStrings(Chk::Scope::Editor);
            CM->refreshScenario();
        }
    }
}

void TrigGeneralWindow::ButtonCommentProperties()
{
    std::optional<ChkdString> unused {};
    auto extendedComment = CM->getExtendedComment<ChkdString>(trigIndex);
    ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), unused, extendedComment, Chk::StringUserFlag::ExtendedTriggerComment, this->trigIndex);

    if ( (result & ChkdStringInputDialog::Result::EditorStringChanged) == ChkdStringInputDialog::Result::EditorStringChanged )
    {
        auto edit = CM->operator()();
        if ( extendedComment )
            CM->setExtendedComment<ChkdString>(this->trigIndex, *extendedComment);
        else
            CM->setExtendedCommentStringId(this->trigIndex, 0);

        CM->deleteUnusedStrings(Chk::Scope::Editor);
    }

    if ( result > 0 )
        CM->refreshScenario();
}

void TrigGeneralWindow::ButtonNotesProperties()
{
    std::optional<ChkdString> unused {};
    auto extendedNotes = CM->getExtendedNotes<ChkdString>(trigIndex);
    ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), unused, extendedNotes, Chk::StringUserFlag::ExtendedTriggerNotes, this->trigIndex);

    if ( (result & ChkdStringInputDialog::Result::EditorStringChanged) == ChkdStringInputDialog::Result::EditorStringChanged )
    {
        auto edit = CM->operator()();
        if ( extendedNotes )
            CM->setExtendedNotes<ChkdString>(this->trigIndex, *extendedNotes);
        else
            CM->setExtendedNotesStringId(this->trigIndex, 0);

        CM->deleteUnusedStrings(Chk::Scope::Editor);
    }

    if ( result > 0 )
        CM->refreshScenario();
}

LRESULT TrigGeneralWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if ( !refreshing )
    {
        switch ( LOWORD(wParam) )
        {
            case Id::EDIT_RAW_FLAGS:
                if ( HIWORD(wParam) == EN_KILLFOCUS )
                    ParseRawFlagsText();
                break;
        }
        switch ( HIWORD(wParam) )
        {
            case BN_CLICKED:
                switch ( LOWORD(wParam) )
                {
                    case Id::BUTTON_ADVANCED:               ToggleAdvancedMode();                                           break;
                    case Id::CHECK_PRESERVE_TRIGGER:        SetPreserveTrigger(checkPreservedFlag.isChecked());             break;
                    case Id::CHECK_DISABLED:                SetDisabledTrigger(checkDisabled.isChecked());                  break;
                    case Id::CHECK_IGNORE_CONDITIONS_ONCE:  SetIgnoreConditionsOnce(checkIgnoreConditionsOnce.isChecked()); break;
                    case Id::CHECK_IGNORE_WAITSKIP_ONCE:    SetIgnoreWaitSkipOnce(checkIgnoreWaitSkipOnce.isChecked());     break;
                    case Id::CHECK_IGNORE_MISCACTIONS_ONCE: SetIgnoreMiscActionsOnce(checkIgnoreManyActions.isChecked());   break;
                    case Id::CHECK_IGNORE_DEFEAT_DRAW:      SetIgnoreDefeatDraw(checkIgnoreDefeatDraw.isChecked());         break;
                    case Id::CHECK_IS_PAUSED:               SetPausedTrigger(checkFlagPaused.isChecked());                  break;
                    case Id::BUTTON_COMMENT_PROPERTIES:     ButtonCommentProperties();                                      break;
                    case Id::BUTTON_NOTES_PROPERTIES:       ButtonNotesProperties();                                        break;
                }
                break;
        }
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT TrigGeneralWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == FALSE )
                OnLeave();
            else if ( wParam == TRUE )
                RefreshWindow(trigIndex);
            break;

        case WM_CLOSE:
            OnLeave();
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}


void TrigGeneralWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::EDIT_COMMENT: EditCommentFocusLost(); break;
        case Id::EDIT_NOTES: EditNotesFocusLost(); break;
    }
}
