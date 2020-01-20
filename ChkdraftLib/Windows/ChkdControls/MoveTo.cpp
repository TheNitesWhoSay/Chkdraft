#include "MoveTo.h"

template <typename indexType>
MoveToDialog<indexType>::~MoveToDialog()
{

}

template <typename indexType>
bool MoveToDialog<indexType>::GetIndex(indexType & index, HWND hParent)
{
    MoveToDialog<indexType> moveTo;
    return moveTo.InternalGetMoveTo(index, hParent);
}
template bool MoveToDialog<u8>::GetIndex(u8 & index, HWND hParent);
template bool MoveToDialog<s8>::GetIndex(s8 & index, HWND hParent);
template bool MoveToDialog<u16>::GetIndex(u16 & index, HWND hParent);
template bool MoveToDialog<s16>::GetIndex(s16 & index, HWND hParent);
template bool MoveToDialog<u32>::GetIndex(u32 & index, HWND hParent);
template bool MoveToDialog<s32>::GetIndex(s32 & index, HWND hParent);
template bool MoveToDialog<int>::GetIndex(int & index, HWND hParent);

template <typename indexType>
MoveToDialog<indexType>::MoveToDialog() : indexMovedTo(0), gotIndex(false)
{

}

template <typename indexType>
bool MoveToDialog<indexType>::InternalGetMoveTo(indexType & index, HWND hParent)
{
    indexMovedTo = 0;
    gotIndex = false;
    CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_MOVETO), hParent);
    if ( gotIndex )
        index = indexMovedTo;

    return gotIndex;
}

template <typename indexType>
BOOL MoveToDialog<indexType>::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
        case IDCANCEL:
            EndDialog(hWnd, IDCANCEL);
            break;

        case IDOK:
            gotIndex = editMoveTo.GetEditNum<indexType>(indexMovedTo);
            EndDialog(hWnd, IDOK);
            break;
    }

    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

template <typename indexType>
BOOL MoveToDialog<indexType>::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_INITDIALOG )
    {
        editMoveTo.FindThis(hWnd, IDC_EDIT1);
        SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
    }

    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
