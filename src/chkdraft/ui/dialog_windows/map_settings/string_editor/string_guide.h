#ifndef STRINGGUIDE_H
#define STRINGGUIDE_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class StringGuideWindow : public WinLib::ClassWindow
{
    public:
        virtual ~StringGuideWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();

    protected:
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        static constexpr int numScStringColors = 28; // Could make StringGuide dynamic with respect to the size of strColors
        WinLib::TextControl colorPrefix[numScStringColors];
        WinLib::TextControl color[numScStringColors];
};

#endif