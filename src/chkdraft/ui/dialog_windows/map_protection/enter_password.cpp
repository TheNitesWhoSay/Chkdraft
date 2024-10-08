#include "enter_password.h"
#include "chkdraft/chkdraft.h"

EnterPasswordDialog::~EnterPasswordDialog()
{

}

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
    if ( auto password = editPassword.GetWinText() )
    {
        editPassword.SetText("");
        bool success = CM->login(*password);
        password->clear();
        if ( success )
        {
            ClassDialog::DestroyDialog();
            mb("Login Successful!");
        }
        else
            mb("Login Failed!");
    }
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

