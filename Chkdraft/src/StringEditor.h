#ifndef STRINGEDITOR_H
#define STRINGEDITOR_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "StringGuide.h"
#include "StringPreview.h"

class StringEditorWindow : public ClassWindow
{
    public:
        StringEditorWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hWnd);
        void saveStrings();
        void addUseItem(std::string str, u32 amount); // Adds items to StringUse list box
        bool updateString(u32 stringNum); // Checks if string matches edit box content, if not, replaces string; returns true iff string changes

        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 currSelString;
        u32 numVisibleStrings;
        TextControl textAboutStrings;
        TextControl textStringUsage;
        EditControl editString;
        StringGuideWindow stringGuide;
        StringPreviewWindow stringPreviewWindow;
        ButtonControl buttonSaveString;
        ButtonControl buttonDeleteString;
        CheckBoxControl checkExtendedString;
        ListBoxControl listStrings;
        ListBoxControl listUsage;
        HDC stringListDC; // String list HDC for speeding up string measurement
};

LRESULT CALLBACK StringPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif