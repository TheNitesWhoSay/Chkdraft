#include "EditControl.h"

bool EditControl::FindThis(HWND hParent, u32 id)
{
	return WindowControl::FindThis(hParent, id) &&
		   WindowControl::RedirectProc();
}

bool EditControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, u32 id)
{
	DWORD dwStyle = WS_VISIBLE|WS_CHILD;
	if ( multiLine )
		dwStyle |= ES_MULTILINE|ES_AUTOVSCROLL;
	else
		dwStyle |= ES_AUTOHSCROLL;

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
	if ( msg == WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x8000) > 0 && wParam == 'A' )
		SendMessage(hWnd, EM_SETSEL, 0, -1); // Select all
	else if ( msg == WM_CHAR && ( (GetKeyState(VK_CONTROL) & 0x8000) > 0 || wParam == VK_RETURN ) )
		return 0; // Prevent key beeps

	return WindowControl::CallDefaultProc(hWnd, msg, wParam, lParam);
}
