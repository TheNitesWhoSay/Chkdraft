#include "Selections.h"

SELECTIONS::SELECTIONS() : headTile(nullptr), headUnit(nullptr), moved(false), numRecentLocations(0), locationFlags(0)
{
	startDrag.x = -1;
	startDrag.y = -1;
	endDrag.x = -1;
	endDrag.y = -1;
}

SELECTIONS::~SELECTIONS()
{
	removeTiles();
	removeUnits();
}

void SELECTIONS::setStartDrag(s32 x, s32 y)
{
	startDrag.x = x;
	startDrag.y = y;
}

void SELECTIONS::setEndDrag(s32 x, s32 y)
{
	endDrag.x = x;
	endDrag.y = y;
}

void SELECTIONS::setDrags(s32 x, s32 y)
{
	startDrag.x = x;
	startDrag.y = y;
	endDrag.x = x;
	endDrag.y = y;
}

void SELECTIONS::removeTile(TileNode* &tile)
{
	if ( headTile->next == nullptr ) // One tile is selected
	{
		headTile = nullptr;
	}
	else // Multiple tiles selected, will need to process
	{
		TileNode* track = headTile,
				* prior = nullptr;

		while ( track != nullptr )
		{
			// If an edge is matched to the tile being removed, un-match the edge
			if ( track->yc == tile->yc ) // Tile is in the same row
			{
				if		( track->xc == tile->xc-1 )	track->neighbors |= NEIGHBOR_RIGHT;	// OR 0100, flips on the RIGHT edge bit
				else if ( track->xc == tile->xc+1 )	track->neighbors |= NEIGHBOR_LEFT ;	// OR 0001, flips on the LEFT edge bit
			}
			else if ( track->xc == tile->xc ) // Tile is in the same column
			{
				if		( track->yc == tile->yc-1 )	track->neighbors |= NEIGHBOR_BOTTOM; // OR 1000, flips on the BOTTOM edge bit
				else if ( track->yc == tile->yc+1 )	track->neighbors |= NEIGHBOR_TOP   ; // OR 0010, flips on the TOP edge bit
			}

			if ( track->next == tile )
				prior = track; // Record the pointer of the tile prior to the tile being removed
			
			track = track->next; // Move to the next tile
		}
	
		if ( prior == nullptr ) // Removing the headTile
		{
			headTile = headTile->next;
		}
		else
		{
			prior->next = tile->next; // Take 'tile' out of the list
		}
	}
	delete tile; // Finally
}

void SELECTIONS::addTile(u16 value, u16 xc, u16 yc)
{
	TileNode* tile = new TileNode;
	tile->xc = xc;
	tile->yc = yc;
	tile->neighbors = 0xF;
	tile->value = value;
	tile->next = headTile;

	if ( headTile != nullptr ) // Tiles selected, will need to process
	{
		TileNode* track = headTile;
		while ( track != nullptr )
		{
			// If tile edges are touching, remove that border
			if ( track->yc == yc ) // Tile is in the same row
			{
				if ( track->xc == xc ) // Tile is in the same column: tile is already selected! 
				{
					removeTile(track);
					return;
				}
				else if	( track->xc == xc-1 ) // 'track' is just left of 'tile'
				{
					tile ->neighbors &= NEIGHBOR_xLEFT  ; // AND 1110, flips off the LEFT edge bit
					track->neighbors &= NEIGHBOR_xRIGHT ; // AND 1011, flips off the RIGHT edge bit
				}
				else if ( track->xc == xc+1 ) // 'track' is just right of 'tile'
				{
					tile ->neighbors &= NEIGHBOR_xRIGHT ; // AND 1011, flips off the RIGHT edge bit
					track->neighbors &= NEIGHBOR_xLEFT  ; // AND 1110, flips off the LEFT edge bit
				}
			}
			else if ( track->xc == xc ) // Tile is in the same column
			{
				if		( track->yc == yc-1 ) // 'track' is just above 'tile'
				{
					tile ->neighbors &= NEIGHBOR_xTOP   ; // AND 1101, flips off the TOP edge bit
					track->neighbors &= NEIGHBOR_xBOTTOM; // AND 0111, flips off the BOTTOM edge bit
				}
				else if ( track->yc == yc+1 ) // 'track' is just below 'tile'
				{
					tile ->neighbors &= NEIGHBOR_xBOTTOM; // AND 0111, flips off the BOTTOM edge bit
					track->neighbors &= NEIGHBOR_xTOP   ; // AND 1101, flips off the TOP edge bit
				}
			}
			track = track->next;
		}
	}
	headTile = tile;
}

void SELECTIONS::addTile(u16 value, u16 xc, u16 yc, u8 neighbors)
{
	TileNode* tile = new TileNode;
	tile->value = value;
	tile->xc = xc;
	tile->yc = yc;
	tile->neighbors = neighbors;
	tile->next = headTile;

	headTile = tile;
}

void SELECTIONS::removeTiles()
{
	TileNode* track = headTile,
			* next;

	while ( track != nullptr )
	{
		next = track->next;
		delete track;
		track = next;
	}

	headTile = nullptr;
}

u16 SELECTIONS::getSelectedLocation()
{
	return selectedLocation;
}

void SELECTIONS::selectLocation(u16 index)
{
	selectedLocation = index;
	numRecentLocations = 1;
	recentLocations[0] = u8(index);
}

void SELECTIONS::selectLocation(s32 clickX, s32 clickY, Scenario* chk, bool canSelectAnywhere)
{
	buffer& MRGN = chk->MRGN();
	if ( MRGN.exists() )
	{
		ChkLocation* loc;
		u16 numLocations = u16(MRGN.size()/CHK_LOCATION_SIZE);
		u16 firstRecentlySelected = NO_LOCATION;
		bool madeSelection = false;
	
		for ( u16 i=0; i<numLocations; i++ )
		{
			if ( i != selectedLocation && ( i != 63 || canSelectAnywhere ) && chk->getLocation(loc, i) )
			{
				s32 locLeft = std::min(loc->xc1, loc->xc2),
					locRight = std::max(loc->xc1, loc->xc2),
					locTop = std::min(loc->yc1, loc->yc2),
					locBottom = std::max(loc->yc1, loc->yc2);

				if ( clickX >= locLeft && clickX <= locRight &&
					 clickY >= locTop && clickY <= locBottom    )
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
						if ( numRecentLocations < 255 )
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
}

void SELECTIONS::addUnit(u16 index)
{
	if ( !unitIsSelected(index) )
	{
		UnitNode* unitNode = new UnitNode;
		unitNode->index = index;
		unitNode->next = headUnit;
		headUnit = unitNode;
	}
}

void SELECTIONS::removeUnit(u16 index)
{
	if ( headUnit != nullptr ) // if there is a unit to remove
	{
		UnitNode* curr = headUnit;
		if ( curr->index == index ) // removing head
		{
			curr = curr->next; // set curr to the item after head
			delete headUnit;
			headUnit = curr; // set head to the item that was after head
		}
		else if ( curr->next != nullptr ) // removing something after head
		{
			while ( curr->next != nullptr )
			{
				if ( curr->next->index == index )
				// curr is the node prior to the node you're removing
				{
					UnitNode* temp = curr->next; // save curr->next for deleting
					curr->next = curr->next->next; // remove curr->next from the list
					delete temp;
				}
				else
					curr = curr->next;
			}
		}
	}
}

void SELECTIONS::removeUnits()
{
	UnitNode* temp;
	while ( headUnit != nullptr )
	{
		temp = headUnit;
		headUnit = headUnit->next;
		delete temp;
	}
	Invariant( headUnit == nullptr );
}

void SELECTIONS::ensureFirst(u16 index)
{
	if ( headUnit != nullptr )
	{
		if ( headUnit->index != index )
		{
			UnitNode* track = headUnit,
					* prev = nullptr;

			while ( track != nullptr )
			{
				if ( track->index == index )
				{
					Invariant( prev != nullptr );
					prev->next = track->next; // remove node from the list
					track->next = headUnit; // set nodes next to headUnit
					headUnit = track; // set headUnit to node
					return;
				}
				prev = track;
				track = track->next;
			}
		}
	}
}

void SELECTIONS::sendLightMove(u16 oldIndex, u16 newIndex)
{
	UnitNode* track = headUnit;
	while ( track != nullptr )
	{
		if ( oldIndex > track->index && newIndex <= track->index ) // The moved unit was somewhere ahead of track and is now behind track
			track->index ++; // Selected unit index needs to be moved forward
		else if ( oldIndex < track->index && newIndex >= track->index ) // The moved unit was somewhere behind track and is now ahead of track
			track->index --; // Selected unit index needs to be moved backward

		track = track->next;
	}

	track = headUnit;
	while ( track != nullptr )
		track = track->next;
}

void SELECTIONS::sendSwap(u16 oldIndex, u16 newIndex)
{
	UnitNode* track = headUnit;
	while ( track != nullptr )
	{
		if ( track->index == newIndex )
			track->index = oldIndex|FLAG_SWAPPED;
		else if ( track->index == oldIndex )
			track->index = newIndex;

		track = track->next;
	}
}

void SELECTIONS::sendMove(u16 oldIndex, u16 newIndex) // The item is being moved back to its oldIndex from its newIndex
{
	UnitNode* track = headUnit;
	while ( track != nullptr )
	{
		if ( track->index == newIndex )
			track->index = oldIndex|FLAG_MOVED;
		else if ( newIndex > track->index && oldIndex <= track->index ) // The moved unit was somewhere ahead of track and is now behind track
			track->index ++; // Selected unit index needs to be moved forward
		else if ( newIndex < track->index && oldIndex >= track->index ) // The moved unit was somewhere behind track and is now ahead of track
			track->index --; // Selected unit index needs to be moved backward

		track = track->next;
	}
}

void SELECTIONS::finishSwap()
{
	UnitNode* track = headUnit;
	while ( track != nullptr )
	{
		if ( track->index & FLAG_SWAPPED )
			track->index &= UNSWAP_FLAG;

		track = track->next;
	}
}

void SELECTIONS::finishMove()
{
	UnitNode* track = headUnit;
	while ( track != nullptr )
	{
		if ( track->index & FLAG_MOVED )
			track->index &= UNMOVE_FLAG;

		track = track->next;
	}
}

bool SELECTIONS::unitIsSelected(u16 index)
{
	UnitNode* curr = headUnit;
	while ( curr != nullptr )
	{
		if ( curr->index == index )
			return true;

		curr = curr->next;
	}
	return false;
}

u16 SELECTIONS::numUnits()
{
	UnitNode* curr = headUnit;
	u16 numberOfUnits = 0;
	while ( curr != nullptr )
	{
		numberOfUnits ++;
		curr = curr->next;
	}
	return numberOfUnits;
}

u16 SELECTIONS::numUnitsUnder(u16 index)
{
	UnitNode* curr = headUnit;
	u16 numUnitsBefore = 0;
	while ( curr != nullptr )
	{
		if ( curr->index < index )
			numUnitsBefore ++;

		curr = curr->next;
	}
	return numUnitsBefore;
}

UnitNode* SELECTIONS::getLastUnit()
{
	UnitNode* curr = headUnit;
	if ( curr != nullptr )
	{
		while ( curr->next != nullptr )
			curr = curr->next;
	}
	return curr;
}

u16 SELECTIONS::getHighestIndex()
{
	UnitNode* curr = headUnit;
	int highestIndex = -1;
	while ( curr != nullptr )
	{
		if ( (int)curr->index > highestIndex )
			highestIndex = (int)curr->index;

		curr = curr->next;
	}
	return (u16)highestIndex;
}

void SELECTIONS::sortUnits(bool ascending)
	// Selection sort for now, perhaps something better later
{
	UnitNode* first = headUnit,
			* track = first,
			* leader = first;

	if ( ascending )
	{
		// Find lowest & update headUnit
		while ( track != nullptr )
		{
			if ( track->index < leader->index )
				leader = track;

			track = track->next;
		}
		// Swap lowest & headUnit
		u16 temp = leader->index;
		leader->index = headUnit->index;
		headUnit->index = temp;

		first = first->next;

		// Find the rest
		while ( first != nullptr )
		{
			track = first;
			leader = first;
			while ( track != nullptr )
			{
				if ( track->index < leader->index )
					leader = track;

				track = track->next;
			}
			temp = leader->index;
			leader->index = first->index;
			first->index = temp;

			first = first->next;
		}
	}
	else // Sort descending
	{
		// Find highest & update headUnit
		while ( track != nullptr )
		{
			if ( track->index > leader->index )
				leader = track;

			track = track->next;
		}
		// Swap highest & headUnit
		u16 temp = leader->index;
		leader->index = headUnit->index;
		headUnit->index = temp;

		first = first->next;

		// Find the rest
		while ( first != nullptr )
		{
			track = first;
			leader = first;
			while ( track != nullptr )
			{
				if ( track->index > leader->index )
					leader = track;

				track = track->next;
			}
			temp = leader->index;
			leader->index = first->index;
			first->index = temp;

			first = first->next;
		}
	}
}
