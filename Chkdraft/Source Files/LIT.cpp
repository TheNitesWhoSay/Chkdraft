#include "LIT.h"
#include "Chkdraft.h"

#include <fstream>
#include <string>

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
	if ( chkd.maps.curr != nullptr )
	{
		if ( RunLit(chkd.maps.curr) )
		{
			chkd.maps.curr->notifyChange(false);
			chkd.maps.curr->refreshScenario();
		}
	}
	else
		Error("No map open!");
}

void LitWindow::ButtonLitSave()
{
	if ( chkd.maps.curr != nullptr )
	{
		if ( RunLit(chkd.maps.curr) )
		{
			chkd.maps.curr->refreshScenario();
			if ( !chkd.maps.SaveCurr(false) )
			{
				MessageBox(NULL, "Compile Succeeded, Save Failed", "Compiler", MB_OK);
				chkd.maps.curr->notifyChange(false);
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

bool LitWindow::GetLitPaths(std::string &litDirectory, std::string &litPath)
{
	char cChkdPath[MAX_PATH] = {};
	if ( GetModuleFileName(NULL, cChkdPath, MAX_PATH) != MAX_PATH )
	{
		std::string chkdPath(cChkdPath);
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

bool LitWindow::GetInputPaths(std::string &luaDirectory, std::string &luaName)
{
	std::string luaPath;
	if ( editPath.GetEditText(luaPath) )
	{
		auto lastBackslashPos = luaPath.find_last_of('\\');
		if ( lastBackslashPos != std::string::npos && lastBackslashPos < luaPath.size() )
		{
			luaDirectory = luaPath.substr(0, lastBackslashPos + 1);
			luaName = luaPath.substr(lastBackslashPos + 1);
			return true;
		}
		else
			Error("Couldn't find last backslash in edit text.");
	}
	else
		Error("Empty path or failed to get edit text.");

	return false;
}

bool LitWindow::WriteLitBat(std::string &luaDirectory, std::string &luaName, std::string &litDirectory, std::string &litBatPath,
	std::string &textOutPath, std::string &trigOutName)
{
	litBatPath = litDirectory + "chkd-LIT_LIT.bat";
	RemoveFile(litBatPath);
	std::ofstream litBat(litBatPath);
	if ( litBat.is_open() )
	{
		litBat << "LIT.exe " <<
			"\"" << luaDirectory << "\\\" " <<
			"\"" << luaName << "\" " <<
			"\"" << litDirectory << "\\\" " <<
			"\"" << trigOutName << "\" > " <<
			"\"" << textOutPath << "\"";
		litBat.close();
		return true;
	}
	return false;
}

bool LitWindow::RunLit(ScenarioPtr map)
{
	std::string litDirectory, litPath, luaDirectory, luaName, litBatPath, litText;
	if ( GetLitPaths(litDirectory, litPath) && GetInputPaths(luaDirectory, luaName) )
	{
		std::string textPath(litDirectory + "chkd-LIT_text.txt"), trigName("chkd-LIT_trigs.txt"), trigPath(litDirectory + trigName);
		if ( WriteLitBat(luaDirectory, luaName, litDirectory, litBatPath, textPath, trigName) )
		{
			RemoveFiles(textPath, trigPath);
			int result = (int)ShellExecute(NULL, "open", litBatPath.c_str(), NULL, litDirectory.c_str(), SW_SHOWDEFAULT);
			if ( result > 32 )
			{
				int waitTimes[] = { 30, 70, 900, 1000 }; // Try at 30ms, 100ms, 1000ms, 2000ms
				buffer litTrigs("litT");
				bool foundLitText = PatientFindFile(textPath.c_str(), 4, waitTimes) && FileToString(textPath, litText);
				bool foundLitTrigs = FileToBuffer(trigPath.c_str(), litTrigs);
				RemoveFiles(textPath, trigPath, litBatPath);
				if ( foundLitTrigs )
				{
					TextTrigCompiler compiler;
					if ( compiler.CompileTriggers(litTrigs, map) )
					{
						foundLitText ? Message(litText, "LIT") : Message("Success!", "Text Trigger Compiler");
						return true;
					}
					else
						Error("Trigger Compilation Failed!");
				}
				else
					foundLitText ? Message(litText, "LIT") : Error("LIT trigger output file was not found or could not be read.");
			}
			else
				Error(std::string("ShellExecute on LIT.bat failed: " + std::to_string(result)).c_str());
		}
		else
			Error("Failed to write LIT.bat");
	}
	RemoveFile(litBatPath);
	return false;
}
