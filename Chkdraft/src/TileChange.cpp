#include "TileChange.h"
#include "GuiMap.h"

TileChange::TileChange(u16 xc, u16 yc, u16 tileValue) : xc(xc), yc(yc), tileValue(tileValue)
{

}

void TileChange::Reverse(void *guiMap)
{
	u16 replacedValue = ((GuiMap*)guiMap)->getTile(xc, yc);
	((GuiMap*)guiMap)->setTile(xc, yc, tileValue);
	tileValue = replacedValue;
}

int32_t TileChange::GetType()
{
	return (int32_t)UndoTypes::TileChange;
}
