#include "sprite_properties.h"
#include "ui/chkd_controls/move_to.h"
#include "chkdraft/chkdraft.h"
#include "mapping/undos/chkd_undos/sprite_change.h"
#include "mapping/undos/chkd_undos/sprite_create_del.h"
#include "mapping/undos/chkd_undos/sprite_index_move.h"
#include <CommCtrl.h>
#include <string>

enum class SpriteListColumn { Name, Owner, Xc, Yc, Index };

enum_t(Id, u32, {
    SpriteList = ID_FIRST,
    ComboPlayer,

    ButtonMoveUp = IDC_BUTTON_MOVEUP,
    ButtonMoveTop = IDC_BUTTON_MOVETOP,
    ButtonMoveDown = IDC_BUTTON_MOVEDOWN,
    ButtonMoveEnd = IDC_BUTTON_MOVEEND,
    ButtonDelete = IDC_BUTTON_DELETE,
    ButtonMoveTo = IDC_BUTTON_MOVE_TO,
    ButtonClose = IDC_BUTTON_CLOSE,

    EditSpriteId = IDC_EDIT_ID,
    EditUnused = IDC_EDIT_UNUSED,
    EditXc = IDC_EDIT_XC,
    EditYc = IDC_EDIT_YC,

    CheckUnused0 = IDC_CHECK_UNUSED0,
    CheckUnused1 = IDC_CHECK_UNUSED1,
    CheckUnused2 = IDC_CHECK_UNUSED2,
    CheckUnused3 = IDC_CHECK_UNUSED3,
    CheckUnused4 = IDC_CHECK_UNUSED4,
    CheckUnused5 = IDC_CHECK_UNUSED5,
    CheckUnused6 = IDC_CHECK_UNUSED6,
    CheckUnused7 = IDC_CHECK_UNUSED7,
    CheckUnused8 = IDC_CHECK_UNUSED8,
    CheckUnused9 = IDC_CHECK_UNUSED9,
    CheckUnused10 = IDC_CHECK_UNUSED10,
    CheckUnused11 = IDC_CHECK_UNUSED11,
    CheckPureSprite = IDC_CHECK_PURESPRITE,
    CheckUnitSprite = IDC_CHECK_UNITSPRITE,
    CheckUnused14 = IDC_CHECK_UNUSED14,
    CheckDisabled = IDC_CHECK_DISABLED
});

SpritePropertiesWindow::SpritePropertiesWindow() : columnSortedBy(SpriteListColumn::Index), flipSort(false), initilizing(true), changeHighlightOnly(false)
{

}

SpritePropertiesWindow::~SpritePropertiesWindow()
{

}

bool SpritePropertiesWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_SPRITEPROPERTIES), hParent) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    return false;
}

bool SpritePropertiesWindow::CreateSubWindows(HWND hWnd)
{
    buttonMoveUp.FindThis(hWnd, Id::ButtonMoveUp);
    buttonMoveTop.FindThis(hWnd, Id::ButtonMoveTop);
    buttonMoveDown.FindThis(hWnd, Id::ButtonMoveDown);
    buttonMoveEnd.FindThis(hWnd, Id::ButtonMoveEnd);
    buttonDelete.FindThis(hWnd, Id::ButtonDelete);
    buttonMoveTo.FindThis(hWnd, Id::ButtonMoveTo);

    editSpriteId.FindThis(hWnd, Id::EditSpriteId);
    editUnused.FindThis(hWnd, Id::EditUnused);
    editXc.FindThis(hWnd, Id::EditXc);
    editYc.FindThis(hWnd, Id::EditYc);
    
    checkUnused0.FindThis(hWnd, Id::CheckUnused0);
    checkUnused1.FindThis(hWnd, Id::CheckUnused1);
    checkUnused2.FindThis(hWnd, Id::CheckUnused2);
    checkUnused3.FindThis(hWnd, Id::CheckUnused3);
    checkUnused4.FindThis(hWnd, Id::CheckUnused4);
    checkUnused5.FindThis(hWnd, Id::CheckUnused5);
    checkUnused6.FindThis(hWnd, Id::CheckUnused6);
    checkUnused7.FindThis(hWnd, Id::CheckUnused7);
    checkUnused8.FindThis(hWnd, Id::CheckUnused8);
    checkUnused9.FindThis(hWnd, Id::CheckUnused9);
    checkUnused10.FindThis(hWnd, Id::CheckUnused10);
    checkUnused11.FindThis(hWnd, Id::CheckUnused11);
    checkPureSprite.FindThis(hWnd, Id::CheckPureSprite);
    checkUnitSprite.FindThis(hWnd, Id::CheckUnitSprite);
    checkUnused14.FindThis(hWnd, Id::CheckUnused14);
    checkDisabled.FindThis(hWnd, Id::CheckDisabled);

    dropPlayer.CreateThis(hWnd, 656, 29, 90, 438, Id::ComboPlayer, false);

    initilizing = true;

    editSpriteId.CreateNumberBuddy(0, 65535);
    editUnused.CreateNumberBuddy(0, 255);

    listSprites.CreateThis(hWnd, 9, 10, 549, 449, false, false, Id::SpriteList);
    listSprites.EnableFullRowSelect();
    listSprites.setFont(5, 13, "Tahoma", false);

    listSprites.AddColumn(0, "Sprite Type", 200, LVCFMT_LEFT);
    listSprites.AddColumn(1, "Owner", 100, LVCFMT_LEFT);
    listSprites.AddColumn(2, "X", 75, LVCFMT_RIGHT);
    listSprites.AddColumn(3, "Y", 75, LVCFMT_RIGHT);
    listSprites.AddColumn(4, "Index", 75, LVCFMT_RIGHT);

    RepopulateList();

    listSprites.Show();
    initilizing = false;

    return true;
}

