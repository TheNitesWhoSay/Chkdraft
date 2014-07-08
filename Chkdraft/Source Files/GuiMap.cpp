#include "GuiMap.h"
#include "Common Files/CommonFiles.h"
#include "Clipboard.h"
#include "Graphics.h"
#include "Location.h"
#include "Terrain.h"

extern HWND hMain;
extern HWND hTerrainProp;
extern HWND hLayer;
extern HWND hUnit;
extern HWND hLocation;
extern HWND hMapSettings;
extern HWND hMini;
extern HWND hZoom;
extern HWND hPlayer;
extern HMENU hMainMenu;

extern bool changeHighlightOnly;

void BuildLocationTree(GuiMap* map);

GuiMap::GuiMap() : lpvBits(nullptr), bitMapHeight(0), bitMapWidth(0), hMapWindow(nullptr),
			 layer(LAYER_TERRAIN), player(0), zoom(1), RedrawMiniMap(true), RedrawMap(true),
			 dragging(false), snapLocations(true), locSnapTileOverGrid(true), lockAnywhere(true),
			 snapUnits(true), stackUnits(false),
			 MemhDC(NULL), MemMinihDC(NULL), mapId(0),
			 MemBitmap(NULL), MemMiniBitmap(NULL), graphics(this), unsavedChanges(false), changeLock(false)
{
	LRESULT layerSel = SendMessage(hLayer, CB_GETCURSEL, NULL, NULL);
	if ( layerSel != CB_ERR )
		layer = u8(layerSel);

	displayPivot.x = 0;
	displayPivot.y = 0;
}

GuiMap::~GuiMap()
{
	if ( hTerrainProp )
		EndDialog(hTerrainProp, NULL);

	if ( lpvBits != nullptr )
	{
		delete[] lpvBits;
		lpvBits = nullptr;
	}

	DeleteObject(MemBitmap);
	DeleteObject(MemMiniBitmap);
	DeleteDC(MemhDC);
	DeleteDC(MemMinihDC);
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
			undoStacks.flushRoots();
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

	undoStacks.addUndoTile((u16)x, (u16)y, MTXM().get<u16>(location));

	TILE().replace<u16>(location, tileNum);
	MTXM().replace<u16>(location, tileNum);

	RECT rcTile;
	rcTile.left	  = x*32-displayPivot.x;
	rcTile.right  = rcTile.left+32;
	rcTile.top	  = y*32-displayPivot.y;
	rcTile.bottom = rcTile.top+32;

	RedrawMap = true;
	RedrawMiniMap = true;
	InvalidateRect(hMapWindow, &rcTile, true);
	return true;
}

u8& GuiMap::currLayer()
{
	return layer;
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
			SendMessage(hZoom, CB_SETCURSEL, i, NULL);
	}
	zoom = newScale;
	RedrawMap = true;
	RedrawMiniMap = true;

	RECT rect;
	GetClientRect(hMapWindow, &rect);
	bitMapWidth  = u16( ((rect.right - rect.left+32)/32*32)/zoom )/32*32;
	bitMapHeight = u16( ((rect.bottom - rect.top+32)/32*32)/zoom )/32*32;
	Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
	UpdateZoomMenuItems();
	RedrawWindow(hMapWindow, NULL, NULL, RDW_INVALIDATE);
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
	if ( getLocation(location, u8(index)) && GetClientRect(hMapWindow, &rect) != 0 )
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
				if ( hLocation == nullptr )
					hLocation = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOCPROP), hMain, LocationPropProc);
				ShowWindow(hLocation, SW_SHOW);
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
		if ( hTerrainProp )
			SendMessageA(hTerrainProp, WM_COMMAND, TILE_UPDATE, NULL);
		else
			hTerrainProp = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_SETTILE), hMain, SetTileProc);
		ShowWindow(hTerrainProp, SW_SHOW);
	}
	SetFocus(hMain);
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

	if ( hUnit != nullptr )
		SendMessage(hUnit, REPOPULATE_LIST, NULL, NULL);
	if ( hLocation != nullptr )
		SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);
	if ( hMapSettings != nullptr )
		SendMessage(hMapSettings, REFRESH_WINDOW, NULL, NULL);

	Redraw(true);
}

