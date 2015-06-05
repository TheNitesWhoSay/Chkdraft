#include "TrigActions.h"

bool TrigActionsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigActions", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigActions", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigActionsWindow::DestroyThis()
{
	return false;
}

void TrigActionsWindow::RefreshWindow()
{

}

void TrigActionsWindow::DoSize()
{
	
}

void TrigActionsWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 5, 5, 200, 20, "Actions...", 0);
}

LRESULT TrigActionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
