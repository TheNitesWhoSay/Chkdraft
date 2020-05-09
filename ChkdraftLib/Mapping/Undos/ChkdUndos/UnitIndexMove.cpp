#include "UnitIndexMove.h"
#include "../../../Windows/MainWindows/GuiMap.h"

UnitIndexMove::~UnitIndexMove()
{

}

std::shared_ptr<UnitIndexMove> UnitIndexMove::Make(u16 oldIndex, u16 newIndex)
{
    return std::shared_ptr<UnitIndexMove>(new UnitIndexMove(oldIndex, newIndex));
}

void UnitIndexMove::Reverse(void *guiMap)
{
    Chk::UnitPtr preserve = ((GuiMap*)guiMap)->layers.getUnit(newIndex);
    ((GuiMap*)guiMap)->layers.deleteUnit(newIndex);
    ((GuiMap*)guiMap)->layers.insertUnit(oldIndex, preserve);
    ((GuiMap*)guiMap)->GetSelections().sendMove(oldIndex, newIndex);
    std::swap(oldIndex, newIndex);
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
    ((GuiMap*)guiMap)->GetSelections().finishMove();
}

int32_t UnitIndexMoveBoundary::GetType()
{
    return UndoTypes::UnitChange;
}

UnitIndexMoveBoundary::UnitIndexMoveBoundary()
{

}
