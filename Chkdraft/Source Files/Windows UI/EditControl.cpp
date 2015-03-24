#include "EditControl.h"

EditControl::EditControl() : isMultiLine(false)
{

}

bool EditControl::FindThis(HWND hParent, u32 id)
{
	return WindowControl::FindThis(hParent, id) &&
		   WindowControl::RedirectProc();
}

bool EditControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, u32 id)
{
	DWORD dwStyle = WS_VISIBLE|WS_CHILD;
	if ( multiLine )
		dwStyle |= ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN|WS_VSCROLL;
	else
		dwStyle |= ES_AUTOHSCROLL;

	isMultiLine = multiLine;

	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, "EDIT", "", dwStyle,
										 x, y, width, height,
										 hParent, (HMENU)id, true );
}

bool EditControl::SetText(const char* newText)
{
	return SetWindowText(getHandle(), newText) != 0;
}

template <typename numType>
bool EditControl::SetEditNum(numType num)
{
	if ( num < 0 ) // Signed
	{
		s32 temp = (s32)num;
		char newText[12];
		_itoa_s(temp, newText, 10);
		return SetText(newText);
	}
	else // Unsigned or irrelevantly signed
	{
		u32 temp = (u32)num;
		char newText[12];
		_itoa_s(temp, newText, 10);
		return SetText(newText);
	}
}
template bool EditControl::SetEditNum<u8>(u8 num);
template bool EditControl::SetEditNum<s8>(s8 num);
template bool EditControl::SetEditNum<u16>(u16 num);
template bool EditControl::SetEditNum<s16>(s16 num);
template bool EditControl::SetEditNum<u32>(u32 num);
template bool EditControl::SetEditNum<s32>(s32 num);
template bool EditControl::SetEditNum<int>(int num);

template <typename numType>
bool EditControl::SetEditBinaryNum(numType num)
{
	char newText[36] = { };
	u32 temp = (u32)num;
	u8 numBits = (sizeof(numType)*8);
	_itoa_s(temp, newText, 2);
	size_t length = strlen(newText);
	if ( length > 0 && length < numBits )
	{
		memmove(&newText[numBits-length], newText, length);
		memset(newText, '0', numBits-length);
		newText[numBits] = '\0';
	}
	return SetText(newText);
}
template bool EditControl::SetEditBinaryNum<u8>(u8 num);
template bool EditControl::SetEditBinaryNum<s8>(s8 num);
template bool EditControl::SetEditBinaryNum<u16>(u16 num);
template bool EditControl::SetEditBinaryNum<s16>(s16 num);
template bool EditControl::SetEditBinaryNum<u32>(u32 num);
template bool EditControl::SetEditBinaryNum<s32>(s32 num);
template bool EditControl::SetEditBinaryNum<int>(int num);

void EditControl::MaximizeTextLimit()
{
	SendMessage(getHandle(), EM_SETLIMITTEXT, 0x7FFFFFFE, NULL);
}

bool EditControl::GetEditText(std::string& dest)
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

bool EditControl::GetEditBinaryNum(u16 &dest)
{
	u16 temp = 0;
	char* editText;
	if ( GetEditText(editText) )
	{
		const u16 u16BitValues[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
									 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

		int length = strlen(editText);
		for ( int i=length-1; i>=0; i-- )
		{
			if ( editText[i] == '1' )
				temp |= u16BitValues[(length-1)-i];
			else if ( editText[i] != '0' )
			{
				delete[] editText;
				return false;
			}
		}
		delete[] editText;
		dest = temp;
		return true;
	}
	else
		return false;
}

bool EditControl::GetEditBinaryNum(u32 &dest)
{
	u32 temp = 0;
	char* editText;
	if ( GetEditText(editText) )
	{
		const u32 u32BitValues[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
									 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
									 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
									 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

		int length = strlen(editText);
		for ( int i=length-1; i>=0; i-- )
		{
			if ( editText[i] == '1' )
				temp |= u32BitValues[(length-1)-i];
			else if ( editText[i] != '0' )
			{
				delete[] editText;
				return false;
			}
		}
		delete[] editText;
		dest = temp;
		return true;
	}
	else
		return false;
}

template <typename numType>
bool EditControl::GetEditNum(numType &dest)
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
		else if ( strlen(text) > 0 && text[0] == '0' )
		{
			dest = 0;
			success = true;
		}
		delete[] text;
	}
	return success;
}
template bool EditControl::GetEditNum<u8>(u8 &dest);
template bool EditControl::GetEditNum<s8>(s8 &dest);
template bool EditControl::GetEditNum<u16>(u16 &dest);
template bool EditControl::GetEditNum<s16>(s16 &dest);
template bool EditControl::GetEditNum<u32>(u32 &dest);
template bool EditControl::GetEditNum<s32>(s32 &dest);
template bool EditControl::GetEditNum<int>(int &dest);

bool EditControl::GetEditText(char* &dest)
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
			strcpy_s(dest, length, text);
			success = true;
		}
		delete[] text;
	}
	return success;
}

LRESULT EditControl::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_KEYDOWN )
	{
		if ( wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000) > 0  ) // Select all
			SendMessage(hWnd, EM_SETSEL, 0, -1);
		else if ( wParam == VK_TAB ) // Insert tab
		{
			DWORD selStart, selEnd;
			SendMessage(hWnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
			SendMessage(hWnd, EM_SETSEL, (WPARAM)selStart, (LPARAM)selEnd);
			SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)"	");
			return 0; // Prevent default selection update
		}
	}
	else if ( msg == WM_CHAR && (GetKeyState('A') & 0x8000) > 0 && (GetKeyState(VK_CONTROL) & 0x8000) > 0 ) // Prevent key-beep for select all
		return 0;
	
	return WindowControl::CallDefaultProc(hWnd, msg, wParam, lParam); // Take default action
}
