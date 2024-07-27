#include "string_preview.h"

StringPreviewWindow::~StringPreviewWindow()
{

}

bool StringPreviewWindow::CreateThis(HWND hParent, u64 windowId)
{
    return
        ClassWindow::RegisterWindowClass(0, NULL, NULL, WinLib::ResourceManager::getSolidBrush(RGB(0, 0, 0)), NULL, "StringPreview", NULL, false) &&
        ClassWindow::CreateClassWindow(0, "", WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 5, 455, 583, 70, hParent, (HMENU)windowId);
}

bool StringPreviewWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

LRESULT StringPreviewWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}
