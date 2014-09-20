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
		dwStyle |= ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN;
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

void EditControl::MaximizeTextLimit()
{
	SendMessage(getHandle(), EM_SETLIMITTEXT, 0x7FFFFFFE, NULL);
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
