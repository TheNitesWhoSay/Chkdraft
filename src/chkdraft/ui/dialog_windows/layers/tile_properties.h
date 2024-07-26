#ifndef TILEPROPERTIES_H
#define TILEPROPERTIES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"

class TilePropWindow : public WinLib::ClassDialog
{
    public:
        virtual ~TilePropWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void UpdateTile();

    protected:
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editTileValue;
};

#endif