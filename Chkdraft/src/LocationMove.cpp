#include "LocationMove.h"
#include "GuiMap.h"

std::shared_ptr<LocationMove> LocationMove::Make(u16 locationIndex, s32 xChange, s32 yChange)
{
    return std::shared_ptr<LocationMove>(new LocationMove(locationIndex, xChange, yChange));
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

LocationMove::LocationMove(u16 locationIndex, s32 xChange, s32 yChange)
    : locationIndex(locationIndex), xChange(xChange), yChange(yChange)
{

}
