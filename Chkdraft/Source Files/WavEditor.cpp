#include "WavEditor.h"
#include "GuiAccel.h"

bool WavEditorWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "WavEditor", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "WavEditor", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_WAVEDITOR) )
	{
		CreateStaticText(getHandle(), 5, 5, 100, 20, "Wav Editor...");
		return true;
	}
	else
		return false;
}

LRESULT WavEditorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
