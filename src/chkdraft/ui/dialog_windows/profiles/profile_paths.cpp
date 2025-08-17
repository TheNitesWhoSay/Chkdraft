#include "profile_paths.h"

enum_t(Id, u32, {
    TEMP = ID_FIRST
});

bool ProfilePathsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Profile Paths", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Profile Paths", WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void ProfilePathsWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void ProfilePathsWindow::RefreshWindow()
{

}

LRESULT ProfilePathsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void ProfilePathsWindow::CreateSubWindows(HWND hWnd)
{
    testText.CreateThis(hWnd, 50, 50, 100, 50, "ProfilePaths...", Id::TEMP);
}
