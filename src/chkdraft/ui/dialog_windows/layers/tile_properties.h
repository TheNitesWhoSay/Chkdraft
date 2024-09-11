#ifndef TILEPROPERTIES_H
#define TILEPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>

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