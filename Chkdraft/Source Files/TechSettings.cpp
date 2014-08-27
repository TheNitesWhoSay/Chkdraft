#include "TechSettings.h"
#include "GuiAccel.h"

bool TechSettingsWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TechSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TechSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_TECHSETTINGS) )
	{
		CreateStaticText(getHandle(), 5, 5, 100, 20, "Tech Settings...");
		return true;
	}
	else
		return false;
}

LRESULT TechSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
