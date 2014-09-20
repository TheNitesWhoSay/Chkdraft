#include "RadioButtonControl.h"

bool RadioButtonControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id)
{
	return WindowControl::CreateControl( 0, "BUTTON", initText, WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON,
										 x, y, width, height, hParent, (HMENU)id, false );
}
