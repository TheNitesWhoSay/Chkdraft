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
        void Activate(WPARAM wParam);
        void DoScroll();
        void DoPaint();
        void LButtonDown(WPARAM wParam, int xc, int yc);
        void MouseWheel(WPARAM wParam);
        void GetMinMaxInfo(LPARAM lParam);
        void VScroll(WPARAM wParam);
        void VScrollLineDown();
        void VScrollLineUp();
        void VScrollPageDown();
        void VScrollPageUp();
        void VScrollThumbPos();
        void VScrollThumbTrack();
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 tilesetIndexedYC;
};

#endif