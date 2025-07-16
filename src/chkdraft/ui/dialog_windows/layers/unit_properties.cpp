#include "unit_properties.h"
#include "ui/chkd_controls/move_to.h"
#include "chkdraft/chkdraft.h"
#include <CommCtrl.h>
#include <string>

enum class UnitListColumn { Name, Owner, Xc, Yc, Index };

enum_t(Id, u32, {
    UnitList = ID_FIRST,
    ComboPlayer,
    ButtonLinkUnlink, ButtonJumpTo,

    GroupValidFieldFlags, GroupValidStateFlags, GroupLinkFlags,
    
    CheckValidFieldOwner, CheckValidFieldLife, CheckValidFieldMana, CheckValidFieldShield, CheckValidFieldResources, CheckValidFieldHangar,
    TextValidFieldRawFlags, EditValidFieldRawFlags,

    CheckValidStateInvincible, CheckValidStateHallucinated, CheckValidStateBurrowed, CheckValidStateCloaked, CheckValidStateLifted,
    TextValidStateRawFlags, EditValidStateRawFlags,

    GroupMisc,
    TextRawStateFlags, TextUnused, TextUniqueId, TextLinkedId,
    EditRawStateFlags, EditUnused, EditUniqueId, EditLinkedId,

    CheckNydus, CheckAddon,
    TextLinkRawFlags,
    EditLinkRawFlags,
    
    ButtonGraphics,

    GroupUnitProperties = IDC_GROUP_PROPERTIES,
    GroupLinkedUnit = IDC_GROUP_LINK,

    ButtonMoveUp = IDC_BUTTON_MOVEUP,
    ButtonMoveTop = IDC_BUTTON_MOVETOP,
    ButtonMoveDown = IDC_BUTTON_MOVEDOWN,
    ButtonMoveEnd = IDC_BUTTON_MOVEEND,
    ButtonDelete = IDC_BUTTON_DELETE,
    ButtonMoveTo = IDC_BUTTON_MOVE_TO,
    ButtonAdvanced = IDC_BUTTON_ADVANCED,
    ButtonClose = IDC_BUTTON_CLOSE,

    EditHp = IDC_EDIT_HP,
    EditMp = IDC_EDIT_MP,
    EditShields = IDC_EDIT_SHIELD,
    EditResources = IDC_EDIT_RESOURCES,
    EditHangar = IDC_EDIT_HANGAR,
    EditUnitId = IDC_EDIT_ID,
    EditXc = IDC_EDIT_XC,
    EditYc = IDC_EDIT_YC,

    CheckInvincible = IDC_CHECK_INVINCIBLE,
    CheckHallucinated = IDC_CHECK_HALLUCINATED,
    CheckBurrowed = IDC_CHECK_BURROWED,
    CheckCloaked = IDC_CHECK_CLOAKED,
    CheckLifted = IDC_CHECK_LIFTED
});

UnitPropertiesWindow::UnitPropertiesWindow() : columnSortedBy(UnitListColumn::Index), flipSort(false), initilizing(true), changeHighlightOnly(false)
{

}

UnitPropertiesWindow::~UnitPropertiesWindow()
{

}

bool UnitPropertiesWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_UNITPROPERTIES), hParent) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    return false;
}

bool UnitPropertiesWindow::CreateSubWindows(HWND hWnd)
{
    groupUnitProperties.FindThis(hWnd, Id::GroupUnitProperties);

    buttonMoveUp.FindThis(hWnd, Id::ButtonMoveUp);
    buttonMoveTop.FindThis(hWnd, Id::ButtonMoveTop);
    buttonMoveDown.FindThis(hWnd, Id::ButtonMoveDown);
    buttonMoveEnd.FindThis(hWnd, Id::ButtonMoveEnd);
    buttonDelete.FindThis(hWnd, Id::ButtonDelete);
    buttonMoveTo.FindThis(hWnd, Id::ButtonMoveTo);
    buttonAdvanced.FindThis(hWnd, Id::ButtonAdvanced);

    editLife.FindThis(hWnd, Id::EditHp);
    editMana.FindThis(hWnd, Id::EditMp);
    editShield.FindThis(hWnd, Id::EditShields);
    editResources.FindThis(hWnd, Id::EditResources);
    editHangar.FindThis(hWnd, Id::EditHangar);
    editUnitId.FindThis(hWnd, Id::EditUnitId);
    editXc.FindThis(hWnd, Id::EditXc);
    editYc.FindThis(hWnd, Id::EditYc);

    checkInvincible.FindThis(hWnd, Id::CheckInvincible);
    checkHallucinated.FindThis(hWnd, Id::CheckHallucinated);
    checkBurrowed.FindThis(hWnd, Id::CheckBurrowed);
    checkCloaked.FindThis(hWnd, Id::CheckCloaked);
    checkLifted.FindThis(hWnd, Id::CheckLifted);

    dropPlayer.CreateThis(hWnd, 657, 29, 90, 438, Id::ComboPlayer, false);

    groupLinkedUnit.FindThis(hWnd, Id::GroupLinkedUnit);
    buttonLinkUnlink.CreateThis(hWnd, 586, 392, 150, 23, "Link Selected Units", Id::ButtonLinkUnlink, false);
    buttonJumpTo.CreateThis(hWnd, 586, buttonLinkUnlink.Bottom()+2, 150, 23, "Jump To Linked Unit", Id::ButtonJumpTo, false);
    buttonLinkUnlink.setDefaultFont();
    buttonJumpTo.setDefaultFont();
    buttonLinkUnlink.DisableThis();
    buttonJumpTo.DisableThis();

    initilizing = true;

    editLife.CreateNumberBuddy(0, 100);
    editMana.CreateNumberBuddy(0, 100);
    editShield.CreateNumberBuddy(0, 100);
    editResources.CreateNumberBuddy(0, 50000);
    editHangar.CreateNumberBuddy(0, 8);
    editUnitId.CreateNumberBuddy(0, 65535);

    listUnits.CreateThis(hWnd, 9, 10, 549, 449, false, false, Id::UnitList);
    listUnits.EnableFullRowSelect();
    listUnits.setFont(5, 13, "Tahoma", false);

    listUnits.AddColumn(0, "Unit Type", 200, LVCFMT_LEFT);
    listUnits.AddColumn(1, "Owner", 100, LVCFMT_LEFT);
    listUnits.AddColumn(2, "X", 75, LVCFMT_RIGHT);
    listUnits.AddColumn(3, "Y", 75, LVCFMT_RIGHT);
    listUnits.AddColumn(4, "Index", 75, LVCFMT_RIGHT);

    if ( advanced )
        CreateAdvancedTab();

    RepopulateList();

    listUnits.Show();
    initilizing = false;

    return true;
}

void UnitPropertiesWindow::CreateAdvancedTab()
{
    initilizing = true;
    this->setDefaultFont(false);
    standardWidth = this->Width();
    this->SetWidth(standardWidth+206);

    if ( advancedTabCreated )
    {
        if ( CM != nullptr && CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
        {
            EnableUnitEditing();
            auto & unit = CM->getUnit(CM->selections.getFirstUnit());
            this->SetUnitFieldText(unit);
        }
        else
            DisableUnitEditing();

        return;
    }
    else
        advancedTabCreated = true;

    HWND hWnd = getHandle();
    groupValidFieldFlags.CreateThis(hWnd, groupUnitProperties.Right()+16, groupUnitProperties.Top(), 190, 135, "Valid Field Flags", Id::GroupValidFieldFlags);
    auto checkLeft = groupValidFieldFlags.Left()+19;
    checkValidFieldOwner.CreateThis(hWnd, checkLeft, groupValidFieldFlags.Top()+20, 65, 16, false, "Owner", Id::CheckValidFieldOwner);
    checkValidFieldLife.CreateThis(hWnd, checkLeft, checkValidFieldOwner.Bottom()+2, 65, 16, false, "Life", Id::CheckValidFieldLife);
    checkValidFieldMana.CreateThis(hWnd, checkLeft, checkValidFieldLife.Bottom()+2, 65, 16, false, "Mana", Id::CheckValidFieldMana);
    checkValidFieldShield.CreateThis(hWnd, checkValidFieldOwner.Right()+7, groupValidFieldFlags.Top()+20, 70, 16, false, "Shield", Id::CheckValidFieldShield);
    checkValidFieldResources.CreateThis(hWnd, checkValidFieldOwner.Right()+7, checkValidFieldShield.Bottom()+2, 70, 16, false, "Resources", Id::CheckValidFieldResources);
    checkValidFieldHangar.CreateThis(hWnd, checkValidFieldOwner.Right()+7, checkValidFieldResources.Bottom()+2, 70, 16, false, "Hangar", Id::CheckValidFieldHangar);
    textValidFieldRawFlags.CreateThis(hWnd, checkLeft, checkValidFieldMana.Bottom()+5, 55, 13, "Raw Flags:", Id::TextValidFieldRawFlags);
    editValidFieldRawFlags.CreateThis(hWnd, textValidFieldRawFlags.Right()+2, checkValidFieldMana.Bottom()+5, 100, 20, false, Id::EditValidFieldRawFlags);

    groupValidStateFlags.CreateThis(hWnd, groupUnitProperties.Right()+16, groupValidFieldFlags.Bottom()+5, 190, 104, "Valid State Flags", Id::GroupValidStateFlags);
    checkValidStateInvincible.CreateThis(hWnd, checkLeft, groupValidStateFlags.Top()+20, 65, 16, false, "Invincible", Id::CheckValidStateInvincible);
    checkValidStateBurrowed.CreateThis(hWnd, checkLeft, checkValidStateInvincible.Bottom()+2, 65, 16, false, "Burrowed", Id::CheckValidStateBurrowed);
    checkValidStateHallucinated.CreateThis(hWnd, checkValidStateInvincible.Right()+7, groupValidStateFlags.Top()+20, 70, 16, false, "Hallucinated", Id::CheckValidStateHallucinated);
    checkValidStateCloaked.CreateThis(hWnd, checkValidStateInvincible.Right()+7, checkValidStateHallucinated.Bottom()+2, 70, 16, false, "Cloaked", Id::CheckValidStateCloaked);
    checkValidStateLifted.CreateThis(hWnd, checkValidStateInvincible.Right()+7, checkValidStateCloaked.Bottom()+2, 70, 16, false, "Lifted", Id::CheckValidStateLifted);
    textValidStateRawFlags.CreateThis(hWnd, checkLeft, checkValidStateLifted.Bottom()+5, 55, 13, "Raw Flags:", Id::TextValidStateRawFlags);
    editValidStateRawFlags.CreateThis(hWnd, textValidStateRawFlags.Right()+2, checkValidStateLifted.Bottom()+5, 100, 20, false, Id::EditValidStateRawFlags);
    
    groupMisc.CreateThis(hWnd, groupUnitProperties.Right()+16, groupValidStateFlags.Bottom()+5, 190, 122, "Misc", Id::GroupMisc);
    textRawStateFlags.CreateThis(hWnd, checkLeft, groupMisc.Top()+20, 83, 13, "Raw State Flags:", Id::TextRawStateFlags);
    editRawStateFlags.CreateThis(hWnd, textRawStateFlags.Right()+2, groupMisc.Top()+20, 72, 20, false, Id::EditRawStateFlags);
    textUnused.CreateThis(hWnd, groupValidStateFlags.Left()+41, editRawStateFlags.Bottom()+5, 42, 13, "Unused:", Id::TextUnused);
    editUnused.CreateThis(hWnd, textUnused.Right()+2, editRawStateFlags.Bottom()+5, 91, 20, false, Id::EditUnused);
    textUniqueId.CreateThis(hWnd, groupValidStateFlags.Left()+30, editUnused.Bottom()+5, 53, 13, "Unique ID:", Id::TextUniqueId);
    editUniqueId.CreateThis(hWnd, textUniqueId.Right()+2, editUnused.Bottom()+5, 91, 20, false, Id::EditUniqueId);
    textLinkedId.CreateThis(hWnd, groupValidStateFlags.Left()+32, editUniqueId.Bottom()+5, 51, 13, "Linked ID:", Id::TextLinkedId);
    editLinkedId.CreateThis(hWnd, textLinkedId.Right()+2, editUniqueId.Bottom()+5, 91, 20, false, Id::EditLinkedId);

    groupLinkFlags.CreateThis(hWnd, groupUnitProperties.Right()+16, groupMisc.Bottom()+10, 190, 68, "Link Flags", Id::GroupLinkFlags);
    checkNydus.CreateThis(hWnd, checkLeft, groupLinkFlags.Top()+20, 65, 16, false, "Nydus", Id::CheckNydus);
    checkAddon.CreateThis(hWnd, checkNydus.Right()+7, groupLinkFlags.Top()+20, 65, 16, false, "Addon", Id::CheckAddon);
    textLinkRawFlags.CreateThis(hWnd, checkLeft, checkNydus.Bottom()+5, 55, 13, "Raw Flags:", Id::TextLinkRawFlags);
    editLinkRawFlags.CreateThis(hWnd, textLinkRawFlags.Right()+2, checkNydus.Bottom()+5, 100, 20, false, Id::EditLinkRawFlags);

    buttonGraphics.CreateThis(hWnd, groupLinkFlags.Right()-76, groupLinkFlags.Bottom()+9, 75, 23, "Graphics...", Id::ButtonGraphics, false, false);

    const WindowsItem* advancedItems[] {
        &groupValidFieldFlags, &checkValidFieldOwner, &checkValidFieldLife, &checkValidFieldMana, &checkValidFieldShield, &checkValidFieldResources,
        &checkValidFieldHangar, &textValidFieldRawFlags, &editValidFieldRawFlags,

        &groupValidStateFlags, &checkValidStateInvincible, &checkValidStateBurrowed, &checkValidStateHallucinated, &checkValidStateCloaked,
        &checkValidStateLifted, &textValidStateRawFlags, &editValidStateRawFlags,

        &groupMisc,
        &textRawStateFlags, &editRawStateFlags,
        &textUnused, &editUnused,
        &textUniqueId, &editUniqueId,
        &textLinkedId, &editLinkedId,

        &groupLinkFlags,
        &checkNydus, &checkAddon,
        &textLinkRawFlags, &editLinkRawFlags,

        &buttonGraphics
    };

    for ( const WindowsItem* item : advancedItems )
        item->setDefaultFont();

    if ( CM != nullptr && CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
    {
        auto & unit = CM->getUnit(CM->selections.getFirstUnit());
        this->SetUnitFieldText(unit);
    }
    else
        DisableUnitEditing();

    initilizing = false;
}

bool UnitPropertiesWindow::DestroyThis()
{
    columnSortedBy = UnitListColumn::Index; // Reset column sorted by
    this->standardWidth = 0;
    this->advancedTabCreated = false;
    return ClassDialog::DestroyDialog();
}

void UnitPropertiesWindow::SetChangeHighlightOnly(bool changeHighlightOnly)
{
    this->changeHighlightOnly = changeHighlightOnly;
}

void UnitPropertiesWindow::ChangeCurrOwner(u8 newOwner)
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitOwner);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);
        if ( unit.owner != newOwner ) // If the current and new owners are different
        {
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Owner;
            CM->changeUnitOwner(unitIndex, newOwner);
            ChangeUnitsDisplayedOwner(unitIndex, newOwner);
        }
    }
    this->SetUnitFieldText();
    CM->Redraw(true);
}

void UnitPropertiesWindow::ChangeDropdownPlayer(u8 newPlayer)
{
    std::string text;
    if ( newPlayer < 12 )
        dropPlayer.SetSel(newPlayer);
    else
        dropPlayer.SetEditNum<u8>(newPlayer + 1);
}

void UnitPropertiesWindow::ChangeUnitsDisplayedOwner(int index, u8 newPlayer)
{
    char owner[32] = {}, padding[2] = { '\0', '\0' };
    if ( newPlayer < 9 )
        padding[0] = '0';
    else
        padding[0] = '\0';

    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, newPlayer+1);
    listUnits.SetItemText(listUnits.GetItemRow(index), (int)UnitListColumn::Owner, owner);
}

void UnitPropertiesWindow::SetListRedraw(bool allowRedraw)
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

bool UnitPropertiesWindow::AddUnitItem(u16 index, const Chk::Unit & unit)
{
    char owner[32] = {}, padding[2] = { '\0', '\0' },
        xc[32] = {}, yc[32] = {}, cIndex[32] = {};

    u16 unitId = (u16)unit.type;

    if ( unit.owner < 9 )
        padding[0] = '0';
    else
        padding[0] = '\0';

    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, unit.owner + 1);
    std::strcpy(xc, std::to_string(unit.xc).c_str());
    std::strcpy(yc, std::to_string(unit.yc).c_str());
    std::strcpy(cIndex, std::to_string(index).c_str());

    auto unitName = CM->getUnitName<ChkdString>((Sc::Unit::Type)unitId, true);

    listUnits.AddRow(4, index);
    listUnits.SetItemText(index, (int)UnitListColumn::Name, *unitName);
    listUnits.SetItemText(index, (int)UnitListColumn::Owner, owner);
    listUnits.SetItemText(index, (int)UnitListColumn::Xc, xc);
    listUnits.SetItemText(index, (int)UnitListColumn::Yc, yc);
    listUnits.SetItemText(index, (int)UnitListColumn::Index, index);
    return true;
}

