#ifndef STRINGGUIDE_H
#define STRINGGUIDE_H
#include "../../../../CommonFiles/CommonFiles.h"
#include "../../../../../WindowsLib/WindowsUi.h"
#include "../../../../../MappingCoreLib/MappingCore.h"

class StringGuideWindow : public WinLib::ClassWindow
{
    public:
        virtual ~StringGuideWindow();
        bool CreateThis(HWND hParent);

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::TextControl colorPrefix[numScStringColors];
        WinLib::TextControl color[numScStringColors];
};

#endif