void GuiMap::clearSelection()
{
	selection.removeTiles();
	selection.removeUnits();
}

void GuiMap::selectAll()
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

				RedrawWindow(hMapWindow, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		case LAYER_UNITS:
			{
				u32 unitTableSize = UNIT().size(),
					numUnits = unitTableSize/UNIT_STRUCT_SIZE;

				HWND hUnitTree = GetDlgItem(hUnit, IDC_UNITLIST);
				LVFINDINFO findInfo = { };
				findInfo.flags = LVFI_PARAM;

				changeHighlightOnly = true;
				for ( u16 i=0; i<numUnits; i++ )
				{
					if ( !selection.unitIsSelected(i) )
					{
						selection.addUnit(i);
						if ( hUnit )
						{
							findInfo.lParam = i;
							s32 lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
							ListView_SetItemState(hUnitTree, lvIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						}
					}
				}
				changeHighlightOnly = false;

				Redraw(true);
			}
			break;
	}
}

void GuiMap::deleteSelection()
{
	switch ( layer )
	{
		case LAYER_TERRAIN:
			{
				u16 xSize = XSize();
				TileNode* track = selection.getFirstTile();

				while ( track != nullptr )
				{
					u32 location = 2*xSize*track->yc+2*track->xc; // Down y rows, over x columns
					undoStacks.addUndoTile((u16)track->xc, (u16)track->yc, MTXM().get<u16>(location));
					TILE().replace<u16>(location, 0);
					MTXM().replace<u16>(location, 0);

					track = track->next;
				}

				selection.removeTiles();
			}
			break;

		case LAYER_UNITS:
			{
				if ( hUnit != nullptr )
					SendMessage(hUnit, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, NULL), NULL);
				else
				{
					while ( selection.hasUnits() )
					{
						// Get the highest index in the selection
							u16 index = selection.getHighestIndex();
							selection.removeUnit(index);
							
							ChkUnit* delUnit;
							if ( getUnit(delUnit, index) )
								undoStacks.addUndoUnitDel(index, delUnit);

							UNIT().del(index*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);
					}
					undoStacks.startNext(UNDO_UNIT_DEL);
				}
			}
			break;

		case LAYER_LOCATIONS:
			{
				if ( hLocation != nullptr )
				{
					locProcLocIndex = NO_LOCATION;
					EndDialog(hLocation, IDCLOSE);
				}
				ChkLocation* loc;
				u16 index = selection.getSelectedLocation();
				if ( index != NO_LOCATION && getLocation(loc, index) )
				{
					std::string locName = "";
					getRawString(locName, loc->stringNum);
					if ( undoStacks.addUndoLocationDel(index, loc, isExtendedString(loc->stringNum), locName) )
						undoStacks.startNext(UNDO_LOCATION_DEL);

					BuildLocationTree(this);

					loc->elevation = 0;
					loc->xc1 = 0;
					loc->xc2 = 0;
					loc->yc1 = 0;
					loc->yc2 = 0;
					u16 stringNum = loc->stringNum;
					loc->stringNum = 0;
					if ( stringNum > 0 )
						removeUnusedString(stringNum);

					selections().selectLocation(NO_LOCATION);
				}
			}
			break;
	}

	RedrawMap = true;
	RedrawMiniMap = true;
	RedrawWindow(hMapWindow, NULL, NULL, RDW_INVALIDATE);
}

void GuiMap::paste(s32 mapClickX, s32 mapClickY, CLIPBOARD &clipboard)
{
	SetFocus(hMapWindow);
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
			undoStacks.doUndo(UNDO_TERRAIN, scenario(), selections());
			break;
		case LAYER_UNITS:
			undoStacks.doUndo(UNDO_UNIT, scenario(), selections());
			if ( hUnit != nullptr )
				SendMessage(hUnit, REPOPULATE_LIST, NULL, NULL);
			break;
		case LAYER_LOCATIONS:
			{
				undoStacks.doUndo(UNDO_LOCATION, scenario(), selections());
				if ( hLocation != nullptr )
					SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);

				BuildLocationTree(this);
			}
			break;
	}
}

void GuiMap::redo()
{
	switch ( layer )
	{
		case LAYER_TERRAIN:
			undoStacks.doRedo(UNDO_TERRAIN, scenario(), selections());
			break;
		case LAYER_UNITS:
			undoStacks.doRedo(UNDO_UNIT, scenario(), selections());
			if ( hUnit != nullptr )
				SendMessage(hUnit, REPOPULATE_LIST, NULL, NULL);
			break;
		case LAYER_LOCATIONS:
			undoStacks.doRedo(UNDO_LOCATION, scenario(), selections());
			if ( hLocation != nullptr )
				SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);
			break;
	}
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
	return hMapWindow;
}

bool GuiMap::setHandle(HWND newMapWin)
{
	return	  ( newMapWin != nullptr )
		   && ( hMapWindow = newMapWin );
}

void GuiMap::PaintMap( GuiMap* currMap, bool pasting, CLIPBOARD& clipboard )
{
	GuiMap* map = this;
	InvalidateRect(map->hMapWindow, NULL, FALSE);

	RECT rect;
	GetClientRect(map->hMapWindow, &rect);
	s32 width = (rect.right - rect.left+32)/32*32;
	s32 height = (rect.bottom - rect.top+32)/32*32;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(map->hMapWindow, &ps);

	u16 scaledWidth = (u16(((double)width)/zoom))/32*32,
		scaledHeight = (u16(((double)height)/zoom))/32*32;

	u32 bitmapSize = (u32)scaledWidth*(u32)scaledHeight*3;

	if ( map->RedrawMap == true )
	{
		map->RedrawMap = false;
		if ( map->lpvBits != nullptr )
			delete[] map->lpvBits;
		map->bitMapWidth = scaledWidth;
		map->bitMapHeight = scaledHeight;
		map->lpvBits = new u8[bitmapSize];

		DeleteObject(map->MemBitmap);
		DeleteDC    (map->MemhDC);
		map->MemhDC = CreateCompatibleDC(hDC);
		map->MemBitmap = CreateCompatibleBitmap(hDC, scaledWidth, scaledHeight);
				
		SelectObject(map->MemhDC, map->MemBitmap);
		
		if ( currMap == map ) // Only redraw minimap for active window
			RedrawWindow(hMini, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);

		graphics.DrawMap(
				 map->bitWidth(),
				 map->bitHeight(),
				 map->display().x,
				 map->display().y,
				 map->GraphicBits(),
				 map->selections(),
				 map->layer,
				 map->GetMemhDC(),
				 !map->lockAnywhere
			   ); // Terrain, Grid, Units, Sprites, Debug
	}

	HDC TemphDC = CreateCompatibleDC(hDC);
	HBITMAP Tempbitmap = CreateCompatibleBitmap(hDC, scaledWidth, scaledHeight);
	SelectObject(TemphDC, Tempbitmap);
	BitBlt(TemphDC, 0, 0, scaledWidth, scaledHeight, this->MemhDC, 0, 0, SRCCOPY);

	if ( currMap == this )
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
				   this->scenario(),
				   this->currLayer()
				 ); // Drag and paste graphics

		if ( layer != LAYER_LOCATIONS )
			DrawSelectingFrame(TemphDC, selections(), display().x, display().y, bitWidth(), bitHeight(), zoom);
	}
	
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchBlt(hDC, 0, 0, width, height, TemphDC, 0, 0, scaledWidth, scaledHeight, SRCCOPY);

	DeleteDC	(TemphDC);
	DeleteObject(Tempbitmap);
	EndPaint(hMapWindow, &ps);
}

void GuiMap::PaintMiniMap(HWND hWnd)
{
	if ( this )
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
						 hMini,
						 this->XSize(),
						 this->YSize(),
						 this->MiniMapScale(this->XSize(), this->YSize()),
						 this->scenario()
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

		RedrawWindow(hMapWindow, NULL, NULL, RDW_INVALIDATE);
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
}

bool GuiMap::DisplayingElevations()
{
	return graphics.DisplayingElevations();
}

void GuiMap::ToggleTileNumSource(bool MTXMoverTILE)
{
	graphics.ToggleTileNumSource(MTXMoverTILE);
	UpdateTerrainViewMenuItems();
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
			s32 xIntervalNum = round(intervalNum);
			x = xIntervalNum*grid.size.x;
			intervalNum = (double(y-grid.offset.y))/grid.size.y;
			s32 yIntervalNum = round(intervalNum);
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
		if ( graphics.ClippingLocationNames() )
			CheckMenuItem(hMainMenu, ID_LOCATIONS_CLIPNAMES, MF_CHECKED);
		else
			CheckMenuItem(hMainMenu, ID_LOCATIONS_CLIPNAMES, MF_UNCHECKED);

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
	BuildLocationTree(this);
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
		s32 xStartIntervalNum = round(intervalNum);
		intervalNum = (double(y1-startSnapY))/lengthY;
		s32 yStartIntervalNum = round(intervalNum);
		intervalNum = (double(x2-startSnapX))/lengthX;
		s32 xEndIntervalNum = round(intervalNum);
		intervalNum = (double(y2-startSnapY))/lengthY;
		s32 yEndIntervalNum = round(intervalNum);

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
	if ( graphics.ClippingLocationNames() )
		CheckMenuItem(hMainMenu, ID_LOCATIONS_CLIPNAMES, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_LOCATIONS_CLIPNAMES, MF_UNCHECKED);

	if ( snapLocations && locSnapTileOverGrid )
		CheckMenuItem(hMainMenu, ID_LOCATIONS_SNAPTOTILE, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_LOCATIONS_SNAPTOTILE, MF_UNCHECKED);

	if ( snapLocations && !locSnapTileOverGrid )
		CheckMenuItem(hMainMenu, ID_LOCATIONS_SNAPTOACTIVEGRID, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_LOCATIONS_SNAPTOACTIVEGRID, MF_UNCHECKED);

	if ( !snapLocations )
		CheckMenuItem(hMainMenu, ID_LOCATIONS_NOSNAP, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_LOCATIONS_NOSNAP, MF_UNCHECKED);

	if ( lockAnywhere )
		CheckMenuItem(hMainMenu, ID_LOCATIONS_LOCKANYWHERE, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_LOCATIONS_LOCKANYWHERE, MF_UNCHECKED);
}

void GuiMap::UpdateZoomMenuItems()
{
	u32 zoomNum;
	for ( zoomNum=0; zoomNum<NUM_ZOOMS; zoomNum++ )
	{
		if ( zoom == zooms[zoomNum] )
			break;
	}

	SendMessage(hZoom, CB_SETCURSEL, zoomNum, 0);

	if ( zoom == .10 )	CheckMenuItem(hMainMenu, ID_ZOOM_10, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_10, MF_UNCHECKED);
	if ( zoom == .25 )	CheckMenuItem(hMainMenu, ID_ZOOM_25, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_25, MF_UNCHECKED);
	if ( zoom == .33 )	CheckMenuItem(hMainMenu, ID_ZOOM_33, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_33, MF_UNCHECKED);
	if ( zoom == .50 )	CheckMenuItem(hMainMenu, ID_ZOOM_50, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_50, MF_UNCHECKED);
	if ( zoom == .66 )	CheckMenuItem(hMainMenu, ID_ZOOM_66, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_66, MF_UNCHECKED);
	if ( zoom == 1.00 )	CheckMenuItem(hMainMenu, ID_ZOOM_100, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_100, MF_UNCHECKED);
	if ( zoom == 1.50 )	CheckMenuItem(hMainMenu, ID_ZOOM_150, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_150, MF_UNCHECKED);
	if ( zoom == 2.00 )	CheckMenuItem(hMainMenu, ID_ZOOM_200, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_200, MF_UNCHECKED);
	if ( zoom == 3.00 )	CheckMenuItem(hMainMenu, ID_ZOOM_300, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_300, MF_UNCHECKED);
	if ( zoom == 4.00 )	CheckMenuItem(hMainMenu, ID_ZOOM_400, MF_CHECKED);
	else				CheckMenuItem(hMainMenu, ID_ZOOM_400, MF_UNCHECKED);
}

void GuiMap::UpdateGridSizesMenu()
{
	GRID& mainGrid = grid(0);

	CheckMenuItem(hMainMenu, ID_GRID_ULTRAFINE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_FINE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_NORMAL, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_LARGE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_EXTRALARGE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_CUSTOM, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_GRID_DISABLED, MF_UNCHECKED);

	if ( mainGrid.size.x == 0 && mainGrid.size.y == 0 )
		CheckMenuItem(hMainMenu, ID_GRID_DISABLED, MF_CHECKED);
	else if ( (mainGrid.offset.x != 0 || mainGrid.offset.y != 0) ||
			  mainGrid.size.x != mainGrid.size.y					)
		CheckMenuItem(hMainMenu, ID_GRID_CUSTOM, MF_CHECKED);
	else if ( mainGrid.size.x == 8 && mainGrid.size.y == 8 )
		CheckMenuItem(hMainMenu, ID_GRID_ULTRAFINE, MF_CHECKED);
	else if ( mainGrid.size.x == 16 && mainGrid.size.y == 16 )
		CheckMenuItem(hMainMenu, ID_GRID_FINE, MF_CHECKED);
	else if ( mainGrid.size.x == 32 && mainGrid.size.y == 32 )
		CheckMenuItem(hMainMenu, ID_GRID_NORMAL, MF_CHECKED);
	else if ( mainGrid.size.x == 64 && mainGrid.size.y == 64 )
		CheckMenuItem(hMainMenu, ID_GRID_LARGE, MF_CHECKED);
	else if ( mainGrid.size.x == 128 && mainGrid.size.y == 128 )
		CheckMenuItem(hMainMenu, ID_GRID_EXTRALARGE, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_GRID_CUSTOM, MF_CHECKED);
}

void GuiMap::UpdateGridColorMenu()
{
	GRID& mainGrid = grid(0);

	CheckMenuItem(hMainMenu, ID_COLOR_BLACK, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_GREY, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_WHITE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_COLOR_OTHER, MF_UNCHECKED);

	if ( mainGrid.red == 0 && mainGrid.green == 0 && mainGrid.blue == 0 )
		CheckMenuItem(hMainMenu, ID_COLOR_BLACK, MF_CHECKED);
	else if ( mainGrid.red == 72 && mainGrid.green == 72 && mainGrid.blue == 88 )
		CheckMenuItem(hMainMenu, ID_COLOR_GREY, MF_CHECKED);
	else if ( mainGrid.red == 255 && mainGrid.green == 255 && mainGrid.blue == 255 )
		CheckMenuItem(hMainMenu, ID_COLOR_WHITE, MF_CHECKED);
	else if ( mainGrid.red == 16 && mainGrid.green == 252 && mainGrid.blue == 24 )
		CheckMenuItem(hMainMenu, ID_COLOR_GREEN, MF_CHECKED);
	else if ( mainGrid.red == 244 && mainGrid.green == 4 && mainGrid.blue == 4 )
		CheckMenuItem(hMainMenu, ID_COLOR_RED, MF_CHECKED);
	else if ( mainGrid.red == 36 && mainGrid.green == 36 && mainGrid.blue == 252 )
		CheckMenuItem(hMainMenu, ID_COLOR_BLUE, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_COLOR_OTHER, MF_CHECKED);
}

void GuiMap::UpdateTerrainViewMenuItems()
{
	if ( graphics.DisplayingElevations() )
		CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEELEVATIONS, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEELEVATIONS, MF_UNCHECKED);

	CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEVALUES, MF_UNCHECKED);
	CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEVALUESMTXM, MF_UNCHECKED);
	if ( graphics.DisplayingTileNums() )
	{
		if ( graphics.mtxmOverTile() )
			CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEVALUESMTXM, MF_CHECKED);
		else
			CheckMenuItem(hMainMenu, ID_TERRAIN_DISPLAYTILEVALUES, MF_CHECKED);
	}
}

void GuiMap::UpdateUnitMenuItems()
{
	if ( snapUnits )
		CheckMenuItem(hMainMenu, ID_UNITS_UNITSSNAPTOGRID, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_UNITS_UNITSSNAPTOGRID, MF_UNCHECKED);

	if ( stackUnits )
		CheckMenuItem(hMainMenu, ID_UNITS_ALLOWSTACK, MF_CHECKED);
	else
		CheckMenuItem(hMainMenu, ID_UNITS_ALLOWSTACK, MF_UNCHECKED);
}

void GuiMap::Scroll(u32 flags)
{
	SCROLLINFO scrollbars = { };
	scrollbars.cbSize = sizeof(SCROLLINFO);
	scrollbars.fMask = SIF_ALL;
	scrollbars.nMin = 0;

	RECT rcMap;
	GetClientRect(hMapWindow, &rcMap);
	s32 screenWidth  = (s32)((double(rcMap.right-rcMap.left))/zoom),
		screenHeight = (s32)((double(rcMap.bottom-rcMap.top))/zoom);

	if ( (flags&VALIDATE_BORDER) )
		ValidateBorder(screenWidth, screenHeight);

	if ( (flags&SCROLL_X) )
	{
		scrollbars.nPos = displayPivot.x; 
		scrollbars.nMax = XSize()*32;
		scrollbars.nPage = screenWidth;
		SetScrollInfo(hMapWindow, SB_HORZ, &scrollbars, true);
	}
	if ( (flags&SCROLL_Y) )
	{
		scrollbars.nPos = displayPivot.y;
		scrollbars.nMax = YSize()*32;
		scrollbars.nPage = screenHeight;
		SetScrollInfo(hMapWindow, SB_VERT, &scrollbars, true);
	}
	RedrawMap = true;
}

void GuiMap::setMapId(u16 mapId)
{
	this->mapId = mapId;
	undoStacks.setMapId(mapId);
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
	int length = GetWindowTextLength(hMapWindow)+1;
	if ( length > 0 )
	{
		char* text;
		try { text = new char[length+1]; }
		catch ( std::bad_alloc ) { return; }

		if ( GetWindowText(hMapWindow, (LPSTR)text, length) > 0 )
		{
			strcat_s(text, length+1, "*");
			SetWindowText(hMapWindow, text);
		}
	}
}

void GuiMap::removeAsterisk()
{
	int length = GetWindowTextLength(hMapWindow)+1;
	if ( length > 0 )
	{
		char* text;
		try { text = new char[length]; }
		catch ( std::bad_alloc ) { return; }
		
		if ( GetWindowText(hMapWindow, (LPSTR)text, length) > 0 )
		{
			if ( text[length-2] == '*' )
			{
				text[length-2] = '\0';
				SetWindowText(hMapWindow, text);
			}
		}
	}
}

void GuiMap::updateMenu()
{
	UpdateLocationMenuItems();
	SendMessage(hLayer, CB_SETCURSEL, (WPARAM)layer, 0);
	UpdateZoomMenuItems();
	SendMessage(hPlayer, CB_SETCURSEL, (WPARAM)player, 0);
	UpdateGridSizesMenu();
	UpdateGridColorMenu();
	UpdateTerrainViewMenuItems();
	UpdateUnitMenuItems();

	//int itemId = 0;
	//CheckMenuItem(hMainMenu, itemId, MF_CHECKED);
	//CheckMenuItem(hMainMenu, itemId, MF_UNCHECKED);
}