void UnitPropertiesWindow::FocusAndSelectIndex(u16 unitIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = unitIndex;
    s32 lvIndex = ListView_FindItem(listUnits.getHandle(), -1, &findInfo);
    ListView_SetItemState(listUnits.getHandle(), lvIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    
    if ( CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
    {
        initilizing = true;
        const Chk::Unit & unit = CM->getUnit(CM->selections.getFirstUnit());
        UpdateLinkArea(unit);
        initilizing = false;
    }
}

void UnitPropertiesWindow::DeselectIndex(u16 unitIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = unitIndex;

    int lvIndex = ListView_FindItem(listUnits.getHandle(), -1, &findInfo);
    ListView_SetItemState(listUnits.getHandle(), lvIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
    
    if ( CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
    {
        initilizing = true;
        const Chk::Unit & unit = CM->getUnit(CM->selections.getFirstUnit());
        UpdateLinkArea(unit);
        initilizing = false;
    }
}

void UnitPropertiesWindow::UpdateEnabledState()
{
    if ( CM->selections.hasUnits() )
    {
        EnableUnitEditing();
        const Chk::Unit & unit = CM->getUnit(CM->selections.getFirstUnit());
        SetUnitFieldText(unit);
    }
    else
        DisableUnitEditing();
}

void UnitPropertiesWindow::RepopulateList()
{
    initilizing = true;
    SetListRedraw(false);

    listUnits.DeleteAllItems();
    if ( CM != nullptr )
    {
        Selections & selections = CM->selections;

        size_t numUnits = CM->numUnits();
        for ( size_t i = 0; i < numUnits; i++ )
        {
            const Chk::Unit & unit = CM->getUnit(i);
            AddUnitItem((u16)i, unit);
        }

        if ( selections.hasUnits() )
        {
            u16 selectedIndex = selections.getFirstUnit();
            listUnits.FocusItem(selectedIndex);

            for ( auto unitIndex : CM->view.units.sel() )
                listUnits.SelectRow(unitIndex);

            EnableUnitEditing();
            const Chk::Unit & unit = CM->getUnit(selectedIndex);
            SetUnitFieldText(unit);

            auto unitName = CM->getUnitName<ChkdString>(unit.type, true);
            WindowsItem::SetWinText(*unitName);

            int row = listUnits.GetItemRow(selections.getHighestUnitIndex());
            listUnits.EnsureVisible(row, false);
            row = listUnits.GetItemRow(selections.getLowestUnitIndex());
            listUnits.EnsureVisible(row, false);
        }
    }

    SetListRedraw(true);
    initilizing = false;
}

void UnitPropertiesWindow::EnableUnitEditing()
{
    dropPlayer.EnableThis();

    WinLib::ButtonControl* buttonControls[] = { &buttonMoveUp, &buttonMoveTop, &buttonMoveDown, &buttonMoveEnd, &buttonDelete, &buttonMoveTo };
    WinLib::EditControl* editControls[] = { &editLife, &editMana, &editShield, &editResources, &editHangar, &editUnitId, &editXc, &editYc };
    WinLib::CheckBoxControl* checkControls[] = { &checkInvincible, &checkHallucinated, &checkBurrowed, &checkCloaked, &checkLifted };
    int numButtonControls = sizeof(buttonControls) / sizeof(WinLib::ButtonControl*),
        numEditControls = sizeof(editControls) / sizeof(WinLib::EditControl*),
        numCheckBoxControls = sizeof(checkControls) / sizeof(WinLib::CheckBoxControl*);

    for ( int i = 0; i < numButtonControls; i++ )
        buttonControls[i]->EnableThis();

    for ( int i = 0; i < numEditControls; i++ )
        editControls[i]->EnableThis();

    for ( int i = 0; i < numCheckBoxControls; i++ )
    {
        checkControls[i]->EnableThis();
        checkControls[i]->SetCheck(false);
    }

    const Chk::Unit & unit = CM->getUnit(CM->selections.getFirstUnit());
    SetUnitFieldText(unit);

    if ( advancedTabCreated )
    {
        WindowsItem* advancedItems[] {
            &groupValidFieldFlags, &checkValidFieldOwner, &checkValidFieldLife, &checkValidFieldMana, &checkValidFieldShield, &checkValidFieldResources,
            &checkValidFieldHangar, &textValidFieldRawFlags, &editValidFieldRawFlags,

            &groupValidStateFlags, &checkValidStateInvincible, &checkValidStateBurrowed, &checkValidStateHallucinated, &checkValidStateCloaked,
            &checkValidStateLifted, &textValidStateRawFlags, &editValidStateRawFlags,

            &groupMisc,
            &textRawStateFlags, &editRawStateFlags,
            &textUnused, &editUnused,
            &textUniqueId, &editUniqueId,
            &textLinkedId, &editLinkedId,

            &groupLinkFlags,
            &checkNydus, &checkAddon,
            &textLinkRawFlags, &editLinkRawFlags,

            &buttonGraphics
        };

        for ( WindowsItem* item : advancedItems )
            item->EnableThis();
    }
}

void UnitPropertiesWindow::DisableUnitEditing()
{
    WindowsItem::SetWinText("");

    dropPlayer.SetSel(-1);
    dropPlayer.DisableThis();

    WinLib::ButtonControl* buttonControls[] = { &buttonMoveUp, &buttonMoveTop, &buttonMoveDown, &buttonMoveEnd, &buttonDelete, &buttonMoveTo };
    WinLib::EditControl* editControls[] = { &editLife, &editMana, &editShield, &editResources, &editHangar, &editUnitId, &editXc, &editYc };
    WinLib::CheckBoxControl* checkControls[] = { &checkInvincible, &checkHallucinated, &checkBurrowed, &checkCloaked, &checkLifted };
    int numButtonControls = sizeof(buttonControls) / sizeof(WinLib::ButtonControl*),
        numEditControls = sizeof(editControls) / sizeof(WinLib::EditControl*),
        numCheckBoxControls = sizeof(checkControls) / sizeof(WinLib::CheckBoxControl*);

    for ( int i = 0; i < numButtonControls; i++ )
        buttonControls[i]->DisableThis();

    for ( int i = 0; i < numEditControls; i++ )
    {
        editControls[i]->SetText("");
        editControls[i]->DisableThis();
    }

    for ( int i = 0; i < numCheckBoxControls; i++ )
    {
        checkControls[i]->SetCheck(false);
        checkControls[i]->DisableThis();
    }

    if ( advancedTabCreated )
    {
        WinLib::CheckBoxControl* advancedCheckControls[] {
            &checkValidFieldOwner, &checkValidFieldLife, &checkValidFieldMana, &checkValidFieldShield, &checkValidFieldResources, &checkValidFieldHangar,
            &checkValidStateInvincible, &checkValidStateBurrowed, &checkValidStateHallucinated, &checkValidStateCloaked, &checkValidStateLifted,
            &checkNydus, &checkAddon
        };
        WinLib::EditControl* advancedEditControls[] { &editValidFieldRawFlags, &editValidStateRawFlags,
            &editRawStateFlags, &editUnused, &editUniqueId, &editLinkedId, &editLinkRawFlags };

        for ( WinLib::CheckBoxControl* checkControl : advancedCheckControls )
        {
            checkControl->SetCheck(false);
            checkControl->DisableThis();
        }

        for ( WinLib::EditControl* editControl : advancedEditControls )
        {
            editControl->SetText("");
            editControl->DisableThis();
        }

        buttonGraphics.DisableThis();
    }
}

void UnitPropertiesWindow::UpdateLinkArea(const Chk::Unit & unit)
{
    bool linked = CM != nullptr && unit.isLinked();
    if ( linked )
    {
        bool found = false;
        auto relatedId = unit.relationClassId;
        for ( size_t i=0; i<CM->read.units.size(); ++i )
        {
            auto & otherUnit = CM->read.units[i];
            if ( unit.relationClassId == otherUnit.classId )
            {
                auto linkedUnitType = otherUnit.type;
                std::string displayText = "Linked ";
                if ( linkedUnitType < Sc::Unit::TotalTypes )
                {
                    std::string unitName = Sc::Unit::defaultDisplayNames[linkedUnitType];
                    constexpr std::string_view terran = "Terran ";
                    constexpr std::string_view zerg = "Zerg ";
                    constexpr std::string_view protoss = "Protoss ";
                    if ( unitName.find(terran) == 0 )
                        displayText += unitName.substr(terran.size(), unitName.size()-terran.size());
                    else if ( unitName.find(zerg) == 0 )
                        displayText += unitName.substr(zerg.size(), unitName.size()-zerg.size());
                    else if ( unitName.find(protoss) == 0 )
                        displayText += unitName.substr(protoss.size(), unitName.size()-protoss.size());
                    else
                        displayText += unitName;
                }
                else
                    displayText += "UnitId:" + std::to_string(u16(linkedUnitType));

                groupLinkedUnit.SetWinText(displayText + " [" + std::to_string(i) + "]");

                buttonLinkUnlink.SetText("Unlink");
                buttonLinkUnlink.EnableThis();
                buttonJumpTo.SetWinText("Jump To Linked Unit");
                buttonJumpTo.EnableThis();
                found = true;
                break;
            }
        }

        if ( !found )
            linked = false;
    }

    if ( !linked )
    {
        bool linkable = false;
        if ( CM->selections.numUnits() == 2 )
        {
            auto firstIndex = CM->view.units.sel()[0];
            auto secondIndex = CM->view.units.sel()[1];
            if ( firstIndex != secondIndex && firstIndex < CM->numUnits() && secondIndex < CM->numUnits() )
            {
                auto & first = CM->getUnit(firstIndex);
                auto & second = CM->getUnit(secondIndex);
                auto firstType = first.type;
                auto secondType = second.type;
                if ( !first.isLinked() && !second.isLinked() && CM->isLinkable(first, second) )
                {
                    groupLinkedUnit.SetWinText("Nydus/Addon Link");
                    buttonLinkUnlink.SetWinText("Link Selected Units");
                    buttonLinkUnlink.EnableThis();
                    buttonJumpTo.SetWinText("Jump To Linked Unit");
                    buttonJumpTo.DisableThis();
                    linkable = true;
                }
            }
        }
        if ( !linkable )
        {
            groupLinkedUnit.SetWinText("Nydus/Addon Link");
            buttonLinkUnlink.SetWinText("Link Selected Units");
            buttonLinkUnlink.DisableThis();
            buttonJumpTo.SetWinText("Jump To Linked Unit");
            buttonJumpTo.DisableThis();
        }
    }
}

void UnitPropertiesWindow::SetUnitFieldText(const Chk::Unit & unit)
{
    initilizing = true;

    if ( unit.owner < 12 )
        dropPlayer.SetSel(unit.owner);
    else
        dropPlayer.SetWinText(std::to_string(unit.owner + 1));

    editLife.SetText(std::to_string(unit.hitpointPercent));
    editMana.SetText(std::to_string(unit.energyPercent));
    editShield.SetText(std::to_string(unit.shieldPercent));
    editResources.SetText(std::to_string(unit.resourceAmount));
    editHangar.SetText(std::to_string(unit.hangarAmount));
    editUnitId.SetText(std::to_string((u16)unit.type));
    editXc.SetText(std::to_string(unit.xc));
    editYc.SetText(std::to_string(unit.yc));

    checkInvincible.SetCheck((unit.stateFlags&Chk::Unit::State::Invincible) == Chk::Unit::State::Invincible);
    checkHallucinated.SetCheck((unit.stateFlags&Chk::Unit::State::Hallucinated) == Chk::Unit::State::Hallucinated);
    checkBurrowed.SetCheck((unit.stateFlags&Chk::Unit::State::Burrow) == Chk::Unit::State::Burrow);
    checkCloaked.SetCheck((unit.stateFlags&Chk::Unit::State::Cloak) == Chk::Unit::State::Cloak);
    checkLifted.SetCheck((unit.stateFlags&Chk::Unit::State::InTransit) == Chk::Unit::State::InTransit);

    UpdateLinkArea(unit);

    if ( advanced )
    {
        checkValidFieldOwner.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Owner) == Chk::Unit::ValidField::Owner);
        checkValidFieldLife.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Hitpoints) == Chk::Unit::ValidField::Hitpoints);
        checkValidFieldMana.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Energy) == Chk::Unit::ValidField::Energy);
        checkValidFieldShield.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Shields) == Chk::Unit::ValidField::Shields);
        checkValidFieldResources.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Resources) == Chk::Unit::ValidField::Resources);
        checkValidFieldHangar.SetCheck((unit.validFieldFlags&Chk::Unit::ValidField::Hangar) == Chk::Unit::ValidField::Hangar);
        editValidFieldRawFlags.SetEditNum(unit.validFieldFlags);

        checkValidStateInvincible.SetCheck((unit.validStateFlags&Chk::Unit::State::Invincible) == Chk::Unit::State::Invincible);
        checkValidStateBurrowed.SetCheck((unit.validStateFlags&Chk::Unit::State::Burrow) == Chk::Unit::State::Burrow);
        checkValidStateHallucinated.SetCheck((unit.validStateFlags&Chk::Unit::State::Hallucinated) == Chk::Unit::State::Hallucinated);
        checkValidStateCloaked.SetCheck((unit.validStateFlags&Chk::Unit::State::Cloak) == Chk::Unit::State::Cloak);
        checkValidStateLifted.SetCheck((unit.validStateFlags&Chk::Unit::State::InTransit) == Chk::Unit::State::InTransit);
        editValidStateRawFlags.SetEditNum(unit.validStateFlags);

        editRawStateFlags.SetEditNum(unit.stateFlags);
        editUnused.SetEditNum(unit.unused);
        editUniqueId.SetEditNum(unit.classId);
        editLinkedId.SetEditNum(unit.relationClassId);
    
        checkNydus.SetCheck((unit.relationFlags&Chk::Unit::RelationFlag::NydusLink) == Chk::Unit::RelationFlag::NydusLink);
        checkAddon.SetCheck((unit.relationFlags&Chk::Unit::RelationFlag::AddonLink) == Chk::Unit::RelationFlag::AddonLink);
        editLinkRawFlags.SetEditNum(unit.relationFlags);
    }

    initilizing = false;
}

void UnitPropertiesWindow::SetUnitFieldText()
{
    if ( CM != nullptr && CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
        SetUnitFieldText(CM->getUnit(CM->selections.getFirstUnit()));
}

void UnitPropertiesWindow::SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2)
{
    int row1 = listUnits.GetItemRow((int)index1),
        row2 = listUnits.GetItemRow((int)index2);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row1;
    item.lParam = index2;

    ListView_SetItem(hListView, &item);
    listUnits.SetItemText(row1, (int)UnitListColumn::Index, (int)index2);

    item.iItem = row2;
    item.lParam = index1;

    ListView_SetItem(hListView, &item);
    listUnits.SetItemText(row2, (int)UnitListColumn::Index, (int)index1);
}

void UnitPropertiesWindow::ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam)
{
    int row = listUnits.GetItemRow((int)oldLParam);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row;
    item.lParam = newLParam;

    ListView_SetItem(hListView, &item);

    listUnits.SetItemText(row, (int)UnitListColumn::Index, (int)newLParam);
}

int UnitPropertiesWindow::CompareLvItems(LPARAM index1, LPARAM index2)
{
    int sort = 0;

    if ( columnSortedBy != UnitListColumn::Index )
    {
        const Chk::Unit & firstUnit = CM->getUnit((u16)index1);
        const Chk::Unit & secondUnit = CM->getUnit((u16)index2);

        switch ( columnSortedBy )
        {
        case UnitListColumn::Name: // Sort by ID / type
            if ( (u16)firstUnit.type < (u16)secondUnit.type )
                sort = -1;
            else if ( (u16)firstUnit.type > (u16)secondUnit.type )
                sort = 1;
            break;
        case UnitListColumn::Owner: // Sort by owner
            if ( firstUnit.owner < secondUnit.owner )
                sort = -1;
            else if ( firstUnit.owner > secondUnit.owner )
                sort = 1;
            break;
        case UnitListColumn::Xc: // Sort by xc
            if ( firstUnit.xc < secondUnit.xc )
                sort = -1;
            else if ( firstUnit.xc > secondUnit.xc )
                sort = 1;
            else if ( firstUnit.yc < secondUnit.yc ) // Try to sort by yc if xc's are equal
                sort = -1;
            else if ( firstUnit.yc > secondUnit.yc )
                sort = 1;
            break;
        case UnitListColumn::Yc: // Sort by yc
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
    else if ( columnSortedBy == UnitListColumn::Index )
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

void UnitPropertiesWindow::LvColumnClicked(NMHDR* nmhdr)
{
    UnitListColumn newColumn = (UnitListColumn)((NMLISTVIEW*)nmhdr)->iSubItem;
    if ( newColumn == columnSortedBy )
        flipSort = !flipSort;
    else
        flipSort = false;

    columnSortedBy = newColumn;
    ListView_SortItems(nmhdr->hwndFrom, ForwardCompareLvItems, this);
}

void UnitPropertiesWindow::LvItemChanged(NMHDR* nmhdr)
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitSel);
    Selections & selections = CM->selections;
    if ( !changeHighlightOnly )
    {
        NMLISTVIEW* itemInfo = (NMLISTVIEW*)nmhdr;
        u16 index = (u16)itemInfo->lParam;

        if ( itemInfo->uNewState & LVIS_SELECTED && initilizing == false ) // Selected
                                                                           // Add item to selection
        {
            bool firstSelected = !selections.hasUnits();
            const auto & sel = CM->view.units.sel();
            if ( std::find(sel.begin(), sel.end(), index) == sel.end() )
                edit->units.select(index);

            if ( firstSelected )
                EnableUnitEditing();

            const Chk::Unit & unit = CM->getUnit(index);
            auto unitName = CM->getUnitName<ChkdString>(unit.type, true);
            WindowsItem::SetWinText(*unitName);
            SetUnitFieldText(unit);

            CM->viewUnit(index);
        }
        else if ( itemInfo->uOldState & LVIS_SELECTED ) // From selected to not selected
                                                        // Remove item from selection
        {
            const auto & sel = CM->view.units.sel();
            if ( std::find(sel.begin(), sel.end(), u32(index)) != sel.end() )
                edit->units.deselect(index);

            if ( CM->selections.hasUnits() && CM->selections.getFirstUnit() < CM->numUnits() )
            {
                initilizing = true;
                const Chk::Unit & unit = CM->getUnit(CM->selections.getFirstUnit());
                UpdateLinkArea(unit);
                initilizing = false;
            }

            if ( !selections.hasUnits()
                && !(GetKeyState(VK_DOWN) & 0x8000
                    || GetKeyState(VK_UP) & 0x8000
                    || GetKeyState(VK_LEFT) & 0x8000
                    || GetKeyState(VK_RIGHT) & 0x8000
                    || GetKeyState(VK_LBUTTON) & 0x8000
                    || GetKeyState(VK_RBUTTON) & 0x8000) )
            {
                DisableUnitEditing();
            }

            if ( selections.hasUnits() )
                CM->viewUnit(selections.getFirstUnit());
            else
                CM->Redraw(false);
        }
    }
}

void UnitPropertiesWindow::NotifyAdvancedToggled()
{
    advanced = !advanced;
    if ( advanced )
    {
        this->CreateAdvancedTab();
        this->buttonAdvanced.SetText("Standard");
    }
    else
    {
        this->SetWidth(standardWidth);
        this->buttonAdvanced.SetText("Advanced");
    }
}

void UnitPropertiesWindow::NotifyClosePressed()
{
    EndDialog(getHandle(), IDCLOSE);
}

void UnitPropertiesWindow::NotifyMoveTopPressed()
{
    if ( CM->selections.hasUnits() )
    {
        listUnits.SetRedraw(false);
        CM->operator()(ActionDescriptor::MoveUnits)->units.moveSelectionsTop();
        RepopulateList();
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyMoveEndPressed()
{
    if ( CM->selections.hasUnits() )
    {
        listUnits.SetRedraw(false);
        CM->operator()(ActionDescriptor::MoveUnits)->units.moveSelectionsBottom();
        RepopulateList();
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyMoveUpPressed()
{
    if ( CM->selections.hasUnits() )
    {
        listUnits.SetRedraw(false);
        CM->operator()(ActionDescriptor::MoveUnits)->units.moveSelectionsUp();
        RepopulateList();
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyMoveDownPressed()
{
    if ( CM->selections.hasUnits() )
    {
        listUnits.SetRedraw(false);
        CM->operator()(ActionDescriptor::MoveUnits)->units.moveSelectionsDown();
        RepopulateList();
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyMoveToPressed()
{
    u32 unitMoveTo = 0;
    if ( CM->selections.hasUnits() && MoveToDialog<u32>::GetIndex(unitMoveTo, getHandle()) && unitMoveTo < u32(CM->numUnits()) )
    {
        listUnits.SetRedraw(false);
        CM->operator()(ActionDescriptor::MoveUnits)->units.moveSelectionsTo(unitMoveTo);
        RepopulateList();
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyDeletePressed()
{
    if ( CM->selections.hasUnits() )
    {
        auto edit = CM->operator()(ActionDescriptor::DeleteUnits);
        listUnits.SetRedraw(false);
        CM->unlinkAndDeleteSelectedUnits();

        RepopulateList();
        CM->Redraw(true);
        listUnits.SetRedraw(true);
    }
}

void UnitPropertiesWindow::NotifyLinkUnlinkPressed()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitLinked);
    size_t numUnits = CM->numUnits();
    if ( CM == nullptr || !CM->selections.hasUnits() || CM->selections.getFirstUnit() >= numUnits )
        return;

    size_t currUnitIndex = size_t(CM->selections.getFirstUnit());
    const auto & unit = CM->getUnit(currUnitIndex);
    if ( unit.isLinked() ) // Break or clear the link
    {
        for ( size_t i=0; i<numUnits; ++i )
        {
            const auto & otherUnit = CM->getUnit(i);
            if ( unit.relationClassId == otherUnit.classId && i != currUnitIndex )
            {
                edit->units[currUnitIndex].relationClassId = 0;
                edit->units[currUnitIndex].relationFlags = 0;
                edit->units[i].relationClassId = 0;
                edit->units[i].relationFlags = 0;
                SetUnitFieldText(unit);
                return;
            }
        }
    }
    else if ( CM->selections.numUnits() == 2 ) // Link up the units
    {
        bool linkable = false;
        auto firstIndex = CM->view.units.sel()[0];
        auto secondIndex = CM->view.units.sel()[1];
        if ( firstIndex != secondIndex && firstIndex < CM->numUnits() && secondIndex < CM->numUnits() )
        {
            auto & first = CM->getUnit(firstIndex);
            auto & second = CM->getUnit(secondIndex);
            auto firstType = first.type;
            auto secondType = second.type;
            if ( !first.isLinked() && !second.isLinked() && CM->isLinkable(first, second) )
            {
                edit->units[firstIndex].relationClassId = second.classId;
                edit->units[secondIndex].relationClassId = first.classId;
                auto relationFlags = first.type == Sc::Unit::Type::ZergNydusCanal ? Chk::Unit::RelationFlag::NydusLink : Chk::Unit::RelationFlag::AddonLink;
                edit->units[firstIndex].relationFlags = relationFlags;
                edit->units[secondIndex].relationFlags = relationFlags;
                SetUnitFieldText(first);
                return;
            }
        }
    }
}

void UnitPropertiesWindow::NotifyJumpToPressed()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitSel);
    size_t numUnits = CM->numUnits();
    if ( CM == nullptr || !CM->selections.hasUnits() || CM->selections.getFirstUnit() >= numUnits )
        return;
    
    const auto & sel = CM->view.units.sel();
    size_t currUnitIndex = size_t(CM->selections.getFirstUnit());
    auto & unit = CM->getUnit(currUnitIndex);
    if ( unit.isLinked() )
    {
        for ( size_t i=0; i<numUnits; ++i )
        {
            auto & otherUnit = CM->getUnit(i);
            if ( unit.relationClassId == otherUnit.classId && i != currUnitIndex )
            {
                edit->units.clearSelections();
                if ( std::find(sel.begin(), sel.end(), u32(i)) == sel.end() )
                    edit->units.select(i);

                this->RepopulateList();
                CM->viewUnit(i);
                return;
            }
        }
    }
}

void UnitPropertiesWindow::NotifyGraphicsPressed()
{
    u16 firstUnit = CM->selections.getFirstUnit();
    if ( std::size_t(firstUnit) < CM->numUnits() )
    {
        if ( chkd.actorWindow.getHandle() == NULL )
            chkd.actorWindow.CreateThis(chkd.getHandle());
        
        NotifyClosePressed();
        ::ShowWindow(chkd.actorWindow.getHandle(), SW_SHOW);
        chkd.actorWindow.FocusAndSelectIndex(CM->view.units.attachedData(firstUnit).drawListIndex);
        chkd.actorWindow.FocusThis();
    }
}

void UnitPropertiesWindow::NotifyInvincibleClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitInvincible);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkInvincible.isChecked() )
        {
            edit->units[unitIndex].stateFlags |= Chk::Unit::State::Invincible;
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Invincible;
        }
        else
            edit->units[unitIndex].stateFlags &= ~Chk::Unit::State::Invincible;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyHallucinatedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitHallucinated);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkHallucinated.isChecked() )
        {
            edit->units[unitIndex].stateFlags |= Chk::Unit::State::Hallucinated;
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Hallucinated;
        }
        else
            edit->units[unitIndex].stateFlags &= ~Chk::Unit::State::Hallucinated;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyBurrowedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitBurrowed);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkBurrowed.isChecked() )
        {
            edit->units[unitIndex].stateFlags |= Chk::Unit::State::Burrow;
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Burrow;
        }
        else
            edit->units[unitIndex].stateFlags &= ~Chk::Unit::State::Burrow;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyCloakedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitCloaked);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkCloaked.isChecked() )
        {
            edit->units[unitIndex].stateFlags |= Chk::Unit::State::Cloak;
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Cloak;
        }
        else
            edit->units[unitIndex].stateFlags &= ~Chk::Unit::State::Cloak;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyLiftedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitLifted);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkLifted.isChecked() )
        {
            edit->units[unitIndex].stateFlags |= Chk::Unit::State::InTransit; // Check lifted state
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::InTransit;
        }
        else
            edit->units[unitIndex].stateFlags &= ~Chk::Unit::State::InTransit; // Uncheck lifted state
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyHpEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitHp);
    u8 hpPercent = 0;
    if ( editLife.GetEditNum<u8>(hpPercent) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            auto & unit = CM->getUnit(unitIndex);
            edit->units[unitIndex].hitpointPercent = hpPercent;
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Hitpoints;
        }
        this->SetUnitFieldText();
        CM->Redraw(false);
    }
}

