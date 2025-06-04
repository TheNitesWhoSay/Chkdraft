#ifndef SPRITEPROPERTIES_H
#define SPRITEPROPERTIES_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "preserved_sprite_stats.h"
#include "ui/chkd_controls/player_dropdown.h"

enum class SpriteListColumn;

class SpritePropertiesWindow : public WinLib::ClassDialog
{
    public:
        SpritePropertiesWindow();
        virtual ~SpritePropertiesWindow();
        bool CreateThis(HWND hParent);
        bool CreateSubWindows(HWND hWnd);
        bool DestroyThis();
        void SetChangeHighlightOnly(bool changeHighlightOnly);
        void ChangeCurrOwner(u8 newOwner);
        void ChangeDropdownPlayer(u8 newPlayer);
        void ChangeSpritesDisplayedOwner(int index, u8 newPlayer);
        void SetListRedraw(bool redraw);
        bool AddSpriteItem(u16 index, const Chk::Sprite & sprite);
        void FocusAndSelectIndex(u16 spriteIndex);
        void DeselectIndex(u16 spriteIndex);
        void UpdateEnabledState();
        void RepopulateList();

    protected:
        std::string GetSpriteName(Sc::Sprite::Type type, bool isUnit);
        void EnableSpriteEditing();
        void DisableSpriteEditing();
        void SetSpriteFieldText(const Chk::Sprite & sprite);
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

        void NotifyCheckClicked(u32 idFrom);

        void NotifyIdEditUpdated();
        void NotifyUnusedEditUpdated();
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
        SpriteListColumn columnSortedBy;
        bool flipSort;
        bool initilizing;
        bool changeHighlightOnly;

        WinLib::ListViewControl listSprites;
        PlayerDropdown dropPlayer;
        WinLib::ButtonControl buttonMoveUp, buttonMoveTop, buttonMoveDown, buttonMoveEnd, buttonDelete, buttonMoveTo;
        WinLib::EditControl editSpriteId, editUnused, editXc, editYc;
        WinLib::CheckBoxControl checkUnused0, checkUnused1, checkUnused2, checkUnused3, checkUnused4, checkUnused5, checkUnused6, checkUnused7, checkUnused8,
            checkUnused9, checkUnused10, checkUnused11, checkPureSprite, checkUnitSprite, checkUnused14, checkDisabled;
};

#endif