#ifndef TERRAINPALETTE_H
#define TERRAINPALETTE_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class TerrainPaletteWindow : public WinLib::ClassDialog
{
    public:
        TerrainPaletteWindow();
        virtual ~TerrainPaletteWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void SetTileDialog(HWND hWnd);
        void SelectTile(std::uint16_t tileValue);

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
        std::shared_ptr<WinLib::DeviceContext> openGlDc;
};

#endif