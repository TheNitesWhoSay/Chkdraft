#ifndef MOVETODIALOG_H
#define MOVETODIALOG_H
#include "../../CommonFiles/CommonFiles.h"
#include "../../../WindowsLib/WindowsUi.h"
#include "../../../MappingCoreLib/MappingCore.h"

template <typename indexType>
class MoveToDialog : public WinLib::ClassDialog
{
    public:
        virtual ~MoveToDialog() {}
        static bool GetIndex(indexType & index, HWND hParent)
        {
            MoveToDialog<indexType> moveTo;
            return moveTo.InternalGetMoveTo(index, hParent);
        }

    protected:
        bool InternalGetMoveTo(indexType & index, HWND hParent)
        {
            indexMovedTo = 0;
            gotIndex = false;
            CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_MOVETO), hParent);
            if ( gotIndex )
                index = indexMovedTo;

            return gotIndex;
        }
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if ( msg == WM_INITDIALOG )
            {
                editMoveTo.FindThis(hWnd, IDC_EDIT1);
                SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
            }
            return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
        }

    private:
        bool gotIndex = false;
        indexType indexMovedTo = 0;
        WinLib::EditControl editMoveTo;
};

#endif