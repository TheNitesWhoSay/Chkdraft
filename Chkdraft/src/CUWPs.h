#ifndef CUWPS_H
#define CUWPS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

class CUWPsWindow : public ClassWindow
{
    public:
        CUWPsWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow(bool includeTree);
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);
        void DisableEditing();
        void EnableEditing();
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual void NotifyTreeSelChanged(LPARAM newValue); // Sent when a new tree item is selected
        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);

    private:
        s32 selectedCuwp;
        TreeViewControl cuwpTree;
        CheckBoxControl checkUsed;
        TextControl healthPercent, manaPercent, shieldPercent, resourceAmount, hangerAmount;
        EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHanger;
        CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif