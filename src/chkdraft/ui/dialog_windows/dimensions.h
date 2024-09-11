#ifndef DIMENSIONS_H
#define DIMENSIONS_H
#include <windows/windows_ui.h>

class DimensionsWindow : public WinLib::ClassDialog
{
    public:
        DimensionsWindow();
        virtual ~DimensionsWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void RefreshWindow();

    protected:
        void UpdateTerrainList();
        void ResizeChangeTileset();
        void CreateSubWindows(HWND hWnd);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::EditControl editWidth;
        WinLib::EditControl editHeight;
        WinLib::EditControl editLeft;
        WinLib::EditControl editTop;
        WinLib::ListBoxControl listTileset;
        WinLib::ListBoxControl listTerrain;
        WinLib::CheckBoxControl checkSmoothBorder;
};

#endif