#include "UnitProperties.h"
#include "MoveTo.h"
#include "Chkdraft.h"
#include "UnitChange.h"
#include "UnitCreateDel.h"
#include "UnitIndexMove.h"
#include <string>

enum ListColumns {
    UNIT_NAME_COLUMN,
    UNIT_OWNER_COLUMN,
    UNIT_XC_COLUMN,
    UNIT_YC_COLUMN,
    UNIT_INDEX_COLUMN
};

enum ID {
    IDC_UNITLIST = ID_FIRST,
    IDC_HP_BUDDY,
    IDC_MP_BUDDY,
    IDC_SHIELD_BUDDY,
    IDC_RESOURCES_BUDDY,
    IDC_HANGER_BUDDY,
    IDC_ID_BUDDY
};

UnitWindow::UnitWindow() : columnSortedBy(UNIT_INDEX_COLUMN), flipSort(false), initilizing(true), changeHighlightOnly(false)
{

}

bool UnitWindow::CreateThis(HWND hParent)
{
    return ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_UNITPROPERTIES), hParent);
}

bool UnitWindow::CreateSubWindows(HWND hWnd)
{
    editLife.FindThis(hWnd, IDC_EDIT_HP);
    editMana.FindThis(hWnd, IDC_EDIT_MP);
    editShield.FindThis(hWnd, IDC_EDIT_SHIELD);
    editResources.FindThis(hWnd, IDC_EDIT_RESOURCES);
    editHanger.FindThis(hWnd, IDC_EDIT_HANGER);
    editUnitId.FindThis(hWnd, IDC_EDIT_ID);
    editXc.FindThis(hWnd, IDC_EDIT_XC);
    editYc.FindThis(hWnd, IDC_EDIT_YC);
    dropPlayer.FindThis(hWnd, IDC_COMBO_PLAYER);

    initilizing = true;
    HWND hPlayer = GetDlgItem(hWnd, IDC_COMBO_PLAYER),
         hHP = GetDlgItem(hWnd, IDC_EDIT_HP),
         hMP = GetDlgItem(hWnd, IDC_EDIT_MP),
         hShield = GetDlgItem(hWnd, IDC_EDIT_SHIELD),
         hResources = GetDlgItem(hWnd, IDC_EDIT_RESOURCES),
         hHanger = GetDlgItem(hWnd, IDC_EDIT_HANGER),
         hID = GetDlgItem(hWnd, IDC_EDIT_ID);

    const char* playerStrings[] = { "Player 01", "Player 02", "Player 03", "Player 04", "Player 05", "Player 06",
                              "Player 07", "Player 08", "Player 09", "Player 10", "Player 11", "Player 12 (Neutral)" };
    for ( int i=0; i<12; i++ )
        SendMessage(hPlayer, CB_ADDSTRING, 0, (LPARAM)playerStrings[i]);
    SendMessage(hPlayer, CB_LIMITTEXT, 0, 0);

    HWND hBuddy[6];
    const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
                              IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
    for ( int i=0; i<6; i++ )
    {
        hBuddy[i] = CreateWindowExA(0, UPDOWN_CLASS, NULL,
                                    WS_CHILDWINDOW|WS_VISIBLE|WS_DISABLED|
                                    UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS|UDS_HOTTRACK,
                                    0, 0, 0, 0,
                                    hWnd, (HMENU)buddyIds[i], NULL, NULL);
    }
    SendMessage(hBuddy[0], UDM_SETBUDDY, (WPARAM)hHP, 0);
    SendMessage(hBuddy[0], UDM_SETRANGE32, 0, 100);
    SendMessage(hBuddy[1], UDM_SETBUDDY, (WPARAM)hMP, 0);
    SendMessage(hBuddy[1], UDM_SETRANGE32, 0, 100);
    SendMessage(hBuddy[2], UDM_SETBUDDY, (WPARAM)hShield, 0);
    SendMessage(hBuddy[2], UDM_SETRANGE32, 0, 100);
    SendMessage(hBuddy[3], UDM_SETBUDDY, (WPARAM)hResources, 0);
    SendMessage(hBuddy[3], UDM_SETRANGE32, 0, 50000);
    SendMessage(hBuddy[4], UDM_SETBUDDY, (WPARAM)hHanger, 0);
    SendMessage(hBuddy[4], UDM_SETRANGE32, 0, 8);
    SendMessage(hBuddy[5], UDM_SETBUDDY, (WPARAM)hID, 0);
    SendMessage(hBuddy[5], UDM_SETRANGE32, 0, 65535);

    listUnits.CreateThis(hWnd, 9, 10, 549, 449, false, false, IDC_UNITLIST);
    listUnits.EnableFullRowSelect();

    HFONT hListFont = CreateFont(13, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");
    listUnits.SetFont(hListFont, false);

    listUnits.AddColumn(0, "Unit type", 200, LVCFMT_LEFT);
    listUnits.AddColumn(1, "Owner", 100, LVCFMT_LEFT);
    listUnits.AddColumn(2, "X", 75, LVCFMT_RIGHT);
    listUnits.AddColumn(3, "Y", 75, LVCFMT_RIGHT);
    listUnits.AddColumn(4, "Index", 75, LVCFMT_RIGHT);

    RepopulateList();

    listUnits.Show();
    initilizing = false;
    
    return true;
}

bool UnitWindow::DestroyThis()
{
    return ClassDialog::DestroyDialog();
}

void UnitWindow::SetChangeHighlightOnly(bool changeHighlightOnly)
{
    this->changeHighlightOnly = changeHighlightOnly;
}

void UnitWindow::ChangeCurrOwner(u8 newPlayer)
{
    std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
    auto &selUnits = CM->GetSelections().getUnits();
    for ( u16 &unitIndex : selUnits )
    {
        ChkUnit unit = CM->getUnit(unitIndex);
        if ( newPlayer != unit.owner )
        {
            unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, ChkUnitField::Owner, unit.owner)));
            ChangeOwner(unitIndex, newPlayer);
            unit.owner = newPlayer;
        }
    }
    CM->AddUndo(unitChanges);
    CM->Redraw(true);
}

