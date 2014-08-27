#include "MdiFrame.h"

MdiFrame::MdiFrame() : hWnd(NULL)
{

}

bool MdiFrame::CreateMdiFrame( HANDLE hWindowMenu, UINT idFirstChild,
							   DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
							   int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu )
{
	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu  = hWindowMenu;
	ccs.idFirstChild = idFirstChild;

	hWnd = CreateWindowEx( dwExStyle, "MDICLIENT", lpWindowName, dwStyle,
						   X, Y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)&ccs );

	return hWnd != NULL;
}

HWND MdiFrame::getHandle()
{
	return hWnd;
}

HWND MdiFrame::getActive()
{
	return (HWND)SendMessage(hWnd, WM_MDIGETACTIVE, NULL, NULL);
}

void MdiFrame::cascade()
{
	SendMessage(hWnd, WM_MDICASCADE, 0, 0);
}

void MdiFrame::tileHorizontally()
{
	SendMessage(hWnd, WM_MDITILE, MDITILE_HORIZONTAL, NULL);
}

void MdiFrame::tileVertically()
{
	SendMessage(hWnd, WM_MDITILE, MDITILE_VERTICAL, NULL);
}

void MdiFrame::destroyActive()
{
	SendMessage(hWnd, WM_MDIDESTROY, (WPARAM)SendMessage(hWnd, WM_MDIGETACTIVE, 0, 0), NULL);
}

void MdiFrame::nextMdi()
{
	SendMessage(getHandle(), WM_MDINEXT, 0, 1);
}
