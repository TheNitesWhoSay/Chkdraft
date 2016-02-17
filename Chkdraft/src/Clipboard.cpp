#include "ClipBoard.h"
#include "Chkdraft.h"
#include "TileChange.h"
#include "UnitCreateDel.h"

void StringToWindowsClipboard(std::string &str)
{
	if ( OpenClipboard(NULL) != 0 )
	{
		if ( EmptyClipboard() != 0 )
		{
			HGLOBAL globalData = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
			if ( globalData != NULL )
			{
				LPVOID lockedData = GlobalLock(globalData);
				if ( lockedData != NULL )
				{
					memcpy(lockedData, str.c_str(), str.size() + 1);
					GlobalUnlock(lockedData);
					SetClipboardData(CF_TEXT, globalData);
				}
				GlobalFree(globalData);
			}
		}
		CloseClipboard();
	}
}

bool WindowsClipboardToString(std::string &str)
{
	bool success = false;
	if ( OpenClipboard(NULL) != 0 )
	{
		HANDLE clipboardData = GetClipboardData(CF_TEXT);
		if ( clipboardData != NULL )
		{
			LPVOID lockedData = GlobalLock(clipboardData);
			if ( lockedData != NULL )
			{
				SIZE_T maxStringSize = GlobalSize(lockedData);
				if ( maxStringSize != 0 )
				{
					const char* cString = (const char*)lockedData;
					SIZE_T nulPos = 0;
					while ( nulPos < maxStringSize && cString[nulPos] != '\0' )
						nulPos++;

					str.clear();
					try {
						str.assign(cString, (size_t)nulPos);
						success = true;
					}
					catch ( std::exception ) {}
				}
				GlobalUnlock(lockedData);
			}
		}
		CloseClipboard();
	}
	return success;
}

Clipboard::Clipboard() : pasting(false), quickPaste(false)
{
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
	prevPaste.x = -1;
	prevPaste.y = -1;
}

Clipboard::~Clipboard()
{
	ClearCopyTiles();
	ClearQuickItems();
}

bool Clipboard::hasTiles()
{
	return copyTiles.size() > 0;
}

void Clipboard::copy(GuiMap &map, u8 layer)
{
    Selections &selections = map.GetSelections();
	if ( layer == LAYER_TERRAIN )
	{
		ClearCopyTiles(); // Clear whatever was previously copied
		if ( selections.hasTiles() )
		{
			TileNode firstTile = selections.getFirstTile();
			edges.left = firstTile.xc * 32;
			edges.right = firstTile.xc * 32 + 32;
			edges.top = firstTile.yc * 32;
			edges.bottom = firstTile.yc * 32 + 32;

			auto &selTiles = selections.getTiles();
			for ( auto &selTile : selTiles ) // Traverse through all tiles
			{
				PasteTileNode tile(selTile.value, selTile.xc * 32, selTile.yc * 32, selTile.neighbors);

				// Record the outermost tile positions for determining their relation to the cursor
				if		( tile.xc	   < edges.left	  ) edges.left	 = tile.xc;
				else if ( tile.xc + 32 > edges.right  ) edges.right  = tile.xc + 32;
				if		( tile.yc	   < edges.top	  ) edges.top	 = tile.yc;
				else if ( tile.yc + 32 > edges.bottom ) edges.bottom = tile.yc + 32;

				copyTiles.insert(copyTiles.end(), tile);
			}

			POINT middle; // Determine where the new middle of the paste is
			middle.x = edges.left+(edges.right-edges.left)/2;
			middle.y = edges.top+(edges.bottom-edges.top)/2; 

			for ( auto &tile : copyTiles ) // Update the tile's relative position to the cursor
			{
				tile.xc -= middle.x;
				tile.yc -= middle.y;
			}
		}
	}
	else if ( layer == LAYER_UNITS )
	{
		ClearCopyUnits();
		if ( selections.hasUnits() )
		{
			ChkUnit* currUnit;

			u16 firstUnitIndex = selections.getFirstUnit();
			if ( map.getUnit(currUnit, firstUnitIndex) )
			{
				edges.left	 = currUnit->xc;
				edges.right	 = currUnit->xc;
				edges.top	 = currUnit->yc;
				edges.bottom = currUnit->yc;
			}

			auto &selectedUnits = selections.getUnits();
			for ( u16 &unitIndex : selectedUnits )
			{
				if ( map.getUnit(currUnit, unitIndex) )
				{
					PasteUnitNode add(currUnit);

					if		( add.xc < edges.left	) edges.left   = add.xc;
					else if ( add.xc > edges.right	) edges.right  = add.xc;
					if		( add.yc < edges.top	) edges.top	   = add.yc;
					else if ( add.yc > edges.bottom ) edges.bottom = add.yc;

					copyUnits.insert(copyUnits.end(), add);
				}
			}

			POINT middle; // Determine where the new middle of the paste is
			middle.x = edges.left+(edges.right-edges.left)/2;
			middle.y = edges.top+(edges.bottom-edges.top)/2;

			for ( auto &unit : copyUnits ) // Update the unit's relative position to the cursor
			{
				unit.xc -= middle.x;
				unit.yc -= middle.y;
			}
		}
	}
}

void Clipboard::addQuickTile(u16 index, s32 xc, s32 yc)
{
	quickTiles.insert(quickTiles.end(), PasteTileNode(index, xc, yc, ALL_NEIGHBORS));
}

void Clipboard::addQuickUnit(ChkUnit* unitRef)
{
	quickUnits.insert(quickUnits.end(), PasteUnitNode(unitRef));
}

void Clipboard::beginPasting(bool isQuickPaste)
{
	quickPaste = isQuickPaste;
	pasting = true;
}

void Clipboard::endPasting()
{
	if ( pasting )
	{
		if ( quickPaste )
		{
			ClearQuickItems();
			quickPaste = false;

			if ( chkd.mainPlot.leftBar.getHandle() != NULL )
			{
				HWND hTree = chkd.mainPlot.leftBar.mainTree.getHandle();
				if ( hTree != NULL )
					TreeView_SelectItem(hTree, NULL);
			}
			
			if ( chkd.terrainPalWindow.getHandle() != nullptr )
				RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
		}

		prevPaste.x = -1;
		prevPaste.y = -1;
		pasting = false;
	}
}

void Clipboard::doPaste(u8 layer, s32 mapClickX, s32 mapClickY, GuiMap &map, Undos &undos, bool allowStack)
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
					u16 xSize = map.XSize();
					u16 ySize = map.YSize();
	
					std::shared_ptr<ReversibleActions> tileChanges(new ReversibleActions);
					auto &tiles = getTiles();
					for ( auto &tile : tiles )
					{
						s32 xc = (tile.xc + mapClickX) / 32;
						s32 yc = (tile.yc + mapClickY) / 32;

						// If within map boundaries
						if ( xc >= 0 && xc < xSize )
						{
							if ( yc >= 0 && yc < ySize )
							{
								u32 startLocation = 2 * xSize*yc + 2 * xc; // Down y rows, over x columns
								if ( map.MTXM().get<u16>(startLocation) != tile.value )
								{
									tileChanges->Insert(std::shared_ptr<TileChange>(new TileChange(xc, yc, map.MTXM().get<u16>(startLocation))));
									map.TILE().replace<u16>(startLocation, tile.value);
									map.MTXM().replace<u16>(startLocation, tile.value);
								}
							}
						}
					}
					undos.AddUndo(tileChanges);
				}
			}
			break;
		case LAYER_UNITS:
			{
				std::shared_ptr<ReversibleActions> unitCreates(new ReversibleActions);
				auto &pasteUnits = getUnits();
				for ( auto &pasteUnit : pasteUnits )
				{
					pasteUnit.unit.xc = u16(mapClickX + pasteUnit.xc);
					pasteUnit.unit.yc = u16(mapClickY + pasteUnit.yc);
					if ( mapClickX + (s32(pasteUnit.xc)) >= 0 && mapClickY + (s32(pasteUnit.yc)) >= 0 )
					{
						bool canPaste = true;
						if ( allowStack == false )
						{
							s32 unitLeft   = pasteUnit.unit.xc - chkd.scData.units.UnitDat(pasteUnit.unit.id)->UnitSizeLeft,
								unitRight  = pasteUnit.unit.xc + chkd.scData.units.UnitDat(pasteUnit.unit.id)->UnitSizeRight,
								unitTop	   = pasteUnit.unit.yc - chkd.scData.units.UnitDat(pasteUnit.unit.id)->UnitSizeUp,
								unitBottom = pasteUnit.unit.yc + chkd.scData.units.UnitDat(pasteUnit.unit.id)->UnitSizeDown;

							ChkUnit* unit;
							u16 numUnits = map.numUnits();
							for ( u16 i=0; i<numUnits; i++ )
							{
								if ( map.getUnit(unit, i) )
								{
									s32 left   = unit->xc - chkd.scData.units.UnitDat(unit->id)->UnitSizeLeft,
										right  = unit->xc + chkd.scData.units.UnitDat(unit->id)->UnitSizeRight,
										top	   = unit->yc - chkd.scData.units.UnitDat(unit->id)->UnitSizeUp,
										bottom = unit->yc + chkd.scData.units.UnitDat(unit->id)->UnitSizeDown;

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
							prevPaste.x = pasteUnit.unit.xc;
							prevPaste.y = pasteUnit.unit.yc;
							u16 numUnits = map.numUnits();
							if ( map.UNIT().add<ChkUnit&>(pasteUnit.unit) )
							{
								unitCreates->Insert(std::shared_ptr<UnitCreateDel>(new UnitCreateDel(numUnits)));
								if ( chkd.unitWindow.getHandle() != nullptr )
									chkd.unitWindow.AddUnitItem(numUnits, &pasteUnit.unit);
							}
						}
					}
				}
                CM->AddUndo(unitCreates);
			}
			break;
	}
}

std::vector<PasteTileNode> &Clipboard::getTiles()
{
	if ( quickPaste )
		return quickTiles;
	else
		return copyTiles;
}

std::vector<PasteUnitNode> &Clipboard::getUnits()
{
	if ( quickPaste )
		return quickUnits;
	else
		return copyUnits;
}

void Clipboard::ClearCopyTiles()
{
	copyTiles.clear();
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
}

void Clipboard::ClearCopyUnits()
{
	copyUnits.clear();
	edges.left = -1;
	edges.top = -1;
	edges.right = -1;
	edges.bottom = -1;
}

void Clipboard::ClearQuickItems()
{
	quickTiles.clear();
	quickUnits.clear();
}
