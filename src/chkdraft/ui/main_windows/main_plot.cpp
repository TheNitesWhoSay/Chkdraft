#include "main_plot.h"
#include "chkdraft/chkdraft.h"

enum_t(Id, u32, {
    Logger = ID_FIRST
});

MainPlot::~MainPlot()
{

}

bool MainPlot::CreateThis(HWND hParent, u64 id)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE + 1), NULL, "Plot", NULL, false) &&
        ClassWindow::CreateClassWindow(WS_EX_TRANSPARENT, "", WS_CHILD | WS_VISIBLE, 0, 0, 162, 500, hParent, (HMENU)id) )
    {
        return true;
    }
    return false;
}

LRESULT MainPlot::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_CREATE:
        {
            leftBar.CreateThis(hWnd);
            loggerWindow.CreateThis(hWnd, leftBar.Width(), 0, 100, 100, true, Id::Logger);
        }
        break;

        case WM_ERASEBKGND:
            if ( CM == nullptr )
                return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            else
                return TRUE; // Prevent plot from drawing a background over maps
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
