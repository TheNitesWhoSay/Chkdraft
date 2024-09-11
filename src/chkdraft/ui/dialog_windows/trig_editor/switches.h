#ifndef SWITCHES_H
#define SWITCHES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class SwitchesWindow : public WinLib::ClassWindow
{
    public:
        SwitchesWindow();
        virtual ~SwitchesWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();
        void RefreshSwitchList();
        void DoSize();
        void DisableEditing();
        void EnableEditing();

    protected:
        void CreateSubWindows(HWND hWnd);
        
        void ToggleUseDefaultString();
        void ButtonSwitchNameProperties();
        void EditSwitchNameFocusLost();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyTreeItemSelected(LPARAM newValue); // Sent when a new tree item is selected
        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WinLib::TreeViewControl switchList;
        WinLib::CheckBoxControl checkUseDefaultName;
        WinLib::TextControl textSwitchName;
        WinLib::EditControl editSwitchName;
        WinLib::ButtonControl buttonSwitchNameProperties;
        s32 selectedSwitch;
        bool refreshing;
        bool refreshingSwitchList;
};

#endif