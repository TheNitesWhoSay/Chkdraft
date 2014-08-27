#include "StringPreview.h"

bool StringPreviewWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, CreateSolidBrush(RGB(0, 0, 0)), NULL, "StringPreview", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, NULL, WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 5, 455, 583, 70, hParent, (HMENU)ID_PREVIEW_STRING) )
	{
		return true;
	}
	else
		return false;
}

LRESULT StringPreviewWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
