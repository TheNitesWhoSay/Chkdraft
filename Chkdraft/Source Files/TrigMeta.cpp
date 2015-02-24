#include "TrigMeta.h"

bool TrigMetaWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigMeta", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigMeta", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigMetaWindow::DestroyThis()
{
	return false;
}

void TrigMetaWindow::RefreshWindow()
{

}

void TrigMetaWindow::DoSize()
{
	
}

void TrigMetaWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "Meta...", 0);
}
