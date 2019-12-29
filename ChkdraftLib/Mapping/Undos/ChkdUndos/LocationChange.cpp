#include "LocationChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

LocationChange::~LocationChange()
{

}

std::shared_ptr<LocationChange> LocationChange::Make(u16 locationIndex, Chk::Location::Field field, u32 data)
{
    return std::shared_ptr<LocationChange>(new LocationChange(locationIndex, field, data));
}

void LocationChange::Reverse(void *guiMap)
{
    u32 replacedData = 0; // ((GuiMap*)guiMap)->GetLocationFieldData(locationIndex, field);
    switch ( field )
    {
        case Chk::Location::Field::Left: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->left;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->left = data; break;
        case Chk::Location::Field::Top: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->top;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->top = data; break;
        case Chk::Location::Field::Right: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->right;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->right = data; break;
        case Chk::Location::Field::Bottom: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->bottom;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->bottom = data; break;
        case Chk::Location::Field::StringId: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->stringId;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->stringId = data; break;
        case Chk::Location::Field::ElevationFlags: replacedData = (u32)((GuiMap*)guiMap)->layers.getLocation(locationIndex)->elevationFlags;
            ((GuiMap*)guiMap)->layers.getLocation(locationIndex)->elevationFlags = data; break;
    }
    data = replacedData;
}

int32_t LocationChange::GetType()
{
    return (int32_t)UndoTypes::LocationChange;
}

LocationChange::LocationChange(u16 locationIndex, Chk::Location::Field field, u32 data)
    : locationIndex(locationIndex), field(field), data(data)
{

}
