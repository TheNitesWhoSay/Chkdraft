#ifndef TERRAINPALETTE_H
#define TERRAINPALETTE_H
#include "CommonFiles/CommonFiles.h"
#include "WindowsUI/WindowsUI.h"

class TerrainPaletteWindow : public ClassDialog
{
    public:
        TerrainPaletteWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void SetTileDialog(HWND hWnd);

    protected:
        void DoScroll(HWND hWnd);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 tilesetIndexedYC;
};

#endif
