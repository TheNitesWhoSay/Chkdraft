#include "UnitChange.h"
#include "GuiMap.h"

UnitChange::UnitChange(u16 unitIndex, ChkUnitField field, u32 data)
	: unitIndex(unitIndex), field(field), data(data)
{

}

void UnitChange::Reverse(void *guiMap)
{
	u32 replacedData = ((GuiMap*)guiMap)->GetUnitFieldData(unitIndex, field);
	((GuiMap*)guiMap)->SetUnitFieldData(unitIndex, field, data);
	data = replacedData;
}

int32_t UnitChange::GetType()
{
	return (int32_t)UndoTypes::UnitChange;
}
