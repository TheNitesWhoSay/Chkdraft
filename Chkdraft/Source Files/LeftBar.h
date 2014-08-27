#ifndef LEFTBAR_H
#define LEFTBAR_H
#include "Windows UI/WindowsUI.h"
#include "MainTree.h"
#include "MiniMap.h"

class LeftBar : public ClassWindow
{
	public:
		MiniMap miniMap;
		MainTree mainTree;

		bool CreateThis(HWND hParent);

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif