#include "LocationCreateDel.h"
#include "GuiMap.h"

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

