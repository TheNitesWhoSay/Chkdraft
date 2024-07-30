#ifndef LOGGERWINDOW_H
#define LOGGERWINDOW_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <cross_cut/simple_icu.h>
#include <deque>
#include <ostream>
#include <streambuf>

class LoggerWindow : public WinLib::ClassWindow, public std::ostream, public std::streambuf
{
    public:
        LoggerWindow();
        virtual ~LoggerWindow();
        bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, u64 id);
        void Refresh();

        bool IsVisible();
        void ToggleVisible();
        void ToggleLineNumbers();

    protected:
        virtual int sync();
        virtual int overflow(int c);
        void SizeSubWindows();

        virtual void ContextMenu(int x, int y);
        virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    private:
        bool isVisible;
        bool showLineNumbers;
        u64 lineNumber;
        std::deque<size_t> lineStart;
        icux::uistring prevLines;
        icux::uistring lineNumbersString;
        std::string currLine;
        WinLib::RichEditControl lineNumbers;
        WinLib::RichEditControl richText;
};

#endif