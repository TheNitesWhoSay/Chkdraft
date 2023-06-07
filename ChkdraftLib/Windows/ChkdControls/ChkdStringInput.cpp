#include "ChkdStringInput.h"
#include "../../CommonFiles/CommonFiles.h"

enum_t(Id, u32, {
    TAB_GAME_STRING = ID_FIRST,
    TAB_EDITOR_STRING,
    GAME_TEMP_TEXT,
    EDITOR_TEMP_TEXT,
    EDIT_GAME_STRING,
    EDIT_EDITOR_STRING,
    BUTTON_GAME_OK,
    BUTTON_GAME_CANCEL,
    BUTTON_GAME_RESET,
    BUTTON_EDITOR_OK,
    BUTTON_EDITOR_CANCEL,
    BUTTON_EDITOR_RESET,
    CHECK_GAME_NO_STRING,
    CHECK_EDITOR_NO_STRING
});

ChkdStringInputDialog::~ChkdStringInputDialog()
{

}

ChkdStringInputDialog::Result ChkdStringInputDialog::GetChkdString(HWND hParent, std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString, Chk::StringUserFlag stringUser, size_t stringUserIndex, size_t stringSubUserIndex)
{
    ChkdStringInputDialog inputDialog;
    return inputDialog.InternalGetChkdString(hParent, gameString, editorString, stringUser, stringUserIndex, stringSubUserIndex);
}

void ChkdStringInputDialog::ChangeTab(Tab tab)
{
    tabStringTypes.SetCurSel((u32)tab);

    tabStringTypes.HideTab(Id::TAB_GAME_STRING);
    tabStringTypes.HideTab(Id::TAB_EDITOR_STRING);

    switch ( tab )
    {
        case Tab::GameString: tabStringTypes.ShowTab(Id::TAB_GAME_STRING); gameStringWindow.FocusThis(); break;
        case Tab::EditorString: tabStringTypes.ShowTab(Id::TAB_EDITOR_STRING); editorStringWindow.FocusThis(); break;
    }

    currTab = tab;
    UpdateWindowText();
}

void ChkdStringInputDialog::ExitDialog(ExitCode exitCode)
{
    this->exitCode = exitCode;
    switch ( exitCode )
    {
        case ExitCode::Ok:
            EndDialog(getHandle(), IDOK);
            break;

        case ExitCode::Cancel:
        default:
            EndDialog(getHandle(), IDCANCEL);
            break;
    }
}

ChkdStringInputDialog::ChkdStringInputDialog() : stringUser(Chk::StringUserFlag::None), stringUserIndex(0), stringSubUserIndex(0),
    userHasGameString(true), userHasEditorString(true), currTab(Tab::GameString), exitCode(ExitCode::None), gameStringWindow(*this), editorStringWindow(*this)
{

}

ChkdStringInputDialog::Result ChkdStringInputDialog::InternalGetChkdString(HWND hParent, std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString, Chk::StringUserFlag stringUser, size_t stringUserIndex, size_t stringSubUserIndex)
{
    this->initialGameString = std::move(gameString);
    this->initialEditorString = std::move(editorString);
    this->newGameString = this->initialGameString;
    this->newEditorString = this->initialEditorString;
    this->stringUser = stringUser;
    this->stringUserIndex = stringUserIndex;
    this->stringSubUserIndex = stringSubUserIndex;
    this->userHasGameString = stringUser != Chk::StringUserFlag::ExtendedTriggerComment && stringUser != Chk::StringUserFlag::ExtendedTriggerNotes;
    this->userHasEditorString = stringUser != Chk::StringUserFlag::TriggerAction && stringUser != Chk::StringUserFlag::TriggerActionSound;
    if ( !this->userHasGameString )
        this->currTab = Tab::EditorString;

    CreateDialogBox(MAKEINTRESOURCE(IDD_INPUTCHKDSTR), hParent);

    bool gameStringChanged = exitCode == ExitCode::Ok && userHasGameString &&
        ((!this->initialGameString && this->newGameString) || (this->initialGameString && !this->newGameString) ||
        (this->initialGameString && this->newGameString && this->initialGameString->compare(*this->newGameString) != 0));

    bool editorStringChanged = exitCode == ExitCode::Ok && userHasEditorString &&
        ((!this->initialEditorString && this->newEditorString) || (this->initialEditorString && !this->newEditorString ) ||
        (this->initialEditorString && this->newEditorString && this->initialEditorString->compare(*this->newEditorString) != 0));

    if ( gameStringChanged )
        gameString = this->newGameString;

    if ( editorStringChanged )
        editorString = this->newEditorString;

    if ( gameStringChanged && editorStringChanged )
        return Result::BothStringsChanged;
    else if ( gameStringChanged )
        return Result::GameStringChanged;
    else if ( editorStringChanged )
        return Result::EditorStringChanged;
    else
        return Result::NoChange;
}

