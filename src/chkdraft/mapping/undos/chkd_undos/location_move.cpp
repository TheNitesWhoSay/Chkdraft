#include "location_move.h"
#include "ui/main_windows/gui_map.h"

LocationMove::~LocationMove()
{

}

std::shared_ptr<LocationMove> LocationMove::Make(u16 locationId, s32 xChange, s32 yChange)
{
    return std::shared_ptr<LocationMove>(new LocationMove(locationId, xChange, yChange));
}

void LocationMove::Reverse(void *guiMap)
{
    auto & loc = ((GuiMap*)guiMap)->getLocation(locationId);
    loc.left += xChange;
    loc.right += xChange;
    loc.top += yChange;
    loc.bottom += yChange;
    xChange = -xChange;
    yChange = -yChange;
}

int32_t LocationMove::GetType()
{
    return UndoTypes::LocationChange;
}

LocationMove::LocationMove(u16 locationId, s32 xChange, s32 yChange)
    : locationId(locationId), xChange(xChange), yChange(yChange)
{

}
