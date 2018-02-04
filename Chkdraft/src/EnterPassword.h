#ifndef ENTERPASSWORD_H
#define ENTERPASSWORD_H
#include "WindowsUI/WindowsUI.h"
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"

class EnterPasswordDialog : public ClassDialog
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
        ButtonControl buttonLogin;
        EditControl editPassword;
};

#endif