#include "StringPreview.h"

bool StringPreviewWindow::CreateThis(HWND hParent, u32 windowId)
{
	return
		ClassWindow::RegisterWindowClass(NULL, NULL, NULL, CreateSolidBrush(RGB(0, 0, 0)), NULL, "StringPreview", NULL, false) &&
		ClassWindow::CreateClassWindow(NULL, NULL, WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 5, 455, 583, 70, hParent, (HMENU)windowId);
}

LRESULT StringPreviewWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
