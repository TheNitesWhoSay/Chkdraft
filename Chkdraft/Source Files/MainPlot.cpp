#include "MainPlot.h"
#include "Chkdraft.h"

bool MainPlot::CreateThis(HWND hParent)
{
	return ClassWindow::RegisterWindowClass(NULL, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE+1), NULL, "Plot", NULL, false) &&
		   ClassWindow::CreateClassWindow(NULL, NULL, WS_CHILD|WS_VISIBLE, 0, 0, 162, 500, hParent, (HMENU)IDR_MAIN_PLOT);
}

LRESULT MainPlot::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_CREATE:
			// Setup Left Bar (holds minimap and selection tree)
			leftBar.CreateThis(hWnd);
			break;

		case WM_ERASEBKGND:
			if ( chkd.maps.curr == nullptr )
				return DefWindowProc(hWnd, msg, wParam, lParam);
			else
				return true; // Prevent plot from drawing a background over maps
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
