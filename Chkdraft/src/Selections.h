#ifndef SELECTIONS_H
#define SELECTIONS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
class GuiMap;

enum class LocSelFlags : u8 { North = BIT_0, South = BIT_1, East = BIT_2, West = BIT_3,
    NorthWest = North|West, NorthEast = North|East, SouthWest = South|West, SouthEast = South|East,
    Middle = North | South | East | West, None = 0 };

enum class UnitSortFlags : u16 { Swapped = (u16)BIT_14, Moved = (u16)BIT_15, Unswap = x16BIT_14, Unmove = x16BIT_15 };

enum class TileNeighbor : u8 { Left = BIT_0, Top = BIT_1, Right = BIT_2, Bottom = BIT_3,
    All = Left | Top | Right | Bottom, xLeft = x8BIT_0, xTop = x8BIT_1, xRight = x8BIT_2, xBottom = x8BIT_3, None = 0 };

class TileNode
{
    public:
        u16 value;
        u16 xc;
        u16 yc;
        TileNeighbor neighbors;
        TileNode() : value(0), xc(0), yc(0), neighbors(TileNeighbor::All) { }
};

class Selections
{
    public:

        Selections(GuiMap &map);
        ~Selections();

        void setStartDrag(s32 x, s32 y);
        void setEndDrag(s32 x, s32 y);
        void setDrags(s32 x, s32 y);
        POINT& getStartDrag() { return startDrag; }
        POINT& getEndDrag() { return endDrag; }
        void setMoved() { moved = true; }
        void resetMoved() { moved = false; }
        bool hasMoved() { return moved; }
        bool startEqualsEndDrag() { return startDrag.x == endDrag.x && startDrag.y == endDrag.y; }
        void sortDragPoints() { AscendingOrder(startDrag.x, endDrag.x); AscendingOrder(startDrag.y, endDrag.y); }

        void addTile(u16 value, u16 xc, u16 yc);
        void addTile(u16 value, u16 xc, u16 yc, TileNeighbor neighbors);
        void removeTile(TileNode* &tile);
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
        void ensureFirst(u16 index); // Moves the unit @ index
        void sendSwap(u16 oldIndex, u16 newIndex);
        void sendMove(u16 oldIndex, u16 newIndex);
        void finishSwap();
        void finishMove();

        bool unitIsSelected(u16 index);
        bool hasUnits() { return selUnits.size() > 0; }
        bool hasTiles() { return selTiles.size() > 0; }
        u16 numUnits();
        u16 numUnitsUnder(u16 index);

        std::vector<TileNode> &getTiles();
        TileNode getFirstTile();
        std::vector<u16> &getUnits();
        u16 getFirstUnit();
        u16 getLastUnit();
        u16 getHighestIndex();
        u16 getLowestIndex();

        void sortUnits(bool ascending);

        bool selectionAreaIsNull() { return startDrag.x == -1 && startDrag.y == -1; }

    private:

        GuiMap &map; // Reference to the map who has the selections described in this class

        POINT startDrag;
        POINT endDrag;

        std::vector<u16> selUnits;
        std::vector<TileNode> selTiles;

        u16 selectedLocation;
        u8 numRecentLocations;
        u8 recentLocations[255];
        LocSelFlags locSelFlags;
        u8 locationFlags;

        bool moved;
};

#endif
