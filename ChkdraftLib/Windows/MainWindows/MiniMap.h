#ifndef MINIMAP_H
#define MINIMAP_H
#include "../../../WindowsLib/WindowsUI.h"
#include "../../CommonFiles/CommonFiles.h"

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