bool SpritePropertiesWindow::DestroyThis()
{
    columnSortedBy = SpriteListColumn::Index; // Reset column sorted by
    return ClassDialog::DestroyDialog();
}

void SpritePropertiesWindow::SetChangeHighlightOnly(bool changeHighlightOnly)
{
    this->changeHighlightOnly = changeHighlightOnly;
}

void SpritePropertiesWindow::ChangeCurrOwner(u8 newOwner)
{
    auto edit = CM->operator()();
    auto undoableChanges = ReversibleActions::Make();
    auto & selSprites = CM->selections.sprites;
    for ( size_t spriteIndex : selSprites )
    {
        const Chk::Sprite & sprite = CM->getSprite(spriteIndex);
        if ( sprite.owner != newOwner ) // If the current and new owners are different
        {
            undoableChanges->Insert(SpriteChange::Make(spriteIndex, sprite));
            edit->sprites[spriteIndex].owner = newOwner;
            ChangeSpritesDisplayedOwner(int(spriteIndex), newOwner);
        }
    }
    CM->AddUndo(undoableChanges);
    CM->Redraw(true);
}

void SpritePropertiesWindow::ChangeDropdownPlayer(u8 newPlayer)
{
    std::string text;
    if ( newPlayer < 12 )
        dropPlayer.SetSel(newPlayer);
    else
        dropPlayer.SetEditNum<u8>(newPlayer + 1);
}

void SpritePropertiesWindow::ChangeSpritesDisplayedOwner(int index, u8 newPlayer)
{
    char owner[32] = {}, padding[2] = { '\0', '\0' };
    if ( newPlayer < 9 )
        padding[0] = '0';
    else
        padding[0] = '\0';

    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, newPlayer+1);
    listSprites.SetItemText(listSprites.GetItemRow(index), (int)SpriteListColumn::Owner, owner);
}

void SpritePropertiesWindow::SetListRedraw(bool allowRedraw)
{
    if ( allowRedraw )
    {
        listSprites.SetRedraw(true);
        ListView_SortItems(listSprites.getHandle(), ForwardCompareLvItems, this);
        listSprites.UpdateWindow();
    }
    else
        listSprites.SetRedraw(false);
}

bool SpritePropertiesWindow::AddSpriteItem(u16 index, const Chk::Sprite & sprite)
{
    char owner[32] = {}, padding[2] = { '\0', '\0' },
        xc[32] = {}, yc[32] = {}, cIndex[32] = {};

    u16 spriteId = (u16)sprite.type;

    if ( sprite.owner < 9 )
        padding[0] = '0';
    else
        padding[0] = '\0';

    std::snprintf(owner, sizeof(owner), "Player %s%i", padding, sprite.owner + 1);
    std::strcpy(xc, std::to_string(sprite.xc).c_str());
    std::strcpy(yc, std::to_string(sprite.yc).c_str());
    std::strcpy(cIndex, std::to_string(index).c_str());

    std::string spriteName = GetSpriteName(sprite.type, sprite.isUnit());

    listSprites.AddRow(4, index);
    listSprites.SetItemText(index, (int)SpriteListColumn::Name, spriteName);
    listSprites.SetItemText(index, (int)SpriteListColumn::Owner, owner);
    listSprites.SetItemText(index, (int)SpriteListColumn::Xc, xc);
    listSprites.SetItemText(index, (int)SpriteListColumn::Yc, yc);
    listSprites.SetItemText(index, (int)SpriteListColumn::Index, index);
    return true;
}

