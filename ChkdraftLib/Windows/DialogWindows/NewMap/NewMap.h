#ifndef NEWMAP_H
#define NEWMAP_H
#include "../../../../WindowsLib/WindowsUi.h"

class NewMap : public WinLib::ClassDialog
{
    public:
        virtual ~NewMap();
        void CreateThis(HWND hParent);

    protected:
        virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editWidth;
        WinLib::EditControl editHeight;
        WinLib::ListBoxControl listInitialTileset;
        WinLib::ListBoxControl listInitialTerrain;
        WinLib::DropdownControl dropDefaultTriggers;
};

#endif