void UnitWindow::ChangeOwner(int index, u8 newPlayer)
{
    char owner[32], padding[2] = { '\0', '\0' };
    if ( newPlayer < 9 )
        padding[0] = '0';
    else
        padding[0] = '\0';
    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, newPlayer+1);

    int row = listUnits.GetItemRow(index);
    listUnits.SetItemText(row, UNIT_OWNER_COLUMN, owner);
}

void UnitWindow::SetListRedraw(bool allowRedraw)
{
    if ( allowRedraw )
    {
        listUnits.SetRedraw(true);
        ListView_SortItems(listUnits.getHandle(), ForwardCompareLvItems, this);
        listUnits.UpdateWindow();
    }
    else
        listUnits.SetRedraw(false);
}

bool UnitWindow::AddUnitItem(u16 index, ChkUnit unit)
{
    char owner[32], padding[2] = { '\0', '\0' },
        xc[32], yc[32], cIndex[32];;

    u16 id = unit.id;

    if ( unit.owner+1 < 10 )
        padding[0] = '0';
    else
        padding[0] = '\0';

    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, unit.owner + 1);
    std::strcpy(xc, std::to_string(unit.xc).c_str());
    std::strcpy(yc, std::to_string(unit.yc).c_str());
    std::strcpy(cIndex, std::to_string(index).c_str());

    ChkdString unitName;
    CM->getUnitName(unitName, id);

    listUnits.AddRow(4, index);
    listUnits.SetItemText(index, UNIT_NAME_COLUMN, unitName.c_str());
    listUnits.SetItemText(index, UNIT_OWNER_COLUMN, owner);
    listUnits.SetItemText(index, UNIT_XC_COLUMN, xc);
    listUnits.SetItemText(index, UNIT_YC_COLUMN, yc);
    listUnits.SetItemText(index, UNIT_INDEX_COLUMN, index);
    return true;
}

void UnitWindow::UpdateEnabledState()
{
    Selections &selections = CM->GetSelections();
    if ( selections.hasUnits() )
    {
        EnableUnitEditing(getHandle());
        u16 selectedIndex = selections.getFirstUnit();
        ChkUnit unit = CM->getUnit(selectedIndex);
        SetUnitFieldText(getHandle(), unit);
    }
    else
        DisableUnitEditing(getHandle());
}

void UnitWindow::RepopulateList()
{
    initilizing = true;
    SetListRedraw(false);

    listUnits.DeleteAllItems();

    if ( CM != nullptr )
    {
        Selections &selections = CM->GetSelections();
        ChkUnit* unit = nullptr;

        int numUnits = CM->numUnits();

        for ( int i = 0; i < numUnits; i++ )
        {
            ChkUnit unit = CM->getUnit(i);
            AddUnitItem(i, unit);
        }

        bool unitsSelected = selections.hasUnits();
        if ( unitsSelected )
        {
            u16 selectedIndex = selections.getFirstUnit();
            listUnits.FocusItem(selectedIndex);
            auto &selUnits = selections.getUnits();
            for ( u16 &unitIndex : selUnits )
                listUnits.SelectRow(unitIndex);
            
            EnableUnitEditing(getHandle());
            ChkUnit unit = CM->getUnit(selectedIndex);
            SetUnitFieldText(getHandle(), unit);

            ChkdString unitName;
            CM->getUnitName(unitName, unit.id);
            SetTitle(unitName.c_str());

            int row = listUnits.GetItemRow(selections.getHighestIndex());
            listUnits.EnsureVisible(row, false);
            row = listUnits.GetItemRow(selections.getLowestIndex());
            listUnits.EnsureVisible(row, false);
        }
    }

    SetListRedraw(true);
    initilizing = false;
}

