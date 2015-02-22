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
			DoSize();
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
	RECT rcCli;
	if ( getClientRect(rcCli) )
		tabs.SetPos(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
}

void TrigModifyWindow::CreateSubWindows(HWND hWnd)
{

}

BOOL TrigModifyWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				SetSmallIcon((HANDLE)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0 ));
				tabs.FindThis(hWnd, IDC_TRIGMODIFYTABS);
				const char* tabLabels[] = { "Meta", "Players", "Conditions", "Actions" };
				for ( int i=0; i<sizeof(tabLabels)/sizeof(const char*); i++ )
					tabs.InsertTab(i, tabLabels[i]);
				CreateSubWindows(hWnd);

				ReplaceChildFonts(defaultFont);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_SIZE:
			DoSize();
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
