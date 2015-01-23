#include "Switches.h"

bool SwitchesWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "Switches", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Switches", WS_CHILD,
			5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)ID_SWITCHES) )
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
