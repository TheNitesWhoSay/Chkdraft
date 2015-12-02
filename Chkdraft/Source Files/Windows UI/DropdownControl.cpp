#include "DropdownControl.h"

bool DropdownControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, bool alwaysList,
	u32 id, int numItems, const char** items, HFONT font)
{
	DWORD style = WS_VISIBLE|WS_CHILD|WS_VSCROLL|CBS_AUTOHSCROLL|CBS_HASSTRINGS;

	if ( editable )
	{
		if ( alwaysList )
			style |= CBS_DROPDOWNLIST;
		else
			style |= CBS_DROPDOWN;
	}
	else
	{
		if ( alwaysList )
			style |= CBS_SIMPLE | CBS_DROPDOWNLIST;
		else
			style |= CBS_SIMPLE | CBS_DROPDOWN;
	}

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

bool DropdownControl::GetEditText(std::string& dest)
{
	char* temp;
	if ( GetEditText(temp) )
	{
		try { dest = temp; }
		catch ( std::exception ) { delete[] temp; return false; }
		delete[] temp;
		return true;
	}
	else
		return false;
}

template <typename numType>
bool DropdownControl::GetEditNum(numType &dest)
{
	bool success = false;
	char* text;
	if ( GetEditText(text) )
	{
		int temp;
		if ( temp = atoi(text) )
		{
			dest = temp;
			success = true;
		}
		else if (std::strlen(text) > 0 && text[0] == '0' )
		{
			dest = 0;
			success = true;
		}
		delete[] text;
	}
	return success;
}
template bool DropdownControl::GetEditNum<u8>(u8 &dest);
template bool DropdownControl::GetEditNum<s8>(s8 &dest);
template bool DropdownControl::GetEditNum<u16>(u16 &dest);
template bool DropdownControl::GetEditNum<s16>(s16 &dest);
template bool DropdownControl::GetEditNum<u32>(u32 &dest);
template bool DropdownControl::GetEditNum<s32>(s32 &dest);
template bool DropdownControl::GetEditNum<int>(int &dest);

bool DropdownControl::GetEditText(char* &dest)
{
	bool success = false;
	int length = GetWindowTextLength(getHandle())+1;
	if ( length > 1 )
	{
		char* text;
		try {
			text = new char[length];
		} catch ( std::bad_alloc ) {
			return false;
		}
		if ( GetWindowText(getHandle(), text, length) )
		{
			text[length-1] = '\0';
			dest = new char[length];
			std::strncpy(dest, text, length);
			success = true;
		}
		delete[] text;
	}
	return success;
}
