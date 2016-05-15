#include "ClassWindow.h"
#include <sstream>

ClassWindow::ClassWindow() : hWndMDIClient(NULL), windowType(WindowTypes::None), allowEditNotify(true)
{
    WindowClassName().clear();
}

ClassWindow::~ClassWindow()
{
    if ( getHandle() != NULL )
        DestroyWindow(getHandle());
}

bool ClassWindow::RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
    LPCTSTR lpszMenuName, LPCTSTR lpszClassName, HICON hIconSm, bool isMDIChild)
{
    if ( isMDIChild )
    {
        return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
            lpszMenuName, lpszClassName, hIconSm, SetupMDIChildProc);
    }
    else
    {
        return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
            lpszMenuName, lpszClassName, hIconSm, SetupWndProc);
    }
}

bool ClassWindow::CreateClassWindow( DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
                                     int x, int y, int nWidth, int nHeight,
                                     HWND hWndParent, HMENU hMenu )
{
    if ( getHandle() == NULL && WindowClassName().length() > 0 )
    {
        windowType = WindowTypes::Regular;

        setHandle(CreateWindowEx(dwExStyle, WindowClassName().c_str(), lpWindowName,
            dwStyle, x, y, nWidth, nHeight,
            hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this));

        if ( getHandle() != NULL )
        {
            WindowClassName().clear();
            return true;
        }
    }
    windowType = WindowTypes::None;
    return false;
}

void ClassWindow::DestroyThis()
{
    WindowsItem::DestroyThis();
    hWndMDIClient = NULL;
    windowType = WindowTypes::None;
    allowEditNotify = true;
    WindowClassName().clear();
}

bool ClassWindow::CreateMdiChild( LPCSTR lpWindowName, DWORD dwStyle,
                                  int x, int y, int nWidth, int nHeight,
                                  HWND hParent )
{
    if ( getHandle() == NULL && WindowClassName().length() > 0 )
    {
        windowType = WindowTypes::MDIChild;
        setHandle(CreateMDIWindow(WindowClassName().c_str(), lpWindowName, dwStyle,
            x, y, nWidth, nHeight,
            hParent, GetModuleHandle(NULL), (LPARAM)this));

        if ( getHandle() != NULL )
        {
            WindowClassName().clear();
            return true;
        }
    }
    windowType = WindowTypes::None;
    return false;
}

bool ClassWindow::BecomeMDIFrame(MdiClient &client, HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HMENU hMenu)
{
    WindowTypes prev = windowType;
    windowType = WindowTypes::MDIFrame;
    if ( client.CreateMdiClient( hWindowMenu, idFirstChild, dwStyle,
                                 X, Y, nWidth, nHeight, getHandle(), hMenu ) )
    {
        this->hWndMDIClient = client.getHandle();
        return true;
    }
    windowType = prev;
    return false;
}

LRESULT ClassWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( windowType )
    {
        case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
        case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
    }
    return DefWindowProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr);
}

void ClassWindow::NotifyTreeSelChanged(LPARAM)
{

}

void ClassWindow::NotifyButtonClicked(int, HWND)
{

}

void ClassWindow::NotifyEditUpdated(int, HWND)
{

}

void ClassWindow::NotifyEditFocused(int idFrom, HWND hWndFrom)
{

}

void ClassWindow::NotifyEditFocusLost(int, HWND)
{

}

void ClassWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{

}

void ClassWindow::NotifyComboEditUpdated(int idFrom, HWND hWndFrom)
{

}

void ClassWindow::NotifyComboEditFocused(int idFrom, HWND hWndFrom)
{

}

void ClassWindow::NotifyComboEditFocusLost(int idFrom, HWND hWndFrom)
{

}

void ClassWindow::NotifyWindowHidden()
{

}

void ClassWindow::NotifyWindowShown()
{

}

LRESULT ClassWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( windowType )
    {
        case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_COMMAND, wParam, lParam); break;
        case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, WM_COMMAND, wParam, lParam); break;
    }
    return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
}

LRESULT ClassWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( windowType )
    {
        case WindowTypes::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, msg, wParam, lParam); break;
        case WindowTypes::MDIChild: return DefMDIChildProc(hWnd, msg, wParam, lParam); break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ClassWindow::SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_NCCREATE )
    {
        LONG classPtr = (LONG)((CREATESTRUCT*)lParam)->lpCreateParams;
        SetWindowLong(hWnd, GWL_USERDATA, classPtr);
        if ( GetWindowLong(hWnd, GWL_USERDATA) == classPtr && classPtr != 0 && SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ForwardWndProc) != 0 )
            return ((ClassWindow*)classPtr)->WndProc(hWnd, msg, wParam, lParam);
        else
            return FALSE;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ClassWindow::SendNotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( allowEditNotify )
    {
        allowEditNotify = false;
        NotifyEditUpdated(idFrom, hWndFrom);
        allowEditNotify = true;
    }
}

LRESULT CALLBACK ClassWindow::SetupMDIChildProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_NCCREATE )
    {
        LONG classPtr = (LONG)((MDICREATESTRUCT*)((CREATESTRUCT*)lParam)->lpCreateParams)->lParam;
        SetWindowLong(hWnd, GWL_USERDATA, classPtr);
        if ( GetWindowLong(hWnd, GWL_USERDATA) == classPtr  && classPtr != 0 && SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ForwardWndProc) != 0 )
            return ((ClassWindow*)classPtr)->WndProc(hWnd, msg, wParam, lParam);
        else
            return FALSE;
    }
    return DefMDIChildProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ClassWindow::ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ClassWindow* classWindow = (ClassWindow*)GetWindowLong(hWnd, GWL_USERDATA);
    switch ( msg )
    {
        case WM_SHOWWINDOW:
        {
            if ( (BOOL)wParam == TRUE )
                classWindow->NotifyWindowShown();
            else
            {
                HWND hEditFocused = GetFocus();
                int editCtrlId = GetDlgCtrlID(hEditFocused);
                classWindow->NotifyEditFocusLost(editCtrlId, hEditFocused);
                classWindow->NotifyComboEditFocusLost(editCtrlId, hEditFocused);
                classWindow->NotifyWindowHidden();
            }
            return classWindow->WndProc(hWnd, msg, wParam, lParam);
        }
        break;

        case WM_NOTIFY:
        {
            switch ( ((NMHDR*)lParam)->code )
            {
                case TVN_SELCHANGED:
                    classWindow->NotifyTreeSelChanged(((NMTREEVIEW*)lParam)->itemNew.lParam);
                    break;
            }
            return classWindow->Notify(hWnd, wParam, (NMHDR*)lParam);
        }
        break;

        case WM_COMMAND:
        {
            switch ( HIWORD(wParam) )
            {
                case BN_CLICKED: classWindow->NotifyButtonClicked(LOWORD(wParam), (HWND)lParam); break;
                case EN_UPDATE: classWindow->SendNotifyEditUpdated(LOWORD(wParam), (HWND)lParam); break;
                case EN_SETFOCUS: classWindow->NotifyEditFocused(LOWORD(wParam), (HWND)lParam); break;
                case EN_KILLFOCUS: classWindow->NotifyEditFocusLost(LOWORD(wParam), (HWND)lParam); break;
                case CBN_SELCHANGE: classWindow->NotifyComboSelChanged(LOWORD(wParam), (HWND)lParam); break;
                case CBN_EDITCHANGE: classWindow->NotifyComboEditUpdated(LOWORD(wParam), (HWND)lParam); break;
                case CBN_SETFOCUS: classWindow->NotifyComboEditFocused(LOWORD(wParam), (HWND)lParam); break;
                case CBN_KILLFOCUS: classWindow->NotifyComboEditFocusLost(LOWORD(wParam), (HWND)lParam); break;
            }
            return classWindow->Command(hWnd, wParam, lParam);
        }
        break;
    }
    return classWindow->WndProc(hWnd, msg, wParam, lParam);
}
