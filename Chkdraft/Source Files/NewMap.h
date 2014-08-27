#ifndef NEWMAP_H
#define NEWMAP_H
#include "Windows UI/WindowsUI.h"

class NewMap : public ClassWindow
{
	public:
		void CreateThis(HWND hParent);

	private:
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif