#include "custom_dat_files.h"

enum_t(Id, u32, {
    TEMP = ID_FIRST
});

bool CustomDatFilesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Custom Dat Files", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Custom Dat Files", WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void CustomDatFilesWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void CustomDatFilesWindow::RefreshWindow()
{

}

LRESULT CustomDatFilesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void CustomDatFilesWindow::CreateSubWindows(HWND hWnd)
{
    testText.CreateThis(hWnd, 50, 50, 100, 50, "CustomDatFiles...", Id::TEMP);
}
