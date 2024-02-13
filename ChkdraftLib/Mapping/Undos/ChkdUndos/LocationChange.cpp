#include "LocationChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

LocationChange::~LocationChange()
{

}

std::shared_ptr<LocationChange> LocationChange::Make(u16 locationId, Chk::Location::Field field, u32 data)
{
    return std::shared_ptr<LocationChange>(new LocationChange(locationId, field, data));
}

void LocationChange::Reverse(void *guiMap)
{
    u32 replacedData = 0; // ((GuiMap*)guiMap)->GetLocationFieldData(locationId, field);
    switch ( field )
    {
        case Chk::Location::Field::Left: replacedData = (u32)((GuiMap*)guiMap)->getLocation(locationId).left;
            ((GuiMap*)guiMap)->getLocation(locationId).left = data; break;
        case Chk::Location::Field::Top: replacedData = (u32)((GuiMap*)guiMap)->getLocation(locationId).top;
            ((GuiMap*)guiMap)->getLocation(locationId).top = data; break;
        case Chk::Location::Field::Right: replacedData = (u32)((GuiMap*)guiMap)->getLocation(locationId).right;
            ((GuiMap*)guiMap)->getLocation(locationId).right = data; break;
        case Chk::Location::Field::Bottom: replacedData = (u32)((GuiMap*)guiMap)->getLocation(locationId).bottom;
            ((GuiMap*)guiMap)->getLocation(locationId).bottom = data; break;
        case Chk::Location::Field::ElevationFlags: replacedData = (u32)((GuiMap*)guiMap)->getLocation(locationId).elevationFlags;
            ((GuiMap*)guiMap)->getLocation(locationId).elevationFlags = data; break;
    }
    data = replacedData;
}

int32_t LocationChange::GetType()
{
    return UndoTypes::LocationChange;
}

LocationChange::LocationChange(u16 locationId, Chk::Location::Field field, u32 data)
    : locationId(locationId), field(field), data(data)
{

}