void ChkdStringInputDialog::UpdateWindowText()
{
    std::string text = "Edit ";
    switch ( currTab )
    {
        case Tab::GameString: text += "Game"; break;
        case Tab::EditorString: text += "Editor"; break;
    }
    text += " String";

    switch ( stringUser )
    {
        case Chk::StringUserFlag::ScenarioName: text += " - [Scenario Name]"; break;
        case Chk::StringUserFlag::ScenarioDescription: text += " - [Scenario Description]"; break;
        case Chk::StringUserFlag::Force: text += " - [Force #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::Location: text += " - [Location #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::OriginalUnitSettings: text += " - [Original Unit Name #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::ExpansionUnitSettings: text += " - [Expansion Unit Name #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::BothUnitSettings: text += " - [Unit Name #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::Sound: text += " - [Sound #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::Switch: text += " - [Switch #" + std::to_string(stringUserIndex+1) + "]"; break;
        case Chk::StringUserFlag::TriggerAction: text += " - [Trigger Text #" + std::to_string(stringUserIndex) + " Action #" + std::to_string(stringSubUserIndex) + "]"; break;
        case Chk::StringUserFlag::TriggerActionSound: text += " - [Trigger Sound #" + std::to_string(stringUserIndex) + " Action #" + std::to_string(stringSubUserIndex) + "]"; break;
        case Chk::StringUserFlag::ExtendedTriggerComment: text += " - [Extended Trigger Comment #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::ExtendedTriggerNotes: text += " - [Extended Trigger Notes #" + std::to_string(stringUserIndex) + "]"; break;
        case Chk::StringUserFlag::BriefingTriggerAction: text += " - [Briefing Trigger #" + std::to_string(stringUserIndex) + " Action #" + std::to_string(stringSubUserIndex) + "]"; break;
        case Chk::StringUserFlag::BriefingTriggerActionSound: text += " - [Briefing Trigger Sound #" + std::to_string(stringUserIndex) + " Action #" + std::to_string(stringSubUserIndex) + "]"; break;
    }

    SetWinText(text);
}

BOOL ChkdStringInputDialog::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case TCN_SELCHANGE:
    {
        Tab selectedTab = (Tab)tabStringTypes.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::GameString: tabStringTypes.ShowTab(Id::TAB_GAME_STRING); gameStringWindow.FocusThis(); break;
        case Tab::EditorString: tabStringTypes.ShowTab(Id::TAB_EDITOR_STRING); editorStringWindow.FocusThis(); break;
        }
        currTab = selectedTab;
        UpdateWindowText();
    }
    break;
    case TCN_SELCHANGING:
    {
        Tab selectedTab = (Tab)tabStringTypes.GetCurSel();
        switch ( selectedTab )
        {
        case Tab::GameString: tabStringTypes.HideTab(Id::TAB_GAME_STRING); break;
        case Tab::EditorString: tabStringTypes.HideTab(Id::TAB_EDITOR_STRING); break;
        }
    }
    break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL ChkdStringInputDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL ChkdStringInputDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_INITDIALOG )
    {
        tabStringTypes.FindThis(hWnd, IDC_TAB_STRINGTYPES);
        if ( userHasGameString )
        {
            tabStringTypes.InsertTab(Id::TAB_GAME_STRING, "Game String");
            gameStringWindow.CreateThis(tabStringTypes.getHandle(), Id::TAB_GAME_STRING);
        }
        if ( userHasEditorString )
        {
            tabStringTypes.InsertTab(Id::TAB_EDITOR_STRING, "Editor String");
            editorStringWindow.CreateThis(tabStringTypes.getHandle(), Id::TAB_EDITOR_STRING);
        }
        UpdateWindowText();
        ChangeTab(currTab);
        ReplaceChildFonts(defaultFont);
        DoSize();

        //editString.FindThis(hWnd, IDC_EDIT1);
        //editString.SetText(initialString);
        //SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
    }
    else if ( msg == WM_CLOSE )
        ExitDialog(ExitCode::Cancel);

    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}

