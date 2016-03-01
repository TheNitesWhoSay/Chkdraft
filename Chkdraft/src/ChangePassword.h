#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H
#include "WindowsUI/WindowsUI.h"
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"

class ChangePasswordDialog : public ClassDialog
{
    public:
        bool CreateThis(HWND hParent);
        void DestroyThis();
        void Hide();

    protected:
        void ButtonApply();
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        ButtonControl buttonApply;
        EditControl editOldPass;
        EditControl editNewPass;
};

#endif
