#include "TreeViewControl.h"

bool TreeViewControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u32 id)
{
	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, WC_TREEVIEW, "", 
										 WS_VISIBLE|WS_CHILD|TVS_NONEVENHEIGHT|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
										 x, y, width, height, hParent, (HMENU)id, true );
}

LRESULT TreeViewControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_CHAR && (GetKeyState(VK_CONTROL) & 0x8000) )
		 return 0; // Prevent ctrl + key from causing beeps
	else
		return WindowControl::CallDefaultProc(hWnd, msg, wParam, lParam);
}
