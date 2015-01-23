#include "ListBoxControl.h"

bool ListBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, bool multiColumn, u32 id)
{
	u32 style = WS_CHILD|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY;
	if ( ownerDrawn && multiColumn )
		style |= LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN|LBS_EXTENDEDSEL;
	else if ( ownerDrawn )
		style |= LBS_OWNERDRAWVARIABLE|WS_VSCROLL;
	else if ( multiColumn )
		style |= LBS_MULTICOLUMN|LBS_EXTENDEDSEL;

	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, "LISTBOX", NULL, style,
										 x, y, width, height, hParent, (HMENU)id, false );
}

void ListBoxControl::ClearItems()
{
	SendMessage(getHandle(), LB_RESETCONTENT, NULL, NULL);
}

int ListBoxControl::AddString(const char* str)
{
	LRESULT result = SendMessage(getHandle(), LB_ADDSTRING, NULL, (LPARAM)str);
	if ( result == LB_ERR || result == LB_ERRSPACE )
		return -1;
	else
		return (int)result;
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

int ListBoxControl::ColumnWidth()
{
	RECT rect;
	if ( SendMessage(getHandle(), LB_GETITEMRECT, 0, (LPARAM)&rect) == LB_ERR )
		return 0;
	else
		return rect.right-rect.left;
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
