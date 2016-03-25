#include "UnitIndexMove.h"
#include "GuiMap.h"

std::shared_ptr<UnitIndexMove> UnitIndexMove::Make(u16 oldIndex, u16 newIndex)
{
    return std::shared_ptr<UnitIndexMove>(new UnitIndexMove(oldIndex, newIndex));
}

void UnitIndexMove::Reverse(void *guiMap)
{
    ChkUnit preserve = ((GuiMap*)guiMap)->getUnit(newIndex);
    ((GuiMap*)guiMap)->deleteUnit(newIndex);
    ((GuiMap*)guiMap)->insertUnit(oldIndex, preserve);
    ((GuiMap*)guiMap)->GetSelections().sendMove(oldIndex, newIndex);
    std::swap(oldIndex, newIndex);
}

int32_t UnitIndexMove::GetType()
{
    return (int32_t)UndoTypes::UnitChange;
}

UnitIndexMove::UnitIndexMove(u16 oldIndex, u16 newIndex)
    : oldIndex(oldIndex), newIndex(newIndex)
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
    return (int32_t)UndoTypes::UnitChange;
}

UnitIndexMoveBoundary::UnitIndexMoveBoundary()
{

}
