#include "LocationMove.h"
#include "../../../Windows/MainWindows/GuiMap.h"

LocationMove::~LocationMove()
{

}

std::shared_ptr<LocationMove> LocationMove::Make(u16 locationIndex, s32 xChange, s32 yChange)
{
    return std::shared_ptr<LocationMove>(new LocationMove(locationIndex, xChange, yChange));
}

void LocationMove::Reverse(void *guiMap)
{
    auto loc = ((GuiMap*)guiMap)->layers.getLocation(locationIndex);
    loc->left += xChange;
    loc->right += xChange;
    loc->top += yChange;
    loc->bottom += yChange;
    xChange = -xChange;
    yChange = -yChange;
}

int32_t LocationMove::GetType()
{
    return UndoTypes::LocationChange;
}

LocationMove::LocationMove(u16 locationIndex, s32 xChange, s32 yChange)
    : locationIndex(locationIndex), xChange(xChange), yChange(yChange)
{

}
