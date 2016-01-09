#include "Switches.h"

bool SwitchesWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "Switches", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Switches", WS_CHILD,
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

}

void SwitchesWindow::DoSize()
{

}

void SwitchesWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "Switches...", 0);
}