void UnitWindow::EnableUnitEditing(HWND hWnd)
{
    HWND hItem = GetDlgItem(hWnd, IDC_COMBO_PLAYER);
    EnableWindow(hItem, TRUE);

    const int buttons[] = { IDC_BUTTON_MOVEUP, IDC_BUTTON_MOVETOP, IDC_BUTTON_MOVEDOWN,
                            IDC_BUTTON_MOVEEND, IDC_BUTTON_DELETE, IDC_BUTTON_MOVE_TO };
    for ( int i=0; i<sizeof(buttons)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, buttons[i]);
        EnableWindow(hItem, TRUE);
    }

    const int editBoxes[] = { IDC_EDIT_HP, IDC_EDIT_MP, IDC_EDIT_SHIELD, IDC_EDIT_RESOURCES,
                              IDC_EDIT_HANGER, IDC_EDIT_ID, IDC_EDIT_XC, IDC_EDIT_YC };
    for ( int i=0; i<sizeof(editBoxes)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, editBoxes[i]);
        EnableWindow(hItem, TRUE);
    }

    const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
                              IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
    for ( int i=0; i<sizeof(buddyIds)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, buddyIds[i]);
        EnableWindow(hItem, TRUE);
    }

    const int checkBoxes[] = { IDC_CHECK_INVINCIBLE, IDC_CHECK_HALLUCINATED, IDC_CHECK_BURROWED,
                               IDC_CHECK_CLOAKED, IDC_CHECK_LIFTED };
    for ( int i=0; i<sizeof(checkBoxes)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, checkBoxes[i]);
        SendMessage(hItem, BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(hItem, TRUE);
    }
}

void UnitWindow::DisableUnitEditing(HWND hWnd)
{
    SetWindowText(hWnd, "");

    HWND hItem = GetDlgItem(hWnd, IDC_COMBO_PLAYER);
    SendMessage(hItem, CB_SETCURSEL, -1, 0);
    EnableWindow(hItem, FALSE);

    const int buttons[] = { IDC_BUTTON_MOVEUP, IDC_BUTTON_MOVETOP, IDC_BUTTON_MOVEDOWN,
                            IDC_BUTTON_MOVEEND, IDC_BUTTON_DELETE, IDC_BUTTON_MOVE_TO };
    for ( int i=0; i<sizeof(buttons)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, buttons[i]);
        EnableWindow(hItem, FALSE);
    }

    const int editBoxes[] = { IDC_EDIT_HP, IDC_EDIT_MP, IDC_EDIT_SHIELD, IDC_EDIT_RESOURCES,
                              IDC_EDIT_HANGER, IDC_EDIT_ID, IDC_EDIT_XC, IDC_EDIT_YC };
    for ( int i=0; i<sizeof(editBoxes)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, editBoxes[i]);
        SetWindowText(hItem, "");
        EnableWindow(hItem, FALSE);
    }

    const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
                              IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
    for ( int i=0; i<sizeof(buddyIds)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, buddyIds[i]);
        EnableWindow(hItem, FALSE);
    }

    const int checkBoxes[] = { IDC_CHECK_INVINCIBLE, IDC_CHECK_HALLUCINATED, IDC_CHECK_BURROWED,
                               IDC_CHECK_CLOAKED, IDC_CHECK_LIFTED };
    for ( int i=0; i<sizeof(checkBoxes)/sizeof(const int); i++ )
    {
        hItem = GetDlgItem(hWnd, checkBoxes[i]);
        SendMessage(hItem, BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(hItem, FALSE);
    }
}

