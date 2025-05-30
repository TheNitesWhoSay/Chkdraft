#include "unit_index_move.h"
#include "ui/main_windows/gui_map.h"

UnitIndexMove::~UnitIndexMove()
{

}

std::shared_ptr<UnitIndexMove> UnitIndexMove::Make(u16 oldIndex, u16 newIndex)
{
    return std::shared_ptr<UnitIndexMove>(new UnitIndexMove(oldIndex, newIndex));
}

void UnitIndexMove::Reverse(void *guiMap)
{
    /*Chk::Unit preserve = ((GuiMap*)guiMap)->getUnit(newIndex);
    ((GuiMap*)guiMap)->deleteUnit(newIndex);
    ((GuiMap*)guiMap)->insertUnit(oldIndex, preserve);
    ((GuiMap*)guiMap)->selections.sendUnitMove(oldIndex, newIndex);
    std::swap(oldIndex, newIndex);*/
}

int32_t UnitIndexMove::GetType()
{
    return UndoTypes::UnitChange;
}

UnitIndexMove::UnitIndexMove(u16 oldIndex, u16 newIndex)
    : oldIndex(oldIndex), newIndex(newIndex)
{

}

UnitIndexMoveBoundary::~UnitIndexMoveBoundary()
{

}

std::shared_ptr<UnitIndexMoveBoundary> UnitIndexMoveBoundary::Make()
{
    return std::shared_ptr<UnitIndexMoveBoundary>(new UnitIndexMoveBoundary());
}

void UnitIndexMoveBoundary::Reverse(void *guiMap)
{
    //((GuiMap*)guiMap)->selections.finishUnitMove();
}

int32_t UnitIndexMoveBoundary::GetType()
{
    return UndoTypes::UnitChange;
}

UnitIndexMoveBoundary::UnitIndexMoveBoundary()
{

}
