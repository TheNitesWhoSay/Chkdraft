#include "ClipBoard.h"
#include "Common Files/CommonFiles.h"
using namespace std;

extern DATA scData;
extern HWND hTilePal;
extern HWND hUnit;
extern HWND hMain;
extern HWND hLeft;

CLIPBOARD::CLIPBOARD() : headCopyTile(nullptr), headQuickTile(nullptr), headCopyUnit(nullptr), headQuickUnit(nullptr), 
						 pasting(false), quickPaste(false)
{
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
	prevPaste.x = -1;
	prevPaste.y = -1;
}

CLIPBOARD::~CLIPBOARD()
{
	ClearCopyTiles();
	ClearQuickItems();
}

bool CLIPBOARD::hasTiles()
{
	return headCopyTile != nullptr;
}

void CLIPBOARD::copy(SELECTIONS* selection, Scenario* chk, u8 layer)
{
	if ( layer == LAYER_TERRAIN )
	{
		ClearCopyTiles(); // Clear whatever was previously copied
		if ( selection->hasTiles() )
		{
			TileNode* track = selection->getFirstTile(); // track = firstSelectionTile
			PasteTileNode* add = new PasteTileNode(track->value, track->xc*32, track->yc*32, track->neighbors, headCopyTile);

			edges.left	 = track->xc*32;
			edges.right	 = track->xc*32 + 32;
			edges.top	 = track->yc*32;
			edges.bottom = track->yc*32 + 32;

			headCopyTile = add;
			track = track->next;

			while ( track != nullptr ) // Traverse through all tiles
			{
				add = new PasteTileNode(track->value, track->xc*32, track->yc*32, track->neighbors, headCopyTile);
				
				// Record the outermost tile positions for determining their relation to the cursor
				if		( add->xc	 < edges.left   ) edges.left	= add->xc;
				else if ( add->xc+32 > edges.right  ) edges.right	= add->xc+32;
				if		( add->yc	 < edges.top    ) edges.top		= add->yc;
				else if ( add->yc+32 > edges.bottom ) edges.bottom	= add->yc+32;

				add->next = headCopyTile;
				headCopyTile = add;
				track = track->next;
			}

			POINT middle; // Determine where the new middle of the paste is
			middle.x = edges.left+(edges.right-edges.left)/2;
			middle.y = edges.top+(edges.bottom-edges.top)/2; 

			PasteTileNode* pTrack = headCopyTile; // Used to traverse through all paste tiles
			while ( pTrack != nullptr ) // Update the tile's relative position to the cursor
			{
				pTrack->xc -= middle.x;
				pTrack->yc -= middle.y;
				pTrack = pTrack->next;
			}
		}
	}
	else if ( layer == LAYER_UNITS )
	{
		ClearCopyUnits();
		if ( selection->hasUnits() )
		{
			UnitNode* track = selection->getFirstUnit();
			PasteUnitNode* add;
			ChkUnit* currUnit;

			if ( chk->getUnit(currUnit, track->index) )
			{
				edges.left	 = currUnit->xc;
				edges.right	 = currUnit->xc;
				edges.top	 = currUnit->yc;
				edges.bottom = currUnit->yc;
			}

			while ( track != nullptr )
			{
				if ( chk->getUnit(currUnit, track->index) )
				{
					add = new PasteUnitNode(currUnit);

					if		( add->xc < edges.left   ) edges.left	= add->xc;
					else if ( add->xc > edges.right  ) edges.right	= add->xc;
					if		( add->yc < edges.top    ) edges.top	= add->yc;
					else if ( add->yc > edges.bottom ) edges.bottom = add->yc;

					add->next = headCopyUnit;;
					headCopyUnit = add;
				}
				track = track->next;
			}

			POINT middle; // Determine where the new middle of the paste is
			middle.x = edges.left+(edges.right-edges.left)/2;
			middle.y = edges.top+(edges.bottom-edges.top)/2;
			PasteUnitNode* pTrack = headCopyUnit; // Used to traverse through all paste units
			while ( pTrack != nullptr ) // Update the tile's relative position to the cursor
			{
				pTrack->xc -= middle.x;
				pTrack->yc -= middle.y;
				pTrack = pTrack->next;
			}
		}
	}
}

void CLIPBOARD::addQuickTile(u16 index, s32 xc, s32 yc)
{
	PasteTileNode* tile;
	try {
		tile = new PasteTileNode(index, xc, yc, ALL_NEIGHBORS, headQuickTile);
	} catch ( std::bad_alloc )
	{ return; }

	headQuickTile = tile;
}

void CLIPBOARD::addQuickUnit(ChkUnit* unitRef)
{
	PasteUnitNode* unit;
	try {
		unit = new PasteUnitNode(unitRef, headQuickUnit);
	} catch ( std::bad_alloc )
	{ return; }

	headQuickUnit = unit;
}

void CLIPBOARD::beginPasting(bool isQuickPaste)
{
	quickPaste = isQuickPaste;
	pasting = true;
}

void CLIPBOARD::endPasting()
{
	if ( pasting )
	{
		if ( quickPaste )
		{
			ClearQuickItems();
			quickPaste = false;

			if ( hLeft != NULL )
			{
				HWND hTree = GetDlgItem(hLeft, IDR_MAIN_TREE);
				if ( hTree != NULL )
					TreeView_SelectItem(hTree, NULL);
			}
			
			if ( hTilePal != nullptr )
				RedrawWindow(hTilePal, NULL, NULL, RDW_INVALIDATE);
		}

		prevPaste.x = -1;
		prevPaste.y = -1;
		pasting = false;
	}
}

void CLIPBOARD::doPaste(u8 layer, s32 mapClickX, s32 mapClickY, Scenario* chk, UNDOS& undos, bool allowStack)
{
	switch ( layer )
	{
		case LAYER_TERRAIN:
			{
				mapClickX += 16;
				mapClickY += 16;

				if ( !( mapClickX/16 == prevPaste.x && mapClickY/16 == prevPaste.y ) )
				{
					prevPaste.x = mapClickX/16;
					prevPaste.y = mapClickY/16;
					u16 xSize = chk->XSize();
					u16 ySize = chk->YSize();
	
					PasteTileNode* track = getFirstTile();
					while ( track != nullptr )
					{
						s32 xc = (track->xc+mapClickX)/32;
						s32 yc = (track->yc+mapClickY)/32;
	
						// If within map boundaries
						if ( xc >= 0 && xc < xSize )
						{
							if ( yc >= 0 && yc < ySize )
							{
								u32 startLocation = 2*xSize*yc+2*xc; // Down y rows, over x columns
								if ( chk->MTXM().get<u16>(startLocation) != track->value )
								{
									undos.addUndoTile((u16)xc, (u16)yc, chk->MTXM().get<u16>(startLocation));
									chk->TILE().replace<u16>(startLocation, track->value);
									chk->MTXM().replace<u16>(startLocation, track->value);
								}
							}
						}
	
						track = track->next;
					}
					undos.startNext(0);
				}
			}
			break;
		case LAYER_UNITS:
			{
				PasteUnitNode* track = getFirstUnit();

				while ( track != nullptr )
				{
					track->unit.xc = u16(mapClickX + track->xc);
					track->unit.yc = u16(mapClickY + track->yc);
					bool canPaste = true;
					if ( allowStack == false )
					{
						s32 unitLeft   = track->unit.xc - scData.units.UnitDat(track->unit.id)->UnitSizeLeft,
							unitRight  = track->unit.xc + scData.units.UnitDat(track->unit.id)->UnitSizeRight,
							unitTop	   = track->unit.yc - scData.units.UnitDat(track->unit.id)->UnitSizeUp,
							unitBottom = track->unit.yc + scData.units.UnitDat(track->unit.id)->UnitSizeDown;

						ChkUnit* unit;
						u16 numUnits = chk->numUnits();
						for ( u16 i=0; i<numUnits; i++ )
						{
							if ( chk->getUnit(unit, i) )
							{
								s32 left   = unit->xc - scData.units.UnitDat(unit->id)->UnitSizeLeft,
									right  = unit->xc + scData.units.UnitDat(unit->id)->UnitSizeRight,
									top	   = unit->yc - scData.units.UnitDat(unit->id)->UnitSizeUp,
									bottom = unit->yc + scData.units.UnitDat(unit->id)->UnitSizeDown;

								if ( unitRight >= left && unitLeft <= right && unitBottom >= top && unitTop <= bottom )
								{
									canPaste = false;
									break;
								}
							}
						}
					}

					if ( canPaste )
					{
						prevPaste.x = track->unit.xc;
						prevPaste.y = track->unit.yc;
						u16 numUnits = chk->numUnits();
						if ( chk->UNIT().add<ChkUnit&>(track->unit) )
						{
							undos.addUndoUnitCreate(numUnits);
							if ( hUnit != nullptr )
								SendMessage(hUnit, ADD_UNIT, numUnits, (LPARAM)&track->unit);
						}
					}
					track = track->next;
				}
			}
			break;
	}
}

PasteTileNode* CLIPBOARD::getFirstTile()
{
	if ( quickPaste )
		return headQuickTile;
	else
		return headCopyTile;
}

PasteUnitNode* CLIPBOARD::getFirstUnit()
{
	if ( quickPaste )
		return headQuickUnit;
	else
		return headCopyUnit;
}

void CLIPBOARD::ClearCopyTiles()
{
	PasteTileNode* track = headCopyTile,
				 * next;

	while ( track != nullptr )
	{
		next = track->next;
		delete track;
		track = next;
	}
	headCopyTile = nullptr;
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
}

void CLIPBOARD::ClearCopyUnits()
{
	PasteUnitNode* track = headCopyUnit,
				 * next;

	while ( track != nullptr )
	{
		next = track->next;
		delete track;
		track = next;
	}
	headCopyUnit = nullptr;
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
}

void CLIPBOARD::ClearQuickItems()
{
	PasteTileNode* nextTile;
	while ( headQuickTile != nullptr )
	{
		nextTile = headQuickTile->next;
		delete headQuickTile;
		headQuickTile = nextTile;
	}

	PasteUnitNode* nextUnit;
	while ( headQuickUnit != nullptr )
	{
		nextUnit = headQuickUnit->next;
		delete headQuickUnit;
		headQuickUnit = nextUnit;
	}
}
