#include "TreeViewControl.h"
#include <SimpleIcu.h>

namespace WinLib {

    TreeViewControl::~TreeViewControl()
    {

    }

    bool TreeViewControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool hasButtons, u64 id)
    {
        DWORD dwStyle = WS_VISIBLE|WS_CHILD|TVS_NONEVENHEIGHT|TVS_HASLINES;
        if ( hasButtons )
            dwStyle |= TVS_HASBUTTONS|TVS_LINESATROOT;

        return WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_TREEVIEW, "", 
                                             dwStyle,
                                             x, y, width, height, hParent, (HMENU)id, true );
    }

    HTREEITEM TreeViewControl::InsertTreeItem(HTREEITEM hParentItem, const std::string & text, LPARAM lParam)
    {
        if ( hParentItem == NULL )
            return InsertParent(text, lParam);
        else
        {
            icux::uistring sysText = icux::toUistring(text);

            TVINSERTSTRUCT tvinsert = { };
            tvinsert.hParent = hParentItem;
            tvinsert.hInsertAfter = TVI_LAST;
            tvinsert.item.mask = TVIF_TEXT|LVIF_PARAM;
            tvinsert.item.pszText = (LPTSTR)sysText.c_str();
            tvinsert.item.lParam = lParam;
            return TreeView_InsertItem(getHandle(), &tvinsert);
        }
    }

    HTREEITEM TreeViewControl::InsertParent(const std::string & text, LPARAM lParam)
    {
        icux::uistring sysText = icux::toUistring(text);

        TVINSERTSTRUCT tvinsert = { };
        tvinsert.hParent = NULL;
        tvinsert.hInsertAfter = TVI_ROOT;
        tvinsert.item.mask = TVIF_TEXT|TVIF_PARAM;
        tvinsert.item.pszText = (LPTSTR)sysText.c_str();
        tvinsert.item.lParam = lParam;
        return TreeView_InsertItem(getHandle(), &tvinsert);
    }

    bool TreeViewControl::SetHandle(HWND hWnd)
    {
        /** By doing this in a slightly circruitous manner WindowControls
            in general don't have to allow easy handle changes */
        return WindowControl::FindThis(GetParent(hWnd), GetDlgCtrlID(hWnd));
    }

    bool TreeViewControl::SetItemText(HTREEITEM hItem, const std::string & newText)
    {
        icux::uistring sysNewText = icux::toUistring(newText);

        TVITEM item = { };
        item.mask = TVIF_TEXT;
        item.hItem = hItem;
        item.pszText = (LPTSTR)sysNewText.c_str();
        return SendMessage(getHandle(), TVM_SETITEM, 0, (LPARAM)&item) == TRUE;
    }

    bool TreeViewControl::ExpandItem(HTREEITEM hItem)
    {
        return SendMessage(getHandle(), TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem) != 0;
    }

    void TreeViewControl::EmptySubTree(HTREEITEM hRoot)
    {
        HTREEITEM hChild = TreeView_GetChild(getHandle(), hRoot);
        while ( hChild != NULL )
        {
            TreeView_DeleteItem(getHandle(), hChild);
            hChild = TreeView_GetChild(getHandle(), hRoot);
        }
    }

    LRESULT TreeViewControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if ( msg == WM_CHAR && (GetKeyState(VK_CONTROL) & 0x8000) )
             return 0; // Prevent ctrl + key from causing beeps
        else
            return WindowControl::ControlProc(hWnd, msg, wParam, lParam);
    }

}
