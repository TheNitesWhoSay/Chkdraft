#include "ChkdStringInput.h"
#include "../../CommonFiles/CommonFiles.h"

ChkdStringInputDialog::~ChkdStringInputDialog()
{

}

bool ChkdStringInputDialog::GetChkdString(ChkdString & str, const ChkdString & initialString, HWND hParent)
{
    ChkdStringInputDialog inputDialog;
    return inputDialog.InternalGetChkdString(str, initialString, hParent);
}

ChkdStringInputDialog::ChkdStringInputDialog() : gotChkdString(false), newString("")
{

}

bool ChkdStringInputDialog::InternalGetChkdString(ChkdString & str, const ChkdString & initialString, HWND hParent)
{
    gotChkdString = false;
    this->initialString = initialString;
    newString = "";
    CreateDialogBox(MAKEINTRESOURCE(IDD_INPUTCHKDSTR), hParent);
    if ( gotChkdString )
        str = newString;

    return gotChkdString;
}

BOOL ChkdStringInputDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case IDCANCEL:
        EndDialog(hWnd, IDCANCEL);
        break;

    case IDOK:
        gotChkdString = editString.GetWinText(newString);
        EndDialog(hWnd, IDOK);
        break;
    }

    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL ChkdStringInputDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_INITDIALOG )
    {
        editString.FindThis(hWnd, IDC_EDIT1);
        editString.SetText(initialString);
        SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
    }

    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
