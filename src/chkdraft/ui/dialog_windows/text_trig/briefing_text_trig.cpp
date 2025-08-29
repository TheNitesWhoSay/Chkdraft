#include "briefing_text_trig.h"
#include "chkdraft/chkdraft.h"
#include "mapping/chkd_profiles.h"
#include <string>

BriefingTextTrigWindow::~BriefingTextTrigWindow()
{

}

bool BriefingTextTrigWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TEXTTRIG), hParent) )
    {
        ClassDialog::SetWinText("Briefing Text Triggers");
        briefingTextTrigMenu.FindThis(getHandle());
        updateMenus();
        ShowWindow(getHandle(), SW_SHOW);
        return true;
    }
    else
        return false;
}

void BriefingTextTrigWindow::RefreshWindow()
{
    updateMenus();
    std::string briefingTrigString;
    BriefingTextTrigGenerator briefingTextTrigs {};
    if ( briefingTextTrigs.generateBriefingTextTrigs((Scenario &)*CM, briefingTrigString) )
    {
        auto start = std::chrono::high_resolution_clock::now();
        SetDialogItemText(IDC_EDIT_TRIGTEXT, briefingTrigString);
        auto finish = std::chrono::high_resolution_clock::now();
        logger.debug() << "Windows updated textbox contents in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    }
    else
        Error("Failed to generate briefing text triggers.");
}

void BriefingTextTrigWindow::updateMenus()
{
    if ( chkd.profiles().triggers.useAddressesForMemory )
        briefingTextTrigMenu.SetCheck(ID_OPTIONS_USEADDRESSESFORMEMORY, true);
    else
        briefingTextTrigMenu.SetCheck(ID_OPTIONS_USEADDRESSESFORMEMORY, false);
}

BOOL BriefingTextTrigWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case ID_OPTIONS_USEADDRESSESFORMEMORY:
        chkd.profiles().triggers.useAddressesForMemory = !chkd.profiles().triggers.useAddressesForMemory;
        chkd.profiles.saveCurrProfile();
        RefreshWindow();
        break;
    case ID_REFRESH_TEXTTRIGS:
        RefreshWindow();
        break;
    case IDC_COMPSAVE:
        if ( CM != nullptr )
        {
            if ( CompileEditText(*CM) )
            {
                CM->refreshScenario();
                RefreshWindow();
                if ( chkd.maps.SaveCurr(false) )
                    WinLib::Message("Success", "Compiler");
                else
                    WinLib::Message("Compile Succeeded, Save Failed", "Compiler");
            }
        }
        else
            Error("No map open!");
        break;
    case ID_COMPILE_TRIGS:
        if ( CM != nullptr )
        {
            if ( CompileEditText(*CM) )
            {
                CM->refreshScenario();
                RefreshWindow();
                WinLib::Message("Success", "Compiler");
            }
        }
        else
            Error("No map open!");
        break;
    }
    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL BriefingTextTrigWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            editControl.FindThis(hWnd, IDC_EDIT_TRIGTEXT);
            editControl.MaximizeTextLimit();
            RefreshWindow();
            break;

        case WM_MOUSEWHEEL:
            PostMessage(GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT), WM_MOUSEWHEEL, wParam, lParam);
            break;

        case WM_SIZE:
            {
                RECT rcClient, rcCompile, rcCompSave, rcRefresh;

                HWND hEdit     = GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT ),
                     hCompile  = GetDlgItem(hWnd, ID_COMPILE_TRIGS),
                     hCompSave = GetDlgItem(hWnd, IDC_COMPSAVE),
                     hRefresh  = GetDlgItem(hWnd, ID_REFRESH_TEXTTRIGS);

                GetClientRect(hWnd, &rcClient);
                GetWindowRect(hCompile, &rcCompile);
                GetWindowRect(hCompSave, &rcCompSave);
                GetWindowRect(hRefresh, &rcRefresh);

                SetWindowPos(hCompSave, hWnd, rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10, rcClient.bottom - (rcCompSave.bottom-rcCompSave.top) - 10, (rcCompSave.right-rcCompSave.left), (rcCompSave.bottom-rcCompSave.top), SWP_NOZORDER|SWP_NOACTIVATE);
                SetWindowPos(hCompile, hWnd, (rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10)-(rcCompile.right-rcCompile.left+5), rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 10, (rcCompile.right-rcCompile.left), (rcCompile.bottom-rcCompile.top), SWP_NOZORDER|SWP_NOACTIVATE);
                SetWindowPos(hRefresh, hWnd, (rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10)-(rcCompile.right-rcCompile.left+5)-(rcRefresh.right-rcRefresh.left)-5, rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 10, rcRefresh.right - rcRefresh.left, rcRefresh.bottom - rcRefresh.top, SWP_NOZORDER|SWP_NOACTIVATE);
                SetWindowPos(hEdit, hWnd, 0, 0, rcClient.right, (rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 20), SWP_NOZORDER|SWP_NOACTIVATE);
            }
            break;

        case WM_CLOSE:
            DestroyDialog();
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

bool BriefingTextTrigWindow::CompileEditText(Scenario & map)
{
    if ( auto briefingTrigText = editControl.GetWinText() )
    {
        BriefingTextTrigCompiler compiler {}; // All data for compilation is gathered on-the-fly, no need to check for updates
        auto edit = CM->operator()(ActionDescriptor::CompileBriefingTextTrigs);
        CM->skipEventRendering();
        if ( compiler.compileBriefingTriggers(*briefingTrigText, (Scenario &)map, *chkd.scData, 0, map.numBriefingTriggers()) )
            return true;
        else
            WinLib::Message("Compilation failed.", "Error!");
    }
    else
        WinLib::Message("Compilation failed, couldn't get edit text.", "Error!");

    return false;
}
