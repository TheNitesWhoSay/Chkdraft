#include "TrigPlayers.h"

bool TrigPlayersWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigPlayers", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigPlayers", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigPlayersWindow::DestroyThis()
{
	return false;
}

void TrigPlayersWindow::RefreshWindow()
{

}

void TrigPlayersWindow::DoSize()
{
	
}

void TrigPlayersWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "Players...", 0);
}
