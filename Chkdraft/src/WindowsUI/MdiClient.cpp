#include "MdiClient.h"

MdiClient::MdiClient()
{

}

bool MdiClient::CreateMdiClient( HANDLE hWindowMenu, UINT idFirstChild, DWORD dwStyle,
							     int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu )
{
	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu  = hWindowMenu;
	ccs.idFirstChild = idFirstChild;

	setHandle(CreateWindow("MDICLIENT", NULL, dwStyle,
		X, Y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), (LPVOID)&ccs));

	return getHandle() != NULL;
}

HWND MdiClient::getActive()
{
	return (HWND)SendMessage(getHandle(), WM_MDIGETACTIVE, NULL, NULL);
}

void MdiClient::cascade()
{
	SendMessage(getHandle(), WM_MDICASCADE, 0, 0);
}

void MdiClient::tileHorizontally()
{
	SendMessage(getHandle(), WM_MDITILE, MDITILE_HORIZONTAL, NULL);
}

void MdiClient::tileVertically()
{
	SendMessage(getHandle(), WM_MDITILE, MDITILE_VERTICAL, NULL);
}

void MdiClient::destroyActive()
{
	SendMessage(getHandle(), WM_MDIDESTROY, (WPARAM)SendMessage(getHandle(), WM_MDIGETACTIVE, 0, 0), NULL);
}

void MdiClient::nextMdi()
{
	SendMessage(getHandle(), WM_MDINEXT, 0, 1);
}
