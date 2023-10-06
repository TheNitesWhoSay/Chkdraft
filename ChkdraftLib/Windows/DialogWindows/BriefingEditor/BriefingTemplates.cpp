#include "BriefingTemplates.h"

BriefingTemplatesWindow::~BriefingTemplatesWindow()
{

}

bool BriefingTemplatesWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "BriefingTemplates", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "BriefingTemplates", WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool BriefingTemplatesWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

void BriefingTemplatesWindow::RefreshWindow()
{

}

void BriefingTemplatesWindow::DoSize()
{

}

void BriefingTemplatesWindow::CreateSubWindows(HWND hWnd)
{
    WinLib::TextControl text;
    text.CreateThis(hWnd, 0, 0, 200, 20, "Briefing Templates...", 0);
}
