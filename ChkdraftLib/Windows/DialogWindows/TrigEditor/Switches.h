#ifndef SWITCHES_H
#define SWITCHES_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class SwitchesWindow : public WinLib::ClassWindow
{
    public:
        SwitchesWindow();
        virtual ~SwitchesWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void RefreshSwitchList();
        void DoSize();
        void DisableEditing();
        void EnableEditing();

    protected:
        void CreateSubWindows(HWND hWnd);
        
        void ToggleUseDefaultString();
        void ToggleUseExtendedString();
        void EditSwitchNameFocusLost();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyTreeSelChanged(LPARAM newValue); // Sent when a new tree item is selected
        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::TreeViewControl switchList;
        WinLib::CheckBoxControl checkUseDefaultName;
        WinLib::TextControl textSwitchName;
        WinLib::EditControl editSwitchName;
        WinLib::CheckBoxControl checkUseExtendedString;
        s32 selectedSwitch;
        bool refreshing;
        bool refreshingSwitchList;
};

#endif