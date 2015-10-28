#include "ClassDialog.h"

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
	switch ( msg )
	{
	case WM_NOTIFY: return ((ClassDialog*)GetWindowLong(hWnd, DWL_USER))->DlgNotify(hWnd, wParam, (NMHDR*)lParam); break;
	case WM_COMMAND: return ((ClassDialog*)GetWindowLong(hWnd, DWL_USER))->DlgCommand(hWnd, wParam, lParam); break;
	}
	return ((ClassDialog*)GetWindowLong(hWnd, DWL_USER))->DlgProc(hWnd, msg, wParam, lParam);
}
