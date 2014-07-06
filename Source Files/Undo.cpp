#include "Undo.h"
#include "Common Files/CommonFiles.h"

extern HWND hMain; // For notifying unsaved changes
bool ChangesLocked(u16 mapId);

UndoNode::~UndoNode()
{
	UndoComponent* track = (UndoComponent*)head,
				 * next;

	while ( track != nullptr )
	{
		next = (UndoComponent*)track->next;
		delete track;
		track = next;
	}
}


UNDOS::UNDOS() : headUndo(nullptr), lastUndo(nullptr), headRedo(nullptr), upcomingUndo(nullptr), rootTypes(0), mapId(0)
{
	startNext(0);
}

UNDOS::~UNDOS()
{
	clip(headUndo, ALL_UNDO_REDOS);
	clip(headRedo, ALL_UNDO_REDOS);

	if ( upcomingUndo != nullptr )
		delete upcomingUndo;
}

bool UNDOS::clipUndos(u32 type)
{
	return clip(headUndo, type);
}

bool UNDOS::clipRedos(u32 type)
{
	return clip(headRedo, type);
}

bool UNDOS::startNext(u32 type)
{
	if ( upcomingUndo != nullptr )
	{
		if ( upcomingUndo->head != nullptr ) // Node has items
		{ // Add upcomingUndo onto the undo stack
			if ( upcomingUndo->flags & UNDO_UNIT )
				clipRedos(UNDO_UNIT);
			else
				clipRedos(upcomingUndo->flags);

			upcomingUndo->next = headUndo;
			headUndo = upcomingUndo;
			upcomingUndo = nullptr;
			
			// ### An undo has been added at this precise moment, flag changes here
			if ( (rootTypes&(headUndo->flags&UNDO_TYPE)) == 0 && ChangesLocked(mapId) == false )
			{
				rootTypes |= (headUndo->flags&UNDO_TYPE);
				headUndo->flags |= UNDO_CHANGE_ROOT;
				SendMessage(hMain, FLAG_UNDOABLE_CHANGE, (WPARAM)mapId, NULL); // Notify that there has been a change
			}

			return startNext(type); // Call again to create a new undo
		}
		else // Node does not have items
		{ // Switch flags to specified type
			upcomingUndo->flags = type;
			return true;
		}
	}
	else // upcomingUndo == nullptr
	{ // Attempt to allocate
		try {
			upcomingUndo = new UndoNode();
		} catch ( std::bad_alloc ) {

			if ( lastUndo != nullptr ) // More undos can be deleted
			{
				deleteNode(lastUndo);
				return startNext(type);
			}
			else // No more undos stacked
				return false; // No room for more undos/can't make room
		}
		return true;
	}
}

bool UNDOS::addUndoTile(u16 xc, u16 yc, u16 value)
{
	UndoTile* tile;

	try {
		tile = new UndoTile(nullptr, xc, yc, value);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_TERRAIN, tile);
}

bool UNDOS::addUndoUnitDel(u16 index, ChkUnit* unitRef)
{
	UndoUnitDel* unit;

	try {
		unit = new UndoUnitDel(index, unitRef);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_UNIT_DEL, unit);
}

bool UNDOS::addUndoUnitCreate(u16 index)
{
	UndoUnitCreate* unit;

	try {
		unit = new UndoUnitCreate(index);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_UNIT_CREATE, unit);
}

bool UNDOS::addUndoUnitSwap(u16 oldIndex, u16 newIndex)
{
	UndoUnitMove* unit;

	try {
		unit = new UndoUnitMove(oldIndex, newIndex);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_UNIT_SWAP, unit);
}

bool UNDOS::addUndoUnitMove(u16 oldIndex, u16 newIndex)
{
	UndoUnitMove* unit;

	try {
		unit = new UndoUnitMove(oldIndex, newIndex);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_UNIT_MOVE, unit);
}

bool UNDOS::addUndoUnitChange(u16 index, u8 field, u32 data)
{
	UndoUnitChange* unit;
	
	try {
		unit = new UndoUnitChange(index, field, data);
	} catch ( std::bad_alloc ) { return false; }
	
	return addToUndo(UNDO_UNIT_CHANGE, unit);
}

bool UNDOS::addUndoUnitMoveToHeader(u16 numDeletes, u16 numCreates)
{
	UndoUnitMoveToHeader* header;

	try {
		header = new UndoUnitMoveToHeader(numDeletes, numCreates);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_UNIT_MOVETO, header);
}

bool UNDOS::addUndoLocationCreate(u16 index)
{
	UndoLocationCreate* location;

	try {
		location = new UndoLocationCreate(index);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_LOCATION_CREATE, location);
}

bool UNDOS::addUndoLocationDel(u16 index, ChkLocation* locationRef, bool isExtended, std::string locName)
{
	UndoLocationDel* location;

	try {
		location = new UndoLocationDel(index, locationRef, isExtended, locName);
	} catch ( std::bad_alloc ) { return false; }
	
	return addToUndo(UNDO_LOCATION_DEL, location);
}

bool UNDOS::addUndoLocationMove(u16 locationIndex, s32 xChange, s32 yChange)
{
	UndoLocationMove* location;

	try {
		location = new UndoLocationMove(locationIndex, xChange, yChange);
	} catch ( std::bad_alloc ) { return false; }
	
	return addToUndo(UNDO_LOCATION_MOVE, location);
}

bool UNDOS::addUndoLocationChange(u16 locationIndex, u8 field, u32 data)
{
	UndoLocationChange* loc;

	try {
		loc = new UndoLocationChange(locationIndex, field, data);
	} catch ( std::bad_alloc ) { return false; }

	return addToUndo(UNDO_LOCATION_CHANGE, loc);
}

void UNDOS::doUndo(u32 type, Scenario* chk, SELECTIONS& sel)
{
	UndoNode* currNode = popUndo(type);

	if ( currNode != nullptr )
	{
		// ### An undo is about to be processed
		if ( (currNode->flags&UNDO_CHANGE_ROOT) == UNDO_CHANGE_ROOT )
		{
			currNode->flags &= (~UNDO_CHANGE_ROOT); // Take off the change root flag
			rootTypes &= (~(currNode->flags&UNDO_TYPE)); // Take off the root type flags
			if ( rootTypes == 0 )
				SendMessage(hMain, UNFLAG_UNDOABLE_CHANGES, mapId, NULL); // Notify that there is no longer a net undoable net change
			else if ( ChangesLocked(mapId) ) // Changes have been locked, flush
				flushRoots();
		}
		else if ( !ChangesLocked(mapId) && (rootTypes&(currNode->flags&UNDO_TYPE)) == 0 ) // No roots of this type
		{
			rootTypes |= (currNode->flags&UNDO_TYPE);
			currNode->flags |= UNDO_CHANGE_ROOT;
			SendMessage(hMain, FLAG_UNDOABLE_CHANGE, (WPARAM)mapId, NULL);
		}

		flipNode(currNode->flags, chk, sel, currNode);
		currNode->next = headRedo;
		headRedo = currNode;
	}
}

void UNDOS::doRedo(u32 type, Scenario* chk, SELECTIONS& sel)
{
	UndoNode* currNode = popRedo(type);

	if ( currNode != nullptr )
	{
		flipNode(currNode->flags, chk, sel, currNode);
		currNode->next = headUndo;
		headUndo = currNode;

		// ### A redo has been converted to an undo
		if ( ChangesLocked(mapId) == false )
		{
			if ( (rootTypes&(currNode->flags&UNDO_TYPE)) == 0 ) // The redo is not a root
			{
				rootTypes |= (currNode->flags&UNDO_TYPE);
				currNode->flags |= UNDO_CHANGE_ROOT;
				SendMessage(hMain, FLAG_UNDOABLE_CHANGE, (WPARAM)mapId, NULL); // Notify that there has been a change
			}
			else // matching root types
			{
				currNode->flags &= (~UNDO_CHANGE_ROOT);
				rootTypes &= (~(currNode->flags&UNDO_TYPE));
				if ( rootTypes == 0 )
					SendMessage(hMain, UNFLAG_UNDOABLE_CHANGES, (WPARAM)mapId, NULL);
			}
		}
	}
}

void UNDOS::flushRoots()
{
	if ( rootTypes > 0 )
	{
		UndoNode* curr = headUndo;
		while ( curr != nullptr && rootTypes > 0 )
		{
			if ( curr->flags&UNDO_CHANGE_ROOT )
			{
				curr->flags &= (~UNDO_CHANGE_ROOT);
				rootTypes &= (~(curr->flags&UNDO_TYPE));
			}
			curr = curr->next;
		}

		curr = headRedo;
		while ( curr != nullptr && rootTypes > 0 )
		{
			if ( curr->flags&UNDO_CHANGE_ROOT )
			{
				curr->flags &= (~UNDO_CHANGE_ROOT);
				rootTypes &= (~(curr->flags&UNDO_TYPE));
			}
			curr = curr->next;
		}
	}
}

void UNDOS::setMapId(u16 mapId)
{
	this->mapId = mapId;
}

void UNDOS::flipNode(u32 flags, Scenario* chk, SELECTIONS& sel, UndoNode* node)
{
	switch ( flags&UNDO_TYPE )
	{
		case UNDO_TERRAIN:
			{
				buffer& MTXM = chk->MTXM(),
					  & TILE = chk->TILE();

				UndoTile* currTile = (UndoTile*)node->head;
				u16 width = chk->DIM().get<u16>(2);
				u32 pos = 0;

				while ( currTile != nullptr )
				{
					pos = (currTile->yc*width+currTile->xc)*2;
					u16 tempVal = MTXM.get<u16>(pos);
					MTXM.replace<u16>(pos, currTile->value);
					TILE.replace<u16>(pos, currTile->value);
					currTile->value = tempVal;
					currTile = currTile->next;
				}
			}
			break;
		case UNDO_UNIT_CREATE:
			{
				node->flags &= (~UNDO_UNIT_CREATE);
				node->flags |= UNDO_UNIT_DEL;
				buffer& UNIT = chk->UNIT();

				UndoUnitCreate* currUnit = (UndoUnitCreate*)node->head,
							  * next;

				node->head = nullptr; // Detatch the current unit stack

				while ( currUnit != nullptr ) // Traverse the detatched stack
				{ // Build the new stack
					next = currUnit->next;
					ChkUnit* unitRef;
					if ( chk->getUnit(unitRef, currUnit->unitIndex) )
						node->head = new UndoUnitDel(node->head, currUnit->unitIndex, unitRef);
					UNIT.del(currUnit->unitIndex*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);
					delete currUnit;
					currUnit = next;
				}
			}
			break;
		case UNDO_UNIT_DEL:
			{
				node->flags &= (~UNDO_UNIT_DEL);
				node->flags |= UNDO_UNIT_CREATE;
				buffer& UNIT = chk->UNIT();

				UndoUnitDel* currUnit = (UndoUnitDel*)node->head,
						   * next;

				node->head = nullptr; // Detatch the current unit stack

				while ( currUnit != nullptr ) // Traverse the detached stack
				{ // Build the new stack
					next = currUnit->next;
					UNIT.insert<ChkUnit&>(currUnit->unitIndex*UNIT_STRUCT_SIZE, currUnit->unit);
					node->head = new UndoUnitCreate(node->head, currUnit->unitIndex);
					delete currUnit;
					currUnit = next;
				}
			}
			break;
		case UNDO_UNIT_SWAP:
			{
				buffer& UNIT = chk->UNIT();

				UndoUnitMove* currUnit = (UndoUnitMove*)node->head,
							* next;

				node->head = nullptr; // Detatch the current unit stack

				while ( currUnit != nullptr ) // Traverse the detached stack
				{ // Build the new stack
					next = currUnit->next;
					if ( UNIT.swap<ChkUnit&>(currUnit->newIndex*UNIT_STRUCT_SIZE, currUnit->oldIndex*UNIT_STRUCT_SIZE) )
						sel.sendSwap(currUnit->oldIndex, currUnit->newIndex);
					node->head = new UndoUnitMove(node->head, currUnit->newIndex, currUnit->oldIndex);
					delete currUnit;
					currUnit = next;
				}
				sel.finishSwap();
			}
			break;
		case UNDO_UNIT_MOVE:
			{
				buffer& UNIT = chk->UNIT();
				ChkUnit preserve;

				UndoUnitMove* currUnit = (UndoUnitMove*)node->head,
							* next;

				node->head = nullptr; // Detatch the current unit stack

				while ( currUnit != nullptr ) // Traverse the detached stack
				{ // Build the new stack
					next = currUnit->next;
					if ( UNIT.get<ChkUnit>(preserve, currUnit->newIndex*UNIT_STRUCT_SIZE) &&
						 UNIT.del<ChkUnit>(currUnit->newIndex*UNIT_STRUCT_SIZE) &&
						 UNIT.insert<ChkUnit&>(currUnit->oldIndex*UNIT_STRUCT_SIZE, preserve) )
					{
						sel.sendMove(currUnit->oldIndex, currUnit->newIndex);
					}
					sel.finishMove();
					node->head = new UndoUnitMove(node->head, currUnit->newIndex, currUnit->oldIndex);
					delete currUnit;
					currUnit = next;
				}
			}
			break;
		case UNDO_UNIT_CHANGE:
			{
				buffer& UNIT = chk->UNIT();

				UndoUnitChange* currUnit = (UndoUnitChange*)node->head,
							  * next;

				node->head = nullptr; // Detatch the current unit stack

				while ( currUnit != nullptr ) // Traverse the detatched stack
				{ // Build the new stack
					next = currUnit->next;
					u32 temp = 0,
						pos = currUnit->unitIndex*UNIT_STRUCT_SIZE + unitFieldLoc[currUnit->field];

					switch ( unitFieldSize[currUnit->field] )
					{
						case 1:
							UNIT.get<u8>((u8&)temp, pos);
							UNIT.replace<u8>(pos, u8(currUnit->data));
							break;
						case 2:
							UNIT.get<u16>((u16&)temp, pos);
							UNIT.replace<u16>(pos, u16(currUnit->data));
							break;
						case 4:
							UNIT.get<u32>(temp, pos);
							UNIT.replace<u32>(pos, currUnit->data);
							break;
					}
					node->head = new UndoUnitChange((UndoUnitChange*)node->head, currUnit->unitIndex, currUnit->field, temp);
					delete currUnit;
					currUnit = next;
				}
			}
			break;
		case UNDO_UNIT_MOVETO:
			{
				if ( node->head != nullptr )
				{
					buffer& UNIT = chk->UNIT();

					// Parse the header node
					UndoUnitMoveToHeader* header = (UndoUnitMoveToHeader*)node->head;
					u16 numDeletes = header->numDeletes,
						numCreates = header->numCreates;
					UndoUnitCreate* trackCreate = (UndoUnitCreate*)header->next;

					node->head = nullptr; // Detatch the MoveTo stack

					if ( numCreates > 0 )
					{
						for ( u16 i=0; i<numCreates; i++ ) // Traverse unit creates
						{
							UndoUnitCreate* next = trackCreate->next;
							ChkUnit* unitRef;
							if ( chk->getUnit(unitRef, trackCreate->unitIndex) )
							{
								try {
									node->head = new UndoUnitDel(node->head, trackCreate->unitIndex, unitRef);
								} catch ( std::bad_alloc ) {
									numCreates --;
									i --;
								}
								UNIT.del<ChkUnit>(trackCreate->unitIndex*UNIT_STRUCT_SIZE);
								sel.removeUnit(trackCreate->unitIndex);
							}
							delete trackCreate;
							trackCreate = next;
						}
						if ( numDeletes > 0 )
						{
							UndoUnitDel* trackDel = (UndoUnitDel*)trackCreate;
							for ( u16 i=0; i<numDeletes; i++ ) // Traverse unit deletes
							{ // Undo & add each item to the new stack
								UndoUnitDel* next = trackDel->next;
								if ( UNIT.insert<ChkUnit&>(trackDel->unitIndex*UNIT_STRUCT_SIZE, trackDel->unit) )
									sel.addUnit(trackDel->unitIndex);
								try {
									node->head = new UndoUnitCreate(node->head, trackDel->unitIndex);
								} catch ( std::bad_alloc )
								{
									numDeletes --;
									i --;
								}
								delete trackDel;
								trackDel = next;
							}
						}
					}
					
					header->numCreates = numDeletes;
					header->numDeletes = numCreates;
					header->next = node->head;
					node->head = header;
				}
			}
			break;
		case UNDO_LOCATION_CREATE:
			{
				node->flags &= (~UNDO_LOCATION_CREATE);
				node->flags |= UNDO_LOCATION_DEL;
				buffer& MRGN = chk->MRGN();

				UndoLocationCreate* currLocation = (UndoLocationCreate*)node->head,
								  * next;

				node->head = nullptr; // Detatch the current location stack

				while ( currLocation != nullptr ) // Traverse the detached stack
				{ // Build the new stack
					next = currLocation->next;

					ChkLocation* locRef;
					if ( chk->getLocation(locRef, currLocation->locationIndex) )
					{
						std::string locName;
						try {
							if ( chk->getString(locName, locRef->stringNum) )
							{
								node->head = new UndoLocationDel(node->head, currLocation->locationIndex, locRef, chk->isExtendedString(locRef->stringNum), locName);
								chk->deleteLocation(currLocation->locationIndex);
							}
							else
							{
								locRef->stringNum = 0;
								locName = "";
								node->head = new UndoLocationDel(node->head, currLocation->locationIndex, locRef, false, locName);
								chk->deleteLocation(currLocation->locationIndex);
							}
						} catch ( std::exception ) {
						} // Simply skips this members addition if out of memory of string errors occur
					}
					
					delete currLocation;
					currLocation = next;
				}
			}
			break;
		case UNDO_LOCATION_DEL:
			{
				node->flags &= (~UNDO_LOCATION_DEL);
				node->flags |= UNDO_LOCATION_CREATE;
				buffer& MRGN = chk->MRGN();

				UndoLocationDel* currLocation = (UndoLocationDel*)node->head,
							   * next;

				node->head = nullptr; // Detatch the current location stack

				while ( currLocation != nullptr ) // Traverse the detached stack
				{ // Build the new stack
					next = currLocation->next;

					// Recreate the string
					u32 stringNum;
					if ( chk->addString(currLocation->locationName, stringNum, currLocation->isExtended) )
						currLocation->location.stringNum = u16(stringNum);
					else
						currLocation->location.stringNum = 0;

					// Add all the location data
					if ( !MRGN.replaceStr(currLocation->locationIndex*CHK_LOCATION_SIZE, (const char*)&currLocation->location, CHK_LOCATION_SIZE) )
					{ // Failed to replace location data
						if ( MRGN.size() < 5100 ) // If there's < 255 locations
						{
							MRGN.add<u8>(0, 5100-MRGN.size()); // Add space for 255 and try again
							MRGN.replaceStr(currLocation->locationIndex*CHK_LOCATION_SIZE, (const char*)&currLocation->location, CHK_LOCATION_SIZE);
						}
					}
					node->head = new UndoLocationCreate(node->head, currLocation->locationIndex);
					delete currLocation;
					currLocation = next;
				}
			}
			break;
		case UNDO_LOCATION_MOVE:
			{
				buffer& MRGN = chk->MRGN();

				UndoLocationMove* currLoc = (UndoLocationMove*)node->head,
								* next;

				node->head = nullptr; // Detatch the current location stack

				while ( currLoc != nullptr ) // Traverse the detatched stack
				{ // Build the new stack
					next = currLoc->next;

					ChkLocation* loc;
					if ( chk->getLocation(loc, currLoc->locationIndex) )
					{
						node->head = new UndoLocationMove(node->head, currLoc->locationIndex, -currLoc->xChange, -currLoc->yChange);
						loc->xc1 -= currLoc->xChange;
						loc->xc2 -= currLoc->xChange;
						loc->yc1 -= currLoc->yChange;
						loc->yc2 -= currLoc->yChange;
					}

					delete currLoc;
					currLoc = next;
				}
			}
			break;
		case UNDO_LOCATION_CHANGE:
			{
				buffer& MRGN = chk->MRGN();

				UndoLocationChange* currLoc = (UndoLocationChange*)node->head,
								  * next;

				node->head = nullptr; // Detatch the current location stack

				while ( currLoc != nullptr ) // Traverse the detatched stack
				{ // Build the new stack
					next = currLoc->next;
					u32 temp = 0,
						pos = currLoc->locationIndex*CHK_LOCATION_SIZE + locationFieldLoc[currLoc->field];

					switch ( locationFieldSize[currLoc->field] )
					{
						case 1:
							MRGN.get<u8>((u8&)temp, pos);
							MRGN.replace<u8>(pos, u8(currLoc->data));
							break;
						case 2:
							MRGN.get<u16>((u16&)temp, pos);
							MRGN.replace<u16>(pos, u16(currLoc->data));
							break;
						case 4:
							MRGN.get<u32>(temp, pos);
							MRGN.replace<u32>(pos, currLoc->data);
							break;
					}
					node->head = new UndoLocationChange((UndoLocationChange*)node->head, currLoc->locationIndex, currLoc->field, temp);
					delete currLoc;
					currLoc = next;
				}
			}
			break;
		default:
			Error("Failed to find matching undo stream");
			break;
	}
}

bool UNDOS::addToUndo(u32 type, void* undoInfo)
{
	if ( upcomingUndo == nullptr || (upcomingUndo->flags&type) == 0 )
	{
		if ( !startNext(type) )
		{
			return false;
		}
	}

	Invariant ( upcomingUndo != nullptr );

	if ( upcomingUndo->head == nullptr )
		upcomingUndo->head = undoInfo;
	else
	{
		((UndoComponent*)undoInfo)->next = upcomingUndo->head;
		upcomingUndo->head = undoInfo;
	}
	return true;
}

UndoNode* UNDOS::popUndo(u32 type)
{
	UndoNode* track = headUndo;

	while ( track != nullptr )
	{
		if ( track->flags & type )
		{
			removeNode(track);
			return track;
		}
		track = track->next;
	}
	return nullptr;
}

UndoNode* UNDOS::popRedo(u32 type)
{
	UndoNode* track = headRedo;

	while ( track != nullptr )
	{
		if ( track->flags & type )
		{
			removeNode(track);
			return track;
		}
		track = track->next;
	}
	return nullptr;
}

bool UNDOS::clip(UndoNode* track, u32 type)
{
	bool didClip = false;
	UndoNode* next;

	while ( track != nullptr )
	{
		next = track->next;
		if ( track->flags & type )
		{
			deleteNode(track);
			didClip = true;
		}
		track = next;
	}
	return didClip;
}

void UNDOS::deleteNode(UndoNode* node)
{
	removeNode(node);
	delete node;
}

void UNDOS::removeNode(UndoNode* node)
{
	UndoNode* prev = node->prev,
			* next = node->next;

	if ( next != nullptr )
		next->prev = prev;
	if ( prev != nullptr )
		prev->next = next;

	if ( node == headRedo )
		headRedo = next;
	else
	{
		if ( node == headUndo )
			headUndo = next;
		if ( node == lastUndo )
			lastUndo = prev;
	}
}