void SpritePropertiesWindow::FocusAndSelectIndex(u16 spriteIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = spriteIndex;
    s32 lvIndex = ListView_FindItem(listSprites.getHandle(), -1, &findInfo);
    ListView_SetItemState(listSprites.getHandle(), lvIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void SpritePropertiesWindow::DeselectIndex(u16 spriteIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = spriteIndex;

    int lvIndex = ListView_FindItem(listSprites.getHandle(), -1, &findInfo);
    ListView_SetItemState(listSprites.getHandle(), lvIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
}

void SpritePropertiesWindow::UpdateEnabledState()
{
    if ( CM->selections.hasSprites() )
    {
        EnableSpriteEditing();
        const Chk::Sprite & sprite = CM->getSprite(CM->selections.getFirstSprite());
        SetSpriteFieldText(sprite);
    }
    else
        DisableSpriteEditing();
}

void SpritePropertiesWindow::RepopulateList()
{
    initilizing = true;
    SetListRedraw(false);

    listSprites.DeleteAllItems();
    if ( CM != nullptr )
    {
        Selections & selections = CM->selections;

        size_t numSprites = CM->numSprites();
        for ( size_t i = 0; i < numSprites; i++ )
        {
            const Chk::Sprite sprite = CM->getSprite(i);
            AddSpriteItem((u16)i, sprite);
        }

        if ( selections.hasSprites() )
        {
            auto selectedIndex = selections.getFirstSprite();
            listSprites.FocusItem(int(selectedIndex));

            auto & selSprites = selections.sprites;
            for ( auto & spriteIndex : selSprites )
                listSprites.SelectRow(int(spriteIndex));

            EnableSpriteEditing();
            const Chk::Sprite & sprite = CM->getSprite(selectedIndex);
            SetSpriteFieldText(sprite);

            auto spriteName = GetSpriteName(sprite.type, sprite.isUnit());
            WindowsItem::SetWinText(spriteName);

            int row = listSprites.GetItemRow(int(selections.getHighestSpriteIndex()));
            listSprites.EnsureVisible(row, false);
            row = listSprites.GetItemRow(int(selections.getLowestSpriteIndex()));
            listSprites.EnsureVisible(row, false);
        }
    }

    SetListRedraw(true);
    initilizing = false;
}

std::string SpritePropertiesWindow::GetSpriteName(Sc::Sprite::Type type, bool isUnit)
{
    if ( isUnit && size_t(type) < Sc::Unit::TotalTypes )
        return std::string("[") + std::to_string(type) + "] " + *CM->getUnitName<ChkdString>(Sc::Unit::Type(type));
    else if ( (isUnit && size_t(type) >= Sc::Unit::TotalTypes) || size_t(type) > chkd.scData.sprites.spriteNames.size() )
        return std::string("[") + std::to_string(type) + "]";
    else
        return std::string("[") + std::to_string(type) + "] " + chkd.scData.sprites.spriteNames[type];
}

void SpritePropertiesWindow::EnableSpriteEditing()
{
    dropPlayer.EnableThis();

    WinLib::ButtonControl* buttonControls[] = { &buttonMoveUp, &buttonMoveTop, &buttonMoveDown, &buttonMoveEnd, &buttonDelete, &buttonMoveTo };
    WinLib::EditControl* editControls[] = { &editSpriteId, &editUnused, &editXc, &editYc };
    WinLib::CheckBoxControl* checkControls[] = { &checkUnused0, &checkUnused1, &checkUnused2, &checkUnused3, &checkUnused4, &checkUnused5, &checkUnused6,
        &checkUnused7, &checkUnused8, &checkUnused9, &checkUnused10, &checkUnused11, &checkPureSprite, &checkUnitSprite, &checkUnused14, &checkDisabled};
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

    const Chk::Sprite & sprite = CM->getSprite(CM->selections.getFirstSprite());
    SetSpriteFieldText(sprite);
}

void SpritePropertiesWindow::DisableSpriteEditing()
{
    WindowsItem::SetWinText("");

    dropPlayer.SetSel(-1);
    dropPlayer.DisableThis();

    WinLib::ButtonControl* buttonControls[] = { &buttonMoveUp, &buttonMoveTop, &buttonMoveDown, &buttonMoveEnd, &buttonDelete, &buttonMoveTo };
    WinLib::EditControl* editControls[] = { &editSpriteId, &editUnused, &editXc, &editYc };
    WinLib::CheckBoxControl* checkControls[] = { &checkUnused0, &checkUnused1, &checkUnused2, &checkUnused3, &checkUnused4, &checkUnused5, &checkUnused6,
        &checkUnused7, &checkUnused8, &checkUnused9, &checkUnused10, &checkUnused11, &checkPureSprite, &checkUnitSprite, &checkUnused14, &checkDisabled};
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
}

void SpritePropertiesWindow::SetSpriteFieldText(const Chk::Sprite & sprite)
{
    initilizing = true;

    if ( sprite.owner < 12 )
        dropPlayer.SetSel(sprite.owner);
    else
        dropPlayer.SetWinText(std::to_string(sprite.owner + 1));

    editSpriteId.SetText(std::to_string((u16)sprite.type));
    editUnused.SetText(std::to_string(int(sprite.unused)));
    editXc.SetText(std::to_string(sprite.xc));
    editYc.SetText(std::to_string(sprite.yc));
    
    checkUnused0.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitZero) == Chk::Sprite::SpriteFlags::BitZero);
    checkUnused1.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitOne) == Chk::Sprite::SpriteFlags::BitOne);
    checkUnused2.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitTwo) == Chk::Sprite::SpriteFlags::BitTwo);
    checkUnused3.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitThree) == Chk::Sprite::SpriteFlags::BitThree);
    checkUnused4.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitFour) == Chk::Sprite::SpriteFlags::BitFour);
    checkUnused5.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitFive) == Chk::Sprite::SpriteFlags::BitFive);
    checkUnused6.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitSix) == Chk::Sprite::SpriteFlags::BitSix);
    checkUnused7.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitSeven) == Chk::Sprite::SpriteFlags::BitSeven);
    checkUnused8.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitEight) == Chk::Sprite::SpriteFlags::BitEight);
    checkUnused9.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitNine) == Chk::Sprite::SpriteFlags::BitNine);
    checkUnused10.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitTen) == Chk::Sprite::SpriteFlags::BitTen);
    checkUnused11.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::BitEleven) == Chk::Sprite::SpriteFlags::BitEleven);
    checkPureSprite.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::DrawAsSprite) == Chk::Sprite::SpriteFlags::DrawAsSprite);
    checkUnitSprite.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::IsUnit) == Chk::Sprite::SpriteFlags::IsUnit);
    checkUnused14.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated) == Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated);
    checkDisabled.SetCheck((sprite.flags & Chk::Sprite::SpriteFlags::SpriteUnitDiabled) == Chk::Sprite::SpriteFlags::SpriteUnitDiabled);

    initilizing = false;
}

void SpritePropertiesWindow::SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2)
{
    int row1 = listSprites.GetItemRow((int)index1),
        row2 = listSprites.GetItemRow((int)index2);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row1;
    item.lParam = index2;

    ListView_SetItem(hListView, &item);
    listSprites.SetItemText(row1, (int)SpriteListColumn::Index, (int)index2);

    item.iItem = row2;
    item.lParam = index1;

    ListView_SetItem(hListView, &item);
    listSprites.SetItemText(row2, (int)SpriteListColumn::Index, (int)index1);
}

void SpritePropertiesWindow::ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam)
{
    int row = listSprites.GetItemRow((int)oldLParam);

    LVITEM item = { };
    item.mask = LVIF_PARAM;
    item.iItem = row;
    item.lParam = newLParam;

    ListView_SetItem(hListView, &item);

    listSprites.SetItemText(row, (int)SpriteListColumn::Index, (int)newLParam);
}

int SpritePropertiesWindow::CompareLvItems(LPARAM index1, LPARAM index2)
{
    int sort = 0;

    if ( columnSortedBy != SpriteListColumn::Index )
    {
        const Chk::Sprite & firstSprite = CM->getSprite((u16)index1);
        const Chk::Sprite & secondSprite = CM->getSprite((u16)index2);

        switch ( columnSortedBy )
        {
        case SpriteListColumn::Name: // Sort by ID / type
            if ( (u16)firstSprite.type < (u16)secondSprite.type )
                sort = -1;
            else if ( (u16)firstSprite.type > (u16)secondSprite.type )
                sort = 1;
            break;
        case SpriteListColumn::Owner: // Sort by owner
            if ( firstSprite.owner < secondSprite.owner )
                sort = -1;
            else if ( firstSprite.owner > secondSprite.owner )
                sort = 1;
            break;
        case SpriteListColumn::Xc: // Sort by xc
            if ( firstSprite.xc < secondSprite.xc )
                sort = -1;
            else if ( firstSprite.xc > secondSprite.xc )
                sort = 1;
            else if ( firstSprite.yc < secondSprite.yc ) // Try to sort by yc if xc's are equal
                sort = -1;
            else if ( firstSprite.yc > secondSprite.yc )
                sort = 1;
            break;
        case SpriteListColumn::Yc: // Sort by yc
            if ( firstSprite.yc < secondSprite.yc )
                sort = -1;
            else if ( firstSprite.yc > secondSprite.yc )
                sort = 1;
            else if ( firstSprite.xc < secondSprite.xc ) // Try to sort by xc if yc's are equal
                sort = -1;
            else if ( firstSprite.xc > secondSprite.xc )
                sort = 1;
            break;
        }
    }
    else if ( columnSortedBy == SpriteListColumn::Index )
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

void SpritePropertiesWindow::LvColumnClicked(NMHDR* nmhdr)
{
    SpriteListColumn newColumn = (SpriteListColumn)((NMLISTVIEW*)nmhdr)->iSubItem;
    if ( newColumn == columnSortedBy )
        flipSort = !flipSort;
    else
        flipSort = false;

    columnSortedBy = newColumn;
    ListView_SortItems(nmhdr->hwndFrom, ForwardCompareLvItems, this);
}

void SpritePropertiesWindow::LvItemChanged(NMHDR* nmhdr)
{
    Selections & selections = CM->selections;
    preservedStats.convertToUndo();
    if ( !changeHighlightOnly )
    {
        NMLISTVIEW* itemInfo = (NMLISTVIEW*)nmhdr;
        u16 index = (u16)itemInfo->lParam;

        if ( itemInfo->uNewState & LVIS_SELECTED && initilizing == false ) // Selected
                                                                           // Add item to selection
        {
            bool firstSelected = !selections.hasSprites();
            selections.addSprite(index);

            if ( firstSelected )
                EnableSpriteEditing();

            const Chk::Sprite & sprite = CM->getSprite(index);
            auto spriteName = GetSpriteName(sprite.type, sprite.isUnit());
            WindowsItem::SetWinText(spriteName);
            SetSpriteFieldText(sprite);

            CM->viewSprite(index);
        }
        else if ( itemInfo->uOldState & LVIS_SELECTED ) // From selected to not selected
                                                        // Remove item from selection
        {
            selections.removeSprite(index);

            if ( !selections.hasSprites()
                && !(GetKeyState(VK_DOWN) & 0x8000
                    || GetKeyState(VK_UP) & 0x8000
                    || GetKeyState(VK_LEFT) & 0x8000
                    || GetKeyState(VK_RIGHT) & 0x8000
                    || GetKeyState(VK_LBUTTON) & 0x8000
                    || GetKeyState(VK_RBUTTON) & 0x8000) )
            {
                DisableSpriteEditing();
            }

            if ( selections.hasSprites() )
                CM->viewSprite(selections.getFirstSprite());
            else
                CM->Redraw(false);
        }
    }
}

void SpritePropertiesWindow::NotifyClosePressed()
{
    EndDialog(getHandle(), IDCLOSE);
}

void SpritePropertiesWindow::NotifyMoveTopPressed()
{
    Selections & selections = CM->selections;

    u16 spriteStackTopIndex = u16(selections.getFirstSprite());
    selections.sortSprites(true); // sort with lowest indexes first

    listSprites.SetRedraw(false);

    auto spriteChanges = ReversibleActions::Make();
    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    u16 i = 0;
    auto & selSprites = selections.sprites;
    for ( size_t & spriteIndex : selSprites )
    {
        if ( spriteIndex != 0 ) // If sprite is not at the destination index and spriteptr can be retrieved
        {
            Chk::Sprite preserve = CM->getSprite(spriteIndex); // Preserve the sprite info
            CM->deleteSprite(spriteIndex);
            CM->insertSprite(i, preserve);
            spriteChanges->Insert(SpriteIndexMove::Make(u16(spriteIndex), i));
            if ( spriteIndex == spriteStackTopIndex )
                spriteStackTopIndex = i;

            spriteIndex = i; // Modify the index that denotes sprite selection
        }
        i++;
    }

    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    if ( spriteChanges->Count() > 2 )
        CM->AddUndo(spriteChanges);

    selections.ensureSpriteFirst(spriteStackTopIndex);
    RepopulateList();
}

void SpritePropertiesWindow::NotifyMoveEndPressed()
{
    Selections & selections = CM->selections;

    u16 spriteStackTopIndex = u16(selections.getFirstSprite());
    selections.sortSprites(false); // Highest First

    listSprites.SetRedraw(false);
    size_t numSprites = CM->numSprites();
    u16 numSpritesSelected = selections.numSprites();

    u16 i = 1;
    auto spriteChanges = ReversibleActions::Make();
    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    auto & selSprites = selections.sprites;
    for ( size_t & spriteIndex : selSprites )
    {
        if ( spriteIndex != numSprites - 1 )
        {
            Chk::Sprite preserve = CM->getSprite(spriteIndex);
            CM->deleteSprite(spriteIndex);
            CM->insertSprite(numSprites - i, preserve);
            spriteChanges->Insert(SpriteIndexMove::Make(u16(spriteIndex), u16(numSprites - i)));

            if ( spriteIndex == spriteStackTopIndex )
                spriteStackTopIndex = u16(numSprites - i);

            spriteIndex = u16(numSprites - i);
        }
        i++;
    }

    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    if ( spriteChanges->Count() > 2 )
        CM->AddUndo(spriteChanges);

    selections.ensureSpriteFirst(spriteStackTopIndex);
    RepopulateList();
}

void SpritePropertiesWindow::NotifyMoveUpPressed()
{
    Selections & selections = CM->selections;
    HWND hSpriteList = listSprites.getHandle();

    selections.sortSprites(true);
    listSprites.SetRedraw(false);

    auto spriteChanges = ReversibleActions::Make();
    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    auto & selSprites = selections.sprites;
    for ( size_t & spriteIndex : selSprites )
    {
        if ( spriteIndex > 0 && !selections.spriteIsSelected(spriteIndex - 1) )
        {
            CM->moveSprite(spriteIndex, spriteIndex-1);
            spriteChanges->Insert(SpriteIndexMove::Make(u16(spriteIndex), u16(spriteIndex - 1)));
            SwapIndexes(hSpriteList, spriteIndex, spriteIndex - 1);
            spriteIndex--;
        }
    }

    ListView_SortItems(hSpriteList, ForwardCompareLvItems, this);
    int row = listSprites.GetItemRow(int(selections.getHighestSpriteIndex()));
    listSprites.EnsureVisible(row, false);
    row = listSprites.GetItemRow(int(selections.getLowestSpriteIndex()));
    listSprites.EnsureVisible(row, false);
    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());

    if ( spriteChanges->Count() > 2 )
        CM->AddUndo(spriteChanges);

    listSprites.SetRedraw(true);
}

