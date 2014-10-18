#include "GroupBoxControl.h"

bool GroupBoxControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id)
{
	return WindowControl::CreateControl( NULL, "BUTTON", initText, WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
										 x, y, width, height, hParent, (HMENU)id, false );
}

bool GroupBoxControl::SetText(const char* newText)
{
	return SetWindowText(getHandle(), (LPCSTR)newText) == TRUE;
}
