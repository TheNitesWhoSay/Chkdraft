#include "ListBoxControl.h"
#include <SimpleIcu.h>
#include <iostream>

namespace WinLib {

    ListBoxControl::ListBoxControl() : autoRedraw(true)
    {

    }

    ListBoxControl::~ListBoxControl()
    {

    }

    bool ListBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height,
        bool ownerDrawn, bool multiColumn, bool scrollBar, bool alphaSort, u64 id)
    {
        u32 style = WS_CHILD|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY;
        if ( ownerDrawn && multiColumn )
            style |= LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN|LBS_EXTENDEDSEL;
        else if ( ownerDrawn )
            style |= LBS_OWNERDRAWVARIABLE|WS_VSCROLL;
        else if ( multiColumn )
            style |= LBS_MULTICOLUMN|LBS_EXTENDEDSEL;

        if ( alphaSort )
            style |= LBS_SORT;

        if ( scrollBar )
            style |= WS_VSCROLL;

        return WindowControl::CreateControl( WS_EX_CLIENTEDGE|LVS_EX_DOUBLEBUFFER, WC_LISTBOX, "", style,
                                             x, y, width, height, hParent, (HMENU)id, true );
    }

    void ListBoxControl::ClearItems()
    {
        SendMessage(getHandle(), LB_RESETCONTENT, 0, 0);
    }

    int ListBoxControl::AddItem(u32 item)
    {
        LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, 0, (LPARAM)item);
        if ( result == LB_ERR || result == LB_ERRSPACE )
            return -1;
        else
            return (int)result;
    }

    int ListBoxControl::AddString(const std::string & str)
    {
#ifdef WINDOWS_UTF16
        std::wstring wStr = icux::toUtf16(str);
        LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, 0, (LPARAM)wStr.c_str());
#else
        LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, 0, (LPARAM)str.c_str());
