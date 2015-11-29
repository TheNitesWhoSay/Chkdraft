#include "StringGuide.h"

enum ID {
	TEXT_COLOR_FIRST = ID_FIRST,
	TEXT_COLOR_LAST = (TEXT_COLOR_FIRST+27)
};

bool StringGuideWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, CreateSolidBrush(RGB(0, 0, 0)), NULL, "StringGuide", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, NULL, WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 463, 7, 125, 365, hParent, NULL) )
	{
		HWND hStringGuide = getHandle();
		HDC hDC = GetDC(hStringGuide);
		SelectObject(hDC, defaultFont);
		SIZE strSize = { };
		for ( int i=0; i<NUM_STRING_COLORS; i++ )
		{
			GetTextExtentPoint32(hDC, stringColorPrefixes[i], std::strlen(stringColorPrefixes[i]), &strSize);
			colorPrefix[i].CreateThis(hStringGuide, 2, i*13, strSize.cx, 13, stringColorPrefixes[i], 0);
			color[i].CreateThis(hStringGuide, strSize.cx+3, i*13, 100, 13, stringColorStrings[i], TEXT_COLOR_FIRST+i);
		}
		ReleaseDC(hDC);
		return true;
	}
	else
		return false;
}

LRESULT StringGuideWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_CTLCOLORSTATIC:
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				int id = GetDlgCtrlID((HWND)lParam);
				if ( id >= TEXT_COLOR_FIRST && id <= TEXT_COLOR_LAST )
					SetTextColor((HDC)wParam, stringColors[id-TEXT_COLOR_FIRST]);
				else
					SetTextColor((HDC)wParam, RGB(16, 252, 24));

				return (BOOL)CreateSolidBrush(RGB(0, 0, 0));
			}
			break;

		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
