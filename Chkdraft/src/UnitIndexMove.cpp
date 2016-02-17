#include "UnitIndexMove.h"
#include "GuiMap.h"

UnitIndexMove::UnitIndexMove(u16 oldIndex, u16 newIndex)
	: oldIndex(oldIndex), newIndex(newIndex)
{

}

void UnitIndexMove::Reverse(void *guiMap)
{
	ChkUnit preserve;
	((GuiMap*)guiMap)->UNIT().get<ChkUnit>(preserve, newIndex*UNIT_STRUCT_SIZE);
	((GuiMap*)guiMap)->UNIT().del<ChkUnit>(newIndex*UNIT_STRUCT_SIZE);
	((GuiMap*)guiMap)->UNIT().insert<ChkUnit&>(oldIndex*UNIT_STRUCT_SIZE, preserve);
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
