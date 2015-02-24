#include "CUWPs.h"

bool CUWPsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "CUWPs", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "CUWPs", WS_CHILD,
			5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool CUWPsWindow::DestroyThis()
{
	return false;
}

void CUWPsWindow::RefreshWindow()
{

}

void CUWPsWindow::DoSize()
{

}

void CUWPsWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "CUWPs...", 0);
}
