#ifndef MAINPLOT_H
#define MAINPLOT_H
#include "Windows UI/WindowsUI.h"
#include "LeftBar.h"

class MainPlot : public ClassWindow
{
	public:
		LeftBar leftBar;
		bool CreateThis(HWND hParent);

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif