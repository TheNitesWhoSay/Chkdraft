#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class ChangePasswordDialog : public WinLib::ClassDialog
{
    public:
        virtual ~ChangePasswordDialog();
        bool CreateThis(HWND hParent);
        void DestroyThis();
        void Hide();

    protected:
        void ButtonApply();
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::ButtonControl buttonApply;
        WinLib::EditControl editOldPass;
        WinLib::EditControl editNewPass;
};

#endif