#include "selections.h"
#include "ui/main_windows/gui_map.h"

void Selections::setDrags(s32 x, s32 y)
{
    startDrag.x = x;
    startDrag.y = y;
    endDrag.x = x;
    endDrag.y = y;
}

void Selections::snapDrags(s32 xInterval, s32 yInterval, bool nonZeroSnap)
{
    if ( xInterval > 0 )
    {
        if ( nonZeroSnap && startDrag.x/xInterval == endDrag.x/xInterval )
        {
            startDrag.x = startDrag.x/xInterval*xInterval;
            endDrag.x = startDrag.x + xInterval;
        }
        else
        {
            if ( (startDrag.x % xInterval) != 0 )
                startDrag.x = (startDrag.x+xInterval/2)/xInterval*xInterval;
            if ( (endDrag.x % xInterval) != 0 )
                endDrag.x = (endDrag.x+xInterval/2)/xInterval*xInterval;
        }
    }
    if ( yInterval > 0 )
    {
        if ( nonZeroSnap && startDrag.y/yInterval == endDrag.y/yInterval )
        {
            startDrag.y = startDrag.y/yInterval*yInterval;
            endDrag.y = startDrag.y + yInterval;
        }
        else
        {
            if ( (startDrag.y % yInterval) != 0 )
                startDrag.y = (startDrag.y+yInterval/2)/yInterval*yInterval;
            if ( (endDrag.y % yInterval) != 0 )
                endDrag.y = (endDrag.y+yInterval/2)/yInterval*yInterval;
        }
    }
}

void Selections::snapEndDrag(s32 xInterval, s32 yInterval)
{
    if ( xInterval > 0 && (endDrag.x % xInterval) != 0 )
        endDrag.x = (endDrag.x+xInterval/2)/xInterval*xInterval;

    if ( yInterval > 0 && (endDrag.y % yInterval) != 0  )
        endDrag.y = (endDrag.y+yInterval/2)/yInterval*yInterval;
}

void Selections::clear()
{
    auto edit = map();
    edit->tiles.clearSelections();
    edit->doodads.clearSelections();
    edit->sprites.clearSelections();
    edit->units.clearSelections();
    edit->tileFog.clearSelections();
    edit->locations.clearSelections();
}

u16 Selections::getSelectedLocation()
{
    const auto & selectedLocations = map.view.locations.sel();
    return !selectedLocations.empty() ? selectedLocations.front() : NO_LOCATION;
}

void Selections::selectLocation(u16 index)
{
    auto edit = map(ActionDescriptor::UpdateLocationSel);
    edit->locations.clearSelections();
    edit->locations.select(index);
    numRecentLocations = 1;
    recentLocations[0] = u8(index);
}

void Selections::selectLocation(s32 clickX, s32 clickY, bool canSelectAnywhere)
{
    auto edit = map(ActionDescriptor::UpdateLocationSel);
    size_t numLocations = map.numLocations();
    u16 firstRecentlySelected = NO_LOCATION;
    bool madeSelection = false;
    
    for ( u16 i=1; i<=numLocations; i++ )
    {
        if ( i != getSelectedLocation() && (i != Chk::LocationId::Anywhere || canSelectAnywhere) )
        {
            const auto & location = map.getLocation(i);
            s32 locLeft = std::min(location.left, location.right),
                locRight = std::max(location.left, location.right),
                locTop = std::min(location.top, location.bottom),
                locBottom = std::max(location.top, location.bottom);

            if ( clickX >= locLeft && clickX <= locRight &&
                 clickY >= locTop && clickY <= locBottom )
            {
                bool recentlySelected = false;
                for ( u8 recentIndex=0; recentIndex<numRecentLocations; recentIndex++ )
                {
                    if ( i == recentLocations[recentIndex] )
                    {
                        recentlySelected = true;
                        break;
                    }
                }

                if ( recentlySelected ) // Location has been recently selected, flag if no earlier location was flagged
                {
                    if ( firstRecentlySelected == NO_LOCATION )
                        firstRecentlySelected = i;
                }
                else // Location hasn't been recently selected, select it
                {
                    edit->locations.clearSelections();
                    edit->locations.select(i);
                    if ( numRecentLocations < Chk::TotalLocations )
                    {

                        recentLocations[numRecentLocations] = u8(i);
                        numRecentLocations ++;
                    }
                    else
                    {
                        recentLocations[0] = u8(i);
                        numRecentLocations = 1;
                    }
                    madeSelection = true;
                    break;
                }
            }
        }
    }

    if ( !madeSelection )
    {
        if ( firstRecentlySelected != NO_LOCATION )
        {
            edit->locations.clearSelections();
            edit->locations.select(firstRecentlySelected);
            recentLocations[0] = u8(firstRecentlySelected);
            numRecentLocations = 1;
        }
        else // Reset recent locations
        {
            map.setActionDescription(ActionDescriptor::ClearLocationSel);
            edit->locations.clearSelections();
            recentLocations[0] = u8(NO_LOCATION);
            numRecentLocations = 1;
        }
    }
}

bool Selections::selFlagsIndicateInside() const
{
    switch ( locSelFlags )
    {
        case LocSelFlags::West:
        case LocSelFlags::North:
        case LocSelFlags::East:
        case LocSelFlags::South:
        case LocSelFlags::NorthWest:
        case LocSelFlags::NorthEast:
        case LocSelFlags::SouthEast:
        case LocSelFlags::SouthWest:
        case LocSelFlags::Middle: return true;
        default: return false;
    }
}

void Selections::removeLocations()
{
    map()->locations.clearSelections();
}

bool Selections::unitIsSelected(u16 index)
{
    for ( const auto unitIndex : map.view.units.sel() )
    {
        if ( unitIndex == index )
            return true;
    }
    return false;
}

bool Selections::doodadIsSelected(size_t index)
{
    for ( size_t doodadIndex : map.view.doodads.sel() )
    {
        if ( doodadIndex == index )
            return true;
    }
    return false;
}

bool Selections::spriteIsSelected(size_t index)
{
    for ( size_t spriteIndex : map.view.sprites.sel() )
    {
        if ( spriteIndex == index )
            return true;
    }
    return false;
}

bool Selections::hasUnits()
{
    return map.view.units.sel().size() > 0;
}

bool Selections::hasDoodads()
{
    return map.view.doodads.sel().size() > 0;
}

bool Selections::hasTiles()
{
    return map.view.tiles.sel().size() > 0;
}

bool Selections::hasSprites()
{
    return map.view.sprites.sel().size() > 0;
}

bool Selections::hasFogTiles()
{
    return map.view.tileFog.sel().size() > 0;
}

u16 Selections::numUnits()
{
    if ( map.view.units.sel().size() < u16_max )
        return (u16)map.view.units.sel().size();
    else
        return u16_max;
}

u16 Selections::numSprites()
{
    if ( map.view.sprites.sel().size() < u16_max )
        return (u16)map.view.sprites.sel().size();
    else
        return u16_max;
}

u16 Selections::numUnitsUnder(u16 index)
{
    u16 numUnitsBefore = 0;
    for ( const auto unitIndex : map.view.units.sel() )
    {
        if ( unitIndex < index )
            numUnitsBefore++;
    }
    return numUnitsBefore;
}

u16 Selections::getFirstUnit()
{
    if ( map.view.units.sel().size() > 0 )
        return map.view.units.sel().front();
    else
        return 0;
}

u16 Selections::getFirstDoodad()
{
    if ( map.view.doodads.sel().size() > 0 )
        return u16(map.view.doodads.sel().front());
    else
        return 0;
}

size_t Selections::getFirstSprite()
{
    if ( map.view.sprites.sel().size() > 0 )
        return map.view.sprites.sel().front();
    else
        return 0;
}

u16 Selections::getHighestUnitIndex()
{
    int highestIndex = -1;
    for ( const auto unitIndex : map.view.units.sel() )
    {
        if ( (int)unitIndex > highestIndex )
            highestIndex = (int)unitIndex;
    }

    if ( highestIndex >= 0 )
        return highestIndex;
    else
        return (u16)highestIndex;
}

u16 Selections::getLowestUnitIndex()
{
    u16 lowestIndex = u16_max;
    for ( const auto unitIndex : map.view.units.sel() )
    {
        if ( unitIndex < lowestIndex )
            lowestIndex = unitIndex;
    }
    return lowestIndex;
}

size_t Selections::getHighestSpriteIndex()
{
    int highestIndex = -1;
    for ( size_t spriteIndex : map.view.sprites.sel() )
    {
        if ( (int)spriteIndex > highestIndex )
            highestIndex = (int)spriteIndex;
    }
    return size_t(highestIndex);
}

size_t Selections::getLowestSpriteIndex()
{
    size_t lowestIndex = std::numeric_limits<size_t>::max();
    for ( size_t spriteIndex : map.view.sprites.sel() )
    {
        if ( spriteIndex < lowestIndex )
            lowestIndex = spriteIndex;
    }
    return lowestIndex;
}
