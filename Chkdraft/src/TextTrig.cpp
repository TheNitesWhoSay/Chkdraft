#include "TextTrig.h"
#include "Chkdraft.h"
#include <string>

bool TextTrigWindow::CreateThis(HWND hParent)
{
	if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_TEXTTRIG), hParent) )
	{
		ShowWindow(getHandle(), SW_SHOW);
		return true;
	}
	else
		return false;
}

void TextTrigWindow::RefreshWindow()
{
	std::string trigString;
	TextTrigGenerator textTrigs;
	if ( textTrigs.GenerateTextTrigs(CM, trigString) )
		SetDlgItemText(getHandle(), IDC_EDIT_TRIGTEXT, (const char*)trigString.c_str());
	else
		Error("Failed to generate text triggers.");
}

BOOL TextTrigWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
	case IDC_COMPSAVE:
		if ( CM != nullptr )
		{
			if ( CompileEditText(CM) )
			{
				CM->refreshScenario();
				if ( chkd.maps.SaveCurr(false) )
					MessageBox(NULL, "Success", "Compiler", MB_OK);
				else
				{
					MessageBox(NULL, "Compile Succeeded, Save Failed", "Compiler", MB_OK);
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
			if ( CompileEditText(CM) )
			{
				CM->notifyChange(false);
				CM->refreshScenario();
				MessageBox(NULL, "Success", "Compiler", MB_OK);
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
				RECT rcClient, rcCompile, rcCompSave;

				HWND hEdit	   = GetDlgItem(hWnd, IDC_EDIT_TRIGTEXT ),
					 hCompile  = GetDlgItem(hWnd, ID_COMPILE_TRIGS),
					 hCompSave = GetDlgItem(hWnd, IDC_COMPSAVE);

				GetClientRect(hWnd, &rcClient);
				GetWindowRect(hCompile, &rcCompile);
				GetWindowRect(hCompSave, &rcCompSave);

				SetWindowPos(hCompSave, hWnd, rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10, rcClient.bottom - (rcCompSave.bottom-rcCompSave.top) - 10, (rcCompSave.right-rcCompSave.left), (rcCompSave.bottom-rcCompSave.top), SWP_NOZORDER|SWP_NOACTIVATE);
				SetWindowPos(hCompile, hWnd, (rcClient.right-rcClient.left-(rcCompSave.right-rcCompSave.left)-10)-(rcCompile.right-rcCompile.left+5), rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 10, (rcCompile.right-rcCompile.left), (rcCompile.bottom-rcCompile.top), SWP_NOZORDER|SWP_NOACTIVATE);
				SetWindowPos(hEdit, hWnd, 0, 0, rcClient.right, (rcClient.bottom - (rcCompile.bottom-rcCompile.top) - 20), SWP_NOZORDER|SWP_NOACTIVATE);
			}
			break;

		case WM_CLOSE:
			EndDialog(hWnd, wParam);
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

bool TextTrigWindow::CompileEditText(ScenarioPtr map)
{
	if ( map != nullptr )
	{
		std::string trigText;
		if ( editControl.GetEditText(trigText) )
		{
			TextTrigCompiler compiler; // All data for compilation is gathered on-the-fly, no need to check for updates
			if ( compiler.CompileTriggers(trigText, map, chkd.scData) )
				return true;
			else
				MessageBox(NULL, "Compilation failed.", "Error!", MB_OK);
		}
		else
			MessageBox(NULL, "Compilation failed, couldn't get edit text.", "Error!", MB_OK);
	}
	return false;
}