void SpritePropertiesWindow::NotifyMoveDownPressed()
{
    Selections & selections = CM->selections;
    HWND hSpriteList = listSprites.getHandle();

    selections.sortSprites(false);
    listSprites.SetRedraw(false);

    auto spriteChanges = ReversibleActions::Make();
    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    auto & selSprites = selections.sprites;
    for ( size_t & spriteIndex : selSprites )
    {
        if ( size_t(spriteIndex+1) < CM->numSprites() && !selections.spriteIsSelected(spriteIndex + 1) )
        {
            CM->moveSprite(spriteIndex, spriteIndex+1);
            spriteChanges->Insert(SpriteIndexMove::Make(u16(spriteIndex), u16(spriteIndex + 1)));
            SwapIndexes(hSpriteList, spriteIndex, spriteIndex + 1);
            spriteIndex++;
        }
    }

    spriteChanges->Insert(SpriteIndexMoveBoundary::Make());
    if ( spriteChanges->Count() > 2 )
        CM->AddUndo(spriteChanges);

    ListView_SortItems(hSpriteList, ForwardCompareLvItems, this);
    int row = listSprites.GetItemRow(int(selections.getLowestSpriteIndex()));
    listSprites.EnsureVisible(row, false);
    row = listSprites.GetItemRow(int(selections.getHighestSpriteIndex()));
    listSprites.EnsureVisible(row, false);
    listSprites.SetRedraw(true);
}

