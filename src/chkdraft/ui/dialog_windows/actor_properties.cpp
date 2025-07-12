#include "actor_properties.h"
#include "chkdraft/chkdraft.h"
#include <CommCtrl.h>

enum class ActorListColumn { DrawListIndex, Type, UnitSpriteIndex, Priority, Name };

enum_t(Id, u32, {
    ActorList = ID_FIRST,

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
    initilizing = true;
    WindowsItem::SetWinText("Actor Properties");

    listActors.CreateThis(hWnd, 9, 10, 549, 449, false, false, Id::ActorList);
    listActors.EnableFullRowSelect();
    listActors.setFont(5, 13, "Tahoma", false);
    
    listActors.AddColumn(0, "Draw List Index", 90, LVCFMT_RIGHT);
    listActors.AddColumn(1, "Type", 65, LVCFMT_LEFT);
    listActors.AddColumn(2, "Unit/Sprite Index", 95, LVCFMT_RIGHT);
    listActors.AddColumn(3, "Draw Priority", 120, LVCFMT_RIGHT);
    listActors.AddColumn(4, "Name", 150, LVCFMT_LEFT);

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
    std::string actorName = isUnit || isSpriteUnit ?
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

}

void ActorPropertiesWindow::DisableActorEditing()
{

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
    return ClassDialog::DlgNotify(hWnd, idFrom, nmhdr);
}

BOOL ActorPropertiesWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
