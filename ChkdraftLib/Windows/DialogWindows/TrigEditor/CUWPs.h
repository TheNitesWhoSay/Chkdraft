#ifndef CUWPS_H
#define CUWPS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class CUWPsWindow : public WinLib::ClassWindow
{
    public:
        CUWPsWindow();
        virtual ~CUWPsWindow();
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
        WinLib::TreeViewControl cuwpTree;
        WinLib::CheckBoxControl checkUsed;
        WinLib::TextControl healthPercent, manaPercent, shieldPercent, resourceAmount, hangerAmount;
        WinLib::EditControl editHitpointPercent, editManaPercent, editShieldPercent, editResources, editHanger;
        WinLib::CheckBoxControl checkInvincible, checkBurrowed, checkLifted, checkHallucinated, checkCloaked;
};

#endif