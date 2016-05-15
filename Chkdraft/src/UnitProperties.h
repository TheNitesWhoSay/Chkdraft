#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "PreservedUnitStats.h"
#include "PlayerDropdown.h"

enum class UnitListColumn;

class UnitPropertiesWindow : public ClassDialog
{
    public:
        UnitPropertiesWindow();
        bool CreateThis(HWND hParent);
        bool CreateSubWindows(HWND hWnd);
        bool DestroyThis();
        void SetChangeHighlightOnly(bool changeHighlightOnly);
        void ChangeCurrOwner(u8 newOwner);
        void ChangeDropdownPlayer(u8 newPlayer);
        void ChangeUnitsDisplayedOwner(int index, u8 newPlayer);
        void SetListRedraw(bool redraw);
        bool AddUnitItem(u16 index, ChkUnit unit);
        void FocusAndSelectIndex(u16 unitIndex);
        void DeselectIndex(u16 unitIndex);
        void UpdateEnabledState();
        void RepopulateList();

    protected:
        void EnableUnitEditing();
        void DisableUnitEditing();
        void SetUnitFieldText(ChkUnit unit);
        void SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2);
        void ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam);
        int CompareLvItems(LPARAM index1, LPARAM index2);
        void LvColumnClicked(NMHDR* nmhdr);
        void LvItemChanged(NMHDR* nmhdr);

        void NotifyClosePressed();
        void NotifyMoveTopPressed();
        void NotifyMoveEndPressed();
        void NotifyMoveUpPressed();
        void NotifyMoveDownPressed();
        void NotifyMoveToPressed();
        void NotifyDeletePressed();

        void NotifyInvincibleClicked();
        void NotifyHallucinatedClicked();
        void NotifyBurrowedClicked();
        void NotifyCloakedClicked();
        void NotifyLiftedClicked();

        void NotifyHpEditUpdated();
        void NotifyMpEditUpdated();
        void NotifyShieldEditUpdated();
        void NotifyResourcesEditUpdated();
        void NotifyHangerEditUpdated();
        void NotifyIdEditUpdated();
        void NotifyXcEditUpdated();
        void NotifyYcEditUpdated();

        virtual void NotifyButtonClicked(int idFrom, HWND hWndFrom); // Sent when a button or checkbox is clicked
        virtual void NotifyEditUpdated(int idFrom, HWND hWndFrom); // Sent when edit text changes, before redraw
        virtual void NotifyEditFocused(int idFrom, HWND hWndFrom); // Sent when an edit box receives focus
        virtual void NotifyEditFocusLost(int idFrom, HWND hWndFrom); // Sent when focus changes or the window is hidden
        virtual void NotifyComboSelChanged(int idFrom, HWND hWndFrom); // Sent when combo is changed by selection
        virtual void NotifyComboEditUpdated(int idFrom, HWND hWndFrom); // Sent when combo is changed by text edits

        BOOL Activate(WPARAM wParam, LPARAM lParam);
        BOOL ShowWindow(WPARAM wParam, LPARAM lParam);

        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        UnitListColumn columnSortedBy;
        bool flipSort;
        bool initilizing;
        bool changeHighlightOnly;
        PreservedUnitStats preservedStats;

        ListViewControl listUnits;
        PlayerDropdown dropPlayer;
        ButtonControl buttonMoveUp, buttonMoveTop, buttonMoveDown, buttonMoveEnd, buttonDelete, buttonMoveTo;
        EditControl editLife, editMana, editShield, editResources, editHanger, editUnitId, editXc, editYc;
        CheckBoxControl checkInvincible, checkHallucinated, checkBurrowed, checkCloaked, checkLifted;
};

#endif