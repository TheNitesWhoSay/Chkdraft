#ifndef STRINGGUIDE_H
#define STRINGGUIDE_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class StringGuideWindow : public ClassWindow
{
    public:
        virtual ~StringGuideWindow();
        bool CreateThis(HWND hParent);

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        TextControl colorPrefix[numScStringColors];
        TextControl color[numScStringColors];
};

#endif