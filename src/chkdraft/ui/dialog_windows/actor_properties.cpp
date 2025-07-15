#include "actor_properties.h"
#include "chkdraft/chkdraft.h"
#include <CommCtrl.h>

enum class ActorListColumn { DrawListIndex, Type, UnitSpriteIndex, Priority, Name };

enum_t(Id, u32, {
    ActorList = ID_FIRST,

    ButtonPausePlay = IDC_BUTTON_PAUSEPLAY,
    CheckAutoRestart = IDC_CHECK_AUTORESTART,
    EditActorDirection = IDC_EDIT_ACTORDIRECTION,
    EditElevation = IDC_EDIT_ELEVATION,
    ComboAnimation = IDC_COMBO_ANIMATION,
    ListboxImages = IDC_LISTBOX_IMAGES,
    CheckPrimaryImage = IDC_CHECK_PRIMARYIMAGE,

    EditImageId = IDC_EDIT_IMAGEID,
    EditIscriptId = IDC_EDIT_ISCRIPTID,
    EditOwner = IDC_EDIT_OWNER,
    EditImageDirection = IDC_EDIT_IMAGEDIRECTION,
    EditImageX = IDC_EDIT_IMAGEX,
    EditImageY = IDC_EDIT_IMAGEY,
    EditOffsetX = IDC_EDIT_OFFSETX,
    EditOffsetY = IDC_EDIT_OFFSETY,
    EditBaseFrame = IDC_EDIT_BASEFRAME,
    EditGrpFrame = IDC_EDIT_GRPFRAME,
    CheckFlipped = IDC_CHECK_FLIPPED,
    CheckRotationEnabled = IDC_CHECK_ROTATIONENABLED,
    CheckHidden = IDC_CHECK_HIDDEN,
    CheckDrawIfCloaked = IDC_CHECK_DRAWIFCLOAKED,
    ComboRemapping = IDC_COMBO_REMAPPING,
    ComboSelectionColor = IDC_COMBO_SELECTIONCOLOR,
    ComboDrawFunction = IDC_COMBO_DRAWFUNCTION,

    ButtonClose = IDC_BUTTON_CLOSE
});

ActorPropertiesWindow::ActorPropertiesWindow() : columnSortedBy(ActorListColumn::Priority), flipSort(false), initilizing(true)
{

}

ActorPropertiesWindow::~ActorPropertiesWindow()
{

}

bool ActorPropertiesWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_ACTORPROPERTIES), hParent) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    return false;
}

bool ActorPropertiesWindow::CreateSubWindows(HWND hWnd)
{
    buttonPausePlayAnimation.FindThis(hWnd, Id::ButtonPausePlay);
    checkAutoRestart.FindThis(hWnd, Id::CheckAutoRestart);
    editActorDirection.FindThis(hWnd, Id::EditActorDirection);
    editElevation.FindThis(hWnd, Id::EditElevation);
    dropAnimation.FindThis(hWnd, Id::ComboAnimation);
    checkPrimaryImage.FindThis(hWnd, Id::CheckPrimaryImage);

    editImageId.FindThis(hWnd, Id::EditImageId);
    editIscriptId.FindThis(hWnd, Id::EditIscriptId);
    editOwner.FindThis(hWnd, Id::EditOwner);
    editImageDirection.FindThis(hWnd, Id::EditImageDirection);
    editImageX.FindThis(hWnd, Id::EditImageX);
    editImageY.FindThis(hWnd, Id::EditImageY);
    editOffsetX.FindThis(hWnd, Id::EditOffsetX);
    editOffsetY.FindThis(hWnd, Id::EditOffsetY);
    editBaseFrame.FindThis(hWnd, Id::EditBaseFrame);
    editGrpFrame.FindThis(hWnd, Id::EditGrpFrame);
    checkFlipped.FindThis(hWnd, Id::CheckFlipped);
    checkRotationEnabled.FindThis(hWnd, Id::CheckRotationEnabled);
    checkHidden.FindThis(hWnd, Id::CheckHidden);
    checkDrawIfCloaked.FindThis(hWnd, Id::CheckDrawIfCloaked);
    dropRemapping.FindThis(hWnd, Id::ComboRemapping);
    dropSelectionColor.FindThis(hWnd, Id::ComboSelectionColor);
    dropDrawFunction.FindThis(hWnd, Id::ComboDrawFunction);

    listboxImages.CreateThis(hWnd, 584, 230, 167, 175, true, false, false, false, false, Id::ListboxImages);

    editActorDirection.CreateNumberBuddy(0, 255);
    editElevation.CreateNumberBuddy(0, 255);

    editImageId.CreateNumberBuddy(0, 65535);
    editIscriptId.CreateNumberBuddy(0, 65535);
    editOwner.CreateNumberBuddy(0, 255);
    editImageDirection.CreateNumberBuddy(0, 255);
    editImageX.CreateNumberBuddy(-2147483648, 2147483647);
    editImageY.CreateNumberBuddy(-2147483648, 2147483647);
    editOffsetX.CreateNumberBuddy(-128, 127);
    editOffsetY.CreateNumberBuddy(-128, 127);
    editBaseFrame.CreateNumberBuddy(0, 65535);
    editGrpFrame.CreateNumberBuddy(0, 65535);

    initilizing = true;
    dropAnimation.SetHeight(438);
    for ( std::size_t i=0; i<std::size(Sc::Sprite::AnimName); ++i )
        dropAnimation.AddItem(std::string(Sc::Sprite::AnimName[i]), i);
    
    dropRemapping.SetHeight(438);
    dropSelectionColor.SetHeight(438);
    dropDrawFunction.SetHeight(438);
    dropRemapping.AddItem("ofire", 0);
    dropRemapping.AddItem("gfire", 1);
    dropRemapping.AddItem("bfire", 2);
    dropRemapping.AddItem("bexpl", 3);
    dropRemapping.AddItem("trans50", 4);
    dropRemapping.AddItem("red", 5);
    dropRemapping.AddItem("green", 6);
    dropSelectionColor.AddItem("0", 0);
    dropSelectionColor.AddItem("1", 0);
    dropSelectionColor.AddItem("2", 0);
    for ( std::size_t i=0; i<std::size(MapImage::drawFunctionNames); ++i )
        dropDrawFunction.AddItem(std::string(MapImage::drawFunctionNames[i]), i);

    WindowsItem::SetWinText("Actor Properties");

    listActors.CreateThis(hWnd, 9, 10, 549, 449, false, false, Id::ActorList);
    listActors.EnableFullRowSelect();
    listActors.setFont(5, 13, "Tahoma", false);
    
    listActors.AddColumn(0, "Draw List Index", 90, LVCFMT_RIGHT);
    listActors.AddColumn(1, "Type", 65, LVCFMT_LEFT);
    listActors.AddColumn(2, "Unit/Sprite Index", 95, LVCFMT_RIGHT);
    listActors.AddColumn(3, "Draw Priority", 120, LVCFMT_RIGHT);
    listActors.AddColumn(4, "Name", 150, LVCFMT_LEFT);

    DisableActorEditing();
    RepopulateList();

    listActors.Show();
    initilizing = false;

    return true;
}

bool ActorPropertiesWindow::DestroyThis()
{
    columnSortedBy = ActorListColumn::Priority; // Reset column sorted by
    return ClassDialog::DestroyDialog();
}

void ActorPropertiesWindow::SetListRedraw(bool redraw)
{
    if ( redraw )
    {
        listActors.SetRedraw(true);
        ListView_SortItems(listActors.getHandle(), ForwardCompareLvItems, this);
        listActors.UpdateWindow();
    }
    else
        listActors.SetRedraw(false);
}

bool ActorPropertiesWindow::AddActor(bool isUnit, bool isSpriteUnit, std::size_t unitOrSpriteType, std::size_t unitOrSpriteIndex, const MapActor & actor)
{
    std::string actorName = (isUnit || isSpriteUnit) ?
        Sc::Unit::defaultDisplayNames[unitOrSpriteType < Sc::Unit::defaultDisplayNames.size() ? unitOrSpriteType : 0] :
        chkd.scData.sprites.spriteNames[unitOrSpriteType < chkd.scData.sprites.spriteNames.size() ? unitOrSpriteType : 0];

    listActors.AddRow(4, actor.drawListIndex);
    listActors.SetItemText(actor.drawListIndex, (int)ActorListColumn::DrawListIndex, std::to_string(actor.drawListIndex));
    listActors.SetItemText(actor.drawListIndex, (int)ActorListColumn::Type, isUnit ? "Unit" : (isSpriteUnit ? "Sprite Unit" : "Sprite"));
    listActors.SetItemText(actor.drawListIndex, (int)ActorListColumn::UnitSpriteIndex, std::to_string(unitOrSpriteIndex));
    listActors.SetItemText(actor.drawListIndex, (int)ActorListColumn::Priority, std::to_string(CM->animations.drawList[actor.drawListIndex]));
    listActors.SetItemText(actor.drawListIndex, (int)ActorListColumn::Name, actorName);
    return true;
}