void SpritePropertiesWindow::NotifyMoveToPressed()
{
    u32 spriteMoveTo = 0;
    if ( MoveToDialog<u32>::GetIndex(spriteMoveTo, getHandle()) && spriteMoveTo < u32(CM->numSprites()) )
    {
        if ( spriteMoveTo == 0 )
        {
            NotifyMoveTopPressed();
        }
        else if ( spriteMoveTo + CM->selections.numSprites() >= CM->numSprites() )
        {
            NotifyMoveEndPressed();
        }
        else if ( spriteMoveTo > 0 )
        {
            Selections & selections = CM->selections;
            u16 numSpritesSelected = selections.numSprites();
            size_t limit = CM->numSprites() - 1;

            u16 spriteStackTopIndex = u16(selections.getFirstSprite());
            u16 numSprites = selections.numSprites(),
                shift = numSprites - 1;

            selections.sortSprites(false); // Highest First
            listSprites.SetRedraw(false);

            std::vector<Chk::Sprite> selectedSprites{size_t(numSpritesSelected)};
            auto spriteCreateDels = ReversibleActions::Make();
            u16 i = 0;
            auto & selSprites = selections.sprites;
            for ( size_t & spriteIndex : selSprites )
            { // Remove each selected sprite from the map, store in selectedSprites
                u32 loc = ((u32)spriteIndex)*sizeof(Chk::Sprite);
                selectedSprites[shift - i] = CM->getSprite(spriteIndex);
                CM->deleteSprite(spriteIndex);
                spriteCreateDels->Insert(SpriteCreateDel::Make((u16)spriteIndex, selectedSprites[shift - i]));
                spriteIndex = u16(spriteMoveTo + shift - i);
                i++;
            }

            for ( int i = 0; i < numSprites; i++ )
            {
                CM->insertSprite(spriteMoveTo + i, selectedSprites[i]);
                spriteCreateDels->Insert(SpriteCreateDel::Make(spriteMoveTo + i));
            }

            selections.finishSpriteMove();
            selections.ensureSpriteFirst(spriteStackTopIndex);
            CM->AddUndo(spriteCreateDels);
            RepopulateList();
        }
    }
}

void SpritePropertiesWindow::NotifyDeletePressed()
{
    Selections & selections = CM->selections;
    HWND hSpriteList = listSprites.getHandle();
    listSprites.SetRedraw(false);
    auto spriteDeletes = ReversibleActions::Make();
    while ( selections.hasSprites() )
    {
        u16 index = u16(selections.getHighestSpriteIndex());
        selections.removeSprite(index);
        listSprites.RemoveRow(index);

        int row = listSprites.GetItemRow(index);

        const Chk::Sprite & sprite = CM->getSprite(index);
        spriteDeletes->Insert(SpriteCreateDel::Make(index, sprite));

        CM->deleteSprite(index);

        for ( size_t i = index + 1; i <= CM->numSprites(); i++ )
            ChangeIndex(hSpriteList, i, i - 1);
    }
    CM->AddUndo(spriteDeletes);
    CM->Redraw(true);
    listSprites.SetRedraw(true);
}

void SpritePropertiesWindow::NotifyCheckClicked(u32 idFrom)
{
    auto edit = CM->operator()();
    auto spriteChanges = ReversibleActions::Make();
    auto & selSprites = CM->selections.sprites;
    for ( size_t & spriteIndex : selSprites )
    {
        const Chk::Sprite & sprite = CM->getSprite(spriteIndex);
        spriteChanges->Insert(SpriteChange::Make(spriteIndex, sprite));

        WinLib::CheckBoxControl* checkControl = nullptr;
        Chk::Sprite::SpriteFlags flag = Chk::Sprite::SpriteFlags(0);
        switch ( idFrom )
        {
        case Id::CheckUnused0: checkControl = &checkUnused0; flag = Chk::Sprite::SpriteFlags::BitZero; break;
        case Id::CheckUnused1: checkControl = &checkUnused1; flag = Chk::Sprite::SpriteFlags::BitOne; break;
        case Id::CheckUnused2: checkControl = &checkUnused2; flag = Chk::Sprite::SpriteFlags::BitTwo; break;
        case Id::CheckUnused3: checkControl = &checkUnused3; flag = Chk::Sprite::SpriteFlags::BitThree; break;
        case Id::CheckUnused4: checkControl = &checkUnused4; flag = Chk::Sprite::SpriteFlags::BitFour; break;
        case Id::CheckUnused5: checkControl = &checkUnused5; flag = Chk::Sprite::SpriteFlags::BitFive; break;
        case Id::CheckUnused6: checkControl = &checkUnused6; flag = Chk::Sprite::SpriteFlags::BitSix; break;
        case Id::CheckUnused7: checkControl = &checkUnused7; flag = Chk::Sprite::SpriteFlags::BitSeven; break;
        case Id::CheckUnused8: checkControl = &checkUnused8; flag = Chk::Sprite::SpriteFlags::BitEight; break;
        case Id::CheckUnused9: checkControl = &checkUnused9; flag = Chk::Sprite::SpriteFlags::BitNine; break;
        case Id::CheckUnused10: checkControl = &checkUnused10; flag = Chk::Sprite::SpriteFlags::BitTen; break;
        case Id::CheckUnused11: checkControl = &checkUnused11; flag = Chk::Sprite::SpriteFlags::BitEleven; break;
        case Id::CheckPureSprite: checkControl = &checkPureSprite; flag = Chk::Sprite::SpriteFlags::DrawAsSprite; break;
        case Id::CheckUnitSprite: checkControl = &checkUnitSprite; flag = Chk::Sprite::SpriteFlags::IsUnit; break;
        case Id::CheckUnused14: checkControl = &checkUnused14; flag = Chk::Sprite::SpriteFlags::OverlayFlipped_Deprecated; break;
        case Id::CheckDisabled: checkControl = &checkDisabled; flag = Chk::Sprite::SpriteFlags::SpriteUnitDiabled; break;
        }

        if ( checkControl->isChecked() )
            edit->sprites[spriteIndex].flags |= flag;
        else
            edit->sprites[spriteIndex].flags &= ~flag;
    }
    CM->AddUndo(spriteChanges);
}

void SpritePropertiesWindow::NotifyIdEditUpdated()
{
    auto edit = CM->operator()();
    u16 spriteID = 0;
    if ( editSpriteId.GetEditNum<u16>(spriteID) )
    {
        auto & selSprites = CM->selections.sprites;
        for ( size_t & spriteIndex : selSprites )
        {
            edit->sprites[spriteIndex].type = (Sc::Sprite::Type)spriteID;
            int row = listSprites.GetItemRow(int(spriteIndex));

            auto spriteName = GetSpriteName(Sc::Sprite::Type(spriteID), CM->getSprite(spriteIndex).isUnit());
            listSprites.SetItemText(row, (int)SpriteListColumn::Name, spriteName);

            if ( spriteIndex == CM->selections.getFirstSprite() )
                WindowsItem::SetWinText(spriteName);
        }
        CM->Redraw(true);
        ListView_SortItems(listSprites.getHandle(), ForwardCompareLvItems, this);
    }
}

void SpritePropertiesWindow::NotifyUnusedEditUpdated()
{
    auto edit = CM->operator()();
    u8 unused = 0;
    if ( editUnused.GetEditNum<u8>(unused) )
    {
        auto & selSprites = CM->selections.sprites;
        for ( size_t & spriteIndex : selSprites )
            edit->sprites[spriteIndex].unused = unused;

        CM->Redraw(false);
    }
}

