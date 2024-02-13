#ifndef ENTERPASSWORD_H
#define ENTERPASSWORD_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class EnterPasswordDialog : public WinLib::ClassDialog
{
    public:
        virtual ~EnterPasswordDialog();
        bool CreateThis(HWND hParent);
        void DestroyThis();
        void Hide();
        void ButtonLogin();

    protected:
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::ButtonControl buttonLogin;
        WinLib::EditControl editPassword;
};

#endif