#endif
        if ( result == LB_ERR || result == LB_ERRSPACE )
            return -1;
        else
            return (int)result;
    }

    int ListBoxControl::AddStrings(const std::vector<std::string> & strs)
    {
        int result = -1;
        for ( auto str : strs )
            result = AddString(str);

        return result;
    }

    bool ListBoxControl::InsertString(int index, const std::string & string)
    {
#ifdef WINDOWS_UTF16
        std::wstring wString = icux::toUtf16(string);
        LRESULT result = SendMessage(getHandle(), LB_INSERTSTRING, (WPARAM)index, (LPARAM)wString.c_str());
#else
        LRESULT result = SendMessage(getHandle(), LB_INSERTSTRING, (WPARAM)index, (LPARAM)string.c_str());
#endif
        return result != LB_ERR && result != LB_ERRSPACE;
    }

    bool ListBoxControl::InsertItem(int index, u32 item)
    {
        LRESULT result = SendMessage(getHandle(), LB_INSERTSTRING, (WPARAM)index, (LPARAM)item);
        return result != LB_ERR && result != LB_ERRSPACE;
    }

    bool ListBoxControl::ClearSel()
    {
        LONG result = GetWindowLong(getHandle(), GWL_STYLE);
        if ( (result&LBS_EXTENDEDSEL) == LBS_EXTENDEDSEL )
            return SendMessage(getHandle(), LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1) != LB_ERR;
        else
            return SendMessage(getHandle(), LB_SETCURSEL, (WPARAM)-1, (LPARAM)NULL) != LB_ERR;
    }

    bool ListBoxControl::SetCurSel(int index)
    {
        return SendMessage(getHandle(), LB_SETCURSEL, (WPARAM)index, (LPARAM)NULL) != LB_ERR;
    }

    bool ListBoxControl::SelectItem(int index)
    {
        return SendMessage(getHandle(), LB_SETSEL, TRUE, index) != LB_ERR;
    }

    bool ListBoxControl::SetItemData(int index, u32 data)
    {
        return SendMessage(getHandle(), LB_SETITEMDATA, (WPARAM)index, (LPARAM)data) != LB_ERR;
    }

    bool ListBoxControl::SetItemHeight(int index, int height)
    {
        return SendMessage(getHandle(), LB_SETITEMHEIGHT, (WPARAM)index, (LPARAM)height) != LB_ERR;
    }

    bool ListBoxControl::RemoveItem(int index)
    {
        return SendMessage(getHandle(), LB_DELETESTRING, index, 0) != LB_ERR;
    }

    bool ListBoxControl::SetTopIndex(int index)
    {
        return SendMessage(getHandle(), LB_SETTOPINDEX, (WPARAM)index, 0) != LB_ERR;
    }

    int ListBoxControl::GetNumItems()
    {
        LRESULT result = SendMessage(getHandle(), LB_GETCOUNT, 0, 0);
        if ( result != LB_ERR )
            return (int)result;
        else
            return 0;
    }

    int ListBoxControl::ItemHeight()
    {
        LRESULT result = SendMessage(getHandle(), LB_GETITEMHEIGHT, 0, 0);
        if ( result != LB_ERR )
            return (int)result;
        else
            return 0;
    }

    bool ListBoxControl::GetItemHeight(int index, int & height)
    {
        LRESULT result = SendMessage(getHandle(), LB_GETITEMHEIGHT, (WPARAM)index, 0);
        if ( result != LB_ERR )
        {
            height = (int)result;
            return true;
        }
        else
            return false;
    }

    int ListBoxControl::ColumnWidth()
    {
        RECT rect;
        if ( SendMessage(getHandle(), LB_GETITEMRECT, 0, (LPARAM)&rect) == LB_ERR )
            return 0;
        else
            return rect.right-rect.left;
    }

    bool ListBoxControl::GetCurSel(int & sel)
    {
        LRESULT result = SendMessage(getHandle(), LB_GETCURSEL, 0, 0);
        if ( result != LB_ERR )
        {
            sel = (int)result;
            return true;
        }
        else
            return false;
    }

    int ListBoxControl::GetNumSel()
    {
        LRESULT result = SendMessage(getHandle(), LB_GETSELCOUNT, 0, 0);
        if ( result != LB_ERR )
            return (int)result;
        else
            return 0;
    }

    bool ListBoxControl::GetSelString(int index, output_param std::string & str)
    {
        LRESULT numSel = SendMessage(getHandle(), LB_GETSELCOUNT, 0, 0);
        if ( numSel != LB_ERR && numSel > 0 )
        {
            if ( index < numSel ) // Index must be within the amount of items selected
            {
                int arraySize = index+1;
                std::unique_ptr<int> selections = std::unique_ptr<int>(new int[arraySize]);
                LRESULT result = SendMessage(getHandle(), LB_GETSELITEMS, (WPARAM)arraySize, (LPARAM)selections.get());
                if ( result != LB_ERR )
                {
                    int selectedItem = selections.get()[index];
                    result = SendMessage(getHandle(), LB_GETTEXTLEN, (WPARAM)selectedItem, 0);
                    if ( result != LB_ERR )
                    {
                        int textLength = 1+(int)result;
                        std::unique_ptr<TCHAR> text = std::unique_ptr<TCHAR>(new TCHAR[textLength]);
                        result = SendMessage(getHandle(), LB_GETTEXT, (WPARAM)selectedItem, (LPARAM)text.get());
                        if ( result != LB_ERR )
                        {
                            str = icux::toUtf8(text.get());
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool ListBoxControl::GetCurSelString(output_param std::string & str)
    {
        int selectedItem = -1;
        if ( GetCurSel(selectedItem) )
        {
            LRESULT result = SendMessage(getHandle(), LB_GETTEXTLEN, (WPARAM)selectedItem, 0);
            if ( result != LB_ERR )
            {
                int textLength = 1 + (int)result;
                std::unique_ptr<TCHAR> text = std::unique_ptr<TCHAR>(new TCHAR[textLength]);
                result = SendMessage(getHandle(), LB_GETTEXT, (WPARAM)selectedItem, (LPARAM)text.get());
                if ( result != LB_ERR )
                {
                    str = icux::toUtf8(text.get());
                    return true;
                }
            }
        }
        return false;
    }

    bool ListBoxControl::GetSelItem(int index, int & itemData)
    {
        LRESULT numSel = SendMessage(getHandle(), LB_GETSELCOUNT, 0, 0);
        if ( numSel != LB_ERR && numSel > 0 )
        {
            if ( index < numSel ) // Index must be within the amount of items selected
            {
                int arraySize = index+1;
                int* selections = nullptr;
                try { selections = new int[arraySize]; } // Need space for this index and all items before
                catch ( std::bad_alloc ) { return false; }
                LRESULT result = SendMessage(getHandle(), LB_GETSELITEMS, (WPARAM)arraySize, (LPARAM)selections);
                if ( result != LB_ERR )
                {
                    itemData = int(SendMessage(getHandle(), LB_GETITEMDATA, selections[index], 0));
                    delete[] selections;
                    return true;
                }
                else
                    delete[] selections;
            }
        }
        return false;
    }

    bool ListBoxControl::GetItemData(int index, u32 & data)
    {
        LRESULT result = SendMessage(getHandle(), LB_GETITEMDATA , (WPARAM)index, 0);
        if ( result != LB_ERR )
        {
            data = (u32)result;
            return true;
        }
        else
            return false;
    }

    int ListBoxControl::GetTopIndex()
    {
        return (int)SendMessage(getHandle(), LB_GETTOPINDEX, 0, 0);
    }

    LRESULT ListBoxControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch ( msg )
        {
            case LB_ADDSTRING:
                if ( !autoRedraw )
                {
                    try {
                        itemsToAdd.push((u32)lParam);
                        return itemsToAdd.size()-1;
                    } catch ( std::exception ) { return -1; }
                }
                break;
            case WM_SETREDRAW:
                if ( wParam == TRUE && autoRedraw == false )
                {
                    SendMessage(GetParent(hWnd), LB::WM_PREMEASUREITEMS, 0, (LPARAM)hWnd);
                    while ( !itemsToAdd.empty() )
                    {
                        WindowControl::ControlProc(hWnd, LB_ADDSTRING, 0, (LPARAM)itemsToAdd.front());
                        itemsToAdd.pop();
                    }
                    SendMessage(GetParent(hWnd), LB::WM_POSTMEASUREITEMS, 0, (LPARAM)hWnd);
                    autoRedraw = true;
                }
                else if ( wParam == FALSE && autoRedraw == true )
                    autoRedraw = false;
                break;
            case WM_PAINT:
                {
                    SendMessage(GetParent(hWnd), LB::WM_PREDRAWITEMS, 0, (LPARAM)hWnd);
                    LRESULT result = WindowControl::ControlProc(hWnd, msg, wParam, lParam);
                    // Could replace default drawing with double buffering
                    SendMessage(GetParent(hWnd), LB::WM_POSTDRAWITEMS, 0, (LPARAM)hWnd);
                    return result;
                }
                break;
            case WM_MOUSEWHEEL:
                SetTopIndex(GetTopIndex()-(int((s16(HIWORD(wParam)))/WHEEL_DELTA)));
                return 0;
                break;
            case WM_LBUTTONDBLCLK:
                SendMessage(GetParent(hWnd), LB::WM_DBLCLKITEM, 0, (LPARAM)hWnd);
                return 0;
                break;
            default:
                return WindowControl::ControlProc(hWnd, msg, wParam, lParam);
                break;
        }
        return WindowControl::ControlProc(hWnd, msg, wParam, lParam);
    }

}
