#include "Maps.h"
#include "Common Files/CommonFiles.h"
#include "Unit.h"
#include "Terrain.h"
#include "GuiAccel.h"

extern HWND hMain;
extern HWND hPlot;
extern HWND hMaps;
extern HWND hMini;
extern HWND hTool;
extern HWND hLeft;
extern HWND hStatus;
extern HWND hTerrainProp;
extern HWND hPlayer;
extern HWND hUnit;
extern HWND hLayer;
extern HWND hZoom;
extern HWND hPlayer;
extern HWND hTerrainCombo;
extern HWND hWndTV;

void ChangeOwner(HWND hUnitList, int index, u8 newPlayer);

u16 lastUsedMapID(0);

MAPS::MAPS() : UntitledNumber(0),
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

void MAPS::Focus(HWND hFocus)
{
	MapNode* currNode = firstMap;
	while ( currNode != nullptr && currNode->map.getHandle() != hFocus )
		currNode = currNode->next;

	if ( currNode != nullptr /*&& curr != (&currNode->map)*/ ) // If a corresponding map was found and this map is not already the current
	{
		curr = &currNode->map; // Sets the current map to the map given by the handle
		// Need to update location tree, menu checkboxes
		BuildLocationTree(curr);
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

bool MAPS::NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers, HWND hMaps)
{
	MapNode* newMap = new MapNode;
	
	if ( newMap->map.CreateNew(width, height, tileset, terrain, triggers) )
	{
		char title[256] = { "Untitled" };
		if ( UntitledNumber > 0 )
			sprintf_s(title, 256, "Untitled %d", UntitledNumber);

		if ( newMap->map.setHandle(AddMDIChild(hMaps, title)) )
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

bool MAPS::OpenMap(HWND hMaps, const char* fileName)
{
	MapNode* newMap = new MapNode;

	if ( newMap->map.LoadFile(fileName) )
	{
		if ( newMap->map.setHandle( AddMDIChild(hMaps, fileName) ) )
		{
			SetWindowText(newMap->map.getHandle(), fileName);
			PushNode(newMap);
			Focus(newMap->map.getHandle());
			curr->Scroll(SCROLL_X|SCROLL_Y);
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
		delete newMap;
		Error(LastError);
	}

	return false;
}

bool MAPS::OpenMap(HWND hMaps)
{
	return OpenMap(hMaps, nullptr);
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
		Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
		RedrawWindow(hMini, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
	}

	if ( firstMap == nullptr ) // Disable mapping functionality
	{
		int toolbarItems[] =
		{
			ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
			ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
		};

		for ( int i=0; i<sizeof(toolbarItems)/sizeof(int); i++ )
			SendMessage(hTool, TB_ENABLEBUTTON, toolbarItems[i], false);

		ShowWindow(hLayer, false);
		ShowWindow(hZoom, false);
		ShowWindow(hPlayer, false);
		ShowWindow(hTerrainCombo, false);

		HMENU hMenu = GetMenu(hMain);

		for ( int i=0; i<numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_DISABLED);

		ShowWindow(hLeft, SW_HIDE);

		SendMessage(hStatus, SB_SETTEXT, 0, NULL);
		SendMessage(hStatus, SB_SETTEXT, 1, NULL);
	}
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
	if ( curr && curr->currLayer() != newLayer )
	{
		curr->selections().removeTiles();
		curr->currLayer() = newLayer;

		HWND hLayerBox = hLayer;
		if ( SendMessage(hLayerBox, CB_GETCURSEL, 0, 0) != newLayer )
			SendMessage(hLayerBox, CB_SETCURSEL, newLayer, 0);

		if ( newLayer == LAYER_FOG || newLayer == LAYER_UNITS || newLayer == LAYER_SPRITES || newLayer == LAYER_VIEW_FOG )
			// Layers where player#'s are relevant
		{
			ChangePlayer(curr->currPlayer());
			ShowWindow(hPlayer, SW_SHOW);
		}
		else // Layers where player#'s are irrelevant
		{
			SendMessage(hStatus, SB_SETTEXT, 2, NULL);
			ShowWindow(hPlayer, SW_HIDE);
		}

		if ( newLayer == LAYER_TERRAIN )
			ShowWindow(hTerrainCombo, SW_SHOW);
		else
			ShowWindow(hTerrainCombo, SW_HIDE);

		if ( hTerrainProp )
			EndDialog(hTerrainProp, NULL);

		//RedrawWindow(curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
		curr->Redraw(false);

		char layerString[32];
		SendMessage(hLayerBox, CB_GETLBTEXT, newLayer, (INT_PTR)layerString);
		SendMessage(hStatus, SB_SETTEXT, 1, (INT_PTR)layerString);
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
				SendMessage(hZoom, CB_SETCURSEL, i-1, 0);
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
				SendMessage(hZoom, CB_SETCURSEL, i+1, 0);
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
			if ( curr->getUnit(unit, currSelUnit->index) )
				unit->owner = newPlayer;

			if ( hUnit )
			{
				HWND hOwner = GetDlgItem(hUnit, IDC_COMBO_PLAYER);
				HWND hUnitList = GetDlgItem(hUnit, IDC_UNITLIST);
				if ( newPlayer < 12 )
					SendMessage(hOwner, CB_SETCURSEL, newPlayer, NULL);
				else
				{
					char* text;
					if ( GetEditText(hPlayer, text) )
					{
						SetWindowText(hOwner, text);
						delete[] text;
					}
				
				}
				ChangeOwner(hUnitList, currSelUnit->index, newPlayer);
			}

			currSelUnit = currSelUnit->next;
		}

		curr->Redraw(true);
	}

	char color[32], race[32], playerText[64];
	sprintf_s(color, "Red");
	sprintf_s(race, "Terran");
	sprintf_s(playerText, "Player %i: %s %s", curr->currPlayer()+1, color, race);
	SendMessage(hStatus, SB_SETTEXT, 2, (INT_PTR)playerText);
}

void MAPS::cut()
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

void MAPS::copy()
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
			if ( hTerrainProp )
				SendMessageA(hTerrainProp, WM_COMMAND, TILE_UPDATE, NULL);
			else
				hTerrainProp = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_SETTILE), hMain, SetTileProc);
			ShowWindow(hTerrainProp, SW_SHOW);
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
			SendMessage(hTool, TB_ENABLEBUTTON, toolbarItems[i], true);

		ShowWindow(hLayer, true);
		ShowWindow(hZoom, true);
		ShowWindow(hTerrainCombo, true);

		HMENU hMenu = GetMenu(hMain);

		for ( int i=0; i<numOnOffMenuItems; i++ )
			EnableMenuItem(hMenu, onOffMenuItems[i], MF_ENABLED);

		ShowWindow(hLeft, SW_SHOW);

		SendMessage(hStatus, SB_SETTEXT, 1, (INT_PTR)"Terrain");
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

extern MAPS maps;
extern HWND hMaps;

#define DIM_ERROR 10

BOOL CALLBACK NewMapProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				HWND hTrigStatic = GetDlgItem(hWnd, IDC_TRIGGERS_GROUP);
				RECT rcTrigStatic;
				GetWindowRect(hTrigStatic, &rcTrigStatic);
				POINT upperLeft;
				upperLeft.x = rcTrigStatic.left;
				upperLeft.y = rcTrigStatic.top;
				ScreenToClient(hWnd, &upperLeft);

				SendMessage(hWnd, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
				HWND hDefaultTrigs = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_HASSTRINGS,
						upperLeft.x+10, upperLeft.y+20, 305, 200, hWnd, (HMENU)IDD_COMBO_TRIGS, (HINSTANCE)GetModuleHandle(NULL), NULL);
					// Add the trigger options
						const char* defaultTriggerTitles[] = { "No triggers", "Default melee triggers", "Melee with observers (2 players)",
							"Melee with observers (3 players)", "Melee with observers (4 players)", "Melee with observers (5 players)",
							"Melee with observers (6 players)", "Melee with observers (7 players)"};
						for ( int i=0; i<8; i++ )
							SendMessage(hDefaultTrigs, CB_ADDSTRING, NULL, (LPARAM)defaultTriggerTitles[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					SendMessage(hDefaultTrigs, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hWidth = GetDlgItem(hWnd, IDC_EDIT_WIDTH);
				SendMessage(hWidth, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hWidth, "128");

				HWND hHeight = GetDlgItem(hWnd, IDC_EDIT_HEIGHT);
				SendMessage(hHeight, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hHeight, "128");

				HWND hTileset = GetDlgItem(hWnd, IDC_LIST_TILESET);
				for ( int i=0; i<8; i++ )
					SendMessage(hTileset, LB_ADDSTRING, NULL, (LPARAM)tilesetNames[i]);
				SendMessage(hTileset, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hTileset, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
				for ( int i=0; i<numBadlandsInitTerrain; i++ )
					SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)badlandsInitTerrain[i]);
				SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hInitialTerrain, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
				return true;
			}
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case ID_CREATEMAP:
						{
							int width=0, height=0, tileset=0, terrain=0, triggers=0;
							TCHAR lpszWidth[11], lpszHeight[11];
							WORD dimLength;

							dimLength = (WORD) SendDlgItemMessage(hWnd, IDC_EDIT_WIDTH, EM_LINELENGTH, 0, 0);
							*((LPWORD)lpszWidth) = dimLength;
							SendDlgItemMessage(hWnd, IDC_EDIT_WIDTH, EM_GETLINE, 0, (LPARAM)lpszWidth);	lpszWidth[dimLength] = NULL;
							width = atoi(lpszWidth);
	
							dimLength = (WORD) SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_LINELENGTH, 0, 0);	*((LPWORD)lpszHeight) = dimLength;
							SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_GETLINE, 0, (LPARAM)lpszHeight);	lpszHeight[dimLength] = NULL;
							height = atoi(lpszHeight);
	
							tileset = SendDlgItemMessage(hWnd, IDC_LIST_TILESET, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0 );
							terrain = SendDlgItemMessage(hWnd, IDC_LIST_DEFAULTTERRAIN, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0 );
							triggers = SendDlgItemMessage(hWnd, IDD_COMBO_TRIGS, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0 );
	
							if ( width>0 && height>0 )
							{
								if ( width > 65535 ) width %= 65536;
								if ( height > 65535 ) height %= 65536;
								if ( (u32)width*height < 429496729 )
								{
									if ( maps.NewMap(width, height, tileset, terrain, triggers, hMaps) )
									{
										maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, NULL, NULL));
										maps.curr->Scroll(SCROLL_X|SCROLL_Y);
	
										// Tiling Code
										u16 tilenum = 0;
										u16 xSize = maps.curr->XSize();
										for ( u32 xStart=0; xStart<maps.curr->XSize(); xStart+=16)
										{
											for ( u16 yc=0; yc<maps.curr->YSize(); yc++ )
											{
												for ( u16 xc=(u16)xStart; xc<xStart+16; xc++ )
												{
													u32 location = 2*xSize*yc+2*xc; // Down y rows, over x columns
													maps.curr->TILE().replace<u16>(location, tilenum);
													maps.curr->MTXM().replace<u16>(location, tilenum);
													tilenum++;
												}
											}
										}

										maps.curr->Redraw(true);
									}
									EndDialog(hWnd, ID_CREATEMAP);
								}
								else
								{
									Error("Maps cannot be over 2gb");
									EndDialog(hWnd, DIM_ERROR);
								}
							}
							else
								EndDialog(hWnd, DIM_ERROR);
						}
						break;

					case IDCANCEL:
						EndDialog(hWnd, IDCANCEL);
						break;

					default:
						{
							if ( HIWORD(wParam) == LBN_SELCHANGE )
							{
								int ItemIndex = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM) 0, (LPARAM)0);
								switch ( LOWORD(wParam) )
								{
									case IDC_LIST_TILESET:
										{
											HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
											HWND hDefaultTrigs = GetDlgItem(hWnd, IDD_COMBO_TRIGS);
											SendMessage(hInitialTerrain, LB_RESETCONTENT, 0, 0);
											switch ( ItemIndex )
											{
												case TERRAIN_BADLANDS: // Badlands
													{
														for (int i=0; i<numBadlandsInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)badlandsInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_SPACE: // Space Platform
													{
														for (int i=0; i<numSpaceInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)spaceInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_INSTALLATION: // Installation
													{
														for (int i=0; i<numInstallationInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)installInitTerrain[i]);
														SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
													}
													break;
												case TERRAIN_ASH: // Ash World
													{
														for (int i=0; i<numAshInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)ashInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_JUNGLE: // Jungle World
													{
														for (int i=0; i<numJungInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)jungInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_DESERT: // Desert World
													{
														for (int i=0; i<numDesertInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)desertInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_ICE: // Ice World
													{
														for (int i=0; i<numIceInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)iceInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
												case TERRAIN_TWILIGHT: // Twilight World
													{
														for (int i=0; i<numTwilightInitTerrain; i++)
															SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)twilightInitTerrain[i]);
														SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
													}
													break;
											}
											SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
										}
										break;
								}
							}	
						}
						break;
				}
				return true;
			}
			break;

		default:
			return false;
			break;
	}
	return 0;
}
