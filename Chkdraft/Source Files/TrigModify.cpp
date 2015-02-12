#include "TrigModify.h"
#include "Chkdraft.h"

#define NO_TRIGGER (u32(-1))

TrigModifyWindow::TrigModifyWindow()
{

}

bool TrigModifyWindow::CreateThis(HWND hParent, u32 trigIndex)
{
	if ( getHandle() == NULL )
	{
		if ( ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_TRIGMODIFY), hParent) )
		{
			CreateSubWindows(getHandle());
			ReplaceChildFonts(defaultFont);
			RefreshWindow(trigIndex);
			ShowWindow(getHandle(), SW_SHOWNORMAL);
			return true;
		}
		else
			return false;
	}
	else
		return SetParent(hParent);
}

bool TrigModifyWindow::DestroyThis()
{
	return ClassWindow::DestroyDialog();
}

void TrigModifyWindow::RefreshWindow(u32 trigIndex)
{
	
}

void TrigModifyWindow::DoSize()
{
	
}

void TrigModifyWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	text.CreateThis(hWnd, 5, 5, 100, 23, "(Coming soon)", 0);
}

BOOL TrigModifyWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			break;

		case WM_CLOSE:
			ShowWindow(hWnd, SW_HIDE);
			break;

		default:
			return FALSE;
			break;
	}
	return TRUE;
}
