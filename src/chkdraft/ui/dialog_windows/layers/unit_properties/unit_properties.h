#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "preserved_unit_stats.h"
#include "ui/chkd_controls/player_dropdown.h"

enum class UnitListColumn;

class UnitPropertiesWindow : public WinLib::ClassDialog
{
    public:
        UnitPropertiesWindow();
        virtual ~UnitPropertiesWindow();
        bool CreateThis(HWND hParent);
        bool CreateSubWindows(HWND hWnd);
        void CreateAdvancedTab();
        bool DestroyThis();
        void SetChangeHighlightOnly(bool changeHighlightOnly);
        void ChangeCurrOwner(u8 newOwner);
        void ChangeDropdownPlayer(u8 newPlayer);
        void ChangeUnitsDisplayedOwner(int index, u8 newPlayer);
        void SetListRedraw(bool redraw);
        bool AddUnitItem(u16 index, const Chk::Unit & unit);
        void FocusAndSelectIndex(u16 unitIndex);
        void DeselectIndex(u16 unitIndex);
        void UpdateEnabledState();
        void RepopulateList();

    protected:
        void EnableUnitEditing();
        void DisableUnitEditing();
        void UpdateLinkArea(const Chk::Unit & unit);
        void SetUnitFieldText(const Chk::Unit & unit);
        void SetUnitFieldText();
        void SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2);
        void ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam);
        int CompareLvItems(LPARAM index1, LPARAM index2);
        void LvColumnClicked(NMHDR* nmhdr);
        void LvItemChanged(NMHDR* nmhdr);

        void NotifyAdvancedToggled();
        void NotifyClosePressed();
        void NotifyMoveTopPressed();
        void NotifyMoveEndPressed();
        void NotifyMoveUpPressed();
        void NotifyMoveDownPressed();
        void NotifyMoveToPressed();
        void NotifyDeletePressed();
        void NotifyLinkUnlinkPressed();
        void NotifyJumpToPressed();

        void NotifyInvincibleClicked();
        void NotifyHallucinatedClicked();
        void NotifyBurrowedClicked();
        void NotifyCloakedClicked();
        void NotifyLiftedClicked();

        void NotifyHpEditUpdated();
        void NotifyMpEditUpdated();
        void NotifyShieldEditUpdated();
        void NotifyResourcesEditUpdated();
        void NotifyHangarEditUpdated();
        void NotifyIdEditUpdated();
        void NotifyXcEditUpdated();
        void NotifyYcEditUpdated();
        
        void NotifyValidFieldOwnerClicked();
        void NotifyValidFieldLifeClicked();
        void NotifyValidFieldManaClicked();
        void NotifyValidFieldShieldClicked();
        void NotifyValidFieldResourcesClicked();
        void NotifyValidFieldHangarClicked();

        void NotifyValidFieldRawFlagsEditUpdated();

        void NotifyValidStateInvincibleClicked();
        void NotifyValidStateBurrowedClicked();
        void NotifyValidStateHallucinatedClicked();
        void NotifyValidStateCloakedClicked();
        void NotifyValidStateLiftedClicked();

        void NotifyValidStateRawFlagsEditUpdated();

        void NotifyStateRawFlagsEditUpdated();
        void NotifyUnusedEditUpdated();
        void NotifyUniqueIdEditUpdated();
        void NotifyLinkedIdEditUpdated();

        void NotifyLinkNydusClicked();
        void NotifyLinkAddonClicked();

        void NotifyRelationRawFlagsEditUpdated();

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
        int standardWidth = 0;
        bool advancedTabCreated = false;
        bool advanced = false;
        bool flipSort;
        bool initilizing;
        bool changeHighlightOnly;

        WinLib::GroupBoxControl groupUnitProperties;
        WinLib::ListViewControl listUnits;
        PlayerDropdown dropPlayer;
        WinLib::ButtonControl buttonMoveUp, buttonMoveTop, buttonMoveDown, buttonMoveEnd, buttonDelete, buttonMoveTo,
            buttonLinkSelection, buttonUnlink, buttonJumpToLink, buttonAdvanced, buttonLinkUnlink, buttonJumpTo;
        WinLib::EditControl editLife, editMana, editShield, editResources, editHangar, editUnitId, editXc, editYc;
        WinLib::CheckBoxControl checkInvincible, checkHallucinated, checkBurrowed, checkCloaked, checkLifted;

        WinLib::GroupBoxControl groupLinkedUnit, groupValidFieldFlags, groupValidStateFlags, groupLinkFlags;
        WinLib::CheckBoxControl checkValidFieldOwner, checkValidFieldLife, checkValidFieldMana, checkValidFieldShield,
            checkValidFieldResources, checkValidFieldHangar;
        WinLib::TextControl textValidFieldRawFlags;
        WinLib::EditControl editValidFieldRawFlags;

        WinLib::CheckBoxControl checkValidStateInvincible, checkValidStateHallucinated, checkValidStateBurrowed,
            checkValidStateCloaked, checkValidStateLifted;
        WinLib::TextControl textValidStateRawFlags;
        WinLib::EditControl editValidStateRawFlags;

        WinLib::GroupBoxControl groupMisc;
        WinLib::TextControl textRawStateFlags, textUnused, textUniqueId, textLinkedId;
        WinLib::EditControl editRawStateFlags, editUnused, editUniqueId, editLinkedId;

        WinLib::CheckBoxControl checkNydus, checkAddon;
        WinLib::TextControl textLinkRawFlags;
        WinLib::EditControl editLinkRawFlags;
};

#endif