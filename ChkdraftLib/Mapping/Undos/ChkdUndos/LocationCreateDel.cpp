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
        *location = *((GuiMap*)guiMap)->layers.getLocation(locationId);
        std::shared_ptr<RawString> locName = ((GuiMap*)guiMap)->strings.getLocationName<RawString>((size_t)locationId, Chk::Scope::Game);
        locationName = locName != nullptr ? *locName : "";
        ((GuiMap*)guiMap)->layers.deleteLocation(locationId);
    }
    else // Do create
    {
        Chk::LocationPtr newLocation = Chk::LocationPtr(new Chk::Location(*location));
        newLocation->stringId = 0;
        ((GuiMap*)guiMap)->layers.addLocation(newLocation);
        newLocation->stringId = (u16)((GuiMap*)guiMap)->strings.addString(locationName);
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
