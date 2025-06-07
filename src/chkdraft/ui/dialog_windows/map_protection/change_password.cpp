#include "change_password.h"
#include "chkdraft/chkdraft.h"
#include <string>

ChangePasswordDialog::~ChangePasswordDialog()
{

}

bool ChangePasswordDialog::CreateThis(HWND hParent)
{
    if ( CM != nullptr )
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
    auto oldPass = editOldPass.GetWinText();
    auto newPass = editNewPass.GetWinText();
    editOldPass.SetText("");
    editNewPass.SetText("");
    if ( CM != nullptr )
    {
        if ( CM->setPassword(*oldPass, *newPass) )
            mb("Password Set Successfully.");
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
