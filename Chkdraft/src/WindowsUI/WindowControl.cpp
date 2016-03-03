#include "WindowControl.h"

WindowControl::WindowControl() : defaultProc(NULL)
{

}

WindowControl::~WindowControl()
{
    
}

bool WindowControl::FindThis(HWND hParent, u32 controlId)
{
    setHandle(GetDlgItem(hParent, controlId));
    return getHandle() != NULL;
}

bool WindowControl::FindThis(HWND controlHandle)
{
    setHandle(controlHandle);
    return getHandle() != NULL;
}

bool WindowControl::DestroyThis()
{
    if ( getHandle() != NULL && DestroyWindow(getHandle()) == TRUE )
    {
        setHandle(NULL);
        defaultProc = NULL;
        return true;
    }
    else
        return false;
}

bool WindowControl::CreateControl( DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                                   int x, int y, int nWidth, int nHeight,
                                   HWND hWndParent, HMENU hMenu, bool redirectProc )
{
    setHandle(CreateWindowExA(dwExStyle, lpClassName, lpWindowName,
        dwStyle, x, y, nWidth, nHeight,
        hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this));

    if ( getHandle() != NULL && redirectProc && !RedirectProc() )
        DestroyThis();

    return getHandle() != NULL;
}

bool WindowControl::RedirectProc()
{
    if ( getHandle() != NULL )
    {
        SetWindowLong(getHandle(), GWL_USERDATA, (LONG)this);
        defaultProc = (WNDPROC)GetWindowLong(getHandle(), GWL_WNDPROC);
        return defaultProc != 0 &&
               GetWindowLong(getHandle(), GWL_USERDATA) == (LONG)this &&
               SetWindowLong(getHandle(), GWL_WNDPROC, (LONG)&ForwardControlProc) != 0;
    }
    else
        return false;
}

bool WindowControl::CreateThis(HWND, int, int, int, int)
{
    return false;
}

LRESULT WindowControl::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return CallWindowProc(defaultProc, hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr);
}

LRESULT WindowControl::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProc(defaultProc, hWnd, WM_COMMAND, wParam, lParam);
}

LRESULT WindowControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProc(defaultProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowControl::ForwardControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_NOTIFY: return ((WindowControl*)GetWindowLong(hWnd, GWL_USERDATA))->Notify(hWnd, wParam, (NMHDR*)lParam); break;
        case WM_COMMAND: return ((WindowControl*)GetWindowLong(hWnd, GWL_USERDATA))->Command(hWnd, wParam, lParam); break;
    }
    return ((WindowControl*)GetWindowLong(hWnd, GWL_USERDATA))->ControlProc(hWnd, msg, wParam, lParam);
}