void SpritePropertiesWindow::NotifyXcEditUpdated()
{
    auto edit = CM->operator()();
    u16 spriteXC = 0;
    if ( editXc.GetEditNum<u16>(spriteXC) )
    {
        auto & selSprites = CM->selections.sprites;
        for ( size_t & spriteIndex : selSprites )
        {
            edit->sprites[spriteIndex].xc = spriteXC;
            int row = listSprites.GetItemRow(int(spriteIndex));
            listSprites.SetItemText(row, (int)SpriteListColumn::Xc, spriteXC);
        }
        CM->Redraw(true);
        ListView_SortItems(listSprites.getHandle(), ForwardCompareLvItems, this);
    }
}

void SpritePropertiesWindow::NotifyYcEditUpdated()
{
    auto edit = CM->operator()();
    u16 spriteYC = 0;
    if ( editYc.GetEditNum<u16>(spriteYC) )
    {
        auto & selSprites = CM->selections.sprites;
        for ( size_t & spriteIndex : selSprites )
        {
            edit->sprites[spriteIndex].yc = spriteYC;
            int row = listSprites.GetItemRow(int(spriteIndex));
            listSprites.SetItemText(row, (int)SpriteListColumn::Yc, spriteYC);
        }
        CM->Redraw(true);
        ListView_SortItems(listSprites.getHandle(), ForwardCompareLvItems, this);
    }
}

void SpritePropertiesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ButtonClose: NotifyClosePressed(); break;
    case Id::ButtonMoveTop: NotifyMoveTopPressed(); break;
    case Id::ButtonMoveEnd: NotifyMoveEndPressed(); break;
    case Id::ButtonMoveUp: NotifyMoveUpPressed(); break;
    case Id::ButtonMoveDown: NotifyMoveDownPressed(); break;
    case Id::ButtonMoveTo: NotifyMoveToPressed(); break;
    case Id::ButtonDelete: NotifyDeletePressed(); break;
    case Id::CheckUnused0: case Id::CheckUnused1: case Id::CheckUnused2: case Id::CheckUnused3:
    case Id::CheckUnused4: case Id::CheckUnused5: case Id::CheckUnused6: case Id::CheckUnused7:
    case Id::CheckUnused8: case Id::CheckUnused9: case Id::CheckUnused10: case Id::CheckUnused11:
    case Id::CheckPureSprite: case Id::CheckUnitSprite: case Id::CheckUnused14: case Id::CheckDisabled:
        NotifyCheckClicked(u32(idFrom));
        break;
    }
}

void SpritePropertiesWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( initilizing )
        return;

    switch ( idFrom )
    {
    case Id::EditSpriteId: NotifyIdEditUpdated(); break;
    case Id::EditUnused: NotifyUnusedEditUpdated(); break;
    case Id::EditXc: NotifyXcEditUpdated(); break;
    case Id::EditYc: NotifyYcEditUpdated(); break;
    }
}

void SpritePropertiesWindow::NotifyEditFocused(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::EditSpriteId: preservedStats.AddStats(CM->selections, Chk::Sprite::Field::Type); break;
    case Id::EditUnused: preservedStats.AddStats(CM->selections, Chk::Sprite::Field::Unused); break;
    case Id::EditXc: preservedStats.AddStats(CM->selections, Chk::Sprite::Field::Xc); break;
    case Id::EditYc: preservedStats.AddStats(CM->selections, Chk::Sprite::Field::Yc); break;
    }
}

void SpritePropertiesWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    if ( idFrom == Id::EditSpriteId || idFrom == Id::EditXc || idFrom == Id::EditYc )
        preservedStats.convertToUndo();
}

void SpritePropertiesWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    if ( idFrom == Id::ComboPlayer )
        ChangeCurrOwner(dropPlayer.GetPlayerBySelNum());
}

void SpritePropertiesWindow::NotifyComboEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( idFrom == Id::ComboPlayer )
    {
        u8 newPlayer = 0;
        if ( dropPlayer.GetPlayerNum(newPlayer) )
            ChangeCurrOwner(newPlayer);
    }
}

BOOL SpritePropertiesWindow::Activate(WPARAM wParam, LPARAM lParam)
{
    if ( LOWORD(wParam) != WA_INACTIVE )
        chkd.SetCurrDialog(getHandle());

    return FALSE;
}

BOOL SpritePropertiesWindow::ShowWindow(WPARAM wParam, LPARAM lParam)
{
    BOOL result = ClassDialog::DlgProc(getHandle(), WM_SHOWWINDOW, wParam, lParam);
    if ( wParam == TRUE )
        SetFocus(listSprites.getHandle());

    return result;
}

BOOL SpritePropertiesWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case LVN_COLUMNCLICK: LvColumnClicked(nmhdr); break;
    case LVN_ITEMCHANGED: LvItemChanged(nmhdr); break;
    }
    return ClassDialog::DlgNotify(hWnd, idFrom, nmhdr);
}

BOOL SpritePropertiesWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
