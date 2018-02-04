#include "LocationCreateDel.h"
#include "GuiMap.h"

LocationCreateDel::~LocationCreateDel()
{

}

std::shared_ptr<LocationCreateDel> LocationCreateDel::Make(u16 locationIndex, ChkLocation &location, std::string &locationName)
{
    return std::shared_ptr<LocationCreateDel>(new LocationCreateDel(locationIndex, location, locationName));
}

std::shared_ptr<LocationCreateDel> LocationCreateDel::Make(u16 locationIndex)
{
    return std::shared_ptr<LocationCreateDel>(new LocationCreateDel(locationIndex));
}

void LocationCreateDel::Reverse(void *guiMap)
{
    if ( location == nullptr ) // Do delete
    {
        location = std::unique_ptr<ChkLocation>(new ChkLocation);
        *location = ((GuiMap*)guiMap)->getLocation(locationIndex);
        ((GuiMap*)guiMap)->getLocationName(locationIndex, locationName);
        ((GuiMap*)guiMap)->deleteLocation(locationIndex);
    }
    else // Do create
    {
        ((GuiMap*)guiMap)->insertLocation(locationIndex, *location, locationName);
        location = nullptr;
        locationName.clear();
    }
}

int32_t LocationCreateDel::GetType()
{
    return (int32_t)UndoTypes::LocationChange;
}

LocationCreateDel::LocationCreateDel(u16 locationIndex, ChkLocation &location, std::string &locationName) // Undo deletion
    : locationIndex(locationIndex), location(nullptr), locationName(locationName)
{
    this->location = std::unique_ptr<ChkLocation>(new ChkLocation);
    (*(this->location)) = location;
}

LocationCreateDel::LocationCreateDel(u16 locationIndex) // Undo creation
    : locationIndex(locationIndex), location(nullptr), locationName("")
{

}
