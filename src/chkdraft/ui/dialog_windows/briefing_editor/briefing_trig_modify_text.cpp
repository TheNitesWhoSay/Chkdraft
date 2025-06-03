#include "briefing_trig_modify_text.h"
#include "chkdraft/chkdraft.h"
#include "mapping/settings.h"

enum_t(Id, u32, {
    TAB_PLAYERS = 0,
    TAB_ACTIONS,
    TAB_TEXT,

    EDIT_TRIGMODIFYTEXT = ID_FIRST,
    CHECK_AUTOCOMPILE,
    BUTTON_COMPILE,
    BUTTON_COMPILEANDSAVE,
    BUTTON_RELOAD
});

BriefingTrigModifyTextWindow::BriefingTrigModifyTextWindow() : briefingTrigIndex(0), autoCompile(false)
{

}

BriefingTrigModifyTextWindow::~BriefingTrigModifyTextWindow()
{

}

bool BriefingTrigModifyTextWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
    {
        briefingTrigIndex = 0;
        return SetParent(hParent);
    }

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "BriefingTrigModifyTextWindow", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "BriefingTrigModifyTextWindow", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool BriefingTrigModifyTextWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->briefingTrigIndex = 0;
    this->autoCompile = false;
    return true;
}

void BriefingTrigModifyTextWindow::SetTrigIndex(u32 briefingTrigIndex)
{
    this->briefingTrigIndex = briefingTrigIndex;
}

void BriefingTrigModifyTextWindow::RefreshWindow(u32 briefingTrigIndex)
{
    this->briefingTrigIndex = briefingTrigIndex;
    BriefingTextTrigGenerator briefingTextTrigs {};
    briefingTrigText.clear();
    if ( briefingTextTrigs.generateBriefingTextTrigs((Scenario &)*CM, briefingTrigIndex, briefingTrigText) )
        editText.SetText(briefingTrigText);
    else
        mb(briefingTrigIndex, "Failed to generate briefing text triggers.");
}

void BriefingTrigModifyTextWindow::DoSize()
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

void BriefingTrigModifyTextWindow::ParentHidden()
{
    OnLeave();
}

void BriefingTrigModifyTextWindow::CreateSubWindows(HWND hWnd)
{
    editText.CreateThis(hWnd, 0, 0, 100, 100, true, Id::EDIT_TRIGMODIFYTEXT);
    checkAutoCompile.CreateThis(hWnd, 0, 0, 85, 20, false, "Auto-compile", Id::CHECK_AUTOCOMPILE);
    
    buttonReload.CreateThis(hWnd, 0, 0, 75, 23, "Reset", Id::BUTTON_RELOAD);
    buttonCompile.CreateThis(hWnd, 20, 105, 75, 23, "Compile", Id::BUTTON_COMPILE);
    buttonCompileAndSave.CreateThis(hWnd, 75, 105, 93, 23, "Compile && Save", Id::BUTTON_COMPILEANDSAVE);
}

bool BriefingTrigModifyTextWindow::unsavedChanges()
{
    if ( auto newText = editText.GetWinText() )
        return briefingTrigText.compare(*newText) != 0;
    else
        return false;
}

void BriefingTrigModifyTextWindow::Compile(bool silent, bool saveAfter)
{
    if ( CM != nullptr )
    {
        if ( auto newText = editText.GetWinText() )
        {
            if ( briefingTrigText.compare(*newText) != 0 )
            {
                if ( CompileEditText(*newText) )
                {
                    briefingTrigText = *newText;
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

                    RefreshWindow(briefingTrigIndex);
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

bool BriefingTrigModifyTextWindow::CompileEditText(std::string & newText)
{
    if ( CM != nullptr )
    {
        if ( briefingTrigIndex < CM->numBriefingTriggers() )
        {
            BriefingTextTrigCompiler compiler {}; // All data for compilation is gathered on-the-fly, no need to check for updates
            if ( compiler.compileBriefingTrigger(newText, (Scenario &)*CM, chkd.scData, briefingTrigIndex) )
                return true;
            else
                WinLib::Message("Compilation failed.", "Error!");
        }
        else
            WinLib::Message("Compilation failed, couldn't find selected trigger", "Error!");
    }
    return false;
}

void BriefingTrigModifyTextWindow::OnLeave()
{
    if ( autoCompile )
        Compile(true, false);
    else if ( unsavedChanges() )
    {
        if ( WinLib::GetYesNo("Compile changes?", "Message") == WinLib::PromptResult::Yes )
            Compile(false, false);
    }
}

LRESULT BriefingTrigModifyTextWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case Id::CHECK_AUTOCOMPILE:
        if ( HIWORD(wParam) == BN_CLICKED )
            autoCompile = checkAutoCompile.isChecked();
        break;
    case Id::BUTTON_RELOAD:
        if ( HIWORD(wParam) == BN_CLICKED )
            RefreshWindow(briefingTrigIndex);
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

LRESULT BriefingTrigModifyTextWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == FALSE )
                OnLeave();
            else if ( wParam == TRUE )
                RefreshWindow(briefingTrigIndex);
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
