#include "LocationCreateDel.h"
#include "../../../Windows/MainWindows/GuiMap.h"

LocationCreateDel::~LocationCreateDel()
{

}

std::shared_ptr<LocationCreateDel> LocationCreateDel::Make(u16 locationId, Chk::Location & location, std::string & locationName)
{
    return std::shared_ptr<LocationCreateDel>(new LocationCreateDel(locationId, location, locationName));
}

std::shared_ptr<LocationCreateDel> LocationCreateDel::Make(u16 locationId)
{
    return std::shared_ptr<LocationCreateDel>(new LocationCreateDel(locationId));
}

void LocationCreateDel::Reverse(void *guiMap)
{
    if ( location == nullptr ) // Do delete
    {
        location = std::unique_ptr<Chk::Location>(new Chk::Location);
        *location = ((GuiMap*)guiMap)->getLocation(locationId);
        auto locName = ((GuiMap*)guiMap)->getLocationName<RawString>((size_t)locationId, Chk::StrScope::Game);
        locationName = locName ? *locName : "";
        ((GuiMap*)guiMap)->deleteLocation(locationId);
        ((GuiMap*)guiMap)->deleteUnusedStrings(Chk::StrScope::Both);
    }
    else // Do create
    {
        Chk::Location newLocation = *location;
        newLocation.stringId = 0;
        size_t newLocationId = ((GuiMap*)guiMap)->addLocation(newLocation);
        ((GuiMap*)guiMap)->getLocation(newLocationId).stringId = (u16)((GuiMap*)guiMap)->addString(locationName);
        location = nullptr;
        locationName.clear();
    }
}

int32_t LocationCreateDel::GetType()
{
    return UndoTypes::LocationChange;
}

LocationCreateDel::LocationCreateDel(u16 locationId, Chk::Location & location, std::string & locationName) // Undo deletion
    : locationId(locationId), location(nullptr), locationName(locationName)
{
    this->location = std::unique_ptr<Chk::Location>(new Chk::Location);
    (*(this->location)) = location;
}

LocationCreateDel::LocationCreateDel(u16 locationId) // Undo creation
    : locationId(locationId), location(nullptr), locationName("")
{

}