void UnitPropertiesWindow::NotifyMpEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitMp);
    u8 mpPercent = 0;
    if ( editMana.GetEditNum<u8>(mpPercent) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            auto & unit = CM->getUnit(unitIndex);
            edit->units[unitIndex].energyPercent = mpPercent;
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Energy;
        }
        this->SetUnitFieldText();
        CM->Redraw(false);
    }
}

void UnitPropertiesWindow::NotifyShieldEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitShields);
    u8 shieldPercent = 0;
    if ( editShield.GetEditNum<u8>(shieldPercent) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            auto & unit = CM->getUnit(unitIndex);
            edit->units[unitIndex].shieldPercent = shieldPercent;
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Shields;
        }
        this->SetUnitFieldText();
        CM->Redraw(false);
    }
}

void UnitPropertiesWindow::NotifyResourcesEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitResources);
    u32 resources = 0;
    if ( editResources.GetEditNum<u32>(resources) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            auto & unit = CM->getUnit(unitIndex);
            edit->units[unitIndex].resourceAmount = resources;
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Resources;
        }
        this->SetUnitFieldText();

        CM->Redraw(false);
    }
}

void UnitPropertiesWindow::NotifyHangarEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitHangar);
    u16 hangar = 0;
    if ( editHangar.GetEditNum<u16>(hangar) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            auto & unit = CM->getUnit(unitIndex);
            edit->units[unitIndex].hangarAmount = hangar;
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Hangar;
        }
        this->SetUnitFieldText();
        CM->Redraw(true);
    }
}

