#include "Maps.h"
#include "Chkdraft.h"
#include "UnitChange.h"
#include <memory>
#include <string>
#include <utility>

MAPS::MAPS() : curr(nullptr), mappingEnabled(false), UntitledNumber(0), lastUsedMapID(0),
	nonStandardCursor(false), currCursor(nullptr), standardCursor(NULL), sizeAllCursor(NULL),
	neswCursor(NULL), nwseCursor(NULL), nsCursor(NULL), weCursor(NULL)
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

bool MAPS::isInOpenMaps(std::shared_ptr<GuiMap> guiMap)
{
	for ( auto &pair : openMaps )
	{
		if ( guiMap == pair.second )
			return true;
	}
	return false;
}

bool MAPS::Focus(HWND hGuiMap)
{
	for ( auto &pair : openMaps )
	{
		if ( hGuiMap == pair.second->getHandle() )
			return Focus(pair.second);
	}
	return false;
}

bool MAPS::Focus(std::shared_ptr<GuiMap> guiMap)
{
	if ( guiMap != nullptr && isInOpenMaps(guiMap) )
	{
		curr = guiMap;
		chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
		curr->updateMenu();
		return true;
	}
	else
	{
		curr = nullptr;
		return false;
	}
}

std::shared_ptr<GuiMap> MAPS::GetMap(HWND hGuiMap)
{
	if ( hGuiMap == curr->getHandle() )
		return curr;

	for ( auto &pair : openMaps)
	{
		if ( hGuiMap == pair.second->getHandle() )
			return pair.second;
	}

	return nullptr;
}

std::shared_ptr<GuiMap> MAPS::GetMap(u16 mapId)
{
	if ( mapId == 0 || mapId == curr->getMapId() )
		return curr;

	auto it = openMaps.find(mapId);
	if ( it != openMaps.end() )
		return it->second;
	else
		return nullptr;
}

u16 MAPS::GetMapID(std::shared_ptr<GuiMap> guiMap)
{
	if ( guiMap != nullptr )
		return guiMap->getMapId();
	else
		return 0;
}

bool MAPS::NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers)
{
	if ( width == 0 || height == 0 )
	{
		Error("Invalid dimensions");
		return false;
	}

	std::shared_ptr<GuiMap> newMap = AddEmptyMap();

	if ( newMap->CreateNew(width, height, tileset, terrain, triggers) )
	{
		char title[256] = { "Untitled" };
		if ( UntitledNumber > 0 )
			std::snprintf(title, 256, "Untitled %d", UntitledNumber);

		if ( newMap->CreateThis(getHandle(), title) )
		{
			UntitledNumber++;
			EnableMapping();
			Focus(newMap);
			curr->Redraw(true);
			return true;
		}
		else
			Error("Failed to create MDI Child Window!");
	}
	else
	{
		CHKD_ERR("Failed to create map!\n\nError in %s\n\n%s", LastErrorLoc, LastError);
		Error(LastError);
	}

	RemoveMap(newMap);
	return false;
}

bool MAPS::OpenMap(const char* fileName)
{
	auto newMap = AddEmptyMap();

	if ( newMap->LoadFile(fileName) )
	{
		if ( newMap->CreateThis(getHandle(), fileName) )
		{
			SetWindowText(newMap->getHandle(), fileName);
			EnableMapping();
			Focus(newMap);

			if ( newMap->isProtected() && newMap->hasPassword() )
				chkd.enterPasswordWindow.CreateThis(chkd.getHandle());
			else if ( newMap->isProtected() )
				mb("Map is protected and will be opened as view only");

			SetFocus(chkd.getHandle());
			curr->Scroll(SCROLL_X|SCROLL_Y);
			curr->Redraw(true);
			curr->refreshScenario();
			return true;
		} 
		else
			Error("Failed to create MDI Child Window!");
	}

	RemoveMap(newMap);
	return false;
}

bool MAPS::OpenMap()
{
	return OpenMap(nullptr);
}

bool MAPS::SaveCurr(bool saveAs)
{
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
	std::shared_ptr<GuiMap> map = GetMap(hMap);
	if ( map != nullptr )
		RemoveMap(map);

	if ( openMaps.size() == 0 )
		DisableMapping();
}

void MAPS::CloseActive()
{
	SendMessage(MdiClient::getHandle(), WM_MDIDESTROY, (WPARAM)MdiClient::getActive(), NULL);
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
	if ( curr != nullptr && curr->getLayer() != newLayer )
	{
		curr->selections().removeTiles();

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

		curr->setLayer(newLayer);

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

	if ( curr->getLayer() == LAYER_UNITS )
	{
		if ( clipboard.isPasting() )
		{
			auto &units = clipboard.getUnits();
			for ( auto &pasteUnit : units )
				pasteUnit.unit.owner = newPlayer;
		}

		u16 numUnits = curr->numUnits();

		std::shared_ptr<ReversibleActions> unitChanges(new ReversibleActions);
		auto &selUnits = curr->selections().getUnits();
		for ( u16 &unitIndex : selUnits )
		{
			ChkUnit* unit;
			if ( curr->getUnit(unit, unitIndex) && newPlayer != unit->owner )
			{
				unitChanges->Insert(std::shared_ptr<UnitChange>(new UnitChange(unitIndex, UNIT_FIELD_OWNER, unit->owner)));
				unit->owner = newPlayer;
			}

			if ( chkd.unitWindow.getHandle() != nullptr )
			{
				std::string text;
				HWND hOwner = chkd.unitWindow.dropPlayer.getHandle();
				if ( newPlayer < 12 )
					SendMessage(hOwner, CB_SETCURSEL, newPlayer, NULL);
				else if ( chkd.mainToolbar.playerBox.GetEditText(text) )
					SetWindowText(hOwner, text.c_str());

				chkd.unitWindow.ChangeOwner(unitIndex, newPlayer);
			}
		}
		curr->undos().AddUndo(unitChanges);
		curr->Redraw(true);
	}

	char color[32], race[32], playerText[64];
	std::snprintf(color, sizeof(color), "Red");
	std::snprintf(race, sizeof(race), "Terran");
	std::snprintf(playerText, sizeof(playerText), "Player %i: %s %s", curr->currPlayer()+1, color, race);
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
			clipboard.copy(&curr->selections(), curr->scenario(), curr->getLayer());
			curr->deleteSelection();
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
			clipboard.copy(&curr->selections(), curr->scenario(), curr->getLayer());
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
	else if ( curr->getLayer() == LAYER_TERRAIN )
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
	else if ( curr->getLayer() == LAYER_UNITS )
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
		curr->Redraw(false);
}

void MAPS::properties()
{
	if ( curr->getLayer() == LAYER_TERRAIN )
	{
		if ( curr->selections().hasTiles() )
		{
			TileNode tile = curr->selections().getFirstTile();
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
	if ( curr->getLayer() == LAYER_LOCATIONS )
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

u16 MAPS::NextId()
{
	if ( lastUsedMapID < 65535 )
	{
		lastUsedMapID++;
		return lastUsedMapID;
	}
	else // Find one that isn't used, ambiguously use 65535 if all are used
	{
		u16 newMapId = 65535; // Use 65535 if all are used
		for ( u32 mapId = 0; mapId < 65536; mapId++ ) // Try every ID, break if you find one
		{
			bool used = false; // Assume it's not used until you find that it is
			auto it = openMaps.find(mapId);
			if ( it == openMaps.end() ) // mapId is unused
			{
				newMapId = mapId;
				break; // Use the current value of newMapId
			}
		}
		return newMapId; // Use the unused ID or 65535
	}
}

void MAPS::EnableMapping()
{
	if ( !mappingEnabled ) // Enable mapping functionality
	{
		mappingEnabled = true;

		int toolbarItems[] =
		{
			ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
			ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
		};

		for ( int i = 0; i < sizeof(toolbarItems) / sizeof(int); i++ )
			SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], true);

		ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_SHOW);
		ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_SHOW);
		ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_SHOW);

		HMENU hMenu = GetMenu(chkd.getHandle());

		for ( int i = 0; i < numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_ENABLED);

		HWND hLeftBar = chkd.mainPlot.leftBar.getHandle();
		ShowWindow(hLeftBar, SW_SHOW);

		chkd.statusBar.SetText(1, "Terrain");
	}
}

void MAPS::DisableMapping()
{
	if ( mappingEnabled )
	{
		mappingEnabled = false;

		int toolbarItems[] =
		{
			ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
			ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
		};

		for ( int i = 0; i < sizeof(toolbarItems) / sizeof(int); i++ )
			SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], false);

		ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
		ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

		HMENU hMenu = GetMenu(chkd.getHandle());

		for ( int i = 0; i < numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_DISABLED);

		chkd.mainPlot.leftBar.Hide();

		chkd.statusBar.SetText(0, "");
		chkd.statusBar.SetText(1, "");

		chkd.changePasswordWindow.Hide();
	}
}

std::shared_ptr<GuiMap> MAPS::AddEmptyMap()
{
	u16 id = NextId();
	if ( id < u16_max )
	{
		auto it = openMaps.insert(std::pair<u16, std::shared_ptr<GuiMap>>(id, std::shared_ptr<GuiMap>(new GuiMap(true))));
		if ( it != openMaps.end() )
		{
			it->second->setMapId(id);
			return it->second;
		}
	}
	return nullptr;
}

bool MAPS::RemoveMap(std::shared_ptr<GuiMap> guiMap)
{
	if ( guiMap == nullptr )
		return false;

	u16 toDelete = u16_max;
	for ( auto &pair : openMaps )
	{
		if ( guiMap == pair.second )
		{
			toDelete = pair.first;
			break;
		}
	}

	if ( toDelete != u16_max )
	{
		if ( guiMap == curr )
			curr = nullptr;

		openMaps.erase(toDelete);
		return true;
	}

	return false;
}
