#include "LocationCreateDel.h"
#include "../../../Windows/MainWindows/GuiMap.h"

LocationCreateDel::~LocationCreateDel()
{

}

std::shared_ptr<LocationCreateDel> LocationCreateDel::Make(u16 locationIndex, Chk::Location &location, std::string &locationName)
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
        location = std::unique_ptr<Chk::Location>(new Chk::Location);
        *location = *((GuiMap*)guiMap)->layers.getLocation(locationIndex);
        std::shared_ptr<RawString> locName = ((GuiMap*)guiMap)->strings.getLocationName<RawString>((size_t)locationIndex, Chk::Scope::Game);
        locationName = locName != nullptr ? *locName : "";
        ((GuiMap*)guiMap)->layers.deleteLocation(locationIndex);
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
    return (int32_t)UndoTypes::LocationChange;
}

LocationCreateDel::LocationCreateDel(u16 locationIndex, Chk::Location &location, std::string &locationName) // Undo deletion
    : locationIndex(locationIndex), location(nullptr), locationName(locationName)
{
    this->location = std::unique_ptr<Chk::Location>(new Chk::Location);
    (*(this->location)) = location;
}

LocationCreateDel::LocationCreateDel(u16 locationIndex) // Undo creation
    : locationIndex(locationIndex), location(nullptr), locationName("")
{

}
