#include "ListViewControl.h"
#include <SimpleIcu.h>
#include <CommCtrl.h>
#include <iostream>

namespace WinLib {

    ListViewControl::~ListViewControl()
    {

    }

    bool ListViewControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool ownerDrawn, u64 id)
    {
        u32 style = WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS;
        if ( editable )
            style |= LVS_EDITLABELS;
        if ( ownerDrawn )
            style |= LVS_OWNERDRAWFIXED|LVS_OWNERDATA;

        if ( WindowControl::CreateControl(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", style,
                                          x, y, width, height, hParent, (HMENU)id, false) )
        {
            return true;
        }
        return false;
    }

    bool ListViewControl::AddColumn(int insertAt, const std::string & title, int width, int alignmentFlags)
    {
        icux::uistring sysTitle = icux::toUistring(title);

        LV_COLUMN lvCol = { };
        lvCol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM|LVCF_ORDER;
        lvCol.fmt = alignmentFlags;
        lvCol.cx = width;
        lvCol.iSubItem = insertAt+1;
        lvCol.pszText = (LPTSTR)sysTitle.c_str();
        lvCol.cchTextMax = sizeof(title);
        lvCol.iOrder = insertAt;

        return ListView_InsertColumn(getHandle(), insertAt, &lvCol) != -1;
    }

    bool ListViewControl::SetColumnWidth(int column, int width)
    {
        return ListView_SetColumnWidth(getHandle(), column, width) == TRUE;
    }

    void ListViewControl::AddRow(int numColumns, LPARAM lParam)
    {
        LVITEM item = { };
        item.iItem = (int)lParam;
        item.mask = LVIF_COLUMNS|LVIF_PARAM;
        item.cColumns = numColumns;
        item.lParam = lParam;

        ListView_InsertItem(getHandle(), &item);
    }

    void ListViewControl::AddRow()
    {
        LVITEM item = { };
        item.mask = LVIF_COLUMNS;
        item.cColumns = GetNumColumns();

        ListView_InsertItem(getHandle(), &item);
    }

    void ListViewControl::RemoveRow(int rowNum)
    {
        ListView_DeleteItem(getHandle(), rowNum);
    }

    void ListViewControl::SetItemText(int row, int column, const std::string & text)
    {
        icux::uistring sysText = icux::toUistring(text);

        LVITEM subItem = { };
        subItem.mask = LVIF_TEXT;
        subItem.iItem = row;
        subItem.pszText = (LPTSTR)sysText.c_str();
        subItem.cchTextMax = (int)sysText.length();
        subItem.iSubItem = column;

        ListView_SetItem(getHandle(), &subItem);
    }

    void ListViewControl::SetItemText(int row, int column, int value)
    {
        SetItemText(row, column, std::to_string(value));
    }

    void ListViewControl::SetItemData(int row, int column, u32 data)
    {
        LVITEM subItem = { };
        subItem.mask = LVIF_PARAM;
        subItem.iItem = row;
        subItem.iSubItem = column;
        subItem.lParam = (LPARAM)data;

        ListView_SetItem(getHandle(), &subItem);
    }

    void ListViewControl::EnableFullRowSelect()
    {
        if ( getHandle() != NULL )
            SendMessage(getHandle(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
    }

    bool ListViewControl::EnsureVisible(int index, bool ensureEntirelyVisible)
    {
        if ( ensureEntirelyVisible )
            return SendMessage(getHandle(), LVM_ENSUREVISIBLE, (WPARAM)index, (LPARAM)TRUE) == TRUE;
        else
            return SendMessage(getHandle(), LVM_ENSUREVISIBLE, (WPARAM)index, (LPARAM)FALSE) == TRUE;
    }

    bool ListViewControl::DeleteAllItems()
    {
        return SendMessage(getHandle(), LVM_DELETEALLITEMS, 0, 0) == TRUE;
    }

    bool ListViewControl::FocusItem(int index)
    {
        LVITEM item = { };
        item.stateMask = LVIS_FOCUSED;
        item.state = LVIS_FOCUSED;
        return SendMessage(getHandle(), LVM_SETITEMSTATE, index, (LPARAM)&item) == TRUE;
    }

    bool ListViewControl::SelectRow(int index)
    {
        LVITEM item = { };
        item.stateMask = LVIS_SELECTED;
        item.state = LVIS_SELECTED;
        return SendMessage(getHandle(), LVM_SETITEMSTATE, index, (LPARAM)&item) == TRUE;
    }

    void ListViewControl::RedrawHeader()
    {
        HWND hHeader = (HWND)SendMessage(getHandle(), LVM_GETHEADER, 0, 0);
        RedrawWindow(hHeader, NULL, NULL, RDW_INVALIDATE);
    }

    //bool ListViewControl::SortItems(WPARAM value, LPARAM function)
    //{
    //  return false;
        //SendMessage(getHandle(), LVM_SORTITEMS, value, function);
    //}

    int ListViewControl::GetItemRow(int lParam)
    {
        LVFINDINFO lvFind = { };
        lvFind.flags = LVFI_PARAM;
        lvFind.lParam = lParam;
        return ListView_FindItem(getHandle(), -1, &lvFind);
    }

    int ListViewControl::GetColumnWidth(int column)
    {
        return (int)SendMessage(getHandle(), LVM_GETCOLUMNWIDTH, column, 0);
    }

    int ListViewControl::GetNumColumns()
    {
        HWND header = (HWND)SendMessage(getHandle(), LVM_GETHEADER, 0, 0);
        return (int)SendMessage(header, HDM_GETITEMCOUNT, 0, 0);
    }

    bool ListViewControl::GetItemAt(int xCoord, int yCoord, int & itemRow, int & itemColumn)
    {
        LVHITTESTINFO lvHitTestInfo = { };
        lvHitTestInfo.pt.x = (LONG)xCoord;
        lvHitTestInfo.pt.y = (LONG)yCoord;
        lvHitTestInfo.flags = LVHT_ONITEM;
        if ( SendMessage(getHandle(), LVM_HITTEST, 0, (LPARAM)&lvHitTestInfo) != -1 )
        {
            int foundRow = lvHitTestInfo.iItem;
            if ( SendMessage(getHandle(), LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHitTestInfo) != -1 )
            {
                itemRow = foundRow;
                itemColumn = lvHitTestInfo.iSubItem;
                return true;
            }
        }
        return false;
    }

    bool ListViewControl::GetRowAt(int yCoord, int & itemRow)
    {
        int xCoord = 0;
        RECT topBounds = { };
        topBounds.left = LVIR_BOUNDS;
        if ( SendMessage(getHandle(), LVM_GETITEMRECT, 0, (LPARAM)&topBounds) == TRUE )
            xCoord = topBounds.left;

        LVHITTESTINFO lvHitTestInfo = { };
        lvHitTestInfo.pt.x = (LONG)xCoord;
        lvHitTestInfo.pt.y = (LONG)yCoord;
        lvHitTestInfo.flags = LVHT_ONITEM;
        if ( SendMessage(getHandle(), LVM_HITTEST, 0, (LPARAM)&lvHitTestInfo) != -1 )
        {
            itemRow = lvHitTestInfo.iItem;
            return true;
        }
        return false;
    }

    bool ListViewControl::GetColumnAt(int xCoord, int & itemColumn)
    {
        int yCoord = 0;
        RECT topBounds = { };
        topBounds.left = LVIR_BOUNDS;
        if ( SendMessage(getHandle(), LVM_GETITEMRECT, 0, (LPARAM)&topBounds) == TRUE )
            yCoord = topBounds.top;

        LVHITTESTINFO lvHitTestInfo = { };
        lvHitTestInfo.pt.x = (LONG)xCoord;
        lvHitTestInfo.pt.y = (LONG)yCoord;
        lvHitTestInfo.flags = LVHT_ONITEM;
        if ( SendMessage(getHandle(), LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHitTestInfo) != -1 )
        {
            itemColumn = lvHitTestInfo.iSubItem;
            return true;
        }
        return false;
    }

    bool ListViewControl::GetItemRect(int x, int y, RECT & rect)
    {
        if ( x == 0 ) // Work around Microsoft's error
        {
            rect.top = x;
            rect.left = LVIR_LABEL;
            if ( SendMessage(getHandle(), LVM_GETSUBITEMRECT, (WPARAM)y, (LPARAM)(&rect)) != 0 )
            {
                LONG right = rect.right;
                rect.top = x;
                rect.left = LVIR_BOUNDS;
                if ( SendMessage(getHandle(), LVM_GETSUBITEMRECT, (WPARAM)y, (LPARAM)(&rect)) != 0 )
                {
                    rect.right = right;
                    return true;
                }
            }
        }
        else
        {
            rect.top = x;
            rect.left = LVIR_BOUNDS;
            return SendMessage(getHandle(), LVM_GETSUBITEMRECT, (WPARAM)y, (LPARAM)(&rect)) != 0;
        }
        return false;
    }

}
