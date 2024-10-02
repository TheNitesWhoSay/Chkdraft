#ifndef CUWPS_H
#define CUWPS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>

class CUWPsWindow : public WinLib::ClassWindow
{
    public:
        CUWPsWindow();
        virtual ~CUWPsWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow(bool includeTree);
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);
        void DisableEditing();
        void EnableEditing();
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual void NotifyTreeItemSelected(LPARAM newValue); // Sent when a new tree item is selected
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);

    private:
        s32 selectedCuwp;
        WinLib::TreeViewControl cuwpTree;
        WinLib::CheckBoxControl checkUsed;
        WinLib::TextControl healthPercent, manaPercent, shieldPercent, resourceAmount, hangarAmount;
        WinLib::EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHangar;
        WinLib::CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif