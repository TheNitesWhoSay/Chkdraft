#include "TextTrig.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Settings.h"
#include <string>

TextTrigWindow::~TextTrigWindow()
{

}

bool TextTrigWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TEXTTRIG), hParent) )
    {
        textTrigMenu.FindThis(getHandle());
        updateMenus();
        ShowWindow(getHandle(), SW_SHOW);
        return true;
    }
    else
        return false;
}

void TextTrigWindow::RefreshWindow()
{
    updateMenus();
    std::string trigString;
    TextTrigGenerator textTrigs(Settings::useAddressesForMemory, Settings::deathTableStart);
    if ( textTrigs.generateTextTrigs(*CM, trigString) )
    {
        auto start = std::chrono::high_resolution_clock::now();
        SetDialogItemText(IDC_EDIT_TRIGTEXT, trigString);
        auto finish = std::chrono::high_resolution_clock::now();
        logger.debug() << "Windows updated textbox contents in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    }
    else
        Error("Failed to generate text triggers.");
}

void TextTrigWindow::updateMenus()
{
    if ( Settings::useAddressesForMemory )
        textTrigMenu.SetCheck(ID_OPTIONS_USEADDRESSESFORMEMORY, true);
    else
        textTrigMenu.SetCheck(ID_OPTIONS_USEADDRESSESFORMEMORY, false);
}

BOOL TextTrigWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case ID_OPTIONS_USEADDRESSESFORMEMORY:
        Settings::useAddressesForMemory = !Settings::useAddressesForMemory;
        Settings::updateSettingsFile();
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
                {
                    WinLib::Message("Compile Succeeded, Save Failed", "Compiler");
                    CM->notifyChange(false);
                }
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
                CM->notifyChange(false);
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

BOOL TextTrigWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

bool TextTrigWindow::CompileEditText(Scenario & map)
{
    std::string trigText;
    if ( editControl.GetWinText(trigText) )
    {
        TextTrigCompiler compiler(Settings::useAddressesForMemory, Settings::deathTableStart); // All data for compilation is gathered on-the-fly, no need to check for updates
        if ( compiler.compileTriggers(trigText, map, chkd.scData, 0, map.numTriggers()) )
            return true;
        else
            WinLib::Message("Compilation failed.", "Error!");
    }
    else
        WinLib::Message("Compilation failed, couldn't get edit text.", "Error!");

    return false;
}
