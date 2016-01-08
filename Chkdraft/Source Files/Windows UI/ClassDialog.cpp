#include "ClassDialog.h"
#include <CommCtrl.h>

ClassDialog::ClassDialog()
{
	WindowClassName().clear();
}

ClassDialog::~ClassDialog()
{
	if ( getHandle() != NULL )
		DestroyDialog();
}

bool ClassDialog::CreateModelessDialog(LPCSTR lpTemplateName, HWND hWndParent)
{
	if ( getHandle() == NULL )
		setHandle(CreateDialogParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, SetupDialogProc, (LPARAM)this));

	if ( getHandle() != NULL )
		return true;

	return false;
}

INT_PTR ClassDialog::CreateDialogBox(LPCSTR lpTemplateName, HWND hWndParent)
{
	return DialogBoxParam(GetModuleHandle(NULL), lpTemplateName, hWndParent, SetupDialogProc, (LPARAM)this);
}

bool ClassDialog::DestroyDialog()
{
	if ( getHandle() != NULL && EndDialog(getHandle(), IDCLOSE) != 0 )
	{
		setHandle(NULL);
		return true;
	}
	else
		return false;
}

BOOL ClassDialog::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	return FALSE;
}

void ClassDialog::NotifyTreeSelChanged(LPARAM newValue)
{

}

void ClassDialog::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{

}

void ClassDialog::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{

}

void ClassDialog::NotifyEditFocusLost()
{

}

void ClassDialog::NotifyWindowHidden()
{

}

void ClassDialog::NotifyWindowShown()
{

}

BOOL ClassDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL ClassDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL CALLBACK ClassDialog::SetupDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_INITDIALOG )
	{
		LONG classPtr = lParam;
		SetWindowLong(hWnd, DWL_USER, classPtr);
		if ( GetWindowLong(hWnd, DWL_USER) == classPtr && classPtr != 0 && SetWindowLong(hWnd, DWL_DLGPROC, (LONG)ForwardDlgProc) != 0 )
		{
			((ClassDialog*)classPtr)->setHandle(hWnd); // Preload the window handle
			return ((ClassDialog*)classPtr)->DlgProc(hWnd, msg, wParam, lParam);
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}

BOOL CALLBACK ClassDialog::ForwardDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ClassDialog* classDialog = (ClassDialog*)GetWindowLong(hWnd, DWL_USER);
	switch ( msg )
	{
		case WM_SHOWWINDOW:
		{
			if ( (BOOL)wParam == TRUE )
				classDialog->NotifyWindowShown();
			else
			{
				classDialog->NotifyWindowHidden();
				classDialog->NotifyEditFocusLost();
			}
		}
		break;

		case WM_NOTIFY:
		{
			switch ( ((NMHDR*)lParam)->code )
			{
				case TVN_SELCHANGED:
					if ( ((NMTREEVIEW*)lParam)->action == TVN_SELCHANGED )
						classDialog->NotifyTreeSelChanged(((NMTREEVIEW*)lParam)->itemNew.lParam);
					break;
			}
			return classDialog->DlgNotify(hWnd, wParam, (NMHDR*)lParam);
		}
		break;

		case WM_COMMAND:
		{
			switch ( HIWORD(wParam) )
			{
				case BN_CLICKED: classDialog->NotifyButtonClicked(LOWORD(wParam), (HWND)lParam); break;
				case EN_UPDATE: classDialog->NotifyEditUpdated(LOWORD(wParam), (HWND)lParam); break;
				case EN_KILLFOCUS: classDialog->NotifyEditFocusLost(); break;
			}
			return classDialog->DlgCommand(hWnd, wParam, lParam);
		}
		break;
	}
	return classDialog->DlgProc(hWnd, msg, wParam, lParam);
}
