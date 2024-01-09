#ifndef SELECTIONS_H
#define SELECTIONS_H
#include "../CommonFiles/CommonFiles.h"
#include "../../MappingCoreLib/MappingCore.h"
class GuiMap;

enum_t(LocSelFlags, u8, { North = BIT_0, South = BIT_1, East = BIT_2, West = BIT_3,
    NorthWest = North|West, NorthEast = North|East, SouthWest = South|West, SouthEast = South|East,
    Middle = North | South | East | West, None = 0 });


enum_t(SelSortFlags, u16, { Swapped = (u16)BIT_14, Moved = (u16)BIT_15, Unswap = x16BIT_14, Unmove = x16BIT_15 });


enum_t(TileNeighbor, u8, { Left = BIT_0, Top = BIT_1, Right = BIT_2, Bottom = BIT_3,
    All = Left | Top | Right | Bottom, xLeft = x8BIT_0, xTop = x8BIT_1, xRight = x8BIT_2, xBottom = x8BIT_3, None = 0 });

struct TileNode
{
    u16 value = 0;
    u16 xc = 0;
    u16 yc = 0;
    TileNeighbor neighbors = TileNeighbor::All;
};

struct FogTile
{
    u16 xc = 0;
    u16 yc = 0;
    TileNeighbor neighbors = TileNeighbor::All;
};

class Selections
{
    public:

        Selections(GuiMap & map);
        virtual ~Selections();

        void setStartDrag(s32 x, s32 y);
        void setEndDrag(s32 x, s32 y);
        void setDrags(s32 x, s32 y);
        void snapDrags(s32 xInterval, s32 yInterval, bool nonZeroSnap);
        void snapEndDrag(s32 xInterval, s32 yInterval);
        POINT & getStartDrag() { return startDrag; }
        POINT & getEndDrag() { return endDrag; }
        void setMoved() { moved = true; }
        void resetMoved() { moved = false; }
        bool hasMoved() { return moved; }
        bool startEqualsEndDrag() { return startDrag.x == endDrag.x && startDrag.y == endDrag.y; }
        void sortDragPoints() { ascendingOrder(startDrag.x, endDrag.x); ascendingOrder(startDrag.y, endDrag.y); }
        void clear();

        void addTile(u16 value, u16 xc, u16 yc);
        void addTile(u16 value, u16 xc, u16 yc, TileNeighbor neighbors);
        void removeTile(TileNode* & tile);
        void removeTile(u16 xc, u16 yc);
        void removeTiles();

        u16 getSelectedLocation(); // NO_LOCATION if none are selected
        void selectLocation(u16 index); // 0-based index, NO_LOCATION to deselect any selected locations
        void selectLocation(s32 clickX, s32 clickY, bool canSelectAnywhere); // Based on map click
        void setLocationFlags(LocSelFlags flags) { locSelFlags = flags; }
        LocSelFlags getLocationFlags() { return locSelFlags; }
        
        void addUnit(u16 index);
        void removeUnit(u16 index);
        void removeUnits();
        void ensureUnitFirst(u16 index); // Moves the unit @ index
        void sendUnitSwap(u16 oldIndex, u16 newIndex);
        void sendUnitMove(u16 oldIndex, u16 newIndex);
        void finishUnitSwap();
        void finishUnitMove();

        void addDoodad(size_t index);
        void removeDoodad(size_t index);
        void removeDoodads();

        void addSprite(size_t index);
        void removeSprite(size_t index);
        void removeSprites();
        void ensureSpriteFirst(u16 index); // Moves the sprite @ index
        void sendSpriteMove(u16 oldIndex, u16 newIndex);
        void finishSpriteMove();
        
        void addFogTile(u16 xc, u16 yc);
        void addFogTile(u16 xc, u16 yc, TileNeighbor neighbors);
        void removeFogTile(u16 xc, u16 yc);
        void removeFog();

        bool unitIsSelected(u16 index);
        bool doodadIsSelected(size_t doodadIndex);
        bool spriteIsSelected(size_t spriteIndex);
        bool hasUnits() { return selUnits.size() > 0; }
        bool hasDoodads() { return selDoodads.size() > 0; }
        bool hasTiles() { return selTiles.size() > 0; }
        bool hasSprites() { return selSprites.size() > 0; }
        bool hasFogTiles() { return selFogTiles.size() > 0; }
        u16 numUnits();
        u16 numSprites();
        u16 numUnitsUnder(u16 index);

        std::vector<TileNode> & getTiles();
        TileNode getFirstTile();
        std::vector<u16> & getUnits();
        std::vector<size_t> & getDoodads();
        std::vector<size_t> & getSprites();
        std::vector<FogTile> & getFogTiles();
        u16 getFirstUnit();
        u16 getFirstDoodad();
        size_t getFirstSprite();
        u16 getLastUnit();
        u16 getHighestUnitIndex();
        u16 getLowestUnitIndex();
        size_t getHighestSpriteIndex();
        size_t getLowestSpriteIndex();
        
        void sortUnits(bool ascending);
        void sortSprites(bool ascending);

        bool selectionAreaIsNull() { return startDrag.x == -1 && startDrag.y == -1; }

    private:

        GuiMap & map; // Reference to the map who has the selections described in this class

        POINT startDrag;
        POINT endDrag;

        std::vector<u16> selUnits;
        std::vector<size_t> selDoodads;
        std::vector<size_t> selSprites;
        std::vector<TileNode> selTiles;
        std::vector<FogTile> selFogTiles {};

        u16 selectedLocation;
        u8 numRecentLocations;
        u8 recentLocations[Chk::TotalLocations];
        LocSelFlags locSelFlags;
        u8 locationFlags;

        bool moved;
};

#endif