#include "EnterPassword.h"
#include "Chkdraft.h"

bool EnterPasswordDialog::CreateThis(HWND hParent)
{
	ClassDialog::CreateDialogBox(MAKEINTRESOURCE(IDD_ENTERPASS), hParent);
	return true;
}

void EnterPasswordDialog::DestroyThis()
{
	ClassDialog::DestroyDialog();
}

void EnterPasswordDialog::ButtonLogin()
{
	std::string password;
	if ( editPassword.GetEditText(password) )
	{
		editPassword.SetText("");
		bool success = CM->Login(password);
		password.clear();
		if ( success )
		{
			ClassDialog::DestroyDialog();
			mb("Login Successful!");
		}
		else
			mb("Login Failed!");
	}

	password.clear();
}

BOOL EnterPasswordDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( HIWORD(wParam) )
	{
		case BN_CLICKED:
			switch LOWORD(wParam)
			{
				case IDOK: ButtonLogin(); break;
			}
			break;
	}
	return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL EnterPasswordDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
	case WM_INITDIALOG:
		editPassword.FindThis(getHandle(), IDC_EDITENTERPASS);
		buttonLogin.FindThis(getHandle(), IDOK);
		return TRUE;
		break;
	case WM_CLOSE:
		if ( CM->isProtected() )
			mb("Map is protected and will be treated as view-only.");
		ClassDialog::DestroyDialog();
		break;
	}
	return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}

