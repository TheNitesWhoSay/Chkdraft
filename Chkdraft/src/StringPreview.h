#ifndef STRINGPREVIEW_H
#define STRINGPREVIEW_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class StringPreviewWindow : public ClassWindow
{
    public:
        bool CreateThis(HWND hParent, u32 windowId);

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
