#include "UnitMoveTo.h"
#include "GuiAccel.h"

UnitMoveTo::UnitMoveTo() : unitMoveTo(0)
{

}

s32 UnitMoveTo::GetMoveToIndex(HWND hParent)
{
	unitMoveTo = 0;
	CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_MOVETO), hParent);
	return unitMoveTo;
}

BOOL UnitMoveTo::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_COMMAND )
	{
		switch ( LOWORD(wParam) )
		{
			case IDCANCEL:
				EndDialog(hWnd, IDCANCEL);
				break;

			case IDOK:
				if ( !GetEditNum<s32>(hWnd, IDC_EDIT1, unitMoveTo) )
					unitMoveTo = -1;
				EndDialog(hWnd, IDOK);
				break;
		}
	}
	else if ( msg == WM_INITDIALOG )
		SetFocus(GetDlgItem(hWnd, IDC_EDIT1));

	return 0;
}
