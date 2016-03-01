#ifndef CHKDSTRINGINPUT_H
#define CHKDSTRINGINPUT_H
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class ChkdStringInputDialog : public ClassDialog
{
    public:
        static bool GetChkdString(ChkdString &str, const ChkdString &initialString, HWND hParent);

    protected:
        ChkdStringInputDialog();
        bool InternalGetChkdString(ChkdString &str, const ChkdString &initialString, HWND hParent);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool gotChkdString;
        ChkdString newString;
        ChkdString initialString;
        EditControl editString;
};

#endif