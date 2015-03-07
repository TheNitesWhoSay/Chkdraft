#include "TrigModifyText.h"
#include "Chkdraft.h"

enum ID {
	TAB_META = 0,
	TAB_PLAYERS,
	TAB_CONDITIONS,
	TAB_ACTIONS,
	TAB_TEXT,

	EDIT_TRIGMODIFYTEXT = ID_FIRST,
	BUTTON_COMPILE,
	BUTTON_COMPILEANDSAVE,
	BUTTON_RELOAD
};

TrigModifyTextWindow::TrigModifyTextWindow() : trigIndex(0)
{

}

bool TrigModifyTextWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigModifyTextWindow", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigModifyTextWindow", WS_CHILD,
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
	return false;
}

void TrigModifyTextWindow::RefreshWindow(u32 trigIndex)
{
	this->trigIndex = trigIndex;
	TextTrigGenerator textTrigs;
	if ( textTrigs.GenerateTextTrigs(chkd.maps.curr, trigIndex, trigText) )
		editText.SetText(trigText.c_str());
	else
		Error("Failed to generate text triggers.");
}

void TrigModifyTextWindow::DoSize()
{
	RECT rect;
	if ( getClientRect(rect) )
	{
		int width = rect.right-rect.left,
			height = rect.bottom-rect.top;
		
		buttonReload.SetPos(
			width-buttonReload.Width()-10,
			rect.bottom-buttonCompileAndSave.Height()-10,
			buttonReload.Width(),
			buttonReload.Height());
		buttonCompileAndSave.SetPos(
			width-buttonCompileAndSave.Width()-10-(buttonReload.Width()+5),
			rect.bottom-buttonCompileAndSave.Height()-10,
			buttonCompileAndSave.Width(),
			buttonCompileAndSave.Height());
		buttonCompile.SetPos(
			width-buttonReload.Width()-10-(buttonCompileAndSave.Width()+5)-(buttonCompile.Width()+5),
			rect.bottom-buttonCompile.Height()-10,
			buttonCompile.Width(),
			buttonCompile.Height());
		editText.SetPos(3, 1, width-5, rect.bottom-buttonCompile.Height()-20);
	}
}

void TrigModifyTextWindow::CreateSubWindows(HWND hWnd)
{
	editText.CreateThis(hWnd, 0, 0, 100, 100, true, EDIT_TRIGMODIFYTEXT);
	buttonReload.CreateThis(hWnd, 0, 0, 75, 23, "Reset", BUTTON_RELOAD);
	buttonCompile.CreateThis(hWnd, 20, 105, 75, 23, "Compile", BUTTON_COMPILE);
	buttonCompileAndSave.CreateThis(hWnd, 75, 105, 93, 23, "Compile && Save", BUTTON_COMPILEANDSAVE);
}

void TrigModifyTextWindow::Compile(bool silent, bool saveAfter)
{
	if ( chkd.maps.curr != nullptr )
	{
		std::string newText;
		if ( editText.GetEditText(newText) )
		{
			if ( trigText.compare(newText) != 0 )
			{
				if ( CompileEditText(newText) )
				{
					trigText = newText;
					chkd.maps.curr->notifyChange(false);
					chkd.maps.curr->refreshScenario();
					if ( saveAfter )
					{
						if ( chkd.maps.SaveCurr(false) )
							MessageBox(NULL, "Success", "Compiler", MB_OK);
						else
							MessageBox(NULL, "Compile Succeeded, Save Failed", "Compiler", MB_OK);
					}
					else if ( !silent )
						MessageBox(NULL, "Success", "Compiler", MB_OK);
				}
			}
			else if ( !silent )
				MessageBox(NULL, "No change", NULL, MB_OK);
		}
		else if ( !silent )
			MessageBox(NULL, "Compilation failed, couldn't get edit text.", "Error!", MB_OK);
	}
	else
		Error("No map open!");
}

bool TrigModifyTextWindow::CompileEditText(std::string &newText)
{
	if ( chkd.maps.curr != nullptr )
	{
		Trigger* trigger;
		if ( chkd.maps.curr->getTrigger(trigger, trigIndex) )
		{
			TextTrigCompiler compiler; // All data for compilation is gathered on-the-fly, no need to check for updates
			if ( compiler.CompileTrigger(newText, trigger, chkd.maps.curr) )
				return true;
			else
				MessageBox(NULL, "Compilation failed.", "Error!", MB_OK);
		}
		else
			MessageBox(NULL, "Compilation failed, couldn't find selected trigger", "Error!", MB_OK);
	}
	return false;
}

LRESULT TrigModifyTextWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == FALSE )
				Compile(true, false);
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case BUTTON_RELOAD:
						if ( HIWORD(wParam) == BN_CLICKED )
							RefreshWindow(trigIndex);
						break;
					case BUTTON_COMPILE:
						if ( HIWORD(wParam) == BN_CLICKED )
							Compile(false, false);
						break;
					case BUTTON_COMPILEANDSAVE:
						if ( HIWORD(wParam) == BN_CLICKED )
							Compile(false, true);
						break;
				}
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}