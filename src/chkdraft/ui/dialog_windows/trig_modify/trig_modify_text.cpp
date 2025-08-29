#include "trig_modify_text.h"
#include "chkdraft/chkdraft.h"
#include "mapping/chkd_profiles.h"

enum_t(Id, u32, {
    TAB_META = 0,
    TAB_PLAYERS,
    TAB_CONDITIONS,
    TAB_ACTIONS,
    TAB_TEXT,

    EDIT_TRIGMODIFYTEXT = ID_FIRST,
    CHECK_AUTOCOMPILE,
    BUTTON_COMPILE,
    BUTTON_COMPILEANDSAVE,
    BUTTON_RELOAD
});

TrigModifyTextWindow::TrigModifyTextWindow() : trigIndex(0), autoCompile(false)
{

}

TrigModifyTextWindow::~TrigModifyTextWindow()
{

}

bool TrigModifyTextWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
    {
        trigIndex = 0;
        return SetParent(hParent);
    }

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TrigModifyTextWindow", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "TrigModifyTextWindow", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TrigModifyTextWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->trigIndex = 0;
    this->autoCompile = false;
    return true;
}

void TrigModifyTextWindow::SetTrigIndex(u32 trigIndex)
{
    this->trigIndex = trigIndex;
}

void TrigModifyTextWindow::RefreshWindow(u32 trigIndex)
{
    this->trigIndex = trigIndex;
    TextTrigGenerator textTrigs(chkd.profiles().triggers.useAddressesForMemory, chkd.profiles().triggers.deathTableStart);
    trigText.clear();
    if ( textTrigs.generateTextTrigs((Scenario &)*CM, trigIndex, trigText) )
        editText.SetText(trigText);
    else
        mb(trigIndex, "Failed to generate text triggers.");
}

void TrigModifyTextWindow::DoSize()
{
    RECT rect;
    if ( getClientRect(rect) )
    {
        int width = rect.right-rect.left,
            height = rect.bottom-rect.top;
        
        buttonReload.MoveTo(rect.right-buttonReload.Width()-10,
            rect.bottom-buttonCompileAndSave.Height()-10);
        buttonCompileAndSave.MoveTo(buttonReload.Left()-buttonCompileAndSave.Width()-5,
            rect.bottom-buttonCompileAndSave.Height()-10);
        buttonCompile.MoveTo(buttonCompileAndSave.Left()-buttonCompile.Width()-5,
            rect.bottom-buttonCompile.Height()-10);
        checkAutoCompile.MoveTo(buttonCompile.Left()-checkAutoCompile.Width()-5,
            rect.bottom-checkAutoCompile.Height()-10);
        editText.SetPos(3, 1, width-5, rect.bottom-buttonCompile.Height()-20);
    }
}

void TrigModifyTextWindow::ParentHidden()
{
    OnLeave();
}

void TrigModifyTextWindow::CreateSubWindows(HWND hWnd)
{
    editText.CreateThis(hWnd, 0, 0, 100, 100, true, Id::EDIT_TRIGMODIFYTEXT);
    checkAutoCompile.CreateThis(hWnd, 0, 0, 85, 20, false, "Auto-compile", Id::CHECK_AUTOCOMPILE);
    
    buttonReload.CreateThis(hWnd, 0, 0, 75, 23, "Reset", Id::BUTTON_RELOAD);
    buttonCompile.CreateThis(hWnd, 20, 105, 75, 23, "Compile", Id::BUTTON_COMPILE);
    buttonCompileAndSave.CreateThis(hWnd, 75, 105, 93, 23, "Compile && Save", Id::BUTTON_COMPILEANDSAVE);
}

bool TrigModifyTextWindow::unsavedChanges()
{
    if ( auto newText = editText.GetWinText() )
        return trigText.compare(*newText) != 0;
    else
        return false;
}

void TrigModifyTextWindow::Compile(bool silent, bool saveAfter)
{
    if ( CM != nullptr )
    {
        if ( auto newText = editText.GetWinText() )
        {
            if ( trigText.compare(*newText) != 0 )
            {
                if ( CompileEditText(*newText) )
                {
                    trigText = *newText;
                    CM->refreshScenario();
                    if ( saveAfter )
                    {
                        if ( chkd.maps.SaveCurr(false) )
                            WinLib::Message("Success", "Compiler");
                        else
                            WinLib::Message("Compile Succeeded, Save Failed", "Compiler");
                    }
                    else if ( !silent )
                        WinLib::Message("Success", "Compiler");

                    chkd.trigEditorWindow->triggersWindow.RefreshGroupList();
                    RefreshWindow(trigIndex);
                }
            }
            else if ( !silent && saveAfter )
            {
                if ( chkd.maps.SaveCurr(false) )
                    WinLib::Message("Success", "Compiler");
                else
                    WinLib::Message("Compile Succeeded, Save Failed", "Compiler");
            }
            else if ( !silent )
                WinLib::Message("No change");
        }
        else if ( !silent )
            WinLib::Message("Compilation failed, couldn't get edit text.", "Error!");
    }
    else
        Error("No map open!");
}

bool TrigModifyTextWindow::CompileEditText(std::string & newText)
{
    if ( CM != nullptr )
    {
        if ( trigIndex < CM->numTriggers() )
        {
            // All data for compilation is gathered on-the-fly, no need to check for updates
            TextTrigCompiler compiler(chkd.profiles().triggers.useAddressesForMemory, chkd.profiles().triggers.deathTableStart);
            if ( compiler.compileTrigger(newText, (Scenario &)*CM, chkd.scData, trigIndex) )
                return true;
            else
                WinLib::Message("Compilation failed.", "Error!");
        }
        else
            WinLib::Message("Compilation failed, couldn't find selected trigger", "Error!");
    }
    return false;
}

void TrigModifyTextWindow::OnLeave()
{
    if ( autoCompile )
        Compile(true, false);
    else if ( unsavedChanges() )
    {
        if ( WinLib::GetYesNo("Compile changes?", "Message") == WinLib::PromptResult::Yes )
            Compile(false, false);
    }
}

LRESULT TrigModifyTextWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case Id::CHECK_AUTOCOMPILE:
        if ( HIWORD(wParam) == BN_CLICKED )
            autoCompile = checkAutoCompile.isChecked();
        break;
    case Id::BUTTON_RELOAD:
        if ( HIWORD(wParam) == BN_CLICKED )
            RefreshWindow(trigIndex);
        break;
    case Id::BUTTON_COMPILE:
        if ( HIWORD(wParam) == BN_CLICKED )
            Compile(false, false);
        break;
    case Id::BUTTON_COMPILEANDSAVE:
        if ( HIWORD(wParam) == BN_CLICKED )
            Compile(false, true);
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT TrigModifyTextWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
