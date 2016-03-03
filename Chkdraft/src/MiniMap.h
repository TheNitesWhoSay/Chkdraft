#ifndef MINIMAP_H
#define MINIMAP_H
#include "WindowsUI/WindowsUI.h"

class MiniMap : public ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u32 id);

    protected:
        void MiniMapClick(LPARAM ClickPoints);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif