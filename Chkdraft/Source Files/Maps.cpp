#include "Maps.h"
#include "Chkdraft.h"

MAPS::MAPS() : UntitledNumber(0),
			   lastUsedMapID(0),
			   curr(nullptr), firstMap(nullptr),
			   currCursor(nullptr),
			   nonStandardCursor(false)
{
	standardCursor = LoadCursor(NULL, IDC_ARROW);
	sizeAllCursor = LoadCursor(NULL, IDC_SIZEALL);
	neswCursor = LoadCursor(NULL, IDC_SIZENESW);
	nwseCursor = LoadCursor(NULL, IDC_SIZENWSE);
	nsCursor = LoadCursor(NULL, IDC_SIZENS);
	weCursor = LoadCursor(NULL, IDC_SIZEWE);
}

MAPS::~MAPS()
{

}

void MAPS::FocusActive()
{
	if ( firstMap == nullptr )
		curr = nullptr;
	else
	{
		HWND hFocus = NULL;
		if ( curr != nullptr )
			hFocus = curr->getHandle();

		MapNode* currNode = firstMap;
		while ( currNode != nullptr && currNode->map.getHandle() != hFocus )
			currNode = currNode->next;

		if ( currNode != nullptr ) // If a corresponding map was found
		{
			curr = &currNode->map; // Sets the current map to the map given by the handle
			// Need to update location tree, menu checkboxes
			chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
			curr->updateMenu();
		}
	}
}

void MAPS::Focus(HWND hFocus)
{
	MapNode* currNode = firstMap;
	while ( currNode != nullptr && currNode->map.getHandle() != hFocus )
		currNode = currNode->next;

	if ( currNode != nullptr /*&& curr != (&currNode->map)*/ ) // If a corresponding map was found and this map is not already the current
	{
		curr = &currNode->map; // Sets the current map to the map given by the handle
		// Need to update location tree, menu checkboxes
		chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
		curr->updateMenu();
	}
	else
		curr = nullptr;
}

GuiMap* MAPS::GetMap(HWND hMap)
{
	MapNode* currNode = firstMap;
	while ( currNode != nullptr && currNode->map.getHandle() != hMap )
		currNode = currNode->next;

	if ( currNode )
		return &currNode->map;
	else
		return nullptr;
}

GuiMap* MAPS::GetMap(u16 mapID)
{
	if ( mapID == 0 || mapID == curr->getMapId() )
		return curr;
	else
	{
		MapNode* currNode = firstMap;
		while ( currNode != nullptr && currNode->mapID != mapID )
			currNode = currNode->next;

		if ( currNode )
			return &currNode->map;
		else
			return nullptr;
	}
}

u16 MAPS::GetMapID(GuiMap* map)
{
	MapNode* currNode = firstMap;
	while ( currNode != nullptr )
	{
		if ( &currNode->map == map )
			return currNode->mapID;

		currNode = currNode->next;
	}
	return 0;
}

bool MAPS::NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers)
{
	MapNode* newMap = new MapNode;
	
	if ( newMap->map.CreateNew(width, height, tileset, terrain, triggers) )
	{
		char title[256] = { "Untitled" };
		if ( UntitledNumber > 0 )
			sprintf_s(title, 256, "Untitled %d", UntitledNumber);

		if ( newMap->map.CreateThis(getHandle(), title) )
		{
			curr = &newMap->map;
			UntitledNumber++;
			PushNode(newMap);
			Focus(newMap->map.getHandle());
			curr->Redraw(true);
			return true;
		}
		else
		{
			delete newMap;
			Error("Failed to create MDI Child Window!");
		}
	}
	else
	{
		CHKD_ERR("Failed to create scenario file!\n\nError in %s\n\n%s", LastErrorLoc, LastError);
		Error(LastError);
	}

	return false;
}

bool MAPS::OpenMap(const char* fileName)
{
	MapNode* newMap = new MapNode;

	if ( newMap->map.LoadFile(fileName) )
	{
		if ( newMap->map.CreateThis(getHandle(), fileName) )
		{
			SetWindowText(newMap->map.getHandle(), fileName);
			PushNode(newMap);
			Focus(newMap->map.getHandle());
			curr->Scroll(SCROLL_X|SCROLL_Y);
			curr->Redraw(true);
			curr->refreshScenario();
			FocusActive();
			return true;
		} 
		else
		{
			delete newMap;
			Error("Failed to create MDI Child Window!");
		}
	}
	else
		delete newMap;

	return false;
}

bool MAPS::OpenMap()
{
	return OpenMap(nullptr);
}

bool MAPS::SaveCurr(bool saveAs)
{
	FocusActive();
	if ( curr->SaveFile(saveAs) )
	{
		SetWindowText(curr->getHandle(), curr->FilePath());
		return true;
	}
	else
		return false;
}

void MAPS::CloseMap(HWND hMap)
{
	MapNode* curr = firstMap,
		   * deleting = nullptr;

	if ( curr != nullptr )
	{
		if ( firstMap->map.getHandle() == hMap )
		{
			deleting = firstMap;
			firstMap = firstMap->next;
		}
		else if ( curr->next != nullptr )
		{
			while ( curr->next != nullptr )
			{
				if ( curr->next->map.getHandle() == hMap )
				{
					deleting = curr->next;
					curr->next = curr->next->next;
					break;
				}
				curr = curr->next;
			}
		}
	}

	if ( deleting != nullptr )
	{
		delete deleting;
		curr = nullptr;
		FocusActive();
		RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
	}

	if ( firstMap == nullptr ) // Disable mapping functionality
	{
		int toolbarItems[] =
		{
			ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
			ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
		};

		for ( int i=0; i<sizeof(toolbarItems)/sizeof(int); i++ )
			SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], false);

		ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

		HMENU hMenu = GetMenu(chkd.getHandle());

		for ( int i=0; i<numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_DISABLED);

		HWND hLeftBar = chkd.mainPlot.leftBar.getHandle();
		ShowWindow(hLeftBar, SW_HIDE);

		chkd.statusBar.SetText(0, "");
		chkd.statusBar.SetText(1, "");
	}
}

void MAPS::CloseActive()
{
	SendMessage(MdiFrame::getHandle(), WM_MDIDESTROY, (WPARAM)MdiFrame::getActive(), NULL);
}

void MAPS::UpdateTreeView()
{
	if ( curr != nullptr )
	{
		buffer& settings = curr->unitSettings();
	}
}

void MAPS::SetGrid(s16 xSize, s16 ySize)
{
	if ( curr != nullptr )
		curr->SetGrid(xSize, ySize);
}

void MAPS::ChangeLayer(u8 newLayer)
{
	if ( curr != nullptr && curr->currLayer() != newLayer )
	{
		curr->selections().removeTiles();
		curr->currLayer() = newLayer;

		if ( chkd.mainToolbar.layerBox.GetSel() != newLayer )
			chkd.mainToolbar.layerBox.SetSel(newLayer);

		if ( newLayer == LAYER_FOG || newLayer == LAYER_UNITS || newLayer == LAYER_SPRITES || newLayer == LAYER_VIEW_FOG )
			// Layers where player#'s are relevant
		{
			ChangePlayer(curr->currPlayer());
			ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_SHOW);
		}
		else // Layers where player#'s are irrelevant
		{
			chkd.statusBar.SetText(2, "");
			ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
		}

		if ( newLayer == LAYER_TERRAIN )
			ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_SHOW);
		else
			ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

		chkd.tilePropWindow.DestroyThis();
		curr->Redraw(false);
		char layerString[32];
		if ( chkd.mainToolbar.layerBox.GetText(newLayer, layerString, 31) )
			chkd.statusBar.SetText(1, layerString);
	}
}

void MAPS::ChangeZoom(bool increment)
{
	double zoom = curr->getZoom();

	if ( increment )
	{
		for ( int i=1; i<10; i++ )
		{
			if ( zoom == zooms[i] )
			{
				curr->setZoom(zooms[i-1]);
				chkd.mainToolbar.zoomBox.SetSel(i-1);
				break;
			}
		}
	}
	else // decrement
	{
		for ( int i=0; i<9; i++ )
		{
			if ( zoom == zooms[i] )
			{
				curr->setZoom(zooms[i+1]);
				chkd.mainToolbar.zoomBox.SetSel(i+1);
				break;
			}
		}
	}
}

void MAPS::ChangePlayer(u8 newPlayer)
{
	curr->currPlayer() = newPlayer;

	if ( curr->currLayer() == LAYER_UNITS )
	{
		if ( clipboard.isPasting() )
		{
			PasteUnitNode* curr = clipboard.getFirstUnit();
			while ( curr != nullptr )
			{
				curr->unit.owner = newPlayer;
				curr = curr->next;
			}
		}

		u16 numUnits = curr->numUnits();

		UnitNode* currSelUnit = curr->selections().getFirstUnit();
		while ( currSelUnit != nullptr )
		{
			ChkUnit* unit;
			if ( curr->getUnit(unit, currSelUnit->index) && newPlayer != unit->owner )
			{
				curr->undos().addUndoUnitChange(currSelUnit->index, UNIT_FIELD_OWNER, unit->owner);
				unit->owner = newPlayer;
			}

			if ( chkd.unitWindow.getHandle() != nullptr )
			{
				string text;
				HWND hOwner = GetDlgItem(chkd.unitWindow.getHandle(), IDC_COMBO_PLAYER);
				HWND hUnitList = GetDlgItem(chkd.unitWindow.getHandle(), IDC_UNITLIST);
				if ( newPlayer < 12 )
					SendMessage(hOwner, CB_SETCURSEL, newPlayer, NULL);
				else if ( chkd.mainToolbar.playerBox.GetEditText(text) )
					SetWindowText(hOwner, text.c_str());

				chkd.unitWindow.ChangeOwner(hUnitList, currSelUnit->index, newPlayer);
			}

			currSelUnit = currSelUnit->next;
		}

		curr->undos().startNext(0);
		curr->Redraw(true);
	}

	char color[32], race[32], playerText[64];
	sprintf_s(color, "Red");
	sprintf_s(race, "Terran");
	sprintf_s(playerText, "Player %i: %s %s", curr->currPlayer()+1, color, race);
	chkd.statusBar.SetText(2, playerText);
}

void MAPS::cut()
{
	if ( curr != nullptr )
	{
		if ( curr->isProtected() )
			Error("Cannot copy from protected maps!");
		else
		{
			clipboard.copy(&curr->selections(), curr->scenario(), curr->currLayer());
			curr->deleteSelection();
			curr->nextUndo();
			if ( clipboard.isPasting() )
			{
				endPaste();
				RedrawWindow(curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
			}
			ClipCursor(NULL);
		}
	}
}

void MAPS::copy()
{
	if ( curr != nullptr )
	{
		if ( curr->isProtected() )
			Error("Cannot copy from protected maps!");
		else
		{
			clipboard.copy(&curr->selections(), curr->scenario(), curr->currLayer());
			if ( clipboard.isPasting() )
			{
				endPaste();
				RedrawWindow(curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
			}
			ClipCursor(NULL);
		}
	}
}

void MAPS::SetGridColor(u8 red, u8 green, u8 blue)
{
	if ( curr != nullptr )
		curr->SetGridColor(red, green, blue);
}

void MAPS::startPaste(bool isQuickPaste)
{
	if ( curr == nullptr )
		return;
	else if ( curr->currLayer() == LAYER_TERRAIN )
	{
		if ( clipboard.hasTiles() || clipboard.hasQuickTiles() )
		{
			curr->selections().removeTiles();
			clipboard.beginPasting(isQuickPaste);

			RedrawWindow(curr->getHandle(), NULL, NULL, RDW_INVALIDATE);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER;
			tme.hwndTrack = curr->getHandle();
			tme.dwHoverTime = DEFAULT_HOVER_TIME;
			TrackMouseEvent(&tme);
		}
	}
	else if ( curr->currLayer() == LAYER_UNITS )
	{
		if ( clipboard.hasUnits() || clipboard.hasQuickUnits() )
		{
			curr->selections().removeUnits();
			clipboard.beginPasting(isQuickPaste);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER;
			tme.hwndTrack = curr->getHandle();
			tme.dwHoverTime = DEFAULT_HOVER_TIME;
			TrackMouseEvent(&tme);
		}
	}
}

void MAPS::endPaste()
{
	clipboard.endPasting();
	if ( curr != nullptr )
	{
		curr->nextUndo();
		curr->Redraw(false);
	}
}

void MAPS::properties()
{
	if ( curr->currLayer() == LAYER_TERRAIN )
	{
		if ( curr->selections().hasTiles() )
		{
			TileNode tile = *curr->selections().getFirstTile();
			curr->selections().removeTiles();
			curr->selections().addTile(tile.value, tile.xc, tile.yc, NEIGHBOR_LEFT|NEIGHBOR_TOP|NEIGHBOR_RIGHT|NEIGHBOR_BOTTOM);

			RedrawWindow(curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
			if ( chkd.tilePropWindow.getHandle() != NULL )
				chkd.tilePropWindow.UpdateTile();
			else
				chkd.tilePropWindow.CreateThis(chkd.getHandle());
			ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
		}
	}
}

void MAPS::stickCursor()
{
	if ( nonStandardCursor && currCursor != nullptr )
		SetCursor(*currCursor);
}

void MAPS::updateCursor(s32 xc, s32 yc)
{
	if ( curr->currLayer() == LAYER_LOCATIONS )
	{
		u16 selectedLocation = curr->selections().getSelectedLocation();
		if ( selectedLocation != NO_LOCATION )
		{
			ChkLocation* loc;
			if ( curr->getLocation(loc, selectedLocation) )
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

					if ( xc >= leftInnerBound && xc <= rightInnerBound &&
						 yc >= topInnerBound && yc <= bottomInnerBound    )
					{
						currCursor = &sizeAllCursor;
						SetCursor(sizeAllCursor);
					} // Invariant: not in center
					else if ( xc >= leftInnerBound && xc <= rightInnerBound )
					{
						currCursor = &nsCursor;
						SetCursor(nsCursor);
					} // Invariant: on west or east
					else if ( yc >= topInnerBound && yc <= bottomInnerBound )
					{
						currCursor = &weCursor;
						SetCursor(weCursor);
					} // Invariant: is on a corner
					else if ( xc < leftInnerBound && yc < topInnerBound ||
						      xc > rightInnerBound && yc > bottomInnerBound )
					{
						currCursor = &nwseCursor;
						SetCursor(nwseCursor);
					} // Invariant: is on ne or sw corner
					else
					{
						currCursor = &neswCursor;
						SetCursor(neswCursor);
					}
				}
			}
		}
	}
	else if ( nonStandardCursor )
		SetCursor(standardCursor);
}

void MAPS::PushNode(MapNode* map)
{
	if ( firstMap == nullptr ) // Enable mapping functionality
	{
		int toolbarItems[] =
		{
			ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
			ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
		};

		for ( int i=0; i<sizeof(toolbarItems)/sizeof(int); i++ )
			SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], true);

		ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_SHOW);
		ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_SHOW);
		ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_SHOW);

		HMENU hMenu = GetMenu(chkd.getHandle());

		for ( int i=0; i<numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_ENABLED);

		HWND hLeftBar = chkd.mainPlot.leftBar.getHandle();
		ShowWindow(hLeftBar, SW_SHOW);

		chkd.statusBar.SetText(1, "Terrain");
	}

	if ( lastUsedMapID < 65535 )
	{
		lastUsedMapID ++;
		map->mapID = lastUsedMapID;
		map->map.setMapId(map->mapID);
	}
	else // Find one that isn't used, ambiguously use 65535 if all are used
	{
		u16 newMapID;
		for ( newMapID=0; newMapID<=65535; newMapID++ ) // Try every ID, break if you find one
		{
			bool used = false; // Assume it's not used until you find that it is
			MapNode* curr = firstMap;
			while ( curr != nullptr ) // Look through every map
			{
				if ( newMapID == curr->mapID ) // If used, flag it as such and stop looking through maps
				{
					used = true;
					break;
				}
				curr = curr->next;
			}

			if ( !used ) // If you got through all the maps and found the ID wasn't used, you found an unused ID, break
				break;
		}
		map->mapID = newMapID; // Use the unused ID or 65535
		map->map.setMapId(map->mapID);
	}
	
	map->next = firstMap;
	firstMap = map;
}