void UnitPropertiesWindow::NotifyIdEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::ChangeUnitType);
    u16 unitID = 0;
    if ( editUnitId.GetEditNum<u16>(unitID) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            if ( CM->read.units[unitIndex].type != (Sc::Unit::Type)unitID )
                edit->units[unitIndex].type = (Sc::Unit::Type)unitID;

            int row = listUnits.GetItemRow(unitIndex);

            auto unitName = CM->getUnitName<ChkdString>((Sc::Unit::Type)unitID, true);
            listUnits.SetItemText(row, (int)UnitListColumn::Name, *unitName);

            if ( unitIndex == CM->selections.getFirstUnit() )
                WindowsItem::SetWinText(*unitName);
        }
        CM->Redraw(true);
        ListView_SortItems(listUnits.getHandle(), ForwardCompareLvItems, this);
    }
}

void UnitPropertiesWindow::NotifyXcEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::AdjustUnitXc);
    u16 unitXC = 0;
    if ( editXc.GetEditNum<u16>(unitXC) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            edit->units[unitIndex].xc = unitXC;
            int row = listUnits.GetItemRow(unitIndex);
            listUnits.SetItemText(row, (int)UnitListColumn::Xc, unitXC);
        }
        CM->Redraw(true);
        ListView_SortItems(listUnits.getHandle(), ForwardCompareLvItems, this);
    }
}

void UnitPropertiesWindow::NotifyYcEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::AdjustUnitYc);
    u16 unitYC = 0;
    if ( editYc.GetEditNum<u16>(unitYC) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
        {
            edit->units[unitIndex].yc = unitYC;
            int row = listUnits.GetItemRow(unitIndex);
            listUnits.SetItemText(row, (int)UnitListColumn::Yc, unitYC);
        }
        CM->Redraw(true);
        ListView_SortItems(listUnits.getHandle(), ForwardCompareLvItems, this);
    }
}

void UnitPropertiesWindow::NotifyValidFieldOwnerClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldOwner);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldOwner.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Owner;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Owner;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldLifeClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldLife);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldLife.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Hitpoints;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Hitpoints;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldManaClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldMana);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldMana.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Energy;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Energy;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldShieldClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldShields);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldShield.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Shields;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Shields;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldResourcesClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldResources);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldResources.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Resources;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Resources;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldHangarClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidFieldHangar);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidFieldHangar.isChecked() )
            edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Hangar;
        else
            edit->units[unitIndex].validFieldFlags &= ~Chk::Unit::ValidField::Hangar;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidFieldRawFlagsEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitValidFieldRawFlags);
    u16 validFields = 0;
    if ( editValidFieldRawFlags.GetEditNum<u16>(validFields) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].validFieldFlags = validFields;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateInvincibleClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidStateInvincible);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidStateInvincible.isChecked() )
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Invincible;
        else
            edit->units[unitIndex].validStateFlags &= ~Chk::Unit::State::Invincible;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateBurrowedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidStateBurrowed);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidStateBurrowed.isChecked() )
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Burrow;
        else
            edit->units[unitIndex].validStateFlags &= ~Chk::Unit::State::Burrow;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateHallucinatedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidStateHallucinated);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidStateHallucinated.isChecked() )
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Hallucinated;
        else
            edit->units[unitIndex].validStateFlags &= ~Chk::Unit::State::Hallucinated;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateCloakedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidStateCloaked);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidStateCloaked.isChecked() )
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::Cloak;
        else
            edit->units[unitIndex].validStateFlags &= ~Chk::Unit::State::Cloak;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateLiftedClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitValidStateLifted);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkValidStateLifted.isChecked() )
            edit->units[unitIndex].validStateFlags |= Chk::Unit::State::InTransit;
        else
            edit->units[unitIndex].validStateFlags &= ~Chk::Unit::State::InTransit;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyValidStateRawFlagsEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitValidStateRawFlags);
    u16 validState = 0;
    if ( editValidStateRawFlags.GetEditNum<u16>(validState) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].validStateFlags = validState;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyStateRawFlagsEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitStateRawFlags);
    u16 state = 0;
    if ( editRawStateFlags.GetEditNum<u16>(state) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].stateFlags = state;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyUnusedEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitUnused);
    u16 unused = 0;
    if ( editUnused.GetEditNum<u16>(unused) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].unused = unused;
    }
    this->SetUnitFieldText();
}

void UnitPropertiesWindow::NotifyUniqueIdEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitUniqueId);
    u16 uniqueId = 0;
    if ( editUniqueId.GetEditNum<u16>(uniqueId) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].classId = uniqueId;
    }
    this->SetUnitFieldText();
    CM->Redraw(false);
}

