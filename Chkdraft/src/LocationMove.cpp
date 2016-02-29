#include "LocationMove.h"
#include "GuiMap.h"

LocationMove::LocationMove(u16 locationIndex, s32 xChange, s32 yChange)
    : locationIndex(locationIndex), xChange(xChange), yChange(yChange)
{

}

void LocationMove::Reverse(void *guiMap)
{
    ((GuiMap*)guiMap)->MoveLocation(locationIndex, xChange, yChange);
    xChange = -xChange;
    yChange = -yChange;
}

int32_t LocationMove::GetType()
{
    return (int32_t)UndoTypes::LocationChange;
}