void ChkdStringInputDialog::DoSize()
{
    RECT rcCli;
    getClientRect(rcCli);
    tabStringTypes.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
    tabStringTypes.getClientRect(rcCli);
    
    gameStringWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);
    editorStringWindow.SetPos(rcCli.left, rcCli.top+22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22);

    gameStringWindow.DoSize();
    editorStringWindow.DoSize();
}

ChkdStringInputDialog::GameStringWindow::GameStringWindow(ChkdStringInputDialog & parent) : parent(parent)
{

}

ChkdStringInputDialog::GameStringWindow::~GameStringWindow()
{

}

bool ChkdStringInputDialog::GameStringWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) == TRUE &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "GameString", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "GameString", WS_VISIBLE|WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void ChkdStringInputDialog::GameStringWindow::DoSize()
{

}

void ChkdStringInputDialog::GameStringWindow::ApplyChanges()
{
    if ( checkNoString.isChecked() )
        parent.newGameString = std::nullopt;
    else
        parent.newGameString = editString.GetWinText();
}

void ChkdStringInputDialog::GameStringWindow::FocusThis()
{
    editString.FocusThis();
    editString.SetCaret(editString.GetTextLength());
}

void ChkdStringInputDialog::GameStringWindow::CreateSubWindows(HWND hWnd)
{
    RECT rcCli = {};
    WindowsItem::getClientRect(rcCli);

    s32 buttonY = rcCli.bottom-34;
    s32 buttonAreaHeight = rcCli.bottom - buttonY + 11;
    s32 editHeight = rcCli.bottom-rcCli.top - buttonAreaHeight - 11;
    buttonReset.CreateThis(hWnd, rcCli.right-rcCli.left-86, buttonY, 75, 23, "Reset", Id::BUTTON_GAME_RESET);
    buttonCancel.CreateThis(hWnd, buttonReset.Left()-79, buttonY, 75, 23, "Cancel", Id::BUTTON_GAME_CANCEL);
    buttonOk.CreateThis(hWnd, buttonCancel.Left()-79, buttonY, 75, 23, "OK", Id::BUTTON_GAME_OK);
    editString.CreateThis(hWnd, 11, 11, rcCli.right-rcCli.left-22, editHeight, true, Id::EDIT_GAME_STRING);
    editString.SetWinText(parent.initialGameString ? *parent.initialGameString : "");
    checkNoString.CreateThis(hWnd, 11, buttonY, 75, 23, !parent.initialGameString, "No String", Id::CHECK_GAME_NO_STRING);
    if ( parent.currTab == ChkdStringInputDialog::Tab::GameString )
        editString.FocusThis();
}

void ChkdStringInputDialog::GameStringWindow::ButtonOk()
{
    ApplyChanges();
    parent.editorStringWindow.ApplyChanges();
    parent.ExitDialog(ExitCode::Ok);
}

void ChkdStringInputDialog::GameStringWindow::ButtonReset()
{
    parent.newGameString = parent.initialGameString;
    checkNoString.SetCheck(!parent.newGameString);
    editString.SetWinText(parent.newGameString ? *parent.newGameString : "");
}

void ChkdStringInputDialog::GameStringWindow::ButtonCancel()
{
    parent.ExitDialog(ExitCode::Cancel);
}

void ChkdStringInputDialog::GameStringWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::BUTTON_GAME_OK: ButtonOk(); break;
        case Id::BUTTON_GAME_RESET: ButtonReset(); break;
        case Id::BUTTON_GAME_CANCEL: ButtonCancel(); break;
        case Id::CHECK_GAME_NO_STRING:
            if ( checkNoString.isChecked() )
            {
                parent.newGameString.reset();
                editString.SetWinText("");
            }
            break;
    }
}

