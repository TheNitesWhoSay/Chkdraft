#ifndef STRINGEDITOR_H
#define STRINGEDITOR_H
#include "../../../../CommonFiles/CommonFiles.h"
#include "../../../../../WindowsLib/WindowsUi.h"
#include "../../../../../MappingCoreLib/MappingCore.h"
#include "StringGuide.h"
#include "StringPreview.h"

class StringEditorWindow : public WinLib::ClassWindow
{
    public:
        StringEditorWindow();
        virtual ~StringEditorWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hWnd);
        void saveStrings();
        void addUseItem(std::string str, u32 amount); // Adds items to StringUse list box
        bool updateString(u32 stringNum); // Checks if string matches edit box content, if not, replaces string; returns true iff string changes
        void SwitchButtonPressed();

        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked

    private:
        bool extended;
        u32 currSelString;
        u32 numVisibleStrings;
        WinLib::TextControl textAboutStrings;
        WinLib::ButtonControl buttonSwap;
        WinLib::TextControl textStringUsage;
        WinLib::EditControl editString;
        StringGuideWindow stringGuide;
        StringPreviewWindow stringPreviewWindow;
        WinLib::ButtonControl buttonSaveString;
        WinLib::ButtonControl buttonDeleteString;
        WinLib::CheckBoxControl checkExtendedString;
        WinLib::ListBoxControl listStrings;
        WinLib::ListBoxControl listUsage;
        HDC stringListDC; // String list HDC for speeding up string measurement
};

LRESULT CALLBACK StringPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif