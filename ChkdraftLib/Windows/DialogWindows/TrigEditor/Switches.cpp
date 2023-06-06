#include "Switches.h"
#include "../../../Chkdraft.h"
#include "../../ChkdControls/ChkdStringInput.h"
#include <CommCtrl.h>

enum_t(Id, u32, {
    TreeSwitches = ID_FIRST,
    CheckUseDefaultName,
    EditSwitchName,
    ButtonSwitchNameProperties
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
            size_t switchNameStringId = CM->getSwitchNameStringId(selectedSwitch);
            bool hasName = switchNameStringId != Chk::StringId::NoString;
            bool isExtendedString = false;

            checkUseDefaultName.SetCheck(!hasName);

            if ( hasName )
            {
                if ( auto switchName = CM->getString<ChkdString>(switchNameStringId) )
                    editSwitchName.SetText(*switchName);
            }
            else
            {
                textSwitchName.DisableThis();
                editSwitchName.SetText("");
                editSwitchName.DisableThis();
                buttonSwitchNameProperties.DisableThis();
            }
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
            if ( auto switchName = CM->getSwitchName<ChkdString>(i) )
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
    buttonSwitchNameProperties.DisableThis();
    textSwitchName.DisableThis();
    editSwitchName.SetText("");
    editSwitchName.DisableThis();
}

void SwitchesWindow::EnableEditing()
{
    checkUseDefaultName.EnableThis();
    buttonSwitchNameProperties.EnableThis();
    textSwitchName.EnableThis();
    editSwitchName.EnableThis();
}

void SwitchesWindow::CreateSubWindows(HWND hWnd)
{
    switchList.CreateThis(hWnd, 5, 5, 175, 100, false, Id::TreeSwitches);
    checkUseDefaultName.CreateThis(hWnd, 200, 5, 110, 20, true, "Use Default Name", Id::CheckUseDefaultName);
    textSwitchName.CreateThis(hWnd, 200, 32, 70, 20, "Switch Name: ", (u32)0);
    editSwitchName.CreateThis(hWnd, 275, 30, 100, 24, false, Id::EditSwitchName);
    buttonSwitchNameProperties.CreateThis(hWnd, editSwitchName.Left() + editSwitchName.Width() + 1, 30, 23, 23, "", Id::ButtonSwitchNameProperties, true);
    buttonSwitchNameProperties.SetImageFromResourceId(IDB_PROPERTIES);

    RefreshWindow();
}

void SwitchesWindow::ToggleUseDefaultString()
{
    if ( checkUseDefaultName.isChecked() )
    {
        size_t switchNameStringId = CM->getSwitchNameStringId(selectedSwitch);
        if ( switchNameStringId != Chk::StringId::NoString )
        {
            CM->setSwitchNameStringId(selectedSwitch, switchNameStringId);
            CM->deleteString(switchNameStringId);
            CM->notifyChange(false);
        }

        textSwitchName.DisableThis();
        editSwitchName.SetText("");
        editSwitchName.DisableThis();
        buttonSwitchNameProperties.DisableThis();
        RefreshSwitchList();
    }
    else
    {
        textSwitchName.EnableThis();
        editSwitchName.EnableThis();
        buttonSwitchNameProperties.EnableThis();
    }
}

void SwitchesWindow::ButtonSwitchNameProperties()
{
    if ( selectedSwitch >= 0 && selectedSwitch < Chk::TotalSwitches )
    {
        auto gameString = CM->getSwitchName<ChkdString>(selectedSwitch, Chk::StrScope::Game);
        auto editorString = CM->getSwitchName<ChkdString>(selectedSwitch, Chk::StrScope::Editor);
        ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), gameString, editorString, Chk::StringUserFlag::Switch, selectedSwitch);

        if ( (result & ChkdStringInputDialog::Result::GameStringChanged) == ChkdStringInputDialog::Result::GameStringChanged )
        {
            if ( gameString )
                CM->setSwitchName<ChkdString>(selectedSwitch, *gameString, Chk::StrScope::Game);
            else
                CM->setSwitchNameStringId(selectedSwitch, Chk::StringId::NoString, Chk::StrScope::Game);

            CM->deleteUnusedStrings(Chk::StrScope::Game);
        }

        if ( (result & ChkdStringInputDialog::Result::EditorStringChanged) == ChkdStringInputDialog::Result::EditorStringChanged )
        {
            if ( editorString )
                CM->setSwitchName<ChkdString>(selectedSwitch, *editorString, Chk::StrScope::Editor);
            else
                CM->setSwitchNameStringId(selectedSwitch, Chk::StringId::NoString, Chk::StrScope::Editor);

            CM->deleteUnusedStrings(Chk::StrScope::Editor);
        }

        if ( result > 0 )
            CM->refreshScenario();
    }
}

void SwitchesWindow::EditSwitchNameFocusLost()
{
    if ( !checkUseDefaultName.isChecked() )
    {
        auto editText = editSwitchName.GetWinText();
        if ( editText && editText->length() > 0 )
        {
            ChkdString temp(*editText);
            CM->setSwitchName(selectedSwitch, temp);
            CM->deleteUnusedStrings(Chk::StrScope::Both);
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
        case Id::ButtonSwitchNameProperties: ButtonSwitchNameProperties(); break;
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