void ActorPropertiesWindow::FocusAndSelectIndex(u16 drawListIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = drawListIndex;
    s32 lvIndex = ListView_FindItem(listActors.getHandle(), -1, &findInfo);
    ListView_SetItemState(listActors.getHandle(), lvIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void ActorPropertiesWindow::DeselectIndex(u16 drawListIndex)
{
    LVFINDINFO findInfo = {};
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = drawListIndex;

    int lvIndex = ListView_FindItem(listActors.getHandle(), -1, &findInfo);
    ListView_SetItemState(listActors.getHandle(), lvIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
}

void ActorPropertiesWindow::UpdateEnabledState()
{
    if ( CM->read.units.size() > 0 || CM->read.sprites.size() > 0 )
        EnableActorEditing();
    else
        DisableActorEditing();
}

void ActorPropertiesWindow::RepopulateList()
{
    initilizing = true;
    SetListRedraw(false);

    listActors.DeleteAllItems();
    if ( CM != nullptr )
    {
        CM->animations.cleanDrawList();
        Selections & selections = CM->selections;
        
        listActors.AddRow(4, 0);
        listActors.SetItemText(0, (int)ActorListColumn::DrawListIndex, "0");
        listActors.SetItemText(0, (int)ActorListColumn::Type, "(Unused)");
        listActors.SetItemText(0, (int)ActorListColumn::UnitSpriteIndex, "");
        listActors.SetItemText(0, (int)ActorListColumn::Priority, "");
        listActors.SetItemText(0, (int)ActorListColumn::Name, "");
        auto & unitActors = CM->view.units.readAttachedData();
        auto & spriteActors = CM->view.sprites.readAttachedData();
        auto & drawList = CM->animations.drawList;
        for ( std::size_t i=1; i<drawList.size(); ++i )
        {
            std::uint64_t drawEntry = drawList[i];
            if ( drawEntry == MapAnimations::UnusedDrawEntry )
                break;
            else
            {
                std::size_t index = static_cast<std::size_t>(drawEntry & MapAnimations::MaskIndex);
                if ( drawEntry & MapAnimations::FlagUnitActor )
                {
                    const auto & unit = CM->read.units[index];
                    auto & unitActor = unitActors[index];
                    AddActor(true, false, unit.type, index, unitActor);
                }
                else
                {
                    const auto & sprite = CM->read.sprites[index];
                    auto & spriteActor = spriteActors[index];
                    AddActor(false, sprite.isUnit(), sprite.type, index, spriteActor);
                }
            }
        }

        if ( selectedActorIndex > CM->animations.drawList.size() )
            selectedActorIndex = noSelectedActor;

        if ( selectedActorIndex != noSelectedActor )
        {
            listActors.FocusItem(int(selectedActorIndex));
            listActors.SelectRow(int(selectedActorIndex));

            EnableActorEditing();

            int row = listActors.GetItemRow(int(selectedActorIndex));
            listActors.EnsureVisible(row, false);
        }
    }

    SetListRedraw(true);
    initilizing = false;
}

void ActorPropertiesWindow::EnableActorEditing()
{
    if ( selectedActorIndex != noSelectedActor )
    {
        if ( selectedActorIndex == 0 || selectedActorIndex > CM->animations.drawList.size() )
        {
            selectedActorIndex = noSelectedActor;
            DisableActorEditing();
            return;
        }

        buttonPausePlayAnimation.EnableThis();
        checkAutoRestart.EnableThis();
        editActorDirection.EnableThis();
        editElevation.EnableThis();
        dropAnimation.EnableThis();
        listboxImages.EnableThis();
        checkPrimaryImage.EnableThis();

        editImageId.EnableThis();
        editIscriptId.EnableThis();
        editOwner.EnableThis();
        editImageDirection.EnableThis();
        editImageX.EnableThis();
        editImageY.EnableThis();
        editOffsetX.EnableThis();
        editOffsetY.EnableThis();
        editBaseFrame.EnableThis();
        editGrpFrame.EnableThis();
        checkFlipped.EnableThis();
        checkRotationEnabled.EnableThis();
        checkHidden.EnableThis();
        checkDrawIfCloaked.EnableThis();
        dropRemapping.EnableThis();
        dropSelectionColor.EnableThis();
        dropDrawFunction.EnableThis();

        UpdateActorFieldText();
    }
}

void ActorPropertiesWindow::DisableActorEditing()
{
    if ( selectedActorIndex == noSelectedActor )
    {
        buttonPausePlayAnimation.SetText("Pause Animation");
        checkAutoRestart.SetCheck(false);
        editActorDirection.SetText("");
        editElevation.SetText("");
        dropAnimation.SetSel(0);
        listboxImages.ClearItems();
        checkPrimaryImage.SetCheck(false);
        
        editImageId.SetText("");
        editIscriptId.SetText("");
        editOwner.SetText("");
        editImageDirection.SetText("");
        editImageX.SetText("");
        editImageY.SetText("");
        editOffsetX.SetText("");
        editOffsetY.SetText("");
        editBaseFrame.SetText("");
        editGrpFrame.SetText("");
        checkFlipped.SetCheck(false);
        checkRotationEnabled.SetCheck(false);
        checkHidden.SetCheck(false);
        checkDrawIfCloaked.SetCheck(false);
        dropRemapping.SetSel(0);
        dropSelectionColor.SetSel(0);
        dropDrawFunction.SetSel(0);

        buttonPausePlayAnimation.DisableThis();
        checkAutoRestart.DisableThis();
        editActorDirection.DisableThis();
        editElevation.DisableThis();
        dropAnimation.DisableThis();
        listboxImages.DisableThis();
        checkPrimaryImage.DisableThis();

        editImageId.DisableThis();
        editIscriptId.DisableThis();
        editOwner.DisableThis();
        editImageDirection.DisableThis();
        editImageX.DisableThis();
        editImageY.DisableThis();
        editOffsetX.DisableThis();
        editOffsetY.DisableThis();
        editBaseFrame.DisableThis();
        editGrpFrame.DisableThis();
        checkFlipped.DisableThis();
        checkRotationEnabled.DisableThis();
        checkHidden.DisableThis();
        checkDrawIfCloaked.DisableThis();
        dropRemapping.DisableThis();
        dropSelectionColor.DisableThis();
        dropDrawFunction.DisableThis();
    }
}

void ActorPropertiesWindow::UpdateActorFieldText()
{
    if ( selectedActorIndex == noSelectedActor || selectedActorIndex == 0 || selectedActorIndex > CM->animations.drawList.size() )
    {
        selectedActorIndex = noSelectedActor;
        DisableActorEditing();
        return;
    }

    const MapActor* actor = nullptr;
    auto drawEntry = CM->animations.drawList[selectedActorIndex];
    auto unitOrSpriteIndex = drawEntry & MapAnimations::MaskIndex;
    if ( drawEntry != MapAnimations::UnusedDrawEntry )
    {
        if ( drawEntry & MapAnimations::FlagUnitActor )
            actor = &(CM->view.units.readAttachedData()[unitOrSpriteIndex]);
        else
            actor = &(CM->view.sprites.readAttachedData()[unitOrSpriteIndex]);
    }
    if ( actor == nullptr )
    {
        selectedActorIndex = noSelectedActor;
        DisableActorEditing();
        return;
    }

    buttonPausePlayAnimation.SetText(actor->paused ? "Play Animation" : "Pause Animation");
    checkAutoRestart.SetCheck(actor->autoRestart);
    editActorDirection.SetEditNum(actor->direction);
    editElevation.SetEditNum(actor->elevation);
    dropAnimation.SetSel(actor->lastSetAnim % 28);
    
    listboxImages.SetRedraw(false);
    listboxImages.ClearItems();
    auto primaryImageSlot = actor->primaryImageIndex;
    int primaryImageIndex = -1;
    selectedImageSlot = -1;
    auto & images = CM->animations.images;
    for ( std::size_t i=0; i<std::size(actor->usedImages); ++i )
    {
        imageStrings[i] = "";
        auto imageListIndex = actor->usedImages[i];
        if ( imageListIndex != 0 && imageListIndex < images.size() && images[imageListIndex] )
        {
            auto image = images[imageListIndex];
            std::string imageStr {};
            bool hasImageDat = image->imageId < chkd.scData.sprites.numImages();
            if ( hasImageDat )
            {
                const auto & imageDat = chkd.scData.sprites.getImage(image->imageId);
                if ( imageDat.grpFile < chkd.scData.sprites.imagesTbl->numStrings() )
                    imageStr = getArchiveFileName(chkd.scData.sprites.imagesTbl->getString(imageDat.grpFile));
            }
            std::string display = "[" + std::to_string(imageListIndex) + "] " +
                (imageStr.empty() ? "ImageId:" + std::to_string(image->imageId) : imageStr);
            imageStrings[i] = display;
            int insertionIndex = listboxImages.AddItem(i);
            if ( insertionIndex != -1 && primaryImageSlot == i )
            {
                selectedImageSlot = i;
                selectedImageIndex = imageListIndex;
                primaryImageIndex = insertionIndex;
            }
        }
    }
    listboxImages.SetCurSel(primaryImageIndex);
    listboxImages.SetRedraw(true);
    checkPrimaryImage.SetCheck(true);

    UpdateImageFieldText();
}

void ActorPropertiesWindow::UpdateImageFieldText()
{
    auto & images = CM->animations.images;
    auto & image = images[selectedImageIndex];
    editImageId.SetEditNum(image->imageId);
    editIscriptId.SetEditNum(image->iScriptId);
    editOwner.SetEditNum(image->owner);
    editImageDirection.SetEditNum(image->direction);
    editImageX.SetEditNum(image->xc);
    editImageY.SetEditNum(image->yc);
    editOffsetX.SetEditNum(image->xOffset);
    editOffsetY.SetEditNum(image->yOffset);
    editBaseFrame.SetEditNum(image->baseFrame);
    editGrpFrame.SetEditNum(image->frame);
    checkFlipped.SetCheck(image->flipped);
    checkRotationEnabled.SetCheck(image->rotation);
    checkHidden.SetCheck(image->hidden);
    checkDrawIfCloaked.SetCheck(image->drawIfCloaked);
    int drawFunc = int(image->drawFunction)%(int(MapImage::DrawFunction::None)+1);
    dropRemapping.SetSel(int(image->remapping)%7);
    dropSelectionColor.SetSel(int(image->selColor)%3);
    dropDrawFunction.SetSel(drawFunc);
}

void ActorPropertiesWindow::ImageSelectionChanged()
{
    if ( selectedActorIndex == noSelectedActor || selectedActorIndex == 0 || selectedActorIndex > CM->animations.drawList.size() )
    {
        selectedActorIndex = noSelectedActor;
        DisableActorEditing();
        return;
    }

    const MapActor* actor = nullptr;
    auto drawEntry = CM->animations.drawList[selectedActorIndex];
    auto unitOrSpriteIndex = drawEntry & MapAnimations::MaskIndex;
    if ( drawEntry != MapAnimations::UnusedDrawEntry )
    {
        if ( drawEntry & MapAnimations::FlagUnitActor )
            actor = &(CM->view.units.readAttachedData()[unitOrSpriteIndex]);
        else
            actor = &(CM->view.sprites.readAttachedData()[unitOrSpriteIndex]);
    }
    if ( actor == nullptr )
    {
        selectedActorIndex = noSelectedActor;
        DisableActorEditing();
        return;
    }

    LPARAM itemData = 0;
    if ( listboxImages.GetCurSelItem(itemData) )
    {
        auto & images = CM->animations.images;
        this->selectedImageSlot = itemData;
        this->selectedImageIndex = actor->usedImages[std::size_t(itemData)];
        
        checkPrimaryImage.SetCheck(this->selectedImageSlot == actor->primaryImageIndex);
        UpdateImageFieldText();
        listboxImages.RedrawThis();
    }
}

void ActorPropertiesWindow::LvItemChanged(NMHDR* nmhdr)
{
    NMLISTVIEW* itemInfo = (NMLISTVIEW*)nmhdr;
    LPARAM index = itemInfo->lParam;
        
    if ( itemInfo->uNewState & LVIS_SELECTED && initilizing == false ) // Selected, add item to selection
    {
        bool firstSelected = selectedActorIndex == noSelectedActor;
        selectedActorIndex = index;
        if ( firstSelected )
            EnableActorEditing();
        else
            UpdateActorFieldText();
    }
    else if ( itemInfo->uOldState & LVIS_SELECTED && selectedActorIndex != noSelectedActor ) // From selected to not selected, remove item from selection
    {
        if ( index == int(selectedActorIndex) )
        {
            int nextSelection = listActors.GetNextSelection(int(selectedActorIndex));
            selectedActorIndex = nextSelection == -1 ? noSelectedActor : nextSelection;
            if ( nextSelection == -1 )
                DisableActorEditing();
            else
                UpdateActorFieldText();
        }
    }
}

void ActorPropertiesWindow::NotifyClosePressed()
{
    EndDialog(getHandle(), IDCLOSE);
}

void ActorPropertiesWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ButtonClose: NotifyClosePressed(); break;
    }
}

void ActorPropertiesWindow::NotifyComboSelChanged(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
    case Id::ListboxImages: ImageSelectionChanged(); break;
    }
}

