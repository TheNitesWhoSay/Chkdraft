#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "PreservedUnitStats.h"
#include "PlayerDropdown.h"

class UnitWindow : public ClassDialog
{
    public:
        ListViewControl listUnits; // Make me private eventually
        PlayerDropdown dropPlayer; // Make me private eventually

        UnitWindow();
        bool CreateThis(HWND hParent);
        bool CreateSubWindows(HWND hWnd);
        bool DestroyThis();
        void SetChangeHighlightOnly(bool changeHighlightOnly);
        void ChangeCurrOwner(u8 newPlayer);
        void ChangeOwner(int index, u8 newPlayer);
        void SetListRedraw(bool redraw);
        bool AddUnitItem(u16 index, ChkUnit unit);
        void UpdateEnabledState();
        void RepopulateList();

    protected:
        void EnableUnitEditing(HWND hWnd);
        void DisableUnitEditing(HWND hWnd);
        void SetUnitFieldText(HWND hWnd, ChkUnit unit);
        void SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2);
        void ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam);
        int CompareLvItems(LPARAM index1, LPARAM index2);
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u16 columnSortedBy;
        bool flipSort;
        bool initilizing;
        bool changeHighlightOnly;
        PreservedUnitStats preservedStats;

        EditControl editLife;
        EditControl editMana;
        EditControl editShield;
        EditControl editResources;
        EditControl editHanger;
        EditControl editUnitId;
        EditControl editXc;
        EditControl editYc;
};

#endif