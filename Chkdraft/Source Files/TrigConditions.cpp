#include "TrigConditions.h"

bool TrigConditionsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigConditions", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigConditions", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigConditionsWindow::DestroyThis()
{
	return false;
}

void TrigConditionsWindow::RefreshWindow()
{

}

void TrigConditionsWindow::DoSize()
{
	
}

void TrigConditionsWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "Conditions...", 0);
}
