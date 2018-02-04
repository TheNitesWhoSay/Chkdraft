#ifndef NEWMAP_H
#define NEWMAP_H
#include "WindowsUI/WindowsUI.h"

class NewMap : public ClassDialog
{
    public:
        virtual ~NewMap();
        void CreateThis(HWND hParent);

    protected:
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif