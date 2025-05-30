#ifndef SELECTIONS_H
#define SELECTIONS_H
#include <common_files/structs.h>
#include <mapping_core/basics.h>
#include <mapping_core/chk.h>
#include <mapping_core/sc.h>
class GuiMap;

using TileNeighbor = Sc::Terrain::TileNeighbor;

enum_t(LocSelFlags, u8, { North = BIT_0, South = BIT_1, East = BIT_2, West = BIT_3,
    NorthWest = North|West, NorthEast = North|East, SouthWest = South|West, SouthEast = South|East,
    Middle = North | South | East | West, None = 0 });

enum_t(SelSortFlags, u16, { Swapped = (u16)BIT_14, Moved = (u16)BIT_15, Unswap = x16BIT_14, Unmove = x16BIT_15 });

struct RenderedSelTiles
{
    std::size_t xBegin = 0;
    std::size_t xEnd = 0;
    std::size_t yBegin = 0;
    std::size_t yEnd = 0;
    std::vector<std::optional<TileNeighbor>> tiles;
};

class Selections
{
    GuiMap & map; // Reference to the map who has the selections described in this class

    u8 numRecentLocations = 0;
    u8 recentLocations[Chk::TotalLocations] {};
    LocSelFlags locSelFlags = LocSelFlags::None;
    u8 locationFlags = 0;

public:
    bool moved = false;

    point startDrag { -1, -1 };
    point endDrag { -1, -1 };

    std::vector<size_t> doodads {};

    RenderedSelTiles renderTiles; // TODO: This should be part of some kind of common graphics data, not here
    RenderedSelTiles renderFogTiles; // TODO: This should be part of some kind of common graphics data, not here

    Selections(GuiMap & map) : map(map) {}

    void setDrags(s32 x, s32 y);
    void snapDrags(s32 xInterval, s32 yInterval, bool nonZeroSnap);
    void snapEndDrag(s32 xInterval, s32 yInterval);
    bool startEqualsEndDrag() { return startDrag.x == endDrag.x && startDrag.y == endDrag.y; }
    void sortDragPoints() { ascendingOrder(startDrag.x, endDrag.x); ascendingOrder(startDrag.y, endDrag.y); }
    void clear();

    u16 getSelectedLocation(); // NO_LOCATION if none are selected
    void selectLocation(u16 index); // 0-based index, NO_LOCATION to deselect any selected locations
    void selectLocation(s32 clickX, s32 clickY, bool canSelectAnywhere); // Based on map click
    void setLocationFlags(LocSelFlags flags) { locSelFlags = flags; }
    LocSelFlags getLocationFlags() { return locSelFlags; }
    bool selFlagsIndicateInside() const;
    void removeLocations();

    void addDoodad(size_t index);
    void removeDoodad(size_t index);
    void removeDoodads();

    bool unitIsSelected(u16 index);
    bool doodadIsSelected(size_t doodadIndex);
    bool spriteIsSelected(size_t spriteIndex);
    bool hasUnits();
    bool hasDoodads() { return doodads.size() > 0; }
    bool hasTiles();
    bool hasSprites();
    bool hasFogTiles();
    u16 numUnits();
    u16 numSprites();
    u16 numUnitsUnder(u16 index);

    u16 getFirstUnit();
    u16 getFirstDoodad();
    size_t getFirstSprite();
    u16 getHighestUnitIndex();
    u16 getLowestUnitIndex();
    size_t getHighestSpriteIndex();
    size_t getLowestSpriteIndex();
        
    void sortUnits(bool ascending);
    void sortSprites(bool ascending);

    bool selectionAreaIsNull() { return startDrag.x == -1 && startDrag.y == -1; }
};

#endif