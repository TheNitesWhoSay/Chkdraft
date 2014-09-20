#include "DropdownControl.h"

bool DropdownControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, u32 id, int numItems, const char** items, HFONT font)
{
	DWORD style = CBS_DROPDOWN|WS_VISIBLE|WS_CHILD|WS_VSCROLL|CBS_AUTOHSCROLL|CBS_HASSTRINGS;

	if ( editable )
		style |= CBS_DROPDOWN;
	else
		style |= CBS_SIMPLE;

	if ( WindowControl::CreateControl(0, "COMBOBOX", NULL, style, x, y, width, height, hParent, (HMENU)id, false) )
	{
		HWND hWnd = getHandle();
		SendMessage(hWnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
		for ( int i=0; i<numItems; i++ )
			SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)items[i]);
		return true;
	}
	else
		return false;

}

void DropdownControl::SetSel(int index)
{
	SendMessage(getHandle(), CB_SETCURSEL, index, 0);
}

void DropdownControl::ClearEditSel()
{
	PostMessage(getHandle(), CB_SETEDITSEL, NULL, MAKELPARAM(-1, 0));
}

int DropdownControl::GetSel()
{
	return (int)SendMessage(getHandle(), CB_GETCURSEL, NULL, NULL);
}

bool DropdownControl::GetText(int index, char* dest, int destLength)
{
	LRESULT textLength = SendMessage(getHandle(), CB_GETLBTEXTLEN, index, NULL);
	if ( textLength != CB_ERR && destLength >= textLength )
		return SendMessage(getHandle(), CB_GETLBTEXT, index, (LPARAM)dest) != CB_ERR;
	else
		return false;
}
