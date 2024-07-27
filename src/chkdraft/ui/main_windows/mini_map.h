#ifndef MINIMAP_H
#define MINIMAP_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>

class MiniMap : public WinLib::ClassWindow
{
    public:
        virtual ~MiniMap();
        bool CreateThis(HWND hParent, u64 id);

    protected:
        void MiniMapClick(LPARAM ClickPoints);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif