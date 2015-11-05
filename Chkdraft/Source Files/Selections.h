#ifndef SELECTIONS_H
#define SELECTIONS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

#define FLAG_SWAPPED BIT_15
#define FLAG_MOVED BIT_15
#define UNSWAP_FLAG (~FLAG_SWAPPED)
#define UNMOVE_FLAG (~FLAG_MOVED)

class TileNode
{
	public:
		u16 value;
		u16 xc;
		u16 yc;

		u8 neighbors;
			#define NEIGHBOR_LEFT	BIT_0
			#define NEIGHBOR_TOP	BIT_1
			#define NEIGHBOR_RIGHT	BIT_2
			#define NEIGHBOR_BOTTOM BIT_3

			#define ALL_NEIGHBORS (NEIGHBOR_LEFT|NEIGHBOR_TOP|NEIGHBOR_RIGHT|NEIGHBOR_BOTTOM)

			#define NEIGHBOR_xLEFT	 (~NEIGHBOR_LEFT)
			#define NEIGHBOR_xTOP	 (~NEIGHBOR_TOP)
			#define NEIGHBOR_xRIGHT	 (~NEIGHBOR_RIGHT)
			#define NEIGHBOR_xBOTTOM (~NEIGHBOR_BOTTOM)

		TileNode* next;

		TileNode() : value(0), xc(0), yc(0), neighbors(ALL_NEIGHBORS), next(nullptr) { }
};

class SELECTIONS
{
	public:

		SELECTIONS();
		~SELECTIONS();

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
		void addTile(u16 value, u16 xc, u16 yc, u8 neighbors);
		void removeTile(TileNode* &tile);
		void removeTile(u16 xc, u16 yc);
		void removeTiles();

		u16 getSelectedLocation(); // NO_LOCATION if none are selected
		void selectLocation(u16 index); // 0-based index, NO_LOCATION to deselect any selected locations
		void selectLocation(s32 clickX, s32 clickY, Scenario* chk, bool canSelectAnywhere); // Based on map click
		void setLocationFlags(u8 flags) { locationFlags = flags; }
		u8 getLocationFlags() { return locationFlags; }
		
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

		POINT startDrag;
		POINT endDrag;

		std::vector<u16> selUnits;
		std::vector<TileNode> selTiles;

		u16 selectedLocation;
		u8 numRecentLocations;
		u8 recentLocations[255];

		u8 locationFlags;
			#define LOC_SEL_NOTHING	0
			#define LOC_SEL_NORTH	BIT_0
			#define LOC_SEL_SOUTH	BIT_1
			#define LOC_SEL_EAST	BIT_2
			#define LOC_SEL_WEST	BIT_3
			#define LOC_SEL_MIDDLE (LOC_SEL_NORTH|LOC_SEL_SOUTH|LOC_SEL_EAST|LOC_SEL_WEST)
			#define LOC_SEL_NW (LOC_SEL_NORTH|LOC_SEL_WEST)
			#define LOC_SEL_NE (LOC_SEL_NORTH|LOC_SEL_EAST)
			#define LOC_SEL_SW (LOC_SEL_SOUTH|LOC_SEL_WEST)
			#define LOC_SEL_SE (LOC_SEL_SOUTH|LOC_SEL_EAST)

		bool moved;
};

#endif