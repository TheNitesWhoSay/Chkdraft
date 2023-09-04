#include "ClassWindow.h"
#include <CommCtrl.h>
#include <SimpleIcu.h>
#include <sstream>
#include <iostream>

namespace WinLib {

    ClassWindow::ClassWindow() : hWndMDIClient(NULL), windowType(WindowType::None), allowEditNotify(true), defaultProc(NULL)
    {
        WindowClassName().clear();
    }

    ClassWindow::~ClassWindow()
    {
        ResetProc();
        if ( getHandle() != NULL )
            DestroyWindow(getHandle());
    }

    bool ClassWindow::RegisterWindowClass(UINT style, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground,
        u64 menuResourceId, const std::string & lpszClassName, HICON hIconSm, bool isMDIChild)
    {
        if ( isMDIChild )
        {
            return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
                menuResourceId, lpszClassName, hIconSm, SetupMDIChildProc);
        }
        else
        {
            return WindowsItem::RegisterWindowClass(style, hIcon, hCursor, hbrBackground,
                menuResourceId, lpszClassName, hIconSm, SetupWndProc);
        }
    }

    bool ClassWindow::CreateClassWindow( DWORD dwExStyle, const std::string & lpWindowName, DWORD dwStyle,
                                         int x, int y, int nWidth, int nHeight,
                                         HWND hWndParent, HMENU hMenu )
    {
        if ( getHandle() == NULL && WindowClassName().length() > 0 )
        {
            windowType = WindowType::Regular;
            icux::uistring sysWindowName = icux::toUistring(lpWindowName);
            icux::uistring sysWindowClassName = icux::toUistring(WindowClassName());

            setHandle(CreateWindowEx(dwExStyle, sysWindowClassName.c_str(), sysWindowName.c_str(),
                dwStyle, x, y, nWidth, nHeight,
                hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)this));

            if ( getHandle() != NULL )
            {
                WindowClassName().clear();
                return true;
            }
        }
        windowType = WindowType::None;
        return false;
    }

    void ClassWindow::DestroyThis()
    {
        WindowsItem::DestroyThis();
        hWndMDIClient = NULL;
        windowType = WindowType::None;
        allowEditNotify = true;
        defaultProc = NULL;
        WindowClassName().clear();
    }

    bool ClassWindow::CreateMdiChild( const std::string & windowName, DWORD dwStyle,
                                      int x, int y, int nWidth, int nHeight,
                                      HWND hParent )
    {
        if ( getHandle() == NULL && WindowClassName().length() > 0 )
        {
            icux::uistring sysWindowName = icux::toUistring(windowName);
            icux::uistring sysWindowClassName = icux::toUistring(WindowClassName());

            windowType = WindowType::MDIChild;
            setHandle(CreateMDIWindow(sysWindowClassName.c_str(), sysWindowName.c_str(), dwStyle,
                x, y, nWidth, nHeight,
                hParent, GetModuleHandle(NULL), (LPARAM)this));

            if ( getHandle() != NULL )
            {
                WindowClassName().clear();
                return true;
            }
        }
        windowType = WindowType::None;
        return false;
    }

    bool ClassWindow::BecomeMDIFrame(MdiClient & client, HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
        int X, int Y, int nWidth, int nHeight, HMENU hMenu)
    {
        WindowType prev = windowType;
        windowType = WindowType::MDIFrame;
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
            case WindowType::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
            case WindowType::MDIChild: return DefMDIChildProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr); break;
        }
        return DefWindowProc(hWnd, WM_NOTIFY, idFrom, (LPARAM)nmhdr);
    }

    void ClassWindow::NotifyTreeItemSelected(LPARAM)
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

    void ClassWindow::HandleDroppedFile(const std::string & dropFilePath)
    {
    
    }

    LRESULT ClassWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
        switch ( windowType )
        {
            case WindowType::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, WM_COMMAND, wParam, lParam); break;
            case WindowType::MDIChild: return DefMDIChildProc(hWnd, WM_COMMAND, wParam, lParam); break;
        }
        return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
    }

    LRESULT ClassWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch ( windowType )
        {
            case WindowType::MDIFrame: return DefFrameProc(hWnd, hWndMDIClient, msg, wParam, lParam); break;
            case WindowType::MDIChild: return DefMDIChildProc(hWnd, msg, wParam, lParam); break;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK ClassWindow::SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ( msg == WM_NCCREATE )
        {
            LONG_PTR classPtr = (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, classPtr);
            ((ClassWindow*)classPtr)->defaultProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
            if ( GetWindowLongPtr(hWnd, GWLP_USERDATA) == classPtr && classPtr != 0 && SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)ForwardWndProc) != 0 )
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
            LONG_PTR classPtr = (LONG_PTR)((LPMDICREATESTRUCT)((LPCREATESTRUCT)lParam)->lpCreateParams)->lParam;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, classPtr);
            ((ClassWindow*)classPtr)->defaultProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
            if ( GetWindowLongPtr(hWnd, GWLP_USERDATA) == classPtr  && classPtr != 0 && SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)ForwardWndProc) != 0 )
                return ((ClassWindow*)classPtr)->WndProc(hWnd, msg, wParam, lParam);
            else
                return FALSE;
        }
        return DefMDIChildProc(hWnd, msg, wParam, lParam);
    }

    bool ClassWindow::ResetProc()
    {
        return getHandle() != NULL && SetWindowLongPtr(getHandle(), GWLP_WNDPROC, (LONG_PTR)defaultProc) != 0;
    }

    LRESULT CALLBACK ClassWindow::ForwardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ClassWindow* classWindow = (ClassWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        switch ( msg )
        {
            case WM_SHOWWINDOW:
            {
                if ( (BOOL)wParam == TRUE )
                    classWindow->NotifyWindowShown();
                else
                {
                    // When window is being hidden unfocus any focused edit boxes
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
                        classWindow->NotifyTreeItemSelected(((NMTREEVIEW*)lParam)->itemNew.lParam);
                        break;
                }
                return classWindow->Notify(hWnd, wParam, (NMHDR*)lParam);
            }
            break;

            case TV::WM_SELTREEITEM:
                classWindow->NotifyTreeItemSelected(lParam);
                break;

            case WM_DROPFILES:
            {
                icux::codepoint dropFilePath[MAX_PATH];
                DragQueryFile((HDROP)wParam, 0, dropFilePath, MAX_PATH);
                DragFinish((HDROP)wParam);
                classWindow->HandleDroppedFile(icux::toUtf8(dropFilePath));
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

}