void UnitPropertiesWindow::NotifyLinkedIdEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitLinkedId);
    u16 linkedId = 0;
    if ( editLinkedId.GetEditNum<u16>(linkedId) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].relationClassId = linkedId;
    }
    this->SetUnitFieldText();
    CM->Redraw(false);
}

void UnitPropertiesWindow::NotifyLinkNydusClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitNydusLink);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkNydus.isChecked() )
            edit->units[unitIndex].relationFlags |= Chk::Unit::RelationFlag::NydusLink;
        else
            edit->units[unitIndex].relationFlags &= ~Chk::Unit::RelationFlag::NydusLink;
    }
    this->SetUnitFieldText();
    CM->Redraw(false);
}

void UnitPropertiesWindow::NotifyLinkAddonClicked()
{
    auto edit = CM->operator()(ActionDescriptor::ToggleUnitAddonLink);
    for ( auto unitIndex : CM->view.units.sel() )
    {
        const Chk::Unit & unit = CM->getUnit(unitIndex);

        if ( checkAddon.isChecked() )
            edit->units[unitIndex].relationFlags |= Chk::Unit::RelationFlag::AddonLink;
        else
            edit->units[unitIndex].relationFlags &= ~Chk::Unit::RelationFlag::AddonLink;
    }
    this->SetUnitFieldText();
    CM->Redraw(false);
}

void UnitPropertiesWindow::NotifyRelationRawFlagsEditUpdated()
{
    auto edit = CM->operator()(ActionDescriptor::UpdateUnitRelationRawFlags);
    u16 relationFlags = 0;
    if ( editLinkRawFlags.GetEditNum<u16>(relationFlags) )
    {
        for ( auto unitIndex : CM->view.units.sel() )
            edit->units[unitIndex].relationFlags = relationFlags;
    }
    this->SetUnitFieldText();
    CM->Redraw(false);
}

void UnitPropertiesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ButtonAdvanced: NotifyAdvancedToggled(); break;
    case Id::ButtonClose: NotifyClosePressed(); break;
    case Id::ButtonMoveTop: NotifyMoveTopPressed(); break;
    case Id::ButtonMoveEnd: NotifyMoveEndPressed(); break;
    case Id::ButtonMoveUp: NotifyMoveUpPressed(); break;
    case Id::ButtonMoveDown: NotifyMoveDownPressed(); break;
    case Id::ButtonMoveTo: NotifyMoveToPressed(); break;
    case Id::ButtonDelete: NotifyDeletePressed(); break;
    case Id::ButtonLinkUnlink: NotifyLinkUnlinkPressed(); break;
    case Id::ButtonJumpTo: NotifyJumpToPressed(); break;
    case Id::ButtonGraphics: NotifyGraphicsPressed(); break;
    case Id::CheckInvincible: NotifyInvincibleClicked(); break;
    case Id::CheckHallucinated: NotifyHallucinatedClicked(); break;
    case Id::CheckBurrowed: NotifyBurrowedClicked(); break;
    case Id::CheckCloaked: NotifyCloakedClicked(); break;
    case Id::CheckLifted: NotifyLiftedClicked(); break;
    case Id::CheckValidFieldOwner: NotifyValidFieldOwnerClicked(); break;
    case Id::CheckValidFieldLife: NotifyValidFieldLifeClicked(); break;
    case Id::CheckValidFieldMana: NotifyValidFieldManaClicked(); break;
    case Id::CheckValidFieldShield: NotifyValidFieldShieldClicked(); break;
    case Id::CheckValidFieldResources: NotifyValidFieldResourcesClicked(); break;
    case Id::CheckValidFieldHangar: NotifyValidFieldHangarClicked(); break;
    case Id::CheckValidStateInvincible: NotifyValidStateInvincibleClicked(); break;
    case Id::CheckValidStateBurrowed: NotifyValidStateBurrowedClicked(); break;
    case Id::CheckValidStateHallucinated: NotifyValidStateHallucinatedClicked(); break;
    case Id::CheckValidStateCloaked: NotifyValidStateCloakedClicked(); break;
    case Id::CheckValidStateLifted: NotifyValidStateLiftedClicked(); break;
    case Id::CheckNydus: NotifyLinkNydusClicked(); break;
    case Id::CheckAddon: NotifyLinkAddonClicked(); break;
    }
}

void UnitPropertiesWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( initilizing )
        return;

    switch ( idFrom )
    {
    case Id::EditHp: NotifyHpEditUpdated(); break;
    case Id::EditMp: NotifyMpEditUpdated(); break;
    case Id::EditShields: NotifyShieldEditUpdated(); break;
    case Id::EditResources: NotifyResourcesEditUpdated(); break;
    case Id::EditHangar: NotifyHangarEditUpdated(); break;
    case Id::EditUnitId: NotifyIdEditUpdated(); break;
    case Id::EditXc: NotifyXcEditUpdated(); break;
    case Id::EditYc: NotifyYcEditUpdated(); break;
    case Id::EditValidFieldRawFlags: NotifyValidFieldRawFlagsEditUpdated(); break;
    case Id::EditValidStateRawFlags: NotifyValidStateRawFlagsEditUpdated(); break;
    case Id::EditRawStateFlags: NotifyStateRawFlagsEditUpdated(); break;
    case Id::EditUnused: NotifyUnusedEditUpdated(); break;
    case Id::EditUniqueId: NotifyUniqueIdEditUpdated(); break;
    case Id::EditLinkedId: NotifyLinkedIdEditUpdated(); break;
    case Id::EditLinkRawFlags: NotifyRelationRawFlagsEditUpdated(); break;
    }
}

void UnitPropertiesWindow::NotifyEditFocused(int idFrom, HWND hWndFrom)
{

}

void UnitPropertiesWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{

}

void UnitPropertiesWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    if ( idFrom == Id::ComboPlayer )
        ChangeCurrOwner(dropPlayer.GetPlayerBySelNum());
}

void UnitPropertiesWindow::NotifyComboEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( idFrom == Id::ComboPlayer )
    {
        u8 newPlayer = 0;
        if ( dropPlayer.GetPlayerNum(newPlayer) )
            ChangeCurrOwner(newPlayer);
    }
}

BOOL UnitPropertiesWindow::Activate(WPARAM wParam, LPARAM lParam)
{
    if ( LOWORD(wParam) != WA_INACTIVE )
        chkd.SetCurrDialog(getHandle());

    return FALSE;
}

BOOL UnitPropertiesWindow::ShowWindow(WPARAM wParam, LPARAM lParam)
{
    BOOL result = ClassDialog::DlgProc(getHandle(), WM_SHOWWINDOW, wParam, lParam);
    if ( wParam == TRUE )
        SetFocus(listUnits.getHandle());

    return result;
}

BOOL UnitPropertiesWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case LVN_COLUMNCLICK: LvColumnClicked(nmhdr); break;
    case LVN_ITEMCHANGED: LvItemChanged(nmhdr); break;
    }
    return ClassDialog::DlgNotify(hWnd, idFrom, nmhdr);
}

BOOL UnitPropertiesWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
    case WM_ACTIVATE: return Activate(wParam, lParam); break;
    case WM_SHOWWINDOW: return ShowWindow(wParam, lParam); break;
    case WM_CLOSE: DestroyThis(); break;
    default: return ClassDialog::DlgProc(hWnd, msg, wParam, lParam); break;
    }
    return TRUE;
}
