#include "Templates.h"

bool TemplatesWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "Templates", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Templates", WS_CHILD,
			5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)ID_TEMPLATES) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TemplatesWindow::DestroyThis()
{
	return false;
}

void TemplatesWindow::RefreshWindow()
{

}

void TemplatesWindow::DoSize()
{

}

void TemplatesWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 0, 0, 200, 20, "Templates...", 0);
}
