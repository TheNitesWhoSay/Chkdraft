#ifndef ABOUT_H
#define ABOUT_H
#include "../../../WindowsLib/WindowsUi.h"

class AboutWindow : public WinLib::ClassDialog
{
    public:
        AboutWindow();
        virtual ~AboutWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();

    protected:
        void CreateSubWindows(HWND hWnd);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif