#include "ChangePassword.h"
#include "Chkdraft.h"
#include <string>

bool ChangePasswordDialog::CreateThis(HWND hParent)
{
	if ( chkd.maps.curr != nullptr )
	{
		if ( getHandle() != NULL )
			Show();
		else if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_CHANGEPASS), hParent) )
		{
			buttonApply.FindThis(getHandle(), IDC_CHANGEPASSAPPLY);
			editOldPass.FindThis(getHandle(), IDC_EDITOLDPASS);
			editNewPass.FindThis(getHandle(), IDC_EDITNEWPASS);
			Show();
			return true;
		}
	}
	return false;
}

void ChangePasswordDialog::DestroyThis()
{
	Hide();
}

void ChangePasswordDialog::Hide()
{
	editOldPass.SetText("");
	editNewPass.SetText("");
	ClassDialog::Hide();
}

void ChangePasswordDialog::ButtonApply()
{
	std::string oldPass, newPass;
	editOldPass.GetEditText(oldPass);
	editNewPass.GetEditText(newPass);
	editOldPass.SetText("");
	editNewPass.SetText("");
	if ( chkd.maps.curr != nullptr )
	{
		if ( chkd.maps.curr->SetPassword(oldPass, newPass) )
		{
			chkd.maps.curr->notifyChange(false);
			mb("Password Set Successfully.");
		}
		else
			mb("Password Change Failed!");
	}
}

BOOL ChangePasswordDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( HIWORD(wParam) )
	{
	case BN_CLICKED:
		switch LOWORD(wParam)
		{
			case IDC_CHANGEPASSAPPLY : ButtonApply(); break;
		}
	}
	return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL ChangePasswordDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
	case WM_INITDIALOG: return TRUE; break;
	case WM_DESTROY: Hide(); break;
	case WM_CLOSE: Hide(); break;
	}
	return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