void UnitWindow::SetUnitFieldText(HWND hWnd, ChkUnit unit)
{
    initilizing = true;
    HWND hPlayer   = GetDlgItem(hWnd, IDC_COMBO_PLAYER      ), hHp     = GetDlgItem(hWnd, IDC_EDIT_HP         ),
         hMp       = GetDlgItem(hWnd, IDC_EDIT_MP           ), hShield = GetDlgItem(hWnd, IDC_EDIT_SHIELD     ),
         hResource = GetDlgItem(hWnd, IDC_EDIT_RESOURCES    ), hHanger = GetDlgItem(hWnd, IDC_EDIT_HANGER     ),
         hId       = GetDlgItem(hWnd, IDC_EDIT_ID           ), hInvinc = GetDlgItem(hWnd, IDC_CHECK_INVINCIBLE),
         hHallu    = GetDlgItem(hWnd, IDC_CHECK_HALLUCINATED), hBurrow = GetDlgItem(hWnd, IDC_CHECK_BURROWED  ),
         hCloak    = GetDlgItem(hWnd, IDC_CHECK_CLOAKED     ), hLifted = GetDlgItem(hWnd, IDC_CHECK_LIFTED    ),
         hXc       = GetDlgItem(hWnd, IDC_EDIT_XC           ), hYc     = GetDlgItem(hWnd, IDC_EDIT_YC         );

    if ( unit.owner < 12 )
        SendMessage(hPlayer, CB_SETCURSEL, unit.owner, 0);
    else
        SetWindowText(hPlayer, std::to_string(unit.owner + 1).c_str());

    SetWindowText(hHp, std::to_string(unit.hitpoints).c_str());
    SetWindowText(hMp, std::to_string(unit.energy).c_str());
    SetWindowText(hShield, std::to_string(unit.shields).c_str());
    SetWindowText(hResource, std::to_string(unit.resources).c_str());
    SetWindowText(hHanger, std::to_string(unit.hanger).c_str());
    SetWindowText(hId, std::to_string(unit.id).c_str());
    SetWindowText(hXc, std::to_string(unit.xc).c_str());
    SetWindowText(hYc, std::to_string(unit.yc).c_str());

    SendMessage(hInvinc, BM_SETCHECK, unit.stateFlags&UNIT_STATE_INVINCIBLE  , 0);
    SendMessage(hHallu , BM_SETCHECK, unit.stateFlags&UNIT_STATE_HALLUCINATED, 0);
    SendMessage(hBurrow, BM_SETCHECK, unit.stateFlags&UNIT_STATE_BURROWED    , 0);
    SendMessage(hCloak , BM_SETCHECK, unit.stateFlags&UNIT_STATE_CLOAKED     , 0);
    SendMessage(hLifted, BM_SETCHECK, unit.stateFlags&UNIT_STATE_LIFTED      , 0);
    initilizing = false;
}

void UnitWindow::SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2)
{
    int row1 = listUnits.GetItemRow(index1),
        row2 = listUnits.GetItemRow(index2);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row1;
    item.lParam = index2;

    ListView_SetItem(hListView, &item);
    listUnits.SetItemText(row1, UNIT_INDEX_COLUMN, index2);

    item.iItem = row2;
    item.lParam = index1;

    ListView_SetItem(hListView, &item);
    listUnits.SetItemText(row2, UNIT_INDEX_COLUMN, index1);
}

void UnitWindow::ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam)
{
    int row = listUnits.GetItemRow(oldLParam);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row;
    item.lParam = newLParam;

    ListView_SetItem(hListView, &item);

    listUnits.SetItemText(row, UNIT_INDEX_COLUMN, newLParam);
}

int UnitWindow::CompareLvItems(LPARAM index1, LPARAM index2)
{
    int sort = 0;

    if ( columnSortedBy < 4 )
    {
        ChkUnit firstUnit = CM->getUnit((u16)index1),
            secondUnit = CM->getUnit((u16)index2);

        switch ( columnSortedBy )
        {
            case 0: // Sort by ID / type
                if ( firstUnit.id < secondUnit.id )
                    sort = -1;
                else if ( firstUnit.id > secondUnit.id )
                    sort = 1;
                break;
            case 1: // Sort by owner
                if ( firstUnit.owner < secondUnit.owner )
                    sort = -1;
                else if ( firstUnit.owner > secondUnit.owner )
                    sort = 1;
                break;
            case 2: // Sort by xc
                if ( firstUnit.xc < secondUnit.xc )
                    sort = -1;
                else if ( firstUnit.xc > secondUnit.xc )
                    sort = 1;
                else if ( firstUnit.yc < secondUnit.yc ) // Try to sort by yc if xc's are equal
                    sort = -1;
                else if ( firstUnit.yc > secondUnit.yc )
                    sort = 1;
                break;
            case 3: // Sort by yc
                if ( firstUnit.yc < secondUnit.yc )
                    sort = -1;
                else if ( firstUnit.yc > secondUnit.yc )
                    sort = 1;
                else if ( firstUnit.xc < secondUnit.xc ) // Try to sort by xc if yc's are equal
                    sort = -1;
                else if ( firstUnit.xc > secondUnit.xc )
                    sort = 1;
                break;
        }
    }
    else if ( columnSortedBy == 4 )
    {
        if ( index1 < index2 )
            sort = -1;
        else if ( index1 > index2 )
            sort = 1;
    }

    if ( flipSort )
        return -sort;
    else
        return sort;
}

