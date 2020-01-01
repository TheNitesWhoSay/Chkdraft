#include "Switches.h"
#include "../../../Chkdraft.h"

enum_t(Id, u32, {
    TreeSwitches = ID_FIRST,
    CheckUseDefaultName,
    CheckUseExtendedString,
    EditSwitchName
});

SwitchesWindow::SwitchesWindow() : selectedSwitch(-1), refreshing(false), refreshingSwitchList(false)
{

}

SwitchesWindow::~SwitchesWindow()
{

}

bool SwitchesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Switches", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Switches", WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool SwitchesWindow::DestroyThis()
{
    return false;
}

void SwitchesWindow::RefreshWindow()
{
    if ( !refreshing )
    {
        refreshing = true;

        RefreshSwitchList();

        if ( selectedSwitch >= 0 && selectedSwitch < 256 )
        {
            chkd.trigEditorWindow.SetWinText("Trigger Editor - [Switch " + std::to_string(selectedSwitch + 1) + "]");

            EnableEditing();
            size_t switchNameStringId = CM->triggers.getSwitchNameStringId(selectedSwitch);
            bool hasName = switchNameStringId != Chk::StringId::NoString;
            bool isExtendedString = false;

            checkUseDefaultName.SetCheck(!hasName);

            if ( hasName )
            {
                ChkdStringPtr switchName = CM->strings.getString<ChkdString>(switchNameStringId);
                if ( switchName != nullptr )
                    editSwitchName.SetText(*switchName);
            }
            else
            {
                textSwitchName.DisableThis();
                editSwitchName.SetText("");
                editSwitchName.DisableThis();
                checkUseExtendedString.SetCheck(false);
                checkUseExtendedString.DisableThis();
            }

            checkUseExtendedString.SetCheck(isExtendedString);
        }
        else
            DisableEditing();

        refreshing = false;
    }
}

void SwitchesWindow::RefreshSwitchList()
{
    if ( !refreshingSwitchList )
    {
        refreshingSwitchList = true;

        switchList.SetRedraw(false);
        switchList.EmptySubTree(NULL);
        HTREEITEM item = NULL;
        for ( size_t i = 0; i < Chk::TotalSwitches; i++ )
        {
            ChkdStringPtr switchName = CM->strings.getSwitchName<ChkdString>(i);
            if ( switchName != nullptr )
                item = switchList.InsertTreeItem(NULL, "Switch " + std::to_string(i + 1) + " - " + *switchName, i);
            else
                item = switchList.InsertTreeItem(NULL, "Switch " + std::to_string(i + 1), i);

            if ( i == (u32)selectedSwitch )
                TreeView_SelectItem(switchList.getHandle(), item);
        }
        switchList.SetRedraw(true);

        refreshingSwitchList = false;
    }
}

void SwitchesWindow::DoSize()
{
    switchList.SetPos(switchList.Left(), switchList.Top(), switchList.Width(), cliHeight() - switchList.Top() - 5);
}

void SwitchesWindow::DisableEditing()
{
    checkUseDefaultName.SetCheck(false);
    checkUseDefaultName.DisableThis();
    checkUseExtendedString.SetCheck(false);
    checkUseExtendedString.DisableThis();
    textSwitchName.DisableThis();
    editSwitchName.SetText("");
    editSwitchName.DisableThis();
}

void SwitchesWindow::EnableEditing()
{
    checkUseDefaultName.EnableThis();
    checkUseExtendedString.EnableThis();
    textSwitchName.EnableThis();
    editSwitchName.EnableThis();
}

void SwitchesWindow::CreateSubWindows(HWND hWnd)
{
    switchList.CreateThis(hWnd, 5, 5, 175, 100, false, Id::TreeSwitches);
    checkUseDefaultName.CreateThis(hWnd, 200, 5, 110, 20, true, "Use Default Name", Id::CheckUseDefaultName);
    checkUseExtendedString.CreateThis(hWnd, 320, 5, 125, 20, false, "Use Extended String", Id::CheckUseExtendedString);
    textSwitchName.CreateThis(hWnd, 200, 30, 70, 20, "Switch Name: ", (u32)0);
    editSwitchName.CreateThis(hWnd, 275, 30, 100, 20, false, Id::EditSwitchName);

    RefreshWindow();
}

void SwitchesWindow::ToggleUseDefaultString()
{
    if ( checkUseDefaultName.isChecked() )
    {
        size_t switchNameStringId = CM->triggers.getSwitchNameStringId(selectedSwitch);
        if ( switchNameStringId != Chk::StringId::NoString )
        {
            CM->triggers.setSwitchNameStringId(selectedSwitch, switchNameStringId);
            CM->strings.deleteString(switchNameStringId);
            CM->notifyChange(false);
        }

        textSwitchName.DisableThis();
        editSwitchName.SetText("");
        editSwitchName.DisableThis();
        checkUseExtendedString.SetCheck(false);
        checkUseExtendedString.DisableThis();
        RefreshSwitchList();
    }
    else
    {
        textSwitchName.EnableThis();
        editSwitchName.EnableThis();
        checkUseExtendedString.EnableThis();
    }
}

void SwitchesWindow::ToggleUseExtendedString()
{
    throw std::exception("TODO: Replace this way of handling strings");
}

void SwitchesWindow::EditSwitchNameFocusLost()
{
    if ( !checkUseDefaultName.isChecked() )
    {
        std::string editText = editSwitchName.GetWinText();
        if ( editText.length() > 0 )
        {
            ChkdString temp(editText);
            CM->strings.setSwitchName(selectedSwitch, temp);
            chkd.mapSettingsWindow.RefreshWindow();
            CM->notifyChange(false);
        }
    }
}

void SwitchesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::CheckUseDefaultName: ToggleUseDefaultString(); break;
        case Id::CheckUseExtendedString: ToggleUseExtendedString(); break;
    }
}

void SwitchesWindow::NotifyTreeSelChanged(LPARAM newValue)
{
    if ( !refreshingSwitchList )
    {
        if ( ::GetFocus() == editSwitchName.getHandle() )
            EditSwitchNameFocusLost();

        if ( newValue >= 0 && newValue < 256 && !refreshing )
        {
            selectedSwitch = (s32)newValue;
            RefreshWindow();
        }
    }
}

void SwitchesWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::EditSwitchName:
            EditSwitchNameFocusLost();
            break;
    }
}

LRESULT SwitchesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW: if ( wParam == TRUE ) RefreshWindow(); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}
