#include "ListBoxControl.h"
#include <iostream>
using namespace std;

bool ListBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, bool multiColumn, u32 id)
{
	u32 style = WS_CHILD|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY;
	if ( ownerDrawn && multiColumn )
		style |= LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN|LBS_EXTENDEDSEL;
	else if ( ownerDrawn )
		style |= LBS_OWNERDRAWVARIABLE|WS_VSCROLL;
	else if ( multiColumn )
		style |= LBS_MULTICOLUMN|LBS_EXTENDEDSEL;

	return WindowControl::CreateControl( WS_EX_CLIENTEDGE|LVS_EX_DOUBLEBUFFER, "LISTBOX", NULL, style,
										 x, y, width, height, hParent, (HMENU)id, true );
}

void ListBoxControl::ClearItems()
{
	SendMessage(getHandle(), LB_RESETCONTENT, NULL, NULL);
}

int ListBoxControl::AddItem(u32 item)
{
	LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, NULL, (LPARAM)item);
	if ( result == LB_ERR || result == LB_ERRSPACE )
		return -1;
	else
		return (int)result;
}

int ListBoxControl::AddString(const char* str)
{
	LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, NULL, (LPARAM)str);
	if ( result == LB_ERR || result == LB_ERRSPACE )
		return -1;
	else
		return (int)result;
}

bool ListBoxControl::InsertString(int index, const char* string)
{
	LRESULT result = SendMessage(getHandle(), LB_INSERTSTRING, (WPARAM)index, (LPARAM)string);
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
		return SendMessage(getHandle(), LB_SETCURSEL, (WPARAM)-1, (LPARAM)NULL) != LB_ERR;
	else
		return SendMessage(getHandle(), LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1) != LB_ERR;
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
	return SendMessage(getHandle(), LB_DELETESTRING, index, NULL) != LB_ERR;
}

bool ListBoxControl::SetTopIndex(int index)
{
	return SendMessage(getHandle(), LB_SETTOPINDEX, (WPARAM)index, NULL) != LB_ERR;
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
	LRESULT result = SendMessage(getHandle(), LB_GETITEMHEIGHT, 0, NULL);
	if ( result != LB_ERR )
		return (int)result;
	else
		return 0;
}

bool ListBoxControl::GetItemHeight(int index, int &height)
{
	LRESULT result = SendMessage(getHandle(), LB_GETITEMHEIGHT, (WPARAM)index, NULL);
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

bool ListBoxControl::GetCurSel(int &sel)
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
	LRESULT result = SendMessage(getHandle(), LB_GETSELCOUNT, NULL, NULL);
	if ( result != LB_ERR )
		return (int)result;
	else
		return 0;
}

bool ListBoxControl::GetSelString(int index, std::string &str)
{
	LRESULT numSel = SendMessage(getHandle(), LB_GETSELCOUNT, NULL, NULL);
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
				int selectedItem = selections[index];
				delete[] selections;
				result = SendMessage(getHandle(), LB_GETTEXTLEN, (WPARAM)selectedItem, NULL);
				if ( result != LB_ERR )
				{
					int textLength = 1+(int)result;
					char* text = nullptr;
					try { text = new char[textLength]; }
					catch ( std::bad_alloc ) { return false; }
					result = SendMessage(getHandle(), LB_GETTEXT, (WPARAM)selectedItem, (LPARAM)text);
					if ( result != LB_ERR )
					{
						str = text;
						delete[] text;
						return true;
					}
					else
						delete[] text;
				}
			}
			else
				delete[] selections;
		}
	}
	return false;
}

bool ListBoxControl::GetSelItem(int index, int &itemData)
{
	LRESULT numSel = SendMessage(getHandle(), LB_GETSELCOUNT, NULL, NULL);
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
				itemData = int(SendMessage(getHandle(), LB_GETITEMDATA, selections[index], NULL));
				delete[] selections;
				return true;
			}
			else
				delete[] selections;
		}
	}
	return false;
}

bool ListBoxControl::GetItemData(int index, u32 &data)
{
	LRESULT result = SendMessage(getHandle(), LB_GETITEMDATA , (WPARAM)index, NULL);
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
	return SendMessage(getHandle(), LB_GETTOPINDEX, NULL, NULL);
}

LRESULT ListBoxControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_MOUSEWHEEL )
	{
		SetTopIndex(GetTopIndex()-(int((s16(HIWORD(wParam)))/WHEEL_DELTA)));
		return 0;
	}
	if ( msg == WM_LBUTTONDBLCLK )
	{
		SendMessage(GetParent(hWnd), LBN_DBLCLKITEM, 0, (LPARAM)hWnd);
	}
		
	return CallDefaultProc(hWnd, msg, wParam, lParam);
}
