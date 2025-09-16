#include "profile_editor_settings.h"

enum_t(Id, u32, {
    TEMP = ID_FIRST
});

bool ProfileEditorSettingsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Editor Settings", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Editor Settings", WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

void ProfileEditorSettingsWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
}

void ProfileEditorSettingsWindow::RefreshWindow()
{

}

LRESULT ProfileEditorSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void ProfileEditorSettingsWindow::CreateSubWindows(HWND hWnd)
{
    testText.CreateThis(hWnd, 50, 50, 100, 50, "EditorSettings...", Id::TEMP);
}
