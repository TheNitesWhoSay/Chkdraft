#include "UpgradeSettings.h"
#include "GuiAccel.h"

bool UpgradeSettingsWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "UpgradeSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "UpgradeSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_UPGRADESETTINGS) )
	{
		CreateStaticText(getHandle(), 5, 5, 100, 20, "Upgrade Settings...");
		return true;
	}
	else
		return false;
}

LRESULT UpgradeSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
