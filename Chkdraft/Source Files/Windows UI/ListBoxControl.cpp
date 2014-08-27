#include "ListBoxControl.h"

bool ListBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, u32 id)
{
	u32 style = WS_CHILD|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY|LBS_SORT;
	if ( ownerDrawn )
	{
		style |= LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS|WS_VSCROLL;
		style &= (~LBS_SORT);
	}

	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, "LISTBOX", NULL, style,
										 x, y, width, height, hParent, (HMENU)id, false );
}
