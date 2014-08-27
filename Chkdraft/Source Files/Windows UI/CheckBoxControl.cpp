#include "CheckBoxControl.h"

bool CheckBoxControl::CreateThis(HWND hParent, int x, int y, int width, int height, bool checked, const char* text, u32 id)
{
	if ( WindowControl::CreateControl( NULL, "BUTTON", text, WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_VCENTER,
										 x, y, width, height, hParent, (HMENU)id, false ) )
	{
		if ( checked )
			SendMessage(getHandle(), BM_SETCHECK, BST_CHECKED, NULL);

		return true;
	}
	else
		return false;
}
