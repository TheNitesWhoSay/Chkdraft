#include "UnitIndexMove.h"
#include "GuiMap.h"

UnitIndexMove::UnitIndexMove(u16 oldIndex, u16 newIndex)
    : oldIndex(oldIndex), newIndex(newIndex)
{

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

UnitIndexMoveBoundary::UnitIndexMoveBoundary()
{

}

void UnitIndexMoveBoundary::Reverse(void *guiMap)
{
    ((GuiMap*)guiMap)->GetSelections().finishMove();
}

int32_t UnitIndexMoveBoundary::GetType()
{
    return (int32_t)UndoTypes::UnitChange;
}