BOOL UnitWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case LVN_COLUMNCLICK:
        {
            int newColumn = ((NMLISTVIEW*)nmhdr)->iSubItem;
            if ( newColumn == columnSortedBy )
                flipSort = !flipSort;
            else
                flipSort = false;

            columnSortedBy = newColumn;
            ListView_SortItems(nmhdr->hwndFrom, ForwardCompareLvItems, this);
        }
        break;
    case LVN_ITEMCHANGED:
        {
            Selections &selections = CM->GetSelections();
            preservedStats.convertToUndo();
            if ( changeHighlightOnly == false )
            {
                NMLISTVIEW* itemInfo = (NMLISTVIEW*)nmhdr;
                u16 index = (u16)itemInfo->lParam;

                if ( itemInfo->uNewState & LVIS_SELECTED && initilizing == false ) // Selected
                                                                                   // Add item to selection
                {
                    bool firstSelected = !selections.hasUnits();
                    selections.addUnit(index);

                    if ( firstSelected )
                        EnableUnitEditing(hWnd);

                    ChkUnit unit = CM->getUnit(index);
                    ChkdString unitName;
                    CM->getUnitName(unitName, unit.id);
                    SetWindowText(hWnd, unitName.c_str());
                    SetUnitFieldText(hWnd, unit);

                    CM->Redraw(false);
                }
                else if ( itemInfo->uOldState & LVIS_SELECTED ) // From selected to not selected
                                                                // Remove item from selection
                {
                    selections.removeUnit(index);

                    if ( !selections.hasUnits()
                        && !(GetKeyState(VK_DOWN) & 0x8000
                            || GetKeyState(VK_UP) & 0x8000
                            || GetKeyState(VK_LEFT) & 0x8000
                            || GetKeyState(VK_RIGHT) & 0x8000
                            || GetKeyState(VK_LBUTTON) & 0x8000
                            || GetKeyState(VK_RBUTTON) & 0x8000) )
                        DisableUnitEditing(hWnd);

                    CM->Redraw(false);
                }
            }
        }
        break;
    }
    return ClassDialog::DlgNotify(hWnd, idFrom, nmhdr);
}

