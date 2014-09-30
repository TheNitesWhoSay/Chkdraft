#include "TreeViewControl.h"

bool TreeViewControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u32 id)
{
	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_TREEVIEW, "", 
										 WS_VISIBLE|WS_CHILD|TVS_NONEVENHEIGHT|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
										 x, y, width, height, hParent, (HMENU)id, true );
}

TV_INSERTSTRUCT TreeViewControl::InsertChild(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam)
{
	tvinsert.hInsertAfter = TVI_LAST;
	tvinsert.item.mask |= LVIF_PARAM;
	tvinsert.item.pszText = (LPSTR)pszText;
	tvinsert.item.lParam = lParam;
	tvinsert.hParent = TreeView_InsertItem(hWnd, &tvinsert);
	return tvinsert;
}

TV_INSERTSTRUCT TreeViewControl::InsertParent(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam)
{
	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter = TVI_ROOT;
	tvinsert.item.mask |= (TVIF_TEXT|TVIF_PARAM);
	tvinsert.item.pszText = (LPSTR)pszText;
	tvinsert.item.lParam = lParam;
	tvinsert.hParent = TreeView_InsertItem(hWnd, &tvinsert);
	return tvinsert;
}

bool TreeViewControl::SetHandle(HWND hWnd)
{
	return WindowControl::FindThis(GetParent(hWnd), GetDlgCtrlID(hWnd));
}

LRESULT TreeViewControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_CHAR && (GetKeyState(VK_CONTROL) & 0x8000) )
		 return 0; // Prevent ctrl + key from causing beeps
	else
		return WindowControl::CallDefaultProc(hWnd, msg, wParam, lParam);
}