void ChkdStringInputDialog::GameStringWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( checkNoString.isChecked() && idFrom == Id::EDIT_GAME_STRING )
    {
        int length = editString.GetTextLength();
        if ( length > 0 )
            checkNoString.SetCheck(false);
    }
}

ChkdStringInputDialog::EditorStringWindow::EditorStringWindow(ChkdStringInputDialog & parent) : parent(parent)
{

}
ChkdStringInputDialog::EditorStringWindow::~EditorStringWindow()
{

}

bool ChkdStringInputDialog::EditorStringWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) == TRUE &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "EditorString", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "EditorString", WS_VISIBLE|WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top-22,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void ChkdStringInputDialog::EditorStringWindow::DoSize()
{

}

void ChkdStringInputDialog::EditorStringWindow::ApplyChanges()
{
    if ( checkNoString.isChecked() )
        parent.newEditorString = std::nullopt;
    else
        parent.newEditorString = editString.GetWinText();
}

void ChkdStringInputDialog::EditorStringWindow::FocusThis()
{
    editString.FocusThis();
    editString.SetCaret(editString.GetTextLength());
}

void ChkdStringInputDialog::EditorStringWindow::CreateSubWindows(HWND hWnd)
{
    RECT rcCli = {};
    WindowsItem::getClientRect(rcCli);

    s32 buttonY = rcCli.bottom-34;
    s32 buttonAreaHeight = rcCli.bottom - buttonY + 11;
    s32 editHeight = rcCli.bottom-rcCli.top - buttonAreaHeight - 11;
    buttonReset.CreateThis(hWnd, rcCli.right-rcCli.left-86, buttonY, 75, 23, "Reset", Id::BUTTON_EDITOR_RESET);
    buttonCancel.CreateThis(hWnd, buttonReset.Left()-79, buttonY, 75, 23, "Cancel", Id::BUTTON_EDITOR_CANCEL);
    buttonOk.CreateThis(hWnd, buttonCancel.Left()-79, buttonY, 75, 23, "OK", Id::BUTTON_EDITOR_OK);
    editString.CreateThis(hWnd, 11, 11, rcCli.right-rcCli.left-22, editHeight, true, Id::EDIT_EDITOR_STRING);
    editString.SetWinText(parent.initialEditorString ? *parent.initialEditorString : "");
    checkNoString.CreateThis(hWnd, 11, buttonY, 75, 23, !parent.initialEditorString, "No String", Id::CHECK_EDITOR_NO_STRING);
    if ( parent.currTab == ChkdStringInputDialog::Tab::EditorString )
        editString.FocusThis();
}

void ChkdStringInputDialog::EditorStringWindow::ButtonOk()
{
    ApplyChanges();
    parent.gameStringWindow.ApplyChanges();
    parent.ExitDialog(ExitCode::Ok);
}

void ChkdStringInputDialog::EditorStringWindow::ButtonReset()
{
    parent.newEditorString = parent.initialEditorString;
    checkNoString.SetCheck(!parent.newEditorString);
    editString.SetWinText(parent.newEditorString ? *parent.newEditorString : "");
}

void ChkdStringInputDialog::EditorStringWindow::ButtonCancel()
{
    parent.ExitDialog(ExitCode::Cancel);
}

void ChkdStringInputDialog::EditorStringWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::BUTTON_EDITOR_OK: ButtonOk(); break;
        case Id::BUTTON_EDITOR_RESET: ButtonReset(); break;
        case Id::BUTTON_EDITOR_CANCEL: ButtonCancel(); break;
        case Id::CHECK_EDITOR_NO_STRING:
            if ( checkNoString.isChecked() )
            {
                parent.newEditorString.reset();
                editString.SetWinText("");
            }
            break;
    }
}

void ChkdStringInputDialog::EditorStringWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( checkNoString.isChecked() && idFrom == Id::EDIT_EDITOR_STRING )
    {
        int length = editString.GetTextLength();
        if ( length > 0 )
            checkNoString.SetCheck(false);
    }
}