BOOL UnitWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case IDC_COMBO_PLAYER:
        switch ( HIWORD(wParam) )
        {
        case CBN_EDITCHANGE:
        {
            u8 newPlayer;
            if ( dropPlayer.GetPlayerNum(newPlayer) )
                ChangeCurrOwner(newPlayer);
        }
        break;
        case CBN_SELCHANGE:
        {
            u8 newPlayer = (u8)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
            if ( newPlayer != CB_ERR )
                ChangeCurrOwner(newPlayer);
        }
        break;
        }
        break;
    case IDCLOSE:
        EndDialog(hWnd, IDCLOSE);
        break;

    case IDC_BUTTON_MOVETOP:
    {
        Selections &selections = CM->GetSelections();

        u16 unitStackTopIndex = selections.getFirstUnit();
        selections.sortUnits(true); // sort with lowest indexes first

        listUnits.SetRedraw(false);

        ChkUnit preserve;

        std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        u16 i = 0;
        auto &selUnits = selections.getUnits();
        for ( u16 &unitIndex : selUnits )
        {
            if ( unitIndex != 0 ) // If unit is not at the destination index and unitptr can be retrieved
            {
                preserve = CM->getUnit(unitIndex); // Preserve the unit info
                if ( CM->deleteUnit(unitIndex) )
                {
                    if ( CM->insertUnit(i, preserve) )
                    {
                        unitChanges->Insert(std::shared_ptr<UnitIndexMove>(new UnitIndexMove(unitIndex, i)));
                        if ( unitIndex == unitStackTopIndex )
                            unitStackTopIndex = i;
                        unitIndex = i; // Modify the index that denotes unit selection
                    }
                    else // Insertion failed
                    {
                        selections.removeUnit(unitIndex);
                        break; // Can't advance to next, exit loop
                    }
                }
            }
            i++;
        }
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        if ( unitChanges->Count() > 2 )
            CM->AddUndo(unitChanges);
        selections.ensureFirst(unitStackTopIndex);
        RepopulateList();
    }
    break;

    case IDC_BUTTON_MOVEEND:
    {
        Selections &selections = CM->GetSelections();

        u16 unitStackTopIndex = selections.getFirstUnit();
        selections.sortUnits(false); // Highest First

        listUnits.SetRedraw(false);
        u16 numUnits = CM->numUnits();
        u16 numUnitsSelected = selections.numUnits();

        ChkUnit preserve;

        u16 i = 1;
        std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        auto &selUnits = selections.getUnits();
        for ( u16 &unitIndex : selUnits )
        {
            if ( unitIndex != numUnits-1 )
            {
                preserve = CM->getUnit(unitIndex);
                if ( CM->deleteUnit(unitIndex) )
                {
                    if ( CM->insertUnit(numUnits-i, preserve) )
                    {
                        unitChanges->Insert(std::shared_ptr<UnitIndexMove>(new UnitIndexMove(unitIndex, numUnits - i)));

                        if ( unitIndex == unitStackTopIndex )
                            unitStackTopIndex = numUnits - i;

                        unitIndex = numUnits - i;
                    }
                    else
                    {
                        selections.removeUnit(unitIndex);
                        break;
                    }
                }
            }
            i++;
        }
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        if ( unitChanges->Count() > 2 )
            CM->AddUndo(unitChanges);
        selections.ensureFirst(unitStackTopIndex);
        RepopulateList();
    }
    break;

    case IDC_BUTTON_MOVEUP:
    {
        Selections& selections = CM->GetSelections();
        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);

        selections.sortUnits(true);
        listUnits.SetRedraw(false);

        std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        auto &selUnits = selections.getUnits();
        for ( u16 &unitIndex : selUnits )
        {
            if ( unitIndex > 0 && !selections.unitIsSelected(unitIndex - 1) )
            {
                if ( CM->SwapUnits(unitIndex, unitIndex-1) )
                {
                    unitChanges->Insert(std::shared_ptr<UnitIndexMove>(new UnitIndexMove(unitIndex, unitIndex - 1)));
                    SwapIndexes(hUnitList, unitIndex, unitIndex - 1);
                    unitIndex--;
                }
            }
        }
        ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
        int row = listUnits.GetItemRow(selections.getHighestIndex());
        listUnits.EnsureVisible(row, false);
        row = listUnits.GetItemRow(selections.getLowestIndex());
        listUnits.EnsureVisible(row, false);
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        if ( unitChanges->Count() > 2 )
            CM->AddUndo(unitChanges);
        listUnits.SetRedraw(true);
    }
    break;
    case IDC_BUTTON_MOVEDOWN:
    {
        Selections &selections = CM->GetSelections();
        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);

        selections.sortUnits(false);
        listUnits.SetRedraw(false);

        std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        auto &selUnits = selections.getUnits();
        for ( u16 &unitIndex : selUnits )
        {
            if ( unitIndex < CM->numUnits() && !selections.unitIsSelected(unitIndex + 1) )
            {
                if ( CM->SwapUnits(unitIndex, unitIndex+1) )
                {
                    unitChanges->Insert(std::shared_ptr<UnitIndexMove>(new UnitIndexMove(unitIndex, unitIndex + 1)));
                    SwapIndexes(hUnitList, unitIndex, unitIndex + 1);
                    unitIndex++;
                }
            }
        }
        unitChanges->Insert(std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary));
        if ( unitChanges->Count() > 2 )
            CM->AddUndo(unitChanges);
        ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
        int row = listUnits.GetItemRow(selections.getLowestIndex());
        listUnits.EnsureVisible(row, false);
        row = listUnits.GetItemRow(selections.getHighestIndex());
        listUnits.EnsureVisible(row, false);
        listUnits.SetRedraw(true);
    }
    break;
    case IDC_BUTTON_MOVE_TO:
    {
        u32 unitMoveTo;
        if ( MoveToDialog<u32>::GetIndex(unitMoveTo, hWnd) && unitMoveTo < u32(CM->numUnits()) )
        {
            if ( unitMoveTo == 0 )
                return SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MOVETOP, NULL), 0);
            else if ( unitMoveTo > 0 )
            {
                Selections &selections = CM->GetSelections();
                u16 numUnitsSelected = selections.numUnits();
                u16 limit = CM->numUnits() - 1;

                if ( unitMoveTo + numUnitsSelected > limit )
                    return SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MOVEEND, NULL), 0);
                else
                {
                    HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);

                    u16 unitStackTopIndex = selections.getFirstUnit();
                    u16 numUnits = selections.numUnits(),
                        shift = numUnits - 1;
                    selections.sortUnits(false); // Highest First
                    listUnits.SetRedraw(false);

                    ChkUnit* selectedUnits;
                    try {
                        selectedUnits = new ChkUnit[numUnits];
                    }
                    catch ( std::bad_alloc ) {
                        Error("'Move To' failed.\n\nCould not allocate temporary storage, you may have run out of memory");
                        return 0;
                    }

                    std::shared_ptr<ReversibleActions> unitCreateDels(new ReversibleActions);
                    u16 i = 0;
                    auto &selUnits = selections.getUnits();
                    for ( u16 &unitIndex : selUnits )
                    { // Remove each selected unit from the map, store in selectedUnits
                        u32 loc = ((u32)unitIndex)*UNIT_STRUCT_SIZE;
                        selectedUnits[shift - i] = CM->getUnit(unitIndex);
                        if ( CM->deleteUnit(unitIndex) )
                        {
                            unitCreateDels->Insert(std::shared_ptr<UnitCreateDel>(new UnitCreateDel(unitIndex, selectedUnits[shift - i])));
                            unitIndex = u16(unitMoveTo + shift - i);
                        }
                        i++;
                    }

                    for ( int i = 0; i<numUnits; i++ )
                    {
                        if ( CM->insertUnit(unitMoveTo + i, selectedUnits[i]) )
                            unitCreateDels->Insert(std::shared_ptr<UnitCreateDel>(new UnitCreateDel(unitMoveTo + i)));
                    }

                    selections.finishMove();
                    selections.ensureFirst(unitStackTopIndex);
                    CM->AddUndo(unitCreateDels);
                    RepopulateList();
                }
            }
        }
    }
    break;
    case IDC_BUTTON_DELETE:
    {
        Selections& selections = CM->GetSelections();
        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
        SendMessage(hUnitList, WM_SETREDRAW, FALSE, 0);
        std::shared_ptr<ReversibleActions> unitDeletes(new ReversibleActions);
        while ( selections.hasUnits() )
        {
            u16 index = selections.getHighestIndex();
            selections.removeUnit(index);
            listUnits.RemoveRow(index);

            HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
            int row = listUnits.GetItemRow(index);

            unitDeletes->Insert(std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index, CM->getUnit(index))));

            CM->deleteUnit(index);

            for ( int i = index + 1; i <= CM->numUnits(); i++ )
                ChangeIndex(hUnitList, i, i - 1);
        }
        CM->AddUndo(unitDeletes);
        CM->Redraw(true);
        SendMessage(hUnitList, WM_SETREDRAW, TRUE, 0);
    }
    break;
    default:
        switch ( HIWORD(wParam) )
        {
        case BN_CLICKED:
            switch ( LOWORD(wParam) )
            {
            case IDC_CHECK_INVINCIBLE:
            {
                std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
                auto &selUnits = CM->GetSelections().getUnits();
                for ( u16 &unitIndex : selUnits )
                {
                    ChkUnit unit = CM->getUnit(unitIndex);
                    unitChanges->Insert(std::shared_ptr<UnitChange>(new
                        UnitChange(unitIndex, ChkUnitField::StateFlags, unit.stateFlags)));

                    if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
                        unit.stateFlags |= UNIT_STATE_INVINCIBLE;
                    else if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED )
                        unit.stateFlags ^= UNIT_STATE_INVINCIBLE;
                }
                CM->AddUndo(unitChanges);
            }
            break;
            case IDC_CHECK_HALLUCINATED:
            {
                std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
                auto &selUnits = CM->GetSelections().getUnits();
                for ( u16 &unitIndex : selUnits )
                {
                    ChkUnit unit = CM->getUnit(unitIndex);
                    unitChanges->Insert(std::shared_ptr<UnitChange>(new
                        UnitChange(unitIndex, ChkUnitField::StateFlags, unit.stateFlags)));

                    if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
                        unit.stateFlags |= UNIT_STATE_HALLUCINATED;
                    else if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED )
                        unit.stateFlags ^= UNIT_STATE_HALLUCINATED;
                }
                CM->AddUndo(unitChanges);
            }
            break;
            case IDC_CHECK_BURROWED:
            {
                std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
                auto &selUnits = CM->GetSelections().getUnits();
                for ( u16 &unitIndex : selUnits )
                {
                    ChkUnit unit = CM->getUnit(unitIndex);
                    unitChanges->Insert(std::shared_ptr<UnitChange>(new
                        UnitChange(unitIndex, ChkUnitField::StateFlags, unit.stateFlags)));

                    if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
                        unit.stateFlags |= UNIT_STATE_BURROWED;
                    else if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED )
                        unit.stateFlags ^= UNIT_STATE_BURROWED;
                }
                CM->AddUndo(unitChanges);
            }
            break;
            case IDC_CHECK_CLOAKED:
            {
                std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
                auto &selUnits = CM->GetSelections().getUnits();
                for ( u16 &unitIndex : selUnits )
                {
                    ChkUnit unit = CM->getUnit(unitIndex);
                    unitChanges->Insert(std::shared_ptr<UnitChange>(new
                        UnitChange(unitIndex, ChkUnitField::StateFlags, unit.stateFlags)));

                    if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
                        unit.stateFlags |= UNIT_STATE_CLOAKED;
                    else if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED )
                        unit.stateFlags ^= UNIT_STATE_CLOAKED;
                }
                CM->AddUndo(unitChanges);
            }
            break;
            case IDC_CHECK_LIFTED:
            {
                std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
                auto &selUnits = CM->GetSelections().getUnits();
                for ( u16 &unitIndex : selUnits )
                {
                    ChkUnit unit = CM->getUnit(unitIndex);
                    unitChanges->Insert(std::shared_ptr<UnitChange>(new
                        UnitChange(unitIndex, ChkUnitField::StateFlags, unit.stateFlags)));

                    if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
                        unit.stateFlags |= UNIT_STATE_LIFTED; // Check lifted state
                    else if ( SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED )
                        unit.stateFlags ^= UNIT_STATE_LIFTED; // Uncheck lifted state
                }
                CM->AddUndo(unitChanges);
            }
            break;
            }
            break;
        case EN_SETFOCUS:
            switch ( LOWORD(wParam) )
            {
            case IDC_EDIT_HP: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Hitpoints); break;
            case IDC_EDIT_MP: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Energy); break;
            case IDC_EDIT_SHIELD: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Shields); break;
            case IDC_EDIT_RESOURCES: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Resources); break;
            case IDC_EDIT_HANGER: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Hanger); break;
            case IDC_EDIT_ID: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Id); break;
            case IDC_EDIT_XC: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Xc); break;
            case IDC_EDIT_YC: preservedStats.addStats(CM->GetSelections(), ChkUnitField::Yc); break;
            }
            break;
        case EN_KILLFOCUS:
            switch ( LOWORD(wParam) )
            {
            case IDC_EDIT_HP: case IDC_EDIT_MP: case IDC_EDIT_SHIELD: case IDC_EDIT_RESOURCES:
            case IDC_EDIT_HANGER: case IDC_EDIT_ID: case IDC_EDIT_XC: case IDC_EDIT_YC:
            {
                preservedStats.convertToUndo();
            }
            break;
            }
            break;
        case EN_UPDATE:
            if ( !initilizing )
            {
                switch ( LOWORD(wParam) )
                {
                case IDC_EDIT_HP:
                {
                    u8 hpPercent;
                    if ( editLife.GetEditNum<u8>(hpPercent) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                            CM->SetUnitHitpoints(unitIndex, hpPercent);
                        
                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_EDIT_MP:
                {
                    u8 mpPercent;
                    if ( editMana.GetEditNum<u8>(mpPercent) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                            CM->SetUnitEnergy(unitIndex, mpPercent);

                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_EDIT_SHIELD:
                {
                    u8 shieldPercent;
                    if ( editShield.GetEditNum<u8>(shieldPercent) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                            CM->SetUnitShields(unitIndex, shieldPercent);
                        
                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_EDIT_RESOURCES:
                {
                    u32 resources;
                    if ( editResources.GetEditNum<u32>(resources) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                            CM->SetUnitResources(unitIndex, resources);

                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_EDIT_HANGER:
                {
                    u16 hanger;
                    if ( editHanger.GetEditNum<u16>(hanger) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                            CM->SetUnitHanger(unitIndex, hanger);

                        CM->Redraw(true);
                    }
                }
                break;
                case IDC_EDIT_ID:
                {
                    u16 unitID;
                    if ( editUnitId.GetEditNum<u16>(unitID) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                        {
                            if ( CM->SetUnitTypeId(unitIndex, unitID) )
                            {
                                int row = listUnits.GetItemRow(unitIndex);

                                ChkdString unitName;
                                CM->getUnitName(unitName, unitID);
                                listUnits.SetItemText(row, UNIT_NAME_COLUMN, unitName.c_str());

                                if ( unitIndex == CM->GetSelections().getFirstUnit() )
                                    SetWindowText(hWnd, unitName.c_str());
                            }
                        }
                        CM->Redraw(true);
                        ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
                    }
                }
                break;
                case IDC_EDIT_XC:
                {
                    u16 unitXC;
                    if ( editXc.GetEditNum<u16>(unitXC) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                        {
                            if ( CM->SetUnitXc(unitIndex, unitXC) )
                            {
                                int row = listUnits.GetItemRow(unitIndex);
                                listUnits.SetItemText(row, UNIT_XC_COLUMN, unitXC);
                            }
                        }
                        CM->Redraw(true);
                        ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
                    }
                }
                break;
                case IDC_EDIT_YC:
                {
                    u16 unitYC;
                    if ( editYc.GetEditNum<u16>(unitYC) )
                    {
                        HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
                        auto &selUnits = CM->GetSelections().getUnits();
                        for ( u16 &unitIndex : selUnits )
                        {
                            if ( CM->SetUnitYc(unitIndex, unitYC) )
                            {
                                int row = listUnits.GetItemRow(unitIndex);
                                listUnits.SetItemText(row, UNIT_YC_COLUMN, unitYC);
                            }
                        }
                        CM->Redraw(true);
                        ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
                    }
                }
                break;
                }
            }
            break;
        }
    }
    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL UnitWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            return FALSE; // Necessary for proper minimize/restore
            break;

        case WM_INITDIALOG:
            CreateSubWindows(hWnd);
            break;

        case WM_SHOWWINDOW:
            {
                LRESULT result = ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
                if ( wParam == TRUE )
                    SetFocus( GetDlgItem(hWnd, IDC_UNITLIST) );

                return result;
            }
            break;

        case WM_KEYDOWN:
            switch ( wParam )
            {
                case VK_RETURN:
                    {
                        HWND hClose = GetDlgItem(hWnd, IDCLOSE);
                        BOOL closeSelected = SendMessage(hClose, BM_GETSTATE, 0, 0) & BST_PUSHED;
                    }
                    break;
            }
            break;

        case IDCLOSE:
            columnSortedBy = UNIT_INDEX_COLUMN; // Reset to sort by index
            DestroyThis();
            break;

        case WM_CLOSE:
            columnSortedBy = UNIT_INDEX_COLUMN; // Reset to sort by index
            DestroyThis();
            break;

        default:
            return FALSE;
            break;
    }
    return TRUE;
}