BOOL ActorPropertiesWindow::Activate(WPARAM wParam, LPARAM lParam)
{
    if ( LOWORD(wParam) != WA_INACTIVE )
        chkd.SetCurrDialog(getHandle());

    RepopulateList(); // TODO: Better refresh

    return FALSE;
}

BOOL ActorPropertiesWindow::ShowWindow(WPARAM wParam, LPARAM lParam)
{
    BOOL result = ClassDialog::DlgProc(getHandle(), WM_SHOWWINDOW, wParam, lParam);
    if ( wParam == TRUE )
        SetFocus(listActors.getHandle());

    return result;
}

BOOL ActorPropertiesWindow::DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
    case LVN_ITEMCHANGED: LvItemChanged(nmhdr); break;
    }
    return ClassDialog::DlgNotify(hWnd, idFrom, nmhdr);
}

BOOL ActorPropertiesWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_KILLFOCUS:
        listboxImages.RedrawThis();
        break;
    }
    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL ActorPropertiesWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
        case WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
            imageListDc.emplace(listboxImages.getHandle());
            break;

        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            const auto & str = imageStrings[mis->itemData];
            if ( !str.empty() && GetStringDrawSize(*imageListDc, mis->itemWidth, mis->itemHeight, str) )
            {
                mis->itemWidth += 5;
                mis->itemHeight += 2;

                if ( mis->itemHeight > 255 )
                    mis->itemHeight = 255;
            }
            return TRUE;
        }
        break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            imageListDc = std::nullopt;
            break;

        case WM_CTLCOLORLISTBOX:
            if ( listboxImages == (HWND)lParam )
            {
                HBRUSH hBrush = WinLib::ResourceManager::getSolidBrush(RGB(255, 255, 255));
                if ( hBrush != NULL )
                    return (LPARAM)hBrush;
            }
            return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
            break;

        case WinLib::LB::WM_PREDRAWITEMS:
            break;

        case WM_DRAWITEM:
        {
            PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
            bool isSelected = pdis->itemData == selectedImageSlot,
                isFocused = listboxImages.getHandle() == GetFocus(),
                drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

            if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
            {
                WinLib::DeviceContext dc { pdis->hDC };
                const auto & str = imageStrings[pdis->itemData];
                if ( !str.empty() )
                {
                    dc.fillRect(pdis->rcItem, isFocused && isSelected ? RGB(0, 120, 215) : (isSelected ? RGB(240, 240, 240) : RGB(255, 255, 255)));
                    SetBkMode(pdis->hDC, TRANSPARENT);
                    DrawString(dc, pdis->rcItem.left+3, pdis->rcItem.top+1, pdis->rcItem.right-pdis->rcItem.left,
                        (isSelected && isFocused) ? RGB(255, 255, 255) : RGB(0, 0, 0), str);
                }
            }
            return TRUE;
        }
        break;

        case WinLib::LB::WM_POSTDRAWITEMS:
            break;

    case WM_ACTIVATE: return Activate(wParam, lParam); break;
    case WM_SHOWWINDOW: return ShowWindow(wParam, lParam); break;
    case WM_CLOSE: DestroyThis(); break;
    default: return ClassDialog::DlgProc(hWnd, msg, wParam, lParam); break;
    }
    return TRUE;
}
