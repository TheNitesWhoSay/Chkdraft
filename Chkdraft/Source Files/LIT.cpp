#include "LIT.h"
#include "Chkdraft.h"
using namespace std;

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
		if ( RunLua(chkd.maps.curr) )
		{
			chkd.maps.curr->notifyChange(false);
			chkd.maps.curr->refreshScenario();
			MessageBox(NULL, "Success", "Compiler", MB_OK);
		}
	}
	else
		Error("No map open!");
}

void LitWindow::ButtonLitSave()
{
	if ( chkd.maps.curr != nullptr )
	{
		if ( RunLua(chkd.maps.curr) )
		{
			chkd.maps.curr->refreshScenario();
			if ( chkd.maps.SaveCurr(false) )
				MessageBox(NULL, "Success", "Compiler", MB_OK);
			else
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

bool LitWindow::RunLua(ScenarioPtr map)
{
	std::string luaPath;
	char chkdDir[MAX_PATH] = {};
	if ( editPath.GetEditText(luaPath) )
	{
		if ( GetCurrentDirectory(MAX_PATH, chkdDir) )
		{
			string workingDir(string(chkdDir) + "\\chkd\\Tools\\LIT\\bin\\");
			string litPath(workingDir + "LIT.exe");
			string txtPath(workingDir + "out.txt");
			std::remove(txtPath.c_str());
			int result = (int)ShellExecute(NULL, "open", litPath.c_str(), luaPath.c_str(), workingDir.c_str(), SW_SHOWNORMAL);
			if ( result > 32 )
			{
				buffer luaOutputData;
				if ( FileToBuffer(txtPath.c_str(), luaOutputData) )
				{
					TextTrigCompiler compiler;
					if ( compiler.CompileTriggers(luaOutputData, map) )
						return true;
					else
						Error("Trigger compilation failed.");
				}
				else
					Error("LIT error, output file not present.");
			}
			else
				Error(string("ShellExecute on LIT failed: " + to_string(result)).c_str());
		}
		else
			Error("Failed to get Chkdraft's directory.");
	}
	else
		Error("Empty path or failed to get edit text.");

	return false;
}
