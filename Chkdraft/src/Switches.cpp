#include "Switches.h"
#include "Chkdraft.h"

enum class Id
{
    TreeSwitches = ID_FIRST,
    CheckUseDefaultName,
    CheckUseExtendedString,
    EditSwitchName
};

SwitchesWindow::SwitchesWindow() : selectedSwitch(-1), refreshing(false), refreshingSwitchList(false)
{

}

bool SwitchesWindow::CreateThis(HWND hParent, u32 windowId)
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
            u32 switchStringNum = 0;
            bool hasName = false;
            bool isExtendedString = false;
            if ( CM->getSwitchStrId((u8)selectedSwitch, switchStringNum) )
                hasName = switchStringNum != 0;

            checkUseDefaultName.SetCheck(!hasName);

            if ( hasName )
            {
                isExtendedString = CM->isExtendedString(switchStringNum);
                ChkdString switchName;
                if ( CM->getSwitchName(switchName, (u8)selectedSwitch) )
                    editSwitchName.SetText(switchName.c_str());
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
        u32 numSwitches = CM->NumNameableSwitches();
        HTREEITEM item = NULL;
        for ( u32 i = 0; i < numSwitches; i++ )
        {
            ChkdString switchName;
            if ( CM->getSwitchName(switchName, i) )
                item = switchList.InsertTreeItem(NULL, (std::string("Switch ") + std::to_string(i + 1) + " - " + switchName).c_str(), i);
            else
                item = switchList.InsertTreeItem(NULL, (std::string("Switch ") + std::to_string(i + 1)).c_str(), i);

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
    switchList.CreateThis(hWnd, 5, 5, 175, 100, false, (u32)Id::TreeSwitches);
    checkUseDefaultName.CreateThis(hWnd, 200, 5, 110, 20, true, "Use Default Name", (u32)Id::CheckUseDefaultName);
    checkUseExtendedString.CreateThis(hWnd, 320, 5, 125, 20, false, "Use Extended String", (u32)Id::CheckUseExtendedString);
    textSwitchName.CreateThis(hWnd, 200, 30, 70, 20, "Switch Name: ", (u32)0);
    editSwitchName.CreateThis(hWnd, 275, 30, 100, 20, false, (u32)Id::EditSwitchName);

    RefreshWindow();
}

void SwitchesWindow::ToggleUseDefaultString()
{
    if ( checkUseDefaultName.isChecked() )
    {
        if ( CM->switchHasName((u8)selectedSwitch) )
        {
            CM->removeSwitchName((u8)selectedSwitch);
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
    CM->ToggleUseExtendedSwitchName((u8)selectedSwitch);
    checkUseExtendedString.SetCheck(CM->SwitchUsesExtendedName((u8)selectedSwitch));
    CM->notifyChange(false);
}

void SwitchesWindow::EditSwitchNameFocusLost()
{
    if ( !checkUseDefaultName.isChecked() )
    {
        std::string editText = editSwitchName.GetWinText();
        if ( editText.length() > 0 )
        {
            ChkdString temp(editText);
            if ( CM->setSwitchName(temp, (u8)selectedSwitch, checkUseExtendedString.isChecked()) )
            {
                chkd.mapSettingsWindow.RefreshWindow();
                chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
                CM->notifyChange(false);
            }
        }
    }
}

void SwitchesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( (Id)idFrom )
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
    switch ( (Id)idFrom )
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
