#include "Templates.h"

TemplatesWindow::~TemplatesWindow()
{

}

bool TemplatesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Templates", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Templates", WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TemplatesWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void TemplatesWindow::RefreshWindow()
{

}

void TemplatesWindow::DoSize()
{

}

void TemplatesWindow::CreateSubWindows(HWND hWnd)
{
    WinLib::TextControl text;
    text.CreateThis(hWnd, 0, 0, 200, 20, "Templates...", 0);
}
