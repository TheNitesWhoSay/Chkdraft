#ifndef CHKDSTRINGINPUT_H
#define CHKDSTRINGINPUT_H
#include "../../../MappingCoreLib/MappingCore.h"
#include "../../../WindowsLib/WindowsUi.h"

class ChkdStringInputDialog : public WinLib::ClassDialog
{
    public:
        virtual ~ChkdStringInputDialog();
        static bool GetChkdString(ChkdString & str, const ChkdString & initialString, HWND hParent);

    protected:
        ChkdStringInputDialog();
        bool InternalGetChkdString(ChkdString & str, const ChkdString & initialString, HWND hParent);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool gotChkdString;
        ChkdString newString;
        ChkdString initialString;
        WinLib::EditControl editString;
};

#endif