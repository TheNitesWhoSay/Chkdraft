#include "counters.h"

CountersWindow::~CountersWindow()
{

}

bool CountersWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Counters", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Counters", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool CountersWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void CountersWindow::RefreshWindow()
{

}

void CountersWindow::DoSize()
{
    
}

void CountersWindow::CreateSubWindows(HWND hWnd)
{
    WinLib::TextControl text;
    text.CreateThis(hWnd, 0, 0, 200, 20, "Counters...", 0);
}
