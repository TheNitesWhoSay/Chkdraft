#include "LIT.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Settings.h"
#include <fstream>
#include <string>

LitWindow::~LitWindow()
{

}

bool LitWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_DIALOG_LIT), hParent) )
    {
        ShowWindow(getHandle(), SW_SHOW);
        return true;
    }
    else
        return false;
}

void LitWindow::RefreshWindow()
{

}

void LitWindow::ButtonLit()
{
    if ( CM != nullptr )
    {
        if ( RunLit(*CM) )
        {
            CM->notifyChange(false);
            CM->refreshScenario();
        }
    }
    else
        Error("No map open!");
}

void LitWindow::ButtonLitSave()
{
    if ( CM != nullptr )
    {
        if ( RunLit(*CM) )
        {
            CM->refreshScenario();
            if ( !chkd.maps.SaveCurr(false) )
            {
                WinLib::Message("Compile Succeeded, Save Failed", "Compiler");
                CM->notifyChange(false);
            }
        }
    }
    else
        Error("No map open!");
}

BOOL LitWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
        case IDC_BUTTON_LIT: ButtonLit(); break;
        case IDC_BUTTON_LITSAVE: ButtonLitSave(); break;
    }
    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL LitWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            editPath.FindThis(hWnd, IDC_EDIT_LITPATH);
            RefreshWindow();
            break;

        case WM_CLOSE:
            EndDialog(hWnd, wParam);
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

bool LitWindow::GetLitPaths(std::string & litDirectory, std::string & litPath)
{
    std::string chkdPath = WinLib::GetCurrentExePath();
    if ( chkdPath.length() > 0 )
    {
        auto lastBackslashPos = chkdPath.find_last_of('\\');
        if ( lastBackslashPos != std::string::npos && lastBackslashPos < chkdPath.size() )
        {
            litDirectory = std::string(chkdPath.substr(0, lastBackslashPos) + "\\chkd\\Tools\\LIT\\");
            litPath = litDirectory + "RunLIT.bat";
            return true;
        }
    }
    Error("Failed to determine LIT's directory");
    return false;
}

std::optional<std::string> LitWindow::WriteLitBat(const std::string & inputFilePath, const std::string & litDirectory,
    const std::string & textOutPath, const std::string & trigOutName)
{
    auto inputDirectory = ::getSystemFileDirectory(inputFilePath);
    auto inputFile = ::getSystemFileName(inputFilePath);

    std::string litBatPath = litDirectory + "chkd-LIT_LIT.bat";
    removeFile(litBatPath);
    std::ofstream litBat(litBatPath);
    if ( !::findFile(litDirectory + "LIT.exe") )
        logger.error("LIT.exe not found!");

    if ( litBat.is_open() )
    {
        litBat << "LIT.exe "
            << inputDirectory << " "
            << inputFile << " "
            << litDirectory << " "
            << trigOutName << " > "
            << textOutPath;
        litBat.close();
        return litBatPath;
    }
    return std::nullopt;
}

bool LitWindow::RunLit(Scenario & map)
{
    if ( auto litDirectory = GetToolPath("LIT") )
    {
        auto inputPath = editPath.GetWinText();
        if ( inputPath && ::findFile(*inputPath) )
        {
            std::string textPath(*litDirectory + "chkd-LIT_text.txt"), trigName("chkd-LIT_trigs.txt"), trigPath(*litDirectory + trigName);
            if ( auto litBatPath = WriteLitBat(*inputPath, *litDirectory, textPath, trigName) )
            {
                removeFiles(textPath, trigPath);
                int resultCode = 0;
                if ( WinLib::executeOpen(*litBatPath, *litDirectory, resultCode) )
                {
                    int waitTimes[] = { 30, 70, 900, 1000 }; // Try at 30ms, 100ms, 1000ms, 2000ms
                    bool foundLitText = patientFindFile(textPath, 4, waitTimes);
                    auto litText = ::fileToString(textPath);
                    foundLitText = foundLitText && litText;
                    auto litTrigs = ::fileToString(trigPath);
                    removeFiles(textPath, trigPath, *litBatPath);
                    if ( litTrigs )
                    {
                        TextTrigCompiler compiler(Settings::useAddressesForMemory, Settings::deathTableStart);
                        if ( compiler.compileTriggers(*litTrigs, map, chkd.scData, 0, map.numTriggers()) )
                        {
                            foundLitText ? WinLib::Message(*litText, "LIT") : WinLib::Message("Success!", "Text Trigger Compiler");
                            return true;
                        }
                        else
                            Error("Trigger Compilation Failed!");
                    }
                    else
                        foundLitText ? WinLib::Message(*litText, "LIT") : Error("LIT trigger output file was not found or could not be read.");
                }
                else
                {
                    removeFile(*litBatPath);
                    Error("ShellExecute on LIT.bat failed: " + std::to_string(resultCode));
                }
            }
            else
                Error("Failed to write LIT.bat");
        }
        else
            Error(std::string("Input file not found: ") + (inputPath ? *inputPath : "ERROR"));
    }
    else
        Error("Failed to determine LIT's directory");
    return false;
}
