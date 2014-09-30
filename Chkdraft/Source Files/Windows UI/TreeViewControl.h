#ifndef TREEVIEWCONTROL_H
#define TREEVIEWCONTROL_H
#include "WindowControl.h"

class TreeViewControl : public WindowControl
{
	public:
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, u32 id);
		TV_INSERTSTRUCT InsertChild(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam);
		TV_INSERTSTRUCT InsertParent(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam);
		bool SetHandle(HWND hWnd);

	private:
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif