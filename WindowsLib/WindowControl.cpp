#include "WindowControl.h"
#include <SimpleIcu.h>

namespace WinLib {

    WindowControl::WindowControl() : defaultProc(NULL)
    {

    }

    WindowControl::~WindowControl()
    {
        ResetProc();
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

    bool WindowControl::CreateControl( DWORD dwExStyle, const icux::uistring & lpClassName, const std::string & lpWindowName, DWORD dwStyle,
                                       int x, int y, int nWidth, int nHeight,
                                       HWND hWndParent, HMENU hMenu, bool redirectProc )
    {
        icux::uistring sysWindowName = icux::toUistring(lpWindowName);

        setHandle(CreateWindowEx(dwExStyle, lpClassName.c_str(), sysWindowName.c_str(),
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
            SetWindowLongPtr(getHandle(), GWLP_USERDATA, (LONG_PTR)this);
            defaultProc = (WNDPROC)GetWindowLongPtr(getHandle(), GWLP_WNDPROC);
            return defaultProc != 0 &&
                GetWindowLongPtr(getHandle(), GWLP_USERDATA) == (LONG_PTR)this &&
                SetWindowLongPtr(getHandle(), GWLP_WNDPROC, (LONG_PTR)&ForwardControlProc) != 0;
        }
        else
            return false;
    }
    
    bool WindowControl::ResetProc()
    {
        return getHandle() != NULL && SetWindowLongPtr(getHandle(), GWLP_WNDPROC, (LONG_PTR)defaultProc) != 0;
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
            case WM_NOTIFY: return ((WindowControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->Notify(hWnd, wParam, (NMHDR*)lParam); break;
            case WM_COMMAND: return ((WindowControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->Command(hWnd, wParam, lParam); break;
        }
        return ((WindowControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->ControlProc(hWnd, msg, wParam, lParam);
    }

}
