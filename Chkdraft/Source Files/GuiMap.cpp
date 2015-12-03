#include "GuiMap.h"
#include "Chkdraft.h"
#include "TileChange.h"
#include "UnitCreateDel.h"
#include "LocationCreateDel.h"
#include "LocationMove.h"
#include "LocationChange.h"

GuiMap::GuiMap() : lpvBits(nullptr), bitMapHeight(0), bitMapWidth(0),
			 layer(LAYER_TERRAIN), player(0), zoom(1), RedrawMiniMap(true), RedrawMap(true),
			 dragging(false), snapLocations(true), locSnapTileOverGrid(true), lockAnywhere(true),
			 snapUnits(true), stackUnits(false),
			 MemhDC(NULL), MemMinihDC(NULL), mapId(0),
			 MemBitmap(NULL), MemMiniBitmap(NULL), graphics(*this), unsavedChanges(false), changeLock(false), undoStacks(*this)
{
	int layerSel = chkd.mainToolbar.layerBox.GetSel();
	if ( layerSel != CB_ERR )
		layer = u8(layerSel);

	displayPivot.x = 0;
	displayPivot.y = 0;
}

GuiMap::~GuiMap()
{
	chkd.tilePropWindow.DestroyThis();

	if ( lpvBits != nullptr )
	{
		delete[] lpvBits;
		lpvBits = nullptr;
	}

	if ( MemBitmap != NULL )
		DeleteObject(MemBitmap);

	if ( MemMiniBitmap != NULL )
		DeleteObject(MemMiniBitmap);

	if ( MemhDC != NULL )
		DeleteDC(MemhDC);

	if ( MemMinihDC != NULL )
		DeleteDC(MemMinihDC);
}

bool GuiMap::CanExit()
{
	if ( unsavedChanges )
	{
		int result = IDCANCEL;
		if ( MapFile::FilePath() == nullptr || strcmp(MapFile::FilePath(), "") == 0 )
			result = MessageBox(NULL, "Save Changes?", "Untitled", MB_YESNOCANCEL);
		else
			result = MessageBox(NULL, "Save Changes?", MapFile::FilePath(), MB_YESNOCANCEL);

		if ( result == IDYES )
			SaveFile(false);
		else if ( result == IDCANCEL )
			return false;
	}
	return true;
}

bool GuiMap::SaveFile(bool saveAs)
{
	if ( MapFile::SaveFile(saveAs) )
	{
		changeLock = false;
		if ( unsavedChanges )
		{
			unsavedChanges = false;
			removeAsterisk();
			undoStacks.ResetChangeCount();
		}
		return true;
	}
	else
		return false;
}

bool GuiMap::SetTile(s32 x, s32 y, u16 tileNum)
{
	u16 xSize = XSize();
	if ( DIM().size() < 4 || x > xSize || y > YSize() )
		return false;

	u32 location = 2*xSize*y+2*x; // Down y rows, over x columns

	//undoStacks.addUndoTile((u16)x, (u16)y, MTXM().get<u16>(location));
	undoStacks.AddUndo(ReversiblePtr(new TileChange((u16)x, (u16)y, getTile((u16)x, (u16)y))));

	TILE().replace<u16>(location, tileNum);
	MTXM().replace<u16>(location, tileNum);

	RECT rcTile;
	rcTile.left	  = x*32-displayPivot.x;
	rcTile.right  = rcTile.left+32;
	rcTile.top	  = y*32-displayPivot.y;
	rcTile.bottom = rcTile.top+32;

	RedrawMap = true;
	RedrawMiniMap = true;
	InvalidateRect(getHandle(), &rcTile, true);
	return true;
}

u8 GuiMap::getLayer()
{
	return this->layer;
}

bool GuiMap::setLayer(u8 newLayer)
{
	chkd.maps.endPaste();
	ClipCursor(NULL);
	Redraw(false);
	selection.setDrags(-1, -1);
	this->layer = newLayer;
	return true;
}

double GuiMap::getZoom()
{
	return zoom;
}

void GuiMap::setZoom(double newScale)
{
	for ( u32 i=0; i<NUM_ZOOMS; i++ )
	{
		if ( newScale == zooms[i] )
			chkd.mainToolbar.zoomBox.SetSel(i);
	}
	zoom = newScale;
	RedrawMap = true;
	RedrawMiniMap = true;

	RECT rect;
	GetClientRect(getHandle(), &rect);
	bitMapWidth  = u16( ((rect.right - rect.left+32)/32*32)/zoom )/32*32;
	bitMapHeight = u16( ((rect.bottom - rect.top+32)/32*32)/zoom )/32*32;
	Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
	UpdateZoomMenuItems();
	RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

u8& GuiMap::currPlayer()
{
	return player;
}

bool GuiMap::isDragging()
{
	return dragging;
}

void GuiMap::setDragging(bool bDragging)
{
	dragging = bDragging;
}

void GuiMap::viewLocation(u16 index)
{
	ChkLocation* location;
	RECT rect;
	if ( getLocation(location, u8(index)) && GetClientRect(getHandle(), &rect) != 0 )
	{
		s32 width = rect.right - rect.left,
			height = rect.bottom - rect.top,
			locLeft = std::min(location->xc1, location->xc2),
			locRight = std::max(location->xc1, location->xc2),
			locTop = std::min(location->yc1, location->yc2),
			locBottom = std::max(location->yc1, location->yc2),
			locCenterX = locLeft+(locRight-locLeft)/2,
			locCenterY = locTop+(locBottom-locTop)/2;

		displayPivot.x = locCenterX - width/2;
		displayPivot.y = locCenterY - height/2;

		Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
		Redraw(false);
	}
}

u8 GuiMap::getLocSelFlags(s32 xc, s32 yc)
{
	if ( layer == LAYER_LOCATIONS )
	{
		u16 selectedLocation = selections().getSelectedLocation();
		if ( selectedLocation != NO_LOCATION ) // If location is selected, determine which part of it is hovered by mouse
		{
			ChkLocation* loc;
			if ( getLocation(loc, selectedLocation) )
			{
				s32 locationLeft = std::min(loc->xc1, loc->xc2),
					locationRight = std::max(loc->xc1, loc->xc2),
					locationTop = std::min(loc->yc1, loc->yc2),
					locationBottom = std::max(loc->yc1, loc->yc2),
					leftOuterBound = locationLeft-5,
					rightOuterBound = locationRight+5,
					topOuterBound = locationTop-5,
					bottomOuterBound = locationBottom+5;

				if ( xc >= leftOuterBound && xc <= rightOuterBound &&
					 yc >= topOuterBound && yc <= bottomOuterBound    )
				{
					s32 locationWidth = locationRight-locationLeft,
						locationHeight = locationBottom-locationTop,
						leftInnerBound = locationLeft+locationWidth/3,
						rightInnerBound = locationRight-locationWidth/3,
						topInnerBound = locationTop+locationHeight/3,
						bottomInnerBound = locationBottom-locationHeight/3;

					if ( leftInnerBound > locationLeft+5 )
						leftInnerBound = locationLeft+5;
					if ( topInnerBound > locationTop+5 )
						topInnerBound = locationTop+5;
					if ( rightInnerBound < locationRight-5 )
						rightInnerBound = locationRight-5;
					if ( bottomInnerBound < locationBottom-5 )
						bottomInnerBound = locationBottom-5;

					if ( xc >= leftInnerBound && xc <= rightInnerBound &&   // Location in absolute center
						 yc >= topInnerBound && yc <= bottomInnerBound    )
					{
						return LOC_SEL_MIDDLE;
					} // Invariant: not in center
					else if ( xc >= leftInnerBound && xc <= rightInnerBound ) // Location in horizontal center
					{
						if ( yc > bottomInnerBound )
							return LOC_SEL_SOUTH;
						else
							return LOC_SEL_NORTH;
					} // Invariant: on west or east
					else if ( yc >= topInnerBound && yc <= bottomInnerBound )
					{
						if ( xc > rightInnerBound )
							return LOC_SEL_EAST;
						else
							return LOC_SEL_WEST;
					} // Invariant: is on a corner
					else if ( xc < leftInnerBound && yc < topInnerBound )
						return LOC_SEL_NW;
					else if ( xc > rightInnerBound && yc > bottomInnerBound )
						return LOC_SEL_SE;
					else if ( xc < leftInnerBound && yc > bottomInnerBound )
						return LOC_SEL_SW;
					else if ( xc > rightInnerBound && yc < topInnerBound )
						return LOC_SEL_NE;
				}
			}
		}
	}
	return LOC_SEL_NOTHING;
}

bool GuiMap::moveLocation(u32 downX, u32 downY, u32 upX, u32 upY)
{
	return false;
}

void GuiMap::doubleClickLocation(s32 xPos, s32 yPos)
{
	xPos = (s32(((double)xPos)/getZoom()) + display().x),
	yPos = (s32(((double)yPos)/getZoom()) + display().y);
	u16 selectedLoc = selections().getSelectedLocation();
	if ( selectedLoc == NO_LOCATION )
	{
		selections().selectLocation(xPos, yPos, this, !lockAnywhere);
		this->Redraw(false);
		selectedLoc = selections().getSelectedLocation();
	}

	if ( selectedLoc != NO_LOCATION )
	{
		ChkLocation* locRef;
		if ( getLocation(locRef, selectedLoc) )
		{
			s32 locLeft = std::min(locRef->xc1, locRef->xc2),
				locRight = std::max(locRef->xc1, locRef->xc2),
				locTop = std::min(locRef->yc1, locRef->yc2),
				locBottom = std::max(locRef->yc1, locRef->yc2);
	
			if ( xPos >= locLeft && xPos <= locRight && yPos >= locTop && yPos <= locBottom )
			{
				if ( chkd.locationWindow.getHandle() == NULL )
				{
					chkd.locationWindow.CreateThis(chkd.getHandle());
					ShowWindow(chkd.locationWindow.getHandle(), SW_SHOWNORMAL);
				}
				else
				{
					chkd.locationWindow.RefreshLocationInfo();
					ShowWindow(chkd.locationWindow.getHandle(), SW_SHOW);
				}
			}
		}
	}
}

void GuiMap::openTileProperties(s32 xClick, s32 yClick)
{
	u16 xSize = XSize();
	u16 currTile;
	
	if ( MTXM().get<u16>(currTile, 2*(yClick/32*xSize + xClick/32)) )
	{
		if ( selections().hasTiles() )
			selections().removeTiles();
							
		selections().addTile(currTile, u16(xClick/32), u16(yClick/32), -1);
		RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
		if ( chkd.tilePropWindow.getHandle() != NULL )
			chkd.tilePropWindow.UpdateTile();
		else
			chkd.tilePropWindow.CreateThis(chkd.getHandle());

		ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
	}
}

void GuiMap::EdgeDrag(HWND hWnd, int x, int y, u8 layer)
{
	if ( isDragging() )
	{
		if ( x < 0 )
			x = 0;
		if ( y < 0 )
			y = 0;

		RECT rcMap;
		GetClientRect(hWnd, &rcMap);
		TrackMouse(DEFAULT_HOVER_TIME);
		if ( x == 0 ) // Cursor on the left
		{
			if ( (display().x+16)/32 > 0 )
				selection.setEndDrag( ((display().x+16)/32-1)*32, selection.getEndDrag().y );
			if ( display().x > 0 )
				display().x = selection.getEndDrag().x;
		}
		else if ( x >= rcMap.right-2 ) // Cursor on the right
		{
			if ( (display().x+rcMap.right)/32 <XSize() )
				selection.setEndDrag( ((display().x+rcMap.right)/32+1)*32, selection.getEndDrag().y );
			display().x = selection.getEndDrag().x - rcMap.right;
		}
		if ( y == 0 ) // Cursor on the top
		{
			if ( (display().y+16)/32 > 0 )
				selection.setEndDrag( selection.getEndDrag().x, ((display().y+16)/32-1)*32 );
			if ( display().y > 0 )
				display().y = selection.getEndDrag().y;
		}
		else if ( y >= rcMap.bottom-2 ) // Cursor on the bottom
		{
			if ( (display().y+rcMap.bottom)/32 < YSize() )
				selection.setEndDrag( selection.getEndDrag().x, ((display().y+rcMap.bottom)/32+1)*32 );
			display().y = selection.getEndDrag().y - rcMap.bottom;
		}
		Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
		Redraw(false);
	}
}

u8 GuiMap::getDisplayOwner(u8 player)
{
	u8 owner = 0;
	if ( getPlayerOwner(player, owner) )
	{
		switch ( owner )
		{
			case 1: case 5: return 2; break;
			case 2: case 6: return 3; break;
			case 3: return 1; break;
			case 7: return 4; break;
		}
	}
	return 0;
}

void GuiMap::refreshScenario()
{
	selection.removeTiles();
	selection.removeUnits();
	chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

	if ( chkd.unitWindow.getHandle() != nullptr )
		chkd.unitWindow.RepopulateList();
	if ( chkd.locationWindow.getHandle() != NULL )
	{
		if ( chkd.maps.curr->numLocations() == 0 )
			chkd.locationWindow.DestroyThis();
		else
			chkd.locationWindow.RefreshLocationInfo();
	}
	if ( chkd.mapSettingsWindow.getHandle() != NULL )
		chkd.mapSettingsWindow.RefreshWindow();
	if ( chkd.textTrigWindow.getHandle() != NULL )
		chkd.textTrigWindow.RefreshWindow();
	if ( chkd.trigEditorWindow.getHandle() != NULL )
		chkd.trigEditorWindow.RefreshWindow();

	Redraw(true);
}

void GuiMap::clearSelection()
{
	if ( this != nullptr )
	{
		selection.removeTiles();
		selection.removeUnits();
	}
}

void GuiMap::selectAll()
{
	if ( this != nullptr )
	{
		switch ( layer )
		{
			case LAYER_TERRAIN:
				{
					if ( selection.hasTiles() )
						selection.removeTiles();

					u16 tileValue,
						width = XSize(), height = YSize(),
						x=0, y=0;

					for ( x=0; x<width; x++ ) // Add the top row
					{
						if ( MTXM().get<u16>(tileValue, x*2) )
							selection.addTile( tileValue, x, y, NEIGHBOR_TOP );
					}

					for ( y=0; y<height-1; y++ ) // Add the middle rows
					{
						if ( MTXM().get<u16>(tileValue, y*width*2) )
							selection.addTile( tileValue, 0, y, NEIGHBOR_LEFT ); // Add the left tile

						for ( x=1; x<width-1; x++ )
						{
							if ( MTXM().get<u16>(tileValue, (y*width+x)*2) )
								selection.addTile( tileValue, x, y, 0x0 ); // Add the middle portion of the row
						}
						if ( MTXM().get<u16>(tileValue, (y*width+width-1)*2) )
							selection.addTile( tileValue, width-1, y, NEIGHBOR_RIGHT); // Add the right tile
					}

					if ( MTXM().get<u16>(tileValue, (height-1)*width*2) )
						selection.addTile( tileValue, 0, height-1, NEIGHBOR_LEFT|NEIGHBOR_BOTTOM);

					for ( x=1; x<width-1; x++ ) // Add the bottom row
					{
						if ( MTXM().get<u16>(tileValue, ((height-1)*width+x)*2) )
							selection.addTile( tileValue, x, height-1, NEIGHBOR_BOTTOM );
					}
					if ( MTXM().get<u16>(tileValue, ((height-1)*width+width-1)*2 ) )
						selection.addTile( tileValue, width-1, height-1, NEIGHBOR_RIGHT|NEIGHBOR_BOTTOM );

					RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
				}
				break;
			case LAYER_UNITS:
				{
					u32 unitTableSize = UNIT().size(),
						numUnits = unitTableSize/UNIT_STRUCT_SIZE;

					HWND hUnitTree = chkd.unitWindow.listUnits.getHandle();
					LVFINDINFO findInfo = { };
					findInfo.flags = LVFI_PARAM;

					chkd.unitWindow.SetChangeHighlightOnly(true);
					for ( u16 i=0; i<numUnits; i++ )
					{
						if ( !selection.unitIsSelected(i) )
						{
							selection.addUnit(i);
							if ( chkd.unitWindow.getHandle() != nullptr )
							{
								findInfo.lParam = i;
								s32 lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
								ListView_SetItemState(hUnitTree, lvIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
							}
						}
					}
					chkd.unitWindow.SetChangeHighlightOnly(false);
					Redraw(true);
				}
				break;
		}
	}
}

void GuiMap::deleteSelection()
{
	if ( this != nullptr )
	{
		switch ( layer )
		{
			case LAYER_TERRAIN:
				{
					u16 xSize = XSize();

					auto &selTiles = selection.getTiles();
					for ( auto &tile : selTiles )
					{
						u32 location = 2*xSize*tile.yc+2*tile.xc; // Down y rows, over x columns
						undoStacks.AddUndo(ReversiblePtr(new TileChange(tile.xc, tile.yc, getTile(tile.xc, tile.yc))));
						TILE().replace<u16>(location, 0);
						MTXM().replace<u16>(location, 0);
					}

					selection.removeTiles();
				}
				break;

			case LAYER_UNITS:
				{
					if ( chkd.unitWindow.getHandle() != nullptr )
						SendMessage(chkd.unitWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, NULL), NULL);
					else
					{
						ReversibleActionsPtr deletes(new ReversibleActions);
						while ( selection.hasUnits() )
						{
							// Get the highest index in the selection
								u16 index = selection.getHighestIndex();
								selection.removeUnit(index);
							
								ChkUnit* delUnit;
								if ( getUnit(delUnit, index) )
									deletes->Insert(std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index, *delUnit)));

								deleteUnit(index);
						}
						undoStacks.AddUndo(deletes);
					}
				}
				break;

			case LAYER_LOCATIONS:
				{
					if ( chkd.locationWindow.getHandle() != NULL )
						chkd.locationWindow.DestroyThis();
				
					ChkLocation* loc;
					u16 index = selection.getSelectedLocation();
					if ( index != NO_LOCATION && getLocation(loc, index) )
					{
						undoStacks.AddUndo(std::shared_ptr<LocationCreateDel>(new LocationCreateDel(index)));

						chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

						loc->elevation = 0;
						loc->xc1 = 0;
						loc->xc2 = 0;
						loc->yc1 = 0;
						loc->yc2 = 0;
						u16 stringNum = loc->stringNum;
						loc->stringNum = 0;
						if ( stringNum > 0 )
						{
							removeUnusedString(stringNum);
							refreshScenario();
						}

						selections().selectLocation(NO_LOCATION);
					}
				}
				break;
		}

		RedrawMap = true;
		RedrawMiniMap = true;
		RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
	}
}

void GuiMap::paste(s32 mapClickX, s32 mapClickY, CLIPBOARD &clipboard)
{
	//SetFocus(getHandle());
	s32 xc = mapClickX, yc = mapClickY;
	if ( layer == LAYER_UNITS )
	{
		this->snapUnitCoordinate(xc, yc);
		clipboard.doPaste(layer, xc, yc, scenario(), undos(), stackUnits);
	}
	else
		clipboard.doPaste(layer, xc, yc, scenario(), undos(), stackUnits);

	Redraw(true);
}

void GuiMap::undo()
{
	switch ( layer )
	{
		case LAYER_TERRAIN:
			undoStacks.doUndo((int32_t)UndoTypes::TileChange, this);
			//undoStacks.doUndo(UNDO_TERRAIN, scenario(), selections());
			break;
		case LAYER_UNITS:
			undoStacks.doUndo((int32_t)UndoTypes::UnitChange, this);
			//undoStacks.doUndo(UNDO_UNIT, scenario(), selections());
			if ( chkd.unitWindow.getHandle() != nullptr )
				chkd.unitWindow.RepopulateList();
			break;
		case LAYER_LOCATIONS:
			{
				undoStacks.doUndo((int32_t)UndoTypes::LocationChange, this);
				//undoStacks.doUndo(UNDO_LOCATION, scenario(), selections());
				if ( chkd.locationWindow.getHandle() != NULL )
				{
					if ( chkd.maps.curr->numLocations() == 0 )
						chkd.locationWindow.RefreshLocationInfo();
					else
						chkd.locationWindow.DestroyThis();
				}
				refreshScenario();
				chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
			}
			break;
	}
	Redraw(true);
}

void GuiMap::redo()
{
	switch ( layer )
	{
		case LAYER_TERRAIN:
			undoStacks.doRedo((int32_t)UndoTypes::TileChange, this);
			//undoStacks.doRedo(UNDO_TERRAIN, scenario(), selections());
			break;
		case LAYER_UNITS:
			undoStacks.doRedo((int32_t)UndoTypes::UnitChange, this);
			//undoStacks.doRedo(UNDO_UNIT, scenario(), selections());
			if ( chkd.unitWindow.getHandle() != nullptr )
				chkd.unitWindow.RepopulateList();
			break;
		case LAYER_LOCATIONS:
			undoStacks.doRedo((int32_t)UndoTypes::LocationChange, this);
			//undoStacks.doRedo(UNDO_LOCATION, scenario(), selections());
			if ( chkd.locationWindow.getHandle() != NULL )
			{
				if ( chkd.maps.curr->numLocations() == 0 )
					chkd.locationWindow.DestroyThis();
				else
					chkd.locationWindow.RefreshLocationInfo();
			}
			refreshScenario();
			break;
	}
	Redraw(true);
}

void GuiMap::ChangesMade()
{
	notifyChange(true);
}

void GuiMap::ChangesReversed()
{
	changesUndone();
}

float GuiMap::MiniMapScale(u16 xSize, u16 ySize)
{
	if ( xSize >= ySize && xSize > 0 )
	{
		if ( 128.0/xSize > 1 )
			return (float)(128/xSize);
		else
			return (float)(128.0/xSize);
	}
	else
	{
		if ( 128.0/ySize > 1 && ySize > 0 )
			return (float)(128/ySize);
		else
			return (float)(128.0/ySize);
	}
	return 1;
}

HWND GuiMap::getHandle()
{
	return ClassWindow::getHandle();
}

void GuiMap::PaintMap(GuiMapPtr currMap, bool pasting, CLIPBOARD& clipboard )
{
	InvalidateRect(this->getHandle(), NULL, FALSE);

	RECT rect;
	GetClientRect(this->getHandle(), &rect);
	s32 width = (rect.right - rect.left+32)/32*32;
	s32 height = (rect.bottom - rect.top+32)/32*32;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(this->getHandle(), &ps);

	u16 scaledWidth = (u16(((double)width)/zoom))/32*32,
		scaledHeight = (u16(((double)height)/zoom))/32*32;

	u32 bitmapSize = (u32)scaledWidth*(u32)scaledHeight*3;

	if ( this->RedrawMap == true )
	{
		this->RedrawMap = false;
		if ( this->lpvBits != nullptr )
			delete[] this->lpvBits;
		this->bitMapWidth = scaledWidth;
		this->bitMapHeight = scaledHeight;
		this->lpvBits = new u8[bitmapSize];

		DeleteObject(this->MemBitmap);
		DeleteDC    (this->MemhDC);
		this->MemhDC = CreateCompatibleDC(hDC);
		this->MemBitmap = CreateCompatibleBitmap(hDC, scaledWidth, scaledHeight);
				
		SelectObject(this->MemhDC, this->MemBitmap);
		
		if ( currMap == nullptr || currMap.get() == this ) // Only redraw minimap for active window
			RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);

		graphics.DrawMap(
				 this->bitWidth(),
				 this->bitHeight(),
				 this->display().x,
				 this->display().y,
				 this->GraphicBits(),
				 this->selections(),
				 this->layer,
				 this->GetMemhDC(),
				 !this->lockAnywhere
			   ); // Terrain, Grid, Units, Sprites, Debug
	}

	HDC TemphDC = CreateCompatibleDC(hDC);
	HBITMAP Tempbitmap = CreateCompatibleBitmap(hDC, scaledWidth, scaledHeight);
	SelectObject(TemphDC, Tempbitmap);
	BitBlt(TemphDC, 0, 0, scaledWidth, scaledHeight, this->MemhDC, 0, 0, SRCCOPY);

	if ( currMap == nullptr || currMap.get() == this )
	{
		DrawTools( TemphDC,
				   Tempbitmap,
				   scaledWidth,
				   scaledHeight,
				   this->display().x,
				   this->display().y,
				   this->selections(),
				   pasting,
				   clipboard,
				   *this,
				   this->getLayer()
				 ); // Drag and paste graphics

		if ( layer != LAYER_LOCATIONS )
			DrawSelectingFrame(TemphDC, selections(), display().x, display().y, bitWidth(), bitHeight(), zoom);
	}
	
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchBlt(hDC, 0, 0, width, height, TemphDC, 0, 0, scaledWidth, scaledHeight, SRCCOPY);

	DeleteDC	(TemphDC);
	DeleteObject(Tempbitmap);
	EndPaint(getHandle(), &ps);
}

void GuiMap::PaintMiniMap(HWND hWnd)
{
	if ( this != nullptr )
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		s32 width = rect.right - rect.left;
		s32 height = rect.bottom - rect.top;
		PAINTSTRUCT ps;
		HDC hDC, MemhDC;
		HBITMAP MemBitmap;
		hDC = BeginPaint(hWnd, &ps);
		MemhDC = CreateCompatibleDC(hDC);
		MemBitmap = CreateCompatibleBitmap(hDC, width, height);
		SelectObject(MemhDC, MemBitmap);

		if ( this->RedrawMiniMap )
		{
			this->RedrawMiniMap = false;
			DeleteObject(this->MemMiniBitmap);
			DeleteDC    (this->MemMinihDC);
			this->MemMinihDC = CreateCompatibleDC(hDC);
			this->MemMiniBitmap = CreateCompatibleBitmap(hDC, width, height);
				
			SelectObject(this->MemMinihDC, this->MemMiniBitmap);

			DrawMiniMap( this->MemMinihDC,
						 chkd.mainPlot.leftBar.miniMap.getHandle(),
						 this->XSize(),
						 this->YSize(),
						 this->MiniMapScale(this->XSize(), this->YSize()),
						 *this
					   );

			BitBlt(MemhDC, 0, 0, width, height, this->MemMinihDC, 0, 0, SRCCOPY);
			DrawMiniMapBox( MemhDC,
							this->display().x,
							this->display().y,
							this->bitWidth(),
							this->bitHeight(),
							this->XSize(),
							this->YSize(),
							this->MiniMapScale(this->XSize(), this->YSize())
						  );
		}
		else
		{
			BitBlt(MemhDC, 0, 0, width, height, this->MemMinihDC, 0, 0, SRCCOPY);
			DrawMiniMapBox( MemhDC,
							this->display().x,
							this->display().y,
							this->bitWidth(),
							this->bitHeight(),
							this->XSize(),
							this->YSize(),
							this->MiniMapScale(this->XSize(), this->YSize())
						  );
		}

		BitBlt(hDC, 0, 0, width, height, MemhDC, 0, 0, SRCCOPY);
		DeleteObject(MemBitmap);
		DeleteDC    (MemhDC);
		DeleteDC    (hDC);
	}
}

void GuiMap::Redraw(bool includeMiniMap)
{
	if ( this )
	{
		RedrawMap = true;
		if ( includeMiniMap )
			RedrawMiniMap = true;

		RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
	}
}

void GuiMap::ValidateBorder(s32 screenWidth, s32 screenHeight)
{
	if ( displayPivot.x < 0 )
		displayPivot.x = 0;
	else if ( displayPivot.x > ((s32)XSize())*32-screenWidth )
	{
		if ( screenWidth > ((s32)XSize())*32 )
			displayPivot.x = 0;
		else
			displayPivot.x = XSize()*32-screenWidth;
	}

	if ( displayPivot.y < 0 )
		displayPivot.y = 0;
	else if ( displayPivot.y > YSize()*32-screenHeight )
	{
		if ( screenHeight > YSize()*32 )
			displayPivot.y = 0;
		else
			displayPivot.y = YSize()*32-screenHeight;
	}
}

void GuiMap::SetGrid(s16 xSize, s16 ySize)
{
	POINTS& gridSize = (POINTS&)grid(0).size;
	if ( gridSize.x == xSize && gridSize.y == ySize )
	{
		gridSize.x = 0;
		gridSize.y = 0;
	}
	else
	{
		gridSize.x = xSize;
		gridSize.y = ySize;
	}
	UpdateGridSizesMenu();
	Redraw(false);
}

void GuiMap::SetGridColor(u8 red, u8 green, u8 blue)
{
	GRID& currGrid = grid(0);

	currGrid.red = red;
	currGrid.green = green;
	currGrid.blue = blue;

	if ( currGrid.size.x == 0 || currGrid.size.y == 0 )
		SetGrid(32, 32);

	UpdateGridColorMenu();
	Redraw(false);
}

void GuiMap::ToggleDisplayElevations()
{
	graphics.ToggleDisplayElevations();
	UpdateTerrainViewMenuItems();
	Redraw(false);
	if ( chkd.terrainPalWindow.getHandle() != NULL )
		RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingElevations()
{
	return graphics.DisplayingElevations();
}

void GuiMap::ToggleTileNumSource(bool MTXMoverTILE)
{
	graphics.ToggleTileNumSource(MTXMoverTILE);
	UpdateTerrainViewMenuItems();
	Redraw(false);
	if ( chkd.terrainPalWindow.getHandle() != NULL )
		RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingTileNums()
{
	return graphics.DisplayingTileNums();
}

bool GuiMap::snapUnitCoordinate(s32& x, s32& y)
{
	if ( snapUnits )
	{
		GRID grid = this->grid(0);
		if ( grid.size.x > 0 && grid.size.y > 0 )
		{
			double intervalNum = (double(x-grid.offset.x))/grid.size.x;
			s32 xIntervalNum = (s32)round(intervalNum);
			x = xIntervalNum*grid.size.x;
			intervalNum = (double(y-grid.offset.y))/grid.size.y;
			s32 yIntervalNum = (s32)round(intervalNum);
			y = yIntervalNum*grid.size.y;
			return true;
		}
	}
	return false;
}

void GuiMap::ToggleUnitSnap()
{
	snapUnits = !snapUnits;
	UpdateUnitMenuItems();
}

void GuiMap::ToggleUnitStack()
{
	stackUnits = !stackUnits;
	UpdateUnitMenuItems();
}

void GuiMap::ToggleLocationNameClip()
{
	if ( this != nullptr )
	{
		graphics.ToggleLocationNameClip();
		chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, graphics.ClippingLocationNames());
		Redraw(false);
	}
}

void GuiMap::SetLocationSnap(u32 flags)
{
	if ( this != nullptr )
	{
		bool prevSnapLocations = snapLocations;
		if ( flags & NO_LOCATION_SNAP )
			snapLocations = false;
		else
			snapLocations = true;

		if ( flags & SNAP_LOCATION_TO_TILE )
		{
			if ( prevSnapLocations && locSnapTileOverGrid )
				snapLocations = false;
			else
				locSnapTileOverGrid = true;
		}
		else if ( flags & SNAP_LOCATION_TO_GRID )
		{
			if ( prevSnapLocations && !locSnapTileOverGrid )
				snapLocations = false;
			else
				locSnapTileOverGrid = false;
		}

		UpdateLocationMenuItems();
	}
}

void GuiMap::ToggleLockAnywhere()
{
	lockAnywhere = !lockAnywhere;
	UpdateLocationMenuItems();
	if ( selections().getSelectedLocation() == 63 )
		selections().selectLocation(NO_LOCATION);
	chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
	Redraw(false);
}

bool GuiMap::LockAnywhere()
{
	return lockAnywhere;
}

bool GuiMap::GetSnapIntervals(u32& x, u32& y, u32& xOffset, u32& yOffset)
{
	if ( snapLocations )
	{
		if ( locSnapTileOverGrid )
		{
			x = 32;
			y = 32;
			xOffset = 0;
			yOffset = 0;
		}
		else
		{
			GRID& aGrid = grid(0);
			if ( aGrid.size.x > 0 || aGrid.size.y > 0 )
			{
				x = aGrid.size.x;
				y = aGrid.size.y;
				xOffset = aGrid.offset.x;
				yOffset = aGrid.offset.y;
			}
		}
		return true;
	}
	else
		return false;
}

bool GuiMap::SnapLocationDimensions(s32& x1, s32& y1, s32& x2, s32& y2, u32 flags)
{
	u32 lengthX, lengthY, startSnapX, startSnapY;
	if ( GetSnapIntervals(lengthX, lengthY, startSnapX, startSnapY) )
	{
		double intervalNum = (double(x1-startSnapX))/lengthX;
		s32 xStartIntervalNum = (s32)round(intervalNum);
		intervalNum = (double(y1-startSnapY))/lengthY;
		s32 yStartIntervalNum = (s32)round(intervalNum);
		intervalNum = (double(x2-startSnapX))/lengthX;
		s32 xEndIntervalNum = (s32)round(intervalNum);
		intervalNum = (double(y2-startSnapY))/lengthY;
		s32 yEndIntervalNum = (s32)round(intervalNum);

		if ( flags & SNAP_LOC_X1 )
			x1 = xStartIntervalNum*lengthX + startSnapX;
		if ( flags & SNAP_LOC_Y1 )
			y1 = yStartIntervalNum*lengthY + startSnapY;
		if ( flags & SNAP_LOC_X2 )
			x2 = xEndIntervalNum*lengthX + startSnapX;
		if ( flags & SNAP_LOC_Y2 )
			y2 = yEndIntervalNum*lengthY + startSnapY;

		return flags != 0;
	}
	else
		return false;
}

void GuiMap::UpdateLocationMenuItems()
{
	chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, graphics.ClippingLocationNames());
	chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOTILE, snapLocations && locSnapTileOverGrid);
	chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOACTIVEGRID, snapLocations && !locSnapTileOverGrid);
	chkd.mainMenu.SetCheck(ID_LOCATIONS_NOSNAP, !snapLocations);
	chkd.mainMenu.SetCheck(ID_LOCATIONS_LOCKANYWHERE, lockAnywhere);
}

void GuiMap::UpdateZoomMenuItems()
{
	u32 zoomNum;
	for ( zoomNum=0; zoomNum<NUM_ZOOMS; zoomNum++ )
	{
		if ( zoom == zooms[zoomNum] )
			break;
	}

	chkd.mainToolbar.zoomBox.SetSel(zoomNum);

	chkd.mainMenu.SetCheck(ID_ZOOM_10, zoom == .10);
	chkd.mainMenu.SetCheck(ID_ZOOM_25, zoom == .25);
	chkd.mainMenu.SetCheck(ID_ZOOM_33, zoom == .33);
	chkd.mainMenu.SetCheck(ID_ZOOM_50, zoom == .50);
	chkd.mainMenu.SetCheck(ID_ZOOM_66, zoom == .66);
	chkd.mainMenu.SetCheck(ID_ZOOM_100, zoom == 1.00);
	chkd.mainMenu.SetCheck(ID_ZOOM_150, zoom == 1.50);
	chkd.mainMenu.SetCheck(ID_ZOOM_200, zoom == 2.00);
	chkd.mainMenu.SetCheck(ID_ZOOM_300, zoom == 3.00);
	chkd.mainMenu.SetCheck(ID_ZOOM_400, zoom == 4.00);
}

void GuiMap::UpdateGridSizesMenu()
{
	GRID& mainGrid = grid(0);

	chkd.mainMenu.SetCheck(ID_GRID_ULTRAFINE, false);
	chkd.mainMenu.SetCheck(ID_GRID_FINE, false);
	chkd.mainMenu.SetCheck(ID_GRID_NORMAL, false);
	chkd.mainMenu.SetCheck(ID_GRID_LARGE, false);
	chkd.mainMenu.SetCheck(ID_GRID_EXTRALARGE, false);
	chkd.mainMenu.SetCheck(ID_GRID_CUSTOM, false);
	chkd.mainMenu.SetCheck(ID_GRID_DISABLED, false);
	
	if ( mainGrid.size.x == 0 && mainGrid.size.y == 0 )
		chkd.mainMenu.SetCheck(ID_GRID_DISABLED, true);
	else if ( (mainGrid.offset.x != 0 || mainGrid.offset.y != 0) ||
			  mainGrid.size.x != mainGrid.size.y					)
		chkd.mainMenu.SetCheck(ID_GRID_CUSTOM, true);
	else if ( mainGrid.size.x == 8 && mainGrid.size.y == 8 )
		chkd.mainMenu.SetCheck(ID_GRID_ULTRAFINE, true);
	else if ( mainGrid.size.x == 16 && mainGrid.size.y == 16 )
		chkd.mainMenu.SetCheck(ID_GRID_FINE, true);
	else if ( mainGrid.size.x == 32 && mainGrid.size.y == 32 )
		chkd.mainMenu.SetCheck(ID_GRID_NORMAL, true);
	else if ( mainGrid.size.x == 64 && mainGrid.size.y == 64 )
		chkd.mainMenu.SetCheck(ID_GRID_LARGE, true);
	else if ( mainGrid.size.x == 128 && mainGrid.size.y == 128 )
		chkd.mainMenu.SetCheck(ID_GRID_EXTRALARGE, true);
	else
		chkd.mainMenu.SetCheck(ID_GRID_CUSTOM, true);
}

void GuiMap::UpdateGridColorMenu()
{
	GRID& mainGrid = grid(0);

	chkd.mainMenu.SetCheck(ID_COLOR_BLACK, false);
	chkd.mainMenu.SetCheck(ID_COLOR_GREY, false);
	chkd.mainMenu.SetCheck(ID_COLOR_WHITE, false);
	chkd.mainMenu.SetCheck(ID_COLOR_GREEN, false);
	chkd.mainMenu.SetCheck(ID_COLOR_RED, false);
	chkd.mainMenu.SetCheck(ID_COLOR_BLUE, false);
	chkd.mainMenu.SetCheck(ID_COLOR_OTHER, false);
	
	if ( mainGrid.red == 0 && mainGrid.green == 0 && mainGrid.blue == 0 )
		chkd.mainMenu.SetCheck(ID_COLOR_BLACK, true);
	else if ( mainGrid.red == 72 && mainGrid.green == 72 && mainGrid.blue == 88 )
		chkd.mainMenu.SetCheck(ID_COLOR_GREY, true);
	else if ( mainGrid.red == 255 && mainGrid.green == 255 && mainGrid.blue == 255 )
		chkd.mainMenu.SetCheck(ID_COLOR_WHITE, true);
	else if ( mainGrid.red == 16 && mainGrid.green == 252 && mainGrid.blue == 24 )
		chkd.mainMenu.SetCheck(ID_COLOR_GREEN, true);
	else if ( mainGrid.red == 244 && mainGrid.green == 4 && mainGrid.blue == 4 )
		chkd.mainMenu.SetCheck(ID_COLOR_RED, true);
	else if ( mainGrid.red == 36 && mainGrid.green == 36 && mainGrid.blue == 252 )
		chkd.mainMenu.SetCheck(ID_COLOR_BLUE, true);
	else
		chkd.mainMenu.SetCheck(ID_COLOR_OTHER, true);
}

void GuiMap::UpdateTerrainViewMenuItems()
{
	chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEELEVATIONS, graphics.DisplayingElevations());

	chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, false);
	chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, false);
	if ( graphics.DisplayingTileNums() )
	{
		if ( graphics.mtxmOverTile() )
			chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, true);
		else
			chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, true);
	}
}

void GuiMap::UpdateUnitMenuItems()
{
	chkd.mainMenu.SetCheck(ID_UNITS_UNITSSNAPTOGRID, snapUnits);
	chkd.mainMenu.SetCheck(ID_UNITS_ALLOWSTACK, stackUnits);
}

void GuiMap::Scroll(u32 flags)
{
	SCROLLINFO scrollbars = { };
	scrollbars.cbSize = sizeof(SCROLLINFO);
	scrollbars.fMask = SIF_ALL;
	scrollbars.nMin = 0;

	RECT rcMap;
	GetClientRect(getHandle(), &rcMap);
	s32 screenWidth  = (s32)((double(rcMap.right-rcMap.left))/zoom),
		screenHeight = (s32)((double(rcMap.bottom-rcMap.top))/zoom);

	if ( (flags&VALIDATE_BORDER) )
		ValidateBorder(screenWidth, screenHeight);

	if ( (flags&SCROLL_X) )
	{
		scrollbars.nPos = displayPivot.x; 
		scrollbars.nMax = XSize()*32;
		scrollbars.nPage = screenWidth;
		SetScrollInfo(getHandle(), SB_HORZ, &scrollbars, true);
	}
	if ( (flags&SCROLL_Y) )
	{
		scrollbars.nPos = displayPivot.y;
		scrollbars.nMax = YSize()*32;
		scrollbars.nPage = screenHeight;
		SetScrollInfo(getHandle(), SB_VERT, &scrollbars, true);
	}
	Redraw(true);
}

void GuiMap::setMapId(u16 mapId)
{
	this->mapId = mapId;
}

u16 GuiMap::getMapId()
{
	return this->mapId;
}

void GuiMap::notifyChange(bool undoable)
{
	if ( changeLock == false && undoable == false )
		changeLock = true;

	if ( !unsavedChanges )
	{
		unsavedChanges = true;
		addAsterisk();
	}
}

void GuiMap::changesUndone()
{
	if ( !changeLock )
	{
		unsavedChanges = false;
		removeAsterisk();
	}
}

bool GuiMap::changesLocked()
{
	return changeLock;
}

void GuiMap::addAsterisk()
{
	int length = GetWindowTextLength(getHandle())+1;
	if ( length > 0 )
	{
		char* text;
		try { text = new char[length+1]; }
		catch ( std::bad_alloc ) { return; }

		if ( GetWindowText(getHandle(), (LPSTR)text, length) > 0 )
		{
			std::strcat(text, "*");
			SetWindowText(getHandle(), text);
		}
	}
}

void GuiMap::removeAsterisk()
{
	int length = GetWindowTextLength(getHandle())+1;
	if ( length > 0 )
	{
		char* text;
		try { text = new char[length]; }
		catch ( std::bad_alloc ) { return; }
		
		if ( GetWindowText(getHandle(), (LPSTR)text, length) > 0 )
		{
			if ( text[length-2] == '*' )
			{
				text[length-2] = '\0';
				SetWindowText(getHandle(), text);
			}
		}
	}
}

void GuiMap::updateMenu()
{
	UpdateLocationMenuItems();
	chkd.mainToolbar.layerBox.SetSel(layer);
	UpdateZoomMenuItems();
	chkd.mainToolbar.playerBox.SetSel(player);
	UpdateGridSizesMenu();
	UpdateGridColorMenu();
	UpdateTerrainViewMenuItems();
	UpdateUnitMenuItems();
}

bool GuiMap::CreateThis(HWND hClient, const char* title)
{
	if ( !WindowClassIsRegistered("MdiChild") )
	{
		UINT style = CS_DBLCLKS;
		HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
		HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);

		if ( !RegisterWindowClass(CS_DBLCLKS, NULL, hCursor, hBackground, NULL, "MdiChild", NULL, true) )
			DestroyCursor(hCursor);
	}

	return CreateMdiChild(title, WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hClient);
}

void GuiMap::ReturnKeyPress()
{
	if ( this != nullptr )
	{
		if ( getLayer() == LAYER_UNITS )
		{
			if ( selection.hasUnits() )
			{
				if ( chkd.unitWindow.getHandle() == nullptr )
					chkd.unitWindow.CreateThis(chkd.getHandle());
				ShowWindow(chkd.unitWindow.getHandle(), SW_SHOW);
			}
		}
		else if ( getLayer() == LAYER_LOCATIONS )
		{
			if ( selection.getSelectedLocation() != NO_LOCATION )
			{
				if ( chkd.locationWindow.getHandle() == NULL )
				{
					if ( chkd.locationWindow.CreateThis(chkd.getHandle()) )
						ShowWindow(chkd.locationWindow.getHandle(), SW_SHOWNORMAL);
				}
				else
					ShowWindow(chkd.locationWindow.getHandle(), SW_SHOW);
			}
		}
	}
}

LRESULT GuiMap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_PAINT: PaintMap(chkd.maps.curr, chkd.maps.clipboard.isPasting(), chkd.maps.clipboard); break;
		case WM_MDIACTIVATE: ActivateMap((HWND)lParam); return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
		case WM_ERASEBKGND: return 0; break; // Prevent background from showing
		case WM_HSCROLL: return HorizontalScroll(hWnd, msg, wParam, lParam); break;
		case WM_VSCROLL: return VerticalScroll(hWnd, msg, wParam, lParam); break;
		case WM_CHAR: return 0; break;
		case WM_SIZE: return DoSize(hWnd, wParam, lParam); break;
		case WM_CLOSE: return ConfirmWindowClose(hWnd); break;
		case WM_DESTROY: return DestroyWindow(hWnd); break;
		case WM_RBUTTONUP: RButtonUp(); break;
		case WM_LBUTTONDBLCLK: LButtonDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_LBUTTONDOWN: LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
		case WM_MOUSEMOVE: MouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
		case WM_MOUSEHOVER: MouseHover(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
		case WM_LBUTTONUP: LButtonUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
		default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}

LRESULT GuiMap::HorizontalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD(wParam) )
	{
		case SB_LINELEFT	  : display().x -= 8;			  break;
		case SB_LINERIGHT	  : display().x += 8;			  break;
		case SB_THUMBPOSITION : display().x = HIWORD(wParam); break;
		case SB_THUMBTRACK	  : display().x = HIWORD(wParam); break;
		case SB_PAGELEFT: case SB_PAGERIGHT:
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				if ( LOWORD(wParam) == SB_PAGELEFT )
					display().x -= (rect.right-rect.left)/2;
				else
					display().x += (rect.right-rect.left)/2;
			}
			break;
		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	Scroll(SCROLL_X|VALIDATE_BORDER);
	Redraw(true);
	return 0;
}

LRESULT GuiMap::VerticalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case SB_LINEUP		  : display().y -= 8;			  break;
		case SB_LINEDOWN	  : display().y += 8;			  break;
		case SB_THUMBPOSITION : display().y = HIWORD(wParam); break;
		case SB_THUMBTRACK	  : display().y = HIWORD(wParam); break;
		case SB_PAGEUP: case SB_PAGEDOWN:
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				if ( LOWORD(wParam) == SB_PAGEUP )
					display().y -= (rect.right-rect.left)/2;
				else
					display().y += (rect.right-rect.left)/2;
			}
			break;
		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	Scroll(SCROLL_Y|VALIDATE_BORDER);
	Redraw(true);
	return 0;
}

void GuiMap::ActivateMap(HWND hWnd)
{
	chkd.tilePropWindow.DestroyThis();
	chkd.unitWindow.DestroyThis();
	chkd.locationWindow.DestroyThis();
	chkd.mapSettingsWindow.DestroyThis();
	chkd.terrainPalWindow.DestroyThis();
	chkd.trigEditorWindow.DestroyThis();
	
	if ( hWnd != NULL )
	{
		chkd.maps.Focus(hWnd);
		Redraw(true);
		chkd.maps.UpdateTreeView();
	}
}

LRESULT GuiMap::DoSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = ClassWindow::WndProc(hWnd, WM_SIZE, wParam, lParam);
	Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
	return result;
}

LRESULT GuiMap::DestroyWindow(HWND hWnd)
{
	chkd.maps.CloseMap(hWnd);
	RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE);
	return ClassWindow::WndProc(hWnd, WM_DESTROY, NULL, NULL);
}

void GuiMap::RButtonUp()
{
	chkd.maps.endPaste();
	ClipCursor(NULL);
	Redraw(true);
}

void GuiMap::LButtonDoubleClick(int x, int y)
{
	if ( layer == LAYER_LOCATIONS )
		doubleClickLocation(x, y);
}

void GuiMap::LButtonDown(int x, int y, WPARAM wParam)
{	
	selection.resetMoved();
	s32 mapClickX = (s32(((double)x)/getZoom()) + display().x),
		mapClickY = (s32(((double)y)/getZoom()) + display().y);

	switch ( wParam )
	{
		case MK_SHIFT|MK_LBUTTON: // Shift + LClick
			if ( layer == LAYER_TERRAIN )
				openTileProperties(mapClickX, mapClickY);
			break;
	
		case MK_CONTROL|MK_LBUTTON: // Ctrl + LClick
			{
				chkd.tilePropWindow.DestroyThis();
				if ( !chkd.maps.clipboard.isPasting() )
				{
					if ( layer == LAYER_TERRAIN ) // Ctrl + Click tile
						selection.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
					else if ( layer == LAYER_DOODADS || layer == LAYER_UNITS || layer == LAYER_SPRITES )
						selection.setDrags(mapClickX, mapClickY);

					LockCursor();
					TrackMouse(DEFAULT_HOVER_TIME);
					setDragging(true);
				}
			}
			break;
	
		case MK_LBUTTON: // LClick
			{
				chkd.tilePropWindow.DestroyThis();
				if ( chkd.maps.clipboard.isPasting() )
					paste(mapClickX, mapClickY, chkd.maps.clipboard);
				else
				{
					if ( selection.hasTiles() )
						selection.removeTiles();
								
					selection.setDrags(mapClickX, mapClickY);
					if ( layer == LAYER_TERRAIN )
						selection.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
					else if ( layer == LAYER_LOCATIONS )
					{
						s32 x1 = mapClickX, y1 = mapClickY;
						if ( SnapLocationDimensions(x1, y1, x1, y1, SNAP_LOC_X1|SNAP_LOC_Y1) )
							selection.setDrags(x1, y1);
						selection.setLocationFlags(getLocSelFlags(mapClickX, mapClickY));
					}

					SetCapture(getHandle());
					TrackMouse(DEFAULT_HOVER_TIME);
					setDragging(true);
					Redraw(false);
				}
			}
			break;
	}
}

void GuiMap::MouseMove(HWND hWnd, int x, int y, WPARAM wParam)
{
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;

	s32 mapHoverX = (s32(((double)x)/getZoom())) + display().x,
		mapHoverY = (s32(((double)y)/getZoom())) + display().y;

	if ( wParam & MK_LBUTTON ) // If click and dragging
	{
		chkd.maps.stickCursor(); // Stop cursor from reverting
		selection.setMoved();
	}
	else // If not click and dragging
		chkd.maps.updateCursor(mapHoverX, mapHoverY); // Determine proper hover cursor

	// Set status bar tracking pos
	char newPos[64];
	std::snprintf(newPos, 64, "%i, %i (%i, %i)", mapHoverX, mapHoverY, mapHoverX / 32, mapHoverY / 32);
	chkd.statusBar.SetText(0, newPos);
	
	switch ( wParam )
	{
		case MK_CONTROL|MK_LBUTTON:
			{
				RECT rcMap;
				GetClientRect(hWnd, &rcMap);

				if ( x == 0 || y == 0 || x == rcMap.right-2 || y == rcMap.bottom-2 )
					EdgeDrag(hWnd, x, y, layer);

				selection.setEndDrag(mapHoverX, mapHoverY);
				if ( layer == LAYER_TERRAIN )
					selection.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
				else if ( layer == LAYER_LOCATIONS )
				{
					s32 x2 = mapHoverX, y2 = mapHoverY;
					if ( SnapLocationDimensions(x2, y2, x2, y2, SNAP_LOC_X2|SNAP_LOC_Y2) )
						selection.setEndDrag(x2, y2);
				}
				else if ( layer == LAYER_UNITS )
				{
					s32 xc = mapHoverX, yc = mapHoverY;
					if ( snapUnitCoordinate(xc, yc) )
						selection.setEndDrag(xc, yc);
				}

				PaintMap(nullptr, chkd.maps.clipboard.isPasting(), chkd.maps.clipboard);
			}
			break;

		case MK_LBUTTON:
			{
				// If pasting, move paste
				if ( chkd.maps.clipboard.isPasting() )
				{
					s32 xc = mapHoverX, yc = mapHoverY;
					if ( layer == LAYER_UNITS )
						snapUnitCoordinate(xc, yc);

					selection.setEndDrag(xc, yc);
					if ( !chkd.maps.clipboard.isPreviousPasteLoc(u16(xc), u16(yc)) )
						paste((s16)xc, (s16)yc, chkd.maps.clipboard);
				}

				if ( isDragging() )
				{
					RECT rcMap;
					GetClientRect(hWnd, &rcMap);
					if ( x == 0 || y == 0 || x >= rcMap.right-2 || y >= rcMap.bottom-2 )
						EdgeDrag(hWnd, x, y, layer);

					selection.setEndDrag( mapHoverX, mapHoverY );
					if ( layer == LAYER_TERRAIN )
						selection.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
					else if ( layer == LAYER_LOCATIONS )
					{
						s32 x2 = mapHoverX, y2 = mapHoverY;
						if ( SnapLocationDimensions(x2, y2, x2, y2, SNAP_LOC_X2|SNAP_LOC_Y2) )
							selection.setEndDrag(x2, y2);
					}
					else if ( layer == LAYER_UNITS )
					{
						s32 xc = mapHoverX, yc = mapHoverY;
						if ( snapUnitCoordinate(xc, yc) )
							selection.setEndDrag(xc, yc);
					}
				}
				PaintMap(nullptr, chkd.maps.clipboard.isPasting(), chkd.maps.clipboard);
			}
			break;

		default:
			{
				if ( chkd.maps.clipboard.isPasting() == true )
				{
					if ( GetKeyState(VK_SPACE) & 0x8000 )
					{
						RECT rcMap;
						GetClientRect(hWnd, &rcMap);
	
						if ( x == 0 || x == rcMap.right-2 || y == 0 || y == rcMap.bottom-2 )
						{
							if		( x == 0 )
								display().x -= 32;
							else if ( x == rcMap.right-2 )
								display().x += 32;
							if		( y == 0 )
								display().y -= 32;
							else if ( y == rcMap.bottom-2 )
								display().y += 32;
	
							Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
						}
					}

					if ( layer == LAYER_UNITS )
						snapUnitCoordinate(mapHoverX, mapHoverY);

					selection.setEndDrag(mapHoverX, mapHoverY);
					PaintMap(nullptr, chkd.maps.clipboard.isPasting(), chkd.maps.clipboard);
				}
			}
			break;
	}
}

void GuiMap::MouseHover(HWND hWnd, int x, int y, WPARAM wParam)
{
	switch ( wParam )
	{
		case MK_CONTROL|MK_LBUTTON:
		case MK_LBUTTON:
			EdgeDrag(hWnd, x, y, layer);
			break;
	
		default:
			{
				if ( chkd.maps.clipboard.isPasting() == true )
				{
					RECT rcMap;
					GetClientRect(hWnd, &rcMap);
	
					if		( x == 0				)
						display().x -= 8;
					else if ( x >= rcMap.right-2	)
						display().x += 8;
					if		( y == 0				)
						display().y -= 8;
					else if ( y >= rcMap.bottom-2 )
						display().y += 8;
	
					Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
					RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);

					x = (s32(((double)x)/getZoom())) + display().x,
					y = (s32(((double)y)/getZoom())) + display().y;
					selection.setEndDrag(x, y);
					TrackMouse(100);
				}
			}
			break;
	}
}

void GuiMap::LButtonUp(HWND hWnd, int x, int y, WPARAM wParam)
{
	ReleaseCapture();
	if ( isDragging() )
	{
		setDragging(false);
		if ( x < 0 ) x = 0;
		if ( y < 0 ) y = 0;
		x = s16(x/getZoom() + display().x);
		y = s16(y/getZoom() + display().y);

		if ( chkd.maps.clipboard.isPasting() )
			paste((s16)x, (s16)y, chkd.maps.clipboard);
	
		if ( layer == LAYER_TERRAIN )
			TerrainLButtonUp(hWnd, x, y, wParam);
		else if ( layer == LAYER_LOCATIONS )
			LocationLButtonUp(hWnd, x, y, wParam);
		else if ( layer == LAYER_UNITS )
			UnitLButtonUp(hWnd, x, y, wParam);

		selection.setStartDrag(-1, -1);
		selection.setEndDrag(-1, -1);
		setDragging(false);
		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	}

	if ( !chkd.maps.clipboard.isPasting() )
		ClipCursor(NULL);
}

void GuiMap::TerrainLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
	selection.setEndDrag((mapX+16)/32, (mapY+16)/32);
	selection.setStartDrag(selection.getStartDrag().x/32, selection.getStartDrag().y/32);
	u16 width = XSize();
					
	if ( wParam == MK_CONTROL && selection.startEqualsEndDrag() ) // Add/remove single tile to/front existing selection
	{
		selection.setEndDrag(mapX/32, mapY/32);
							
		u16 tileValue;
		if ( MTXM().get<u16>(tileValue, (u32)((selection.getEndDrag().y*width+selection.getEndDrag().x)*2)) )
			selection.addTile(tileValue, (u16)selection.getEndDrag().x, (u16)selection.getEndDrag().y);
	}
	else // Add/remove multiple tiles from selection
	{
		selection.sortDragPoints();
	
		if ( selection.getStartDrag().y < selection.getEndDrag().y &&
			 selection.getStartDrag().x < selection.getEndDrag().x )
		{
			bool multiAdd = selection.getStartDrag().x + 1 < selection.getEndDrag().x ||
							selection.getStartDrag().y + 1 < selection.getEndDrag().y;
	
			if ( selection.getEndDrag().x > XSize() )
				selection.setEndDrag(XSize(), selection.getEndDrag().y);
			if ( selection.getEndDrag().y > YSize() )
				selection.setEndDrag(selection.getEndDrag().x, YSize());
	
			for ( int yRow = selection.getStartDrag().y; yRow < selection.getEndDrag().y; yRow++ )
			{
				for ( int xRow = selection.getStartDrag().x; xRow < selection.getEndDrag().x; xRow++ )
				{
					u16 tileValue;
					if ( MTXM().get<u16>((u16 &)tileValue, (u32)((yRow*width+xRow)*2)) )
						selection.addTile(tileValue, xRow, yRow);
				}
			}
		}
	}
}

void GuiMap::LocationLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
	if ( selection.hasMoved() ) // attempt to move, resize, or create location
	{
		s32 startX = selection.getStartDrag().x,
			startY = selection.getStartDrag().y,
			endX = mapX,
			endY = mapY;
		s32 dragX = endX-startX;
		s32 dragY = endY-startY;

		if ( selection.getLocationFlags() == LOC_SEL_NOTHING ) // Create location
		{
			AscendingOrder(startX, endX);
			AscendingOrder(startY, endY);
			SnapLocationDimensions(startX, startY, endX, endY, SNAP_LOC_ALL);
									
			u16 locationIndex;
			if ( createLocation(startX, startY, endX, endY, locationIndex) )
			{
				undoStacks.AddUndo(std::shared_ptr<LocationCreateDel>(new LocationCreateDel(locationIndex)));
				//undos().addUndoLocationCreate(locationIndex);
				//undos().submitUndo();
				chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
				refreshScenario();
			}
		}
		else // Move or resize location
		{
			u16 selectedLocation = selection.getSelectedLocation();
			ChkLocation* loc;
			if ( selectedLocation != NO_LOCATION && getLocation(loc, selectedLocation) )
			{
				u8 selFlags = selection.getLocationFlags();
				if ( selFlags == LOC_SEL_MIDDLE ) // Move location
				{
					bool xInverted = loc->xc2 < loc->xc1,
						 yInverted = loc->yc2 < loc->yc1;

					loc->xc1 += dragX;
					loc->xc2 += dragX;
					loc->yc1 += dragY;
					loc->yc2 += dragY;
					s32 xc1Preserve = loc->xc1,
						yc1Preserve = loc->yc1,
						xc2Preserve = loc->xc2,
						yc2Preserve = loc->yc2;
										
					if ( xInverted )
					{
						if ( SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2) )
						{
							loc->xc1 += loc->xc2 - xc2Preserve; // Maintain location width
							dragX += loc->xc1 - xc1Preserve;
						}
					}
					else if ( SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1) )
					{
						loc->xc2 += loc->xc1 - xc1Preserve; // Maintain location width
						dragX += loc->xc2 - xc2Preserve;
					}

					if ( yInverted )
					{
						if ( SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2) )
						{
							loc->yc1 += loc->yc2 - yc2Preserve; // Maintain location height
							dragY += loc->yc1 - yc1Preserve;
						}
					}
					else if ( SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1) )
					{
						loc->yc2 += loc->yc1 - yc1Preserve; // Maintain location height
						dragY += loc->yc2 - yc2Preserve;
					}
										
					//undos().addUndoLocationMove(selectedLocation, dragX, dragY);
					undoStacks.AddUndo(std::shared_ptr<LocationMove>(new LocationMove(selectedLocation, -dragX, -dragY)));
				}
				else // Resize location
				{
					if ( selFlags & LOC_SEL_NORTH )
					{
						if ( loc->yc1 <= loc->yc2 ) // Standard yc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_YC1, loc->yc1)));
							loc->yc1 += dragY;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1);
						}
						else // Inverted yc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_YC2, loc->yc2)));
							loc->yc2 += dragY;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2);
						}
									
					}
					else if ( selFlags & LOC_SEL_SOUTH )
					{
						if ( loc->yc1 <= loc->yc2 ) // Standard yc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_YC2, loc->yc2)));
							loc->yc2 += dragY;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2);
						}
						else // Inverted yc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_YC1, loc->yc1)));
							loc->yc1 += dragY;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1);
						}
					}
	
					if ( selFlags & LOC_SEL_WEST )
					{
						if ( loc->xc1 <= loc->xc2 ) // Standard xc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_XC1, loc->xc1)));
							loc->xc1 += dragX;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1);
						}
						else // Inverted xc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_XC2, loc->xc2)));
							loc->xc2 += dragX;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2);
						}
					}
					else if ( selFlags & LOC_SEL_EAST )
					{
						if ( loc->xc1 <= loc->xc2 ) // Standard xc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_XC2, loc->xc2)));
							loc->xc2 += dragX;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2);
						}
						else // Inverted xc
						{
							undoStacks.AddUndo(std::shared_ptr<LocationChange>(new
								LocationChange(selectedLocation, LOC_FIELD_XC1, loc->xc1)));
							loc->xc1 += dragX;
							SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1);
						}
					}
				}
				//undos().submitUndo();
				Redraw(false);
				if ( chkd.locationWindow.getHandle() != NULL )
					chkd.locationWindow.RefreshLocationInfo();
			}
		}
	}
	else // attempt to select location, if you aren't resizing
	{
		selection.selectLocation(selection.getStartDrag().x, selection.getStartDrag().y, this, !LockAnywhere());
		if ( chkd.locationWindow.getHandle() != NULL )
			chkd.locationWindow.RefreshLocationInfo();
	
		Redraw(false);
	}
	selection.setLocationFlags(LOC_SEL_NOTHING);
}

void GuiMap::UnitLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
	selection.setEndDrag(mapX, mapY);
	selection.sortDragPoints();
	if ( wParam != MK_CONTROL )
		// Remove selected units
	{
		if ( chkd.unitWindow.getHandle() != nullptr )
		{
			HWND hUnitTree = chkd.unitWindow.listUnits.getHandle();
			chkd.unitWindow.SetChangeHighlightOnly(true);
			auto &selUnits = selection.getUnits();
			for ( u16 &unitIndex : selUnits )
			{
				LVFINDINFO findInfo = { };
				findInfo.flags = LVFI_PARAM;
				findInfo.lParam = unitIndex;
												
				int lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
				ListView_SetItemState(hUnitTree, lvIndex, 0, LVIS_FOCUSED|LVIS_SELECTED);
			}
			chkd.unitWindow.SetChangeHighlightOnly(false);
		}
		selection.removeUnits();
		chkd.unitWindow.UpdateEnabledState();
	}
		
	u16 numUnits = this->numUnits();
	for ( int i=0; i<numUnits; i++ )
	{
		int unitLeft = 0, unitRight	 = 0,
			unitTop	 = 0, unitBottom = 0;
	
		ChkUnit* unit;
		if ( getUnit(unit, i) )
		{
			if ( unit->id < 228 )
			{
				unitLeft   = unit->xc - chkd.scData.units.UnitDat(unit->id)->UnitSizeLeft;
				unitRight  = unit->xc + chkd.scData.units.UnitDat(unit->id)->UnitSizeRight;
				unitTop	   = unit->yc - chkd.scData.units.UnitDat(unit->id)->UnitSizeUp;
				unitBottom = unit->yc + chkd.scData.units.UnitDat(unit->id)->UnitSizeDown;
			}
			else
			{
				unitLeft   = unit->xc - chkd.scData.units.UnitDat(0)->UnitSizeLeft;
				unitRight  = unit->xc + chkd.scData.units.UnitDat(0)->UnitSizeRight;
				unitTop	   = unit->yc - chkd.scData.units.UnitDat(0)->UnitSizeUp;
				unitBottom = unit->yc + chkd.scData.units.UnitDat(0)->UnitSizeDown;
			}
	
			if (	selection.getStartDrag().x <= unitRight  && selection.getEndDrag().x >= unitLeft
				 && selection.getStartDrag().y <= unitBottom && selection.getEndDrag().y >= unitTop )
			{
				bool wasSelected = selection.unitIsSelected(i);
				if ( wasSelected )
					selection.removeUnit(i);
				else
					selection.addUnit(i);
		
				if ( chkd.unitWindow.getHandle() != nullptr )
				{
					HWND hUnitTree = chkd.unitWindow.listUnits.getHandle();
					LVFINDINFO findInfo = { };
					findInfo.flags = LVFI_PARAM;
					findInfo.lParam = i;
		
					int lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
					if ( wasSelected )
					{
						chkd.unitWindow.SetChangeHighlightOnly(true);
						ListView_SetItemState(hUnitTree, lvIndex, 0, LVIS_FOCUSED|LVIS_SELECTED);
						chkd.unitWindow.SetChangeHighlightOnly(false);
					}
					else
					{
						chkd.unitWindow.SetChangeHighlightOnly(true);
						ListView_SetItemState(hUnitTree, lvIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						chkd.unitWindow.SetChangeHighlightOnly(false);
					}
				}
				chkd.unitWindow.UpdateEnabledState();
			}
		}
	}
	Redraw(true);
}

LRESULT GuiMap::ConfirmWindowClose(HWND hWnd)
{
	if ( CanExit() )
		return ClassWindow::WndProc(hWnd, WM_CLOSE, NULL, NULL);
	else
		return 0;
}

