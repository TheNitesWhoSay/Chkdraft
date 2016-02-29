#include "UnitCreateDel.h"
#include "GuiMap.h"

UnitCreateDel::UnitCreateDel(u16 index, ChkUnit &unit) // Undo deletion
	: index(index), unit(nullptr)
{
	this->unit = std::unique_ptr<ChkUnit>(new ChkUnit);
	(*(this->unit)) = unit;
}

UnitCreateDel::UnitCreateDel(u16 index) // Undo creation
	: index(index), unit(nullptr)
{
	
}

void UnitCreateDel::Reverse(void *guiMap)
{
	if ( unit == nullptr ) // Do delete
	{
		unit = std::unique_ptr<ChkUnit>(new ChkUnit);
		*unit = ((GuiMap*)guiMap)->getUnit(index);
		((GuiMap*)guiMap)->deleteUnit(index);
	}
	else // Do create
	{
		//((GuiMap*)guiMap)->insertUnit(index, *unit);

		// This doesn't belong here, it's the only place this function will work for testing
		((GuiMap*)guiMap)->addUnit(unit->id, unit->owner, unit->xc, unit->yc, unit->stateFlags);
		unit = nullptr;
	}
}

int32_t UnitCreateDel::GetType()
{
	return (int32_t)UndoTypes::UnitChange;
}
