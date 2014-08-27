#ifndef LISTBOXCONTROL_H
#define LISTBOXCONTROL_H
#include "WindowControl.h"

class ListBoxControl : public WindowControl
{
	public:
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool ownerDrawn, u32 id); // Attempts to create a list box
};

#endif