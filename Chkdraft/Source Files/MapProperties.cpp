#include "MapProperties.h"
#include "Chkdraft.h"

bool MapPropertiesWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "MapProperties", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "MapProperties", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_MAPPROPERTIES) )
	{
		GuiMap* map = chkd.maps.curr;
		HWND hMapProperties = getHandle();

		std::string mapTitle(""), mapDescription("");
		u16 currTileset = 0,
			currWidth = 0,
			currHeight = 0;

		if ( map != nullptr )
		{
			map->getMapTitle(mapTitle);
			map->getMapDescription(mapDescription);
			currTileset = map->getTileset(),
			currWidth = map->XSize(),
			currHeight = map->YSize();
		}

		CreateStaticText(hMapProperties, 5, 5, 50, 20, "Map Title");
		CreateEditBox(hMapProperties, 5, 25, 582, 20, mapTitle.c_str(), false, ID_EDIT_MAPTITLE);

		CreateStaticText(hMapProperties, 5, 60, 100, 20, "Map Description");
		CreateEditBox(hMapProperties, 5, 80, 582, 100, mapDescription.c_str(), true, ID_EDIT_MAPDESCRIPTION);

		char sCurrWidth[12], sCurrHeight[12];
		_itoa_s(currWidth, sCurrWidth, 10);
		_itoa_s(currHeight, sCurrHeight, 10);

		CreateStaticText(hMapProperties, 5, 185, 100, 20, "Map Tileset");
		CreateDropdownBox(hMapProperties, 5, 205, 185, 400, (const char**)tilesetNames, NUM_TILESETS, currTileset, false, ID_CB_MAPTILESET);
		CreateStaticText(hMapProperties, 195, 185, 100, 20, "[New] Terrain");
		CreateDropdownBox(hMapProperties, 195, 205, 185, 400, (const char**)initTerrains[currTileset], numTilesetInitTerrains[currTileset], 0, false, ID_CB_NEWMAPTERRAIN);
		CreateStaticText(hMapProperties, 385, 185, 50, 20, "Width");
		CreateEditBox(hMapProperties, 385, 205, 50, 20, sCurrWidth, false, ID_EDIT_NEWMAPWIDTH);
		CreateStaticText(hMapProperties, 440, 185, 50, 20, "Height");
		CreateEditBox(hMapProperties, 440, 205, 50, 20, sCurrHeight, false, ID_EDIT_NEWMAPHEIGHT);
		CreateButton(hMapProperties, 494, 205, 91, 20, "Apply", ID_BUTTON_APPLY);

		const char* sPlayers[] = { "Player 1", "Player 2", "Player 3", "Player 4",
								  "Player 5", "Player 6", "Player 7", "Player 8",
								  "Player 9", "Player 10", "Player 11", "Player 12" };

		for ( int yBox=0; yBox<3; yBox++ )
		{
			for ( int xBox=0; xBox<4; xBox++ )
			{
				int player = yBox*4+xBox;
				u8 displayOwner = 0, race = 0, color = (u8)player;
				if ( map != nullptr )
				{
					displayOwner = map->getDisplayOwner((u8)player);
					map->getPlayerRace((u8)player, race);
				}

				CreateGroupBox(hMapProperties, 5+146*xBox, 242+95*yBox, 141, 91, sPlayers[yBox*4+xBox]);
				CreateStaticText(hMapProperties, 15+146*xBox, 257+95*yBox, 50, 20, "Owner");
				CreateDropdownBox(hMapProperties, 60+146*xBox, 257+95*yBox, 80, 140, playerOwners, numPlayerOwners, displayOwner, false, ID_CB_P1OWNER+player);
				CreateStaticText(hMapProperties, 15+146*xBox, 282+95*yBox, 50, 20, "Race");
				CreateDropdownBox(hMapProperties, 60+146*xBox, 282+95*yBox, 80, 110, playerRaces, numPlayerRaces, race, false, ID_CB_P1RACE+player);

				if ( yBox < 2 )
				{
					if ( map != nullptr )
						map->getPlayerColor((u8)player, color);
					CreateStaticText(hMapProperties, 15+146*xBox, 307+95*yBox, 50, 20, "Color");
					CreateDropdownBox(hMapProperties, 60+146*xBox, 307+95*yBox, 80, 140, playerColors, numPlayerColors, color, true, ID_CB_P1COLOR+player);
				}
			}
		}
		return true;
	}
	else
		return false;
}

LRESULT MapPropertiesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			{
				GuiMap* map = chkd.maps.curr;
				if ( map != nullptr )
				{
					string mapTitle, mapDescription;
					map->getMapTitle(mapTitle);
					map->getMapDescription(mapDescription);
					u16 tileset = map->getTileset(),
						currWidth = map->XSize(),
						currHeight = map->YSize();

					char sCurrWidth[12], sCurrHeight[12];
					_itoa_s(currWidth, sCurrWidth, 10);
					_itoa_s(currHeight, sCurrHeight, 10);

					SetWindowText(GetDlgItem(hWnd, ID_EDIT_MAPTITLE), mapTitle.c_str());
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_MAPDESCRIPTION), mapDescription.c_str());
					SendMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_SETCURSEL, tileset, NULL);
					PostMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_SETEDITSEL, NULL, (-1, 0));
					SendMessage(GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN), CB_SETCURSEL, 0, NULL);
					PostMessage(GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN), CB_SETEDITSEL, NULL, (-1, 0));
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_NEWMAPWIDTH), sCurrWidth);
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_NEWMAPHEIGHT), sCurrHeight);
					
					for ( int player=0; player<12; player++ )
					{
						u8 displayOwner(map->getDisplayOwner(player)), race(0), color(0);
						map->getPlayerRace(player, race);
						SendMessage(GetDlgItem(hWnd, ID_CB_P1OWNER+player), CB_SETCURSEL, displayOwner, NULL);
						PostMessage(GetDlgItem(hWnd, ID_CB_P1OWNER+player), CB_SETEDITSEL, NULL, (-1, 0));
						SendMessage(GetDlgItem(hWnd, ID_CB_P1RACE+player), CB_SETCURSEL, race, NULL);
						PostMessage(GetDlgItem(hWnd, ID_CB_P1RACE+player), CB_SETEDITSEL, NULL, (-1, 0));

						if ( player < 8 )
						{
							map->getPlayerColor(player, color);
							SendMessage(GetDlgItem(hWnd, ID_CB_P1COLOR+player), CB_SETCURSEL, 0, NULL);
							PostMessage(GetDlgItem(hWnd, ID_CB_P1COLOR+player), CB_SETEDITSEL, NULL, (-1, 0));
						}
					}
				}
			}
			break;

		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				SendMessage(hWnd, REFRESH_WINDOW, NULL, NULL);
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case ID_EDIT_MAPTITLE:
						if ( HIWORD(wParam) == EN_KILLFOCUS )
						{
							string newMapTitle;
							if ( GetEditText(GetDlgItem(hWnd, ID_EDIT_MAPTITLE), newMapTitle) )
							{
								u16* mapTitleString;
								if ( chkd.maps.curr->SPRP().getPtr<u16>(mapTitleString, 0, 2) &&
									 parseEscapedString(newMapTitle) &&
									 chkd.maps.curr->replaceString(newMapTitle, *mapTitleString, false, true) )
								{
									chkd.maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_EDIT_MAPDESCRIPTION:
						if ( HIWORD(wParam) == EN_KILLFOCUS )
						{
							string newMapDescription;
							if ( GetEditText(GetDlgItem(hWnd, ID_EDIT_MAPDESCRIPTION), newMapDescription) )
							{
								u16* mapDescriptionString;
								if ( chkd.maps.curr->SPRP().getPtr<u16>(mapDescriptionString, 2, 2) &&
									 parseEscapedString(newMapDescription) &&
									 chkd.maps.curr->replaceString(newMapDescription, *mapDescriptionString, false, true) )
								{
									chkd.maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_BUTTON_APPLY:
						{
							if ( HIWORD(wParam) == BN_CLICKED )
							{
								HWND hButton = GetDlgItem(hWnd, ID_BUTTON_APPLY);
								DestroyWindow(hButton); // Seems to be the best way to remove stubborn highlighting
								CreateButton(hWnd, 494, 205, 91, 20, "Apply", ID_BUTTON_APPLY);
								LRESULT newTileset = SendMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_GETCURSEL, NULL, NULL);
								chkd.maps.curr->setTileset((u16)newTileset);
								u16 newWidth, newHeight;
								if ( GetEditNum<u16>(hWnd, ID_EDIT_NEWMAPWIDTH, newWidth) && GetEditNum<u16>(hWnd, ID_EDIT_NEWMAPHEIGHT, newHeight) )
									chkd.maps.curr->setDimensions((u16)newWidth, (u16)newHeight);

								// Apply new terrain...

								chkd.maps.curr->notifyChange(false);
								chkd.maps.curr->Redraw(true);
							}
						}
						break;

					case ID_CB_MAPTILESET:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							HWND hMapTileset = GetDlgItem(hWnd, ID_CB_MAPTILESET), hMapNewTerrain = GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN);
							LRESULT currTileset = SendMessage(hMapTileset, CB_GETCURSEL, NULL, NULL);
							if ( currTileset != CB_ERR && currTileset < NUM_TILESETS )
							{
								while ( SendMessage(hMapNewTerrain, CB_DELETESTRING, 0, NULL) != CB_ERR );

								for ( int i=0; i<numTilesetInitTerrains[currTileset]; i++ )
									SendMessage(hMapNewTerrain, CB_ADDSTRING, NULL, (LPARAM)initTerrains[currTileset][i]);

								SendMessage(hMapNewTerrain, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
								SendMessage(hMapNewTerrain, CB_SETCURSEL, 0, NULL);
								PostMessage(hMapNewTerrain, CB_SETEDITSEL, NULL, (-1, 0));
							}
						}
						break;
					case ID_CB_P1OWNER: case ID_CB_P2OWNER: case ID_CB_P3OWNER: case ID_CB_P4OWNER:
					case ID_CB_P5OWNER: case ID_CB_P6OWNER: case ID_CB_P7OWNER: case ID_CB_P8OWNER:
					case ID_CB_P9OWNER: case ID_CB_P10OWNER: case ID_CB_P11OWNER: case ID_CB_P12OWNER:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							u32 player = LOWORD(wParam)-ID_CB_P1OWNER; // 0 based player
							u8 newOwner = 0;
							LRESULT sel = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
							if ( player >= 0 && player < 12 && sel != CB_ERR )
							{
								switch ( sel )
								{
									case 0: newOwner = 4; break;
									case 1: newOwner = 3; break;
									case 2: newOwner = 5; break;
									case 3: newOwner = 6; break;
									case 4: newOwner = 7; break;
								}
								if ( newOwner != 0 )
								{
									chkd.maps.curr->setPlayerOwner((u8)player, newOwner);
									chkd.maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_CB_P1RACE: case ID_CB_P2RACE: case ID_CB_P3RACE: case ID_CB_P4RACE:
					case ID_CB_P5RACE: case ID_CB_P6RACE: case ID_CB_P7RACE: case ID_CB_P8RACE:
					case ID_CB_P9RACE: case ID_CB_P10RACE: case ID_CB_P11RACE: case ID_CB_P12RACE:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							u32 player = LOWORD(wParam)-ID_CB_P1RACE; // 0 based player
							LRESULT newRace = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
							if ( player >= 0 && player < 12 && newRace != CB_ERR && newRace >= 0 && newRace < 8 )
							{
								chkd.maps.curr->setPlayerRace((u8)player, (u8)newRace);
								chkd.maps.curr->notifyChange(false);
							}
						}
						break;

					case ID_CB_P1COLOR: case ID_CB_P2COLOR: case ID_CB_P3COLOR: case ID_CB_P4COLOR:
					case ID_CB_P5COLOR: case ID_CB_P6COLOR: case ID_CB_P7COLOR: case ID_CB_P8COLOR:
						switch ( HIWORD(wParam) )
						{
							case CBN_SELCHANGE:
								{
									u32 player = LOWORD(wParam)-ID_CB_P1COLOR; // 0 based player
									LRESULT newColor = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
									if ( player >= 0 && player < 12 && newColor != CB_ERR && newColor >= 0 && newColor < 16 )
									{
										if ( chkd.maps.curr->setPlayerColor((u8)player, (u8)newColor) )
											chkd.maps.curr->Redraw(true);

										chkd.maps.curr->notifyChange(false);
									}
								}
								break;
							case CBN_EDITCHANGE:
								{
									u32 player = LOWORD(wParam)-ID_CB_P1COLOR; // 0 based player
									u8 newColor;
									if ( GetEditNum<u8>((HWND)lParam, newColor) )
									{
										if ( chkd.maps.curr->setPlayerColor((u8)player, newColor) )
											chkd.maps.curr->Redraw(true);

										chkd.maps.curr->notifyChange(false);
									}
								}

						}
						break;
				}
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
