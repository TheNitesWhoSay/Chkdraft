#ifndef STRINGPREVIEW_H
#define STRINGPREVIEW_H
#include "../../../../CommonFiles/CommonFiles.h"
#include "../../../../../WindowsLib/WindowsUi.h"
#include "../../../../../MappingCoreLib/MappingCore.h"

class StringPreviewWindow : public WinLib::ClassWindow
{
    public:
        virtual ~StringPreviewWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif