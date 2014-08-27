#ifndef TREEVIEWCONTROL_H
#define TREEVIEWCONTROL_H
#include "WindowControl.h"

class TreeViewControl : public WindowControl
{
	public:
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u32 id);

	private:
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif