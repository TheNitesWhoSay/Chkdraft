#include "Selections.h"
#include "../Windows/MainWindows/GuiMap.h"

Selections::Selections(GuiMap & map) : map(map), moved(false), numRecentLocations(0), locationFlags(0), locSelFlags(LocSelFlags::None), selectedLocation(0)
{
    memset((void*)&recentLocations[0], 0, sizeof(recentLocations)/sizeof(u8));
    startDrag.x = -1;
    startDrag.y = -1;
    endDrag.x = -1;
    endDrag.y = -1;
}

Selections::~Selections()
{

}

void Selections::setStartDrag(s32 x, s32 y)
{
    startDrag.x = x;
    startDrag.y = y;
}

void Selections::setEndDrag(s32 x, s32 y)
{
    endDrag.x = x;
    endDrag.y = y;
}

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

void Selections::removeTile(TileNode* & tile)
{
    removeTile(tile->xc, tile->yc);
}

void Selections::removeTile(u16 xc, u16 yc)
{
    auto toRemove = selTiles.end();
    for ( auto it = selTiles.begin(); it != selTiles.end(); ++it )
    {
        // If an edge is matched to the tile being removed, un-match the edge
        if ( it->yc == yc ) // Tile is in the same row
        {
            if ( it->xc == xc - 1 ) (u8 &)it->neighbors |= TileNeighbor::Right; // OR 0100, flips on the RIGHT edge bit
            else if ( it->xc == xc + 1 ) (u8 &)it->neighbors |= TileNeighbor::Left; // OR 0001, flips on the LEFT edge bit
        }
        else if ( it->xc == xc ) // Tile is in the same column
        {
            if ( it->yc == yc - 1 ) (u8 &)it->neighbors |= TileNeighbor::Bottom; // OR 1000, flips on the BOTTOM edge bit
            else if ( it->yc == yc + 1 ) (u8 &)it->neighbors |= TileNeighbor::Top; // OR 0010, flips on the TOP edge bit
        }

        if ( it->xc == xc && it->yc == yc )
            toRemove = it;
    }
    
    if ( toRemove != selTiles.end() )
        selTiles.erase(toRemove);
}

void Selections::clear()
{
    removeTiles();
    removeDoodads();
    removeSprites();
    removeUnits();
    removeFog();
    selectedLocation = Chk::LocationId::NoLocation;
}

void Selections::addTile(u16 value, u16 xc, u16 yc)
{
    TileNode tile;
    tile.xc = xc;
    tile.yc = yc;
    tile.neighbors = TileNeighbor::All;
    tile.value = value;

    for ( auto & selTile : selTiles )
    {
        // If tile edges are touching, remove that border
        if ( selTile.yc == yc ) // Tile is in the same row
        {
            if ( selTile.xc == xc ) // Tile is in the same column: tile is already selected!
            {
                removeTile(xc, yc);
                return; // Iterators are now invalid, ensure loop is exited
            }
            else if ( selTile.xc == xc - 1 ) // 'track' is just left of 'tile'
            {
                (u8 &)tile.neighbors &= TileNeighbor::xLeft; // AND 1110, flips off the LEFT edge bit
                (u8 &)selTile.neighbors &= TileNeighbor::xRight; // AND 1011, flips off the RIGHT edge bit
            }
            else if ( selTile.xc == xc + 1 ) // 'track' is just right of 'tile'
            {
                (u8 &)tile.neighbors &= TileNeighbor::xRight; // AND 1011, flips off the RIGHT edge bit
                (u8 &)selTile.neighbors &= TileNeighbor::xLeft; // AND 1110, flips off the LEFT edge bit
            }
        }
        else if ( selTile.xc == xc ) // Tile is in same column
        {
            if ( selTile.yc == yc - 1 ) // 'track' is just above 'tile'
            {
                (u8 &)tile.neighbors &= TileNeighbor::xTop; // AND 1101, flips off the TOP edge bit
                (u8 &)selTile.neighbors &= TileNeighbor::xBottom; // AND 0111, flips off the BOTTOM edge bit
            }
            else if ( selTile.yc == yc + 1 ) // 'track' is just below 'tile'
            {
                (u8 &)tile.neighbors &= TileNeighbor::xBottom; // AND 0111, flips off the BOTTOM edge bit
                (u8 &)selTile.neighbors &= TileNeighbor::xTop; // AND 1101, flips off the TOP edge bit
            }
        }
    }

    selTiles.insert(selTiles.end(), tile);
}

void Selections::addTile(u16 value, u16 xc, u16 yc, TileNeighbor neighbors)
{
    TileNode tile {};
    tile.value = value;
    tile.xc = xc;
    tile.yc = yc;
    tile.neighbors = neighbors;

    selTiles.push_back(tile);
}

void Selections::removeTiles()
{
    selTiles.clear();
}

u16 Selections::getSelectedLocation()
{
    return selectedLocation;
}

void Selections::selectLocation(u16 index)
{
    selectedLocation = index;
    numRecentLocations = 1;
    recentLocations[0] = u8(index);
}

void Selections::selectLocation(s32 clickX, s32 clickY, bool canSelectAnywhere)
{
    size_t numLocations = map.numLocations();
    u16 firstRecentlySelected = NO_LOCATION;
    bool madeSelection = false;
    
    for ( u16 i=1; i<=numLocations; i++ )
    {
        if ( i != selectedLocation && (i != Chk::LocationId::Anywhere || canSelectAnywhere) )
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
                    selectedLocation = i;
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
            selectedLocation = firstRecentlySelected;
            recentLocations[0] = u8(firstRecentlySelected);
            numRecentLocations = 1;
        }
        else // Reset recent locations
        {
            selectedLocation = NO_LOCATION;
            recentLocations[0] = u8(selectedLocation);
            numRecentLocations = 1;
        }
    }
}

void Selections::addUnit(u16 index)
{
    if ( !unitIsSelected(index) )
        selUnits.insert(selUnits.begin(), index);
}

void Selections::removeUnit(u16 index)
{
    auto toErase = std::find(selUnits.begin(), selUnits.end(), index);
    if ( toErase != selUnits.end() )
        selUnits.erase(toErase);
}

void Selections::removeUnits()
{
    selUnits.clear();
}

void Selections::ensureUnitFirst(u16 index)
{
    if ( selUnits.size() > 0 && selUnits[0] != index )
    {
        auto toErase = std::find(selUnits.begin(), selUnits.end(), index);
        if ( toErase != selUnits.end() )
        {
            selUnits.erase(toErase);
            selUnits.insert(selUnits.begin(), index);
        }
    }
}

void Selections::sendUnitSwap(u16 oldIndex, u16 newIndex)
{
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex == newIndex )
            unitIndex = oldIndex | SelSortFlags::Swapped;
        else if ( unitIndex == oldIndex )
            unitIndex = newIndex;
    }
}

void Selections::sendUnitMove(u16 oldIndex, u16 newIndex) // The item is being moved back to its oldIndex from its newIndex
{
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex == newIndex )
            unitIndex = oldIndex | SelSortFlags::Moved;
        else if ( newIndex > unitIndex && oldIndex <= unitIndex ) // The moved unit was somewhere ahead of track and is now behind track
            unitIndex++; // Selected unit index needs to be moved forward
        else if ( newIndex < unitIndex && oldIndex >= unitIndex ) // The moved unit was somewhere behind track and is now ahead of track
            unitIndex--; // Selected unit index needs to be moved backward
    }
}

void Selections::finishUnitSwap()
{
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex & SelSortFlags::Swapped )
            unitIndex &= SelSortFlags::Unswap;
    }
}

void Selections::finishUnitMove()
{
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex & SelSortFlags::Moved )
            unitIndex &= SelSortFlags::Unmove;
    }
}

void Selections::addDoodad(size_t index)
{
    if ( !doodadIsSelected(index) )
        selDoodads.insert(selDoodads.begin(), index);
}

void Selections::removeDoodad(size_t index)
{
    auto toErase = std::find(selDoodads.begin(), selDoodads.end(), index);
    if ( toErase != selDoodads.end() )
        selDoodads.erase(toErase);
}

void Selections::removeDoodads()
{
    selDoodads.clear();
}

void Selections::addSprite(size_t index)
{
    if ( !spriteIsSelected(index) )
        selSprites.insert(selSprites.begin(), index);
}

void Selections::removeSprite(size_t index)
{
    auto toErase = std::find(selSprites.begin(), selSprites.end(), index);
    if ( toErase != selSprites.end() )
        selSprites.erase(toErase);
}

void Selections::removeSprites()
{
    selSprites.clear();
}

void Selections::ensureSpriteFirst(u16 index)
{
    if ( selSprites.size() > 0 && selSprites[0] != size_t(index) )
    {
        auto toErase = std::find(selSprites.begin(), selSprites.end(), size_t(index));
        if ( toErase != selSprites.end() )
        {
            selSprites.erase(toErase);
            selSprites.insert(selSprites.begin(), size_t(index));
        }
    }
}

void Selections::sendSpriteMove(u16 oldIndex, u16 newIndex)
{
    for ( size_t & spriteIndex : selSprites )
    {
        if ( spriteIndex == size_t(newIndex) )
            spriteIndex = oldIndex | SelSortFlags::Moved;
        else if ( size_t(newIndex) > spriteIndex && size_t(oldIndex) <= spriteIndex ) // The moved sprite was somewhere ahead of track and is now behind track
            spriteIndex++; // Selected sprite index needs to be moved forward
        else if ( size_t(newIndex) < spriteIndex && size_t(oldIndex) >= spriteIndex ) // The moved sprite was somewhere behind track and is now ahead of track
            spriteIndex--; // Selected sprite index needs to be moved backward
    }
}

void Selections::finishSpriteMove()
{
    for ( size_t & spriteIndex : selSprites )
    {
        if ( spriteIndex & SelSortFlags::Moved )
            spriteIndex &= SelSortFlags::Unmove;
    }
}

void Selections::addFogTile(u16 xc, u16 yc)
{
    FogTile fogTile {};
    fogTile.xc = xc;
    fogTile.yc = yc;
    fogTile.neighbors = TileNeighbor::All;

    for ( auto & selFogTile : selFogTiles )
    {
        // If tile edges are touching, remove that border
        if ( selFogTile.yc == yc ) // Tile is in the same row
        {
            if ( selFogTile.xc == xc ) // Tile is in the same column: tile is already selected!
            {
                removeFogTile(xc, yc);
                return; // Iterators are now invalid, ensure loop is exited
            }
            else if ( selFogTile.xc == xc - 1 ) // 'track' is just left of 'tile'
            {
                (u8 &)fogTile.neighbors &= TileNeighbor::xLeft; // AND 1110, flips off the LEFT edge bit
                (u8 &)selFogTile.neighbors &= TileNeighbor::xRight; // AND 1011, flips off the RIGHT edge bit
            }
            else if ( selFogTile.xc == xc + 1 ) // 'track' is just right of 'tile'
            {
                (u8 &)fogTile.neighbors &= TileNeighbor::xRight; // AND 1011, flips off the RIGHT edge bit
                (u8 &)selFogTile.neighbors &= TileNeighbor::xLeft; // AND 1110, flips off the LEFT edge bit
            }
        }
        else if ( selFogTile.xc == xc ) // Tile is in same column
        {
            if ( selFogTile.yc == yc - 1 ) // 'track' is just above 'tile'
            {
                (u8 &)fogTile.neighbors &= TileNeighbor::xTop; // AND 1101, flips off the TOP edge bit
                (u8 &)selFogTile.neighbors &= TileNeighbor::xBottom; // AND 0111, flips off the BOTTOM edge bit
            }
            else if ( selFogTile.yc == yc + 1 ) // 'track' is just below 'tile'
            {
                (u8 &)fogTile.neighbors &= TileNeighbor::xBottom; // AND 0111, flips off the BOTTOM edge bit
                (u8 &)selFogTile.neighbors &= TileNeighbor::xTop; // AND 1101, flips off the TOP edge bit
            }
        }
    }

    selFogTiles.push_back(fogTile);
}

void Selections::addFogTile(u16 xc, u16 yc, TileNeighbor neighbors)
{
    FogTile fogTile {};
    fogTile.xc = xc;
    fogTile.yc = yc;
    fogTile.neighbors = neighbors;

    selFogTiles.push_back(fogTile);
}

void Selections::removeFogTile(u16 xc, u16 yc)
{
    auto toRemove = selFogTiles.end();
    for ( auto it = selFogTiles.begin(); it != selFogTiles.end(); ++it )
    {
        // If an edge is matched to the tile being removed, un-match the edge
        if ( it->yc == yc ) // Tile is in the same row
        {
            if ( it->xc == xc - 1 ) (u8 &)it->neighbors |= TileNeighbor::Right; // OR 0100, flips on the RIGHT edge bit
            else if ( it->xc == xc + 1 ) (u8 &)it->neighbors |= TileNeighbor::Left; // OR 0001, flips on the LEFT edge bit
        }
        else if ( it->xc == xc ) // Tile is in the same column
        {
            if ( it->yc == yc - 1 ) (u8 &)it->neighbors |= TileNeighbor::Bottom; // OR 1000, flips on the BOTTOM edge bit
            else if ( it->yc == yc + 1 ) (u8 &)it->neighbors |= TileNeighbor::Top; // OR 0010, flips on the TOP edge bit
        }

        if ( it->xc == xc && it->yc == yc )
            toRemove = it;
    }
    
    if ( toRemove != selFogTiles.end() )
        selFogTiles.erase(toRemove);
}

void Selections::removeFog()
{
    selFogTiles.clear();
}

bool Selections::unitIsSelected(u16 index)
{
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex == index )
            return true;
    }
    return false;
}

bool Selections::doodadIsSelected(size_t index)
{
    for ( size_t doodadIndex : selDoodads )
    {
        if ( doodadIndex == index )
            return true;
    }
    return false;
}

bool Selections::spriteIsSelected(size_t index)
{
    for ( size_t spriteIndex : selSprites )
    {
        if ( spriteIndex == index )
            return true;
    }
    return false;
}

u16 Selections::numUnits()
{
    if ( selUnits.size() < u16_max )
        return (u16)selUnits.size();
    else
        return u16_max;
}

u16 Selections::numSprites()
{
    if ( selSprites.size() < u16_max )
        return (u16)selSprites.size();
    else
        return u16_max;
}

u16 Selections::numUnitsUnder(u16 index)
{
    u16 numUnitsBefore = 0;
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex < index )
            numUnitsBefore++;
    }
    return numUnitsBefore;
}

std::vector<TileNode> & Selections::getTiles()
{
    return selTiles;
}

TileNode Selections::getFirstTile()
{
    TileNode tile;
    tile.xc = 0;
    tile.yc = 0;
    tile.neighbors = TileNeighbor::None;
    tile.value = 0;

    if ( selTiles.size() > 0 )
    {
        tile.xc = selTiles[0].xc;
        tile.yc = selTiles[0].yc;
        tile.neighbors = selTiles[0].neighbors;
        tile.value = selTiles[0].value;
    }

    return tile;
}

std::vector<u16> & Selections::getUnits()
{
    return selUnits;
}

std::vector<size_t> & Selections::getDoodads()
{
    return selDoodads;
}

std::vector<size_t> & Selections::getSprites()
{
    return selSprites;
}

std::vector<FogTile> & Selections::getFogTiles()
{
    return selFogTiles;
}

u16 Selections::getFirstUnit()
{
    if ( selUnits.size() > 0 )
        return selUnits[0];
    else
        return 0;
}

u16 Selections::getFirstDoodad()
{
    if ( selDoodads.size() > 0 )
        return u16(selDoodads[0]);
    else
        return 0;
}

size_t Selections::getFirstSprite()
{
    if ( selSprites.size() > 0 )
        return selSprites[0];
    else
        return 0;
}

u16 Selections::getHighestUnitIndex()
{
    int highestIndex = -1;
    for ( u16 & unitIndex : selUnits )
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
    for ( u16 & unitIndex : selUnits )
    {
        if ( unitIndex < lowestIndex )
            lowestIndex = unitIndex;
    }
    return lowestIndex;
}

size_t Selections::getHighestSpriteIndex()
{
    int highestIndex = -1;
    for ( size_t spriteIndex : selSprites )
    {
        if ( (int)spriteIndex > highestIndex )
            highestIndex = (int)spriteIndex;
    }
    return size_t(highestIndex);
}

size_t Selections::getLowestSpriteIndex()
{
    size_t lowestIndex = std::numeric_limits<size_t>::max();
    for ( size_t spriteIndex : selSprites )
    {
        if ( spriteIndex < lowestIndex )
            lowestIndex = spriteIndex;
    }
    return lowestIndex;
}

void Selections::sortUnits(bool ascending)
{
    if ( ascending )
        std::sort(selUnits.begin(), selUnits.end());
    else // Sort descending
        std::sort(selUnits.begin(), selUnits.end(), std::greater<u16>());
}

void Selections::sortSprites(bool ascending)
{
    if ( ascending )
        std::sort(selSprites.begin(), selSprites.end());
    else // Sort descending
        std::sort(selSprites.begin(), selSprites.end(), std::greater<size_t>());
}
