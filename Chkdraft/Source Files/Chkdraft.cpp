#include "Common Files/CommonFiles.h"
#include "Terrain.h"
#include "Math.h"
#include "Graphics.h"
#include "TextTrig.h"
#include "Unit.h"
#include "Location.h"
#include "MapSettings.h"
#include "Maps.h"
#include "GuiAccel.h"
#include "ChkdPlugins.h"
#include <WindowsX.h>
#include <math.h>

using namespace std;
#ifdef CHKD_DEBUG
CLI cli;
#endif

MAPS maps;
DATA scData;

HWND hMain(nullptr), hTool(nullptr), hPlot(nullptr), hLeft(nullptr), hStatus(nullptr),
	 hMaps(nullptr), hMini(nullptr), hLayer(nullptr), hZoom(nullptr), hPlayer(nullptr),
	 hTerrainCombo(nullptr), hTerrainProp(nullptr), hUnit(nullptr), hTrigText(nullptr),
	 hTilePal(nullptr), hWndTV(nullptr), hLocation(nullptr), hMapSettings(nullptr);
HMENU hMainMenu(nullptr);

bool dlgHotkey = false;
bool editFocused = false;
extern bool changeHighlightOnly;

void DlgKeyListener(HWND hWnd, UINT &msg, WPARAM &wParam, LPARAM &lParam);
void KeyListener(HWND hWnd, UINT &msg, WPARAM &wParam, LPARAM &lParam);
void ParseCmdLine(LPSTR lpCmdLine);

BOOL	CALLBACK NewMapProc	  (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Window for creating new maps
LRESULT CALLBACK WndProc	  (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // For the main window
LRESULT CALLBACK PlotProc	  (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // For holding the left bar
LRESULT CALLBACK LeftBarProc  (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // For minimap and selection tree
LRESULT CALLBACK MiniMapProc  (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // For the minimap specifically
LRESULT CALLBACK MDIChildProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Individual Maps
LRESULT CALLBACK MapMouseProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, GuiMap* map); // TEMPORARY mouse abstraction

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcMainWin = { }, 
			   wcPlot	 = { }, 
			   wcLeftBar = { },
			   wcMiniMap = { },
			   wcChild	 = { };

	wcMainWin.cbSize		 = sizeof(WNDCLASSEX);
    wcMainWin.lpfnWndProc    = WndProc;
    wcMainWin.hInstance      = hInstance;
    wcMainWin.hIcon          = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 32, 32, 0);
	wcMainWin.hIconSm        = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
    wcMainWin.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcMainWin.hbrBackground  = (HBRUSH) (COLOR_APPWORKSPACE + 1);
	wcMainWin.lpszMenuName	 = MAKEINTRESOURCE(IDR_MAIN_MENU);
    wcMainWin.lpszClassName  = "MainChkdraftWin";

	wcPlot.cbSize			 = sizeof(WNDCLASSEX);
	wcPlot.lpfnWndProc		 = PlotProc;
	wcPlot.hInstance		 = hInstance;
	wcPlot.hCursor			 = LoadCursor(NULL, IDC_ARROW);
	wcPlot.hbrBackground	 = (HBRUSH) (COLOR_APPWORKSPACE + 1);
	wcPlot.lpszClassName	 = "Plot";

	wcLeftBar				 = wcPlot;
    wcLeftBar.lpfnWndProc    = LeftBarProc;
    wcLeftBar.hbrBackground  = CreateSolidBrush(RGB(240, 240, 240));
    wcLeftBar.lpszClassName  = "LeftBar";

	wcMiniMap				 = wcPlot;
	wcMiniMap.lpfnWndProc	 = MiniMapProc;
	wcMiniMap.hbrBackground  = CreateSolidBrush(RGB(166, 156, 132));
	wcMiniMap.lpszClassName  = "MiniMap";

	wcChild = wcMainWin;
	wcChild.cbClsExtra	  = 1;
	wcChild.lpfnWndProc   = MDIChildProc; 
	wcChild.style		  = CS_DBLCLKS;
    wcChild.hIcon         = NULL;
	wcChild.hIconSm		  = NULL;
    wcChild.lpszMenuName  = NULL; 
    wcChild.lpszClassName = "MdiChild"; 

	if ( !RegisterClassEx(&wcMainWin) || 
		 !RegisterClassEx(&wcPlot)	  ||
		 !RegisterClassEx(&wcLeftBar) ||
		 !RegisterClassEx(&wcMiniMap) ||
		 !RegisterClassEx(&wcChild) )
	{
		Error("Window Registration Failed!");
		return 0;
	}

	hMain = CreateWindowEx(NULL, "MainChkdraftWin", "Chkdraft 1.0 (Alpha 1.0.1)",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if ( !hMain )
	{
		Error("Window Creation Failed!");
		return 1;
	}

	scData.Load();

	InitCommonControls();
	ShowWindow(hMain, SW_MAXIMIZE);
    UpdateWindow(hMain);

	ParseCmdLine(lpCmdLine);

	/*
	{ // On-Load Test
		
	}//*/

	MSG msg;
	while ( GetMessage(&msg, NULL, 0, 0) > 0 )
	{
		DlgKeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		if ( !IsDialogMessage(hTrigText, &msg) &&
			 !IsDialogMessage(hTerrainProp, &msg) &&
			 !IsDialogMessage(hUnit, &msg) &&
			 !IsDialogMessage(hLocation, &msg) &&
			 !IsDialogMessage(hTilePal, &msg) &&
			 !IsDialogMessage(hMapSettings, &msg) )
		{
			if ( !TranslateMDISysAccel(hMaps, &msg) )
			{
				TranslateMessage(&msg);

				if ( !dlgHotkey )
					KeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);

				DispatchMessage(&msg);
			}
		}
	}
	return msg.wParam;
}

void DlgKeyListener(HWND hWnd, UINT &msg, WPARAM &wParam, LPARAM &lParam)
{
	switch ( msg )
	{
		case WM_KEYDOWN:
			{
				if ( GetParent(hWnd) == hUnit || GetParent(hWnd) == hLocation )
				{
					switch ( wParam )
					{
						case VK_RETURN:
							if ( GetParent(hWnd) == hUnit )
							{
								EndDialog(hUnit, IDCLOSE);
								hUnit = nullptr;
								dlgHotkey = true;
							}
							else if ( GetParent(hWnd) == hLocation )
							{
								EndDialog(hLocation, IDCLOSE);
								locProcLocIndex = NO_LOCATION;
								hLocation = nullptr;
								dlgHotkey = true;
							}
							break;
						case VK_DELETE:
							if ( GetParent(hWnd) == hUnit )
							{
								SendMessage(hUnit, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, 0), NULL);
								dlgHotkey = true;
							}
							break;
						case 'Z':
						case 'Y':
						case 'X':
						case 'C':
						case 'V':
							if ( GetKeyState(VK_CONTROL) & 0x8000 )
								KeyListener(hWnd, msg, wParam, lParam);
							break;
					}
				}
			}
			break;
		case WM_KEYUP:
			if ( wParam == VK_SPACE && maps.curr && maps.clipboard.isPasting() )
				LockCursor(NULL);
			break;
		default:
			dlgHotkey = false;
			break;
	}
}

void KeyListener(HWND hWnd, UINT &msg, WPARAM &wParam, LPARAM &lParam)
{
	switch ( msg )
	{
		case WM_KEYDOWN:
			{
				switch ( wParam )
				{
					case VK_SPACE:
						if ( maps.curr )
							LockCursor(maps.curr->getHandle());
						return; break;

					case VK_DELETE:
						if ( maps.curr )
							maps.curr->deleteSelection();
						return; break;

					case VK_ESCAPE:
						if ( maps.curr != nullptr )
						{
							maps.endPaste();
							maps.curr->clearSelection();
						}
						return; break;

					case VK_RETURN:
						if ( maps.curr != nullptr )
						{
							if ( maps.curr->currLayer() == LAYER_UNITS )
							{
								if ( maps.curr->selections().hasUnits() )
								{
									if ( hUnit == nullptr )
										hUnit = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UNITPROPERTIES), hMain, UnitPropProc);
									ShowWindow(hUnit, SW_SHOW);
								}
							}
							else if ( maps.curr->currLayer() == LAYER_LOCATIONS )
							{
								if ( maps.curr->selections().getSelectedLocation() != NO_LOCATION )
								{
									if ( hLocation == nullptr )
										hLocation = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOCPROP), hMain, LocationPropProc);
									ShowWindow(hLocation, SW_SHOW);
								}
							}
						}
						return; break;
				}

				if ( GetKeyState(VK_CONTROL) & 0x8000 )
				{
					if ( GetKeyState(VK_SHIFT) & 0x8000 )
					{
						switch ( wParam )
						{
							case 'S': // Ctrl + Shift + S: Save-As
								if ( maps.curr )
								{
									maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
									maps.SaveCurr(true);
								}
								return; break;
						}
					}
					else
					{
						switch ( wParam )
						{
							case 'A': // Ctrl + A: Select All
								if ( maps.curr )
									maps.curr->selectAll();
								return; break;
							case 'C': // Ctrl + C
								if ( maps.curr )
									maps.copy();
								return; break;
							case 'D': // Ctrl + D
								maps.ChangeLayer(LAYER_DOODADS);
								return; break;
							case 'E': // Ctrl + E
								FindLeaks();
								return; break;
							case 'F': // Ctrl + F
								maps.ChangeLayer(LAYER_FOG);
								return; break;
							case 'L': // Ctrl + L
								maps.ChangeLayer(LAYER_LOCATIONS);
								return; break;
							case 'V': // Ctrl + V
								if ( maps.curr )
									maps.startPaste(false);
								return; break;
							case 'N': // Ctrl + N
								DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_NEW), hMain, NewMapProc);
								SetFocus(hMain);
								return; break;
							case 'O': // Ctrl + O
								maps.OpenMap(hMaps);
								return; break;
							case 'R': // Ctrl + R
								maps.ChangeLayer(LAYER_SPRITES);
								return; break;
							case 'S': // Ctrl + S
								if ( maps.curr )
								{
									maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
									maps.SaveCurr(false);
								}
								return; break;
							case 'T': // Ctrl + T
								maps.ChangeLayer(LAYER_TERRAIN);
								return; break;
							case 'U': // Ctrl + U
								maps.ChangeLayer(LAYER_UNITS);
								return; break;
							case 'X': // Ctrl + X
								if ( maps.curr )
									maps.cut();
								return; break;
							case 'W': // Ctrl + W
								{

								}
								return; break;
							case 'Y': // Ctrl + Y
								if ( maps.curr )
								{
									maps.curr->redo();
									maps.curr->Redraw(true);
								}
								return; break;
							case 'Z': // Ctrl + Z
								if ( maps.curr )
								{
									maps.curr->undo();
									maps.curr->Redraw(true);
								}
								return; break;
							case VK_OEM_PLUS:
								maps.ChangeZoom(true);
								return; break;
							case VK_OEM_MINUS:
								maps.ChangeZoom(false);
								return; break;
							case VK_F4: // Ctrl + F4
								if ( maps.curr )
									maps.CloseMap(maps.curr->getHandle());
								return; break;
							case VK_F6: // Ctrl + F6
								if ( maps.curr )
									SendMessage(hMaps, WM_MDINEXT, 0, 1);
								return; break;
						}
					}
				}
			}
			break;

		case WM_SYSKEYDOWN: // Sent insted of WM_KEYUP when alt is pressed
			{
				switch ( wParam )
				{
					case 'U': // Alt + U
						maps.SetGrid(8, 8);
						return; break;
					case 'F': // Alt + F
						maps.SetGrid(16, 16);
						return; break;
					case 'G': // Alt + G
						maps.SetGrid(32, 32);
						return; break;
					case 'L': // Alt + L
						maps.SetGrid(64, 64);
						return; break;
					case 'E': // Alt + E
						maps.SetGrid(128, 128);
						return; break;
				}
			}
			break;

		case WM_KEYUP:
			{
				switch ( wParam )
				{
					case VK_SPACE:
						if ( maps.curr && !maps.clipboard.isPasting() )
							LockCursor(NULL);
						return; break;
				}
			}
			break;
	}

	if ( maps.curr && editFocused == false && GetActiveWindow() == hMain )
	{
		u8 layer = maps.curr->currLayer();

		if ( ( layer == LAYER_UNITS	  || layer == LAYER_FOG ||
			   layer == LAYER_SPRITES ) )
		{
			u8 newPlayer;
			switch ( wParam )
			{
				case '1': newPlayer = 0	; break;
				case '2': newPlayer = 1	; break;
				case '3': newPlayer = 2	; break;
				case '4': newPlayer = 3	; break;
				case '5': newPlayer = 4	; break;
				case '6': newPlayer = 5	; break;
				case '7': newPlayer = 6	; break;
				case '8': newPlayer = 7	; break;
				case '9': newPlayer = 8	; break;
				case '0': newPlayer = 9	; break;
				case '-': newPlayer = 10; break;
				case '=': newPlayer = 11; break;
				default	: return		; break;
			}

			SendMessage(hPlayer, CB_SETCURSEL, newPlayer, 0);
			maps.ChangePlayer(newPlayer);
			return;
		}
	}
}

void ParseCmdLine(LPSTR lpCmdLine)
{
	int length = strlen(lpCmdLine);
	if ( length > 1 )
	{
		if ( lpCmdLine[0] == '\"' )
		{
			lpCmdLine[length-1] = NULL;
			maps.OpenMap(hMaps, &lpCmdLine[1]);
		}
		else
			maps.OpenMap(hMaps, &lpCmdLine[0]);

		maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
	}
}

bool ChangesLocked(u16 mapId)
{
	GuiMap* map = maps.GetMap(mapId);
	if ( map != nullptr && map->changesLocked() )
		return true;
	else
		return false;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case FLAG_UNDOABLE_CHANGE: case UNFLAG_UNDOABLE_CHANGES: case FLAG_LOCKED_CHANGE:
			{
				GuiMap* map = maps.GetMap((u16)wParam);
				if ( map != nullptr )
				{
					if ( msg == FLAG_UNDOABLE_CHANGE )
						map->notifyChange(true);
					else if ( msg == FLAG_LOCKED_CHANGE )
						map->notifyChange(false);
					else
						map->changesUndone();
				}
			}
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					// File
					case ID_FILE_NEW1:
						DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_NEW), hWnd, NewMapProc);
						SetFocus(hMain);
						break;
					case ID_FILE_OPEN1:
						maps.OpenMap(hMaps);
						break;
					case ID_FILE_CLOSE1:
						SendMessage(hMaps, WM_MDIDESTROY, (WPARAM)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0), NULL);
						break;
					case ID_FILE_SAVE1:
						maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
						maps.SaveCurr(false);
						break;
					case ID_FILE_SAVEAS1:
						maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
						maps.SaveCurr(true);
						break;
					case ID_FILE_QUIT1:
						PostQuitMessage(0);
						break;

					// Edit
					case ID_EDIT_UNDO1:
						maps.curr->undo();
						maps.curr->Redraw(true);
						break;
					case ID_EDIT_REDO1:
						maps.curr->redo();
						maps.curr->Redraw(true);
						break;
					case ID_EDIT_CUT1:
						if ( maps.curr )
							maps.cut();
						break;
					case ID_EDIT_COPY1:
						if ( maps.curr )
							maps.copy();
						break;
					case ID_EDIT_PASTE1:
						maps.startPaste(true);
						break;
					case ID_EDIT_SELECTALL:
						maps.curr->selectAll();
						break;
					case ID_EDIT_DELETE:
						maps.curr->deleteSelection();
						break;
					case ID_EDIT_PROPERTIES:
						maps.properties();
						break;

					// View
						// Grid
						case ID_GRID_ULTRAFINE:
							maps.SetGrid(8, 8);
							break;
						case ID_GRID_FINE:
							maps.SetGrid(16, 16);
							break;
						case ID_GRID_NORMAL:
							maps.SetGrid(32, 32);
							break;
						case ID_GRID_LARGE:
							maps.SetGrid(64, 64);
							break;
						case ID_GRID_EXTRALARGE:
							maps.SetGrid(128, 128);
							break;
						case ID_GRID_DISABLED:
							maps.SetGrid(0, 0);
							break;
							// Color
							case ID_COLOR_BLACK:
								maps.SetGridColor(0, 0, 0);
								break;
							case ID_COLOR_GREY:
								maps.SetGridColor(72, 72, 88);
								break;
							case ID_COLOR_WHITE:
								maps.SetGridColor(255, 255, 255);
								break;
							case ID_COLOR_GREEN:
								maps.SetGridColor(16, 252, 24);
								break;
							case ID_COLOR_RED:
								maps.SetGridColor(244, 4, 4);
								break;
							case ID_COLOR_BLUE:
								maps.SetGridColor(36, 36, 252);
								break;
							case ID_COLOR_OTHER:
								break;
						// Zoom
						case ID_ZOOM_400: maps.curr->setZoom(zooms[0]); break;
						case ID_ZOOM_300: maps.curr->setZoom(zooms[1]); break;
						case ID_ZOOM_200: maps.curr->setZoom(zooms[2]); break;
						case ID_ZOOM_150: maps.curr->setZoom(zooms[3]); break;
						case ID_ZOOM_100: maps.curr->setZoom(zooms[4]); break;
						case ID_ZOOM_66:  maps.curr->setZoom(zooms[5]); break;
						case ID_ZOOM_50:  maps.curr->setZoom(zooms[6]); break;
						case ID_ZOOM_33:  maps.curr->setZoom(zooms[7]); break;
						case ID_ZOOM_25:  maps.curr->setZoom(zooms[8]); break;
						case ID_ZOOM_10:  maps.curr->setZoom(zooms[9]); break;
						// Terrain
						case ID_TERRAIN_DISPLAYTILEELEVATIONS:
							maps.curr->ToggleDisplayElevations();
							maps.curr->Redraw(false);
							if ( hTilePal != nullptr )
								RedrawWindow(hTilePal, NULL, NULL, RDW_INVALIDATE);
							break;
						case ID_TERRAIN_DISPLAYTILEVALUES:
							maps.curr->ToggleTileNumSource(false);
							maps.curr->Redraw(false);
							if ( hTilePal != nullptr )
								RedrawWindow(hTilePal, NULL, NULL, RDW_INVALIDATE);
							break;
						case ID_TERRAIN_DISPLAYTILEVALUESMTXM:
							maps.curr->ToggleTileNumSource(true);
							maps.curr->Redraw(false);
							if ( hTilePal != nullptr )
								RedrawWindow(hTilePal, NULL, NULL, RDW_INVALIDATE);
							break;

					// Editor
							// Units
							case ID_UNITS_UNITSSNAPTOGRID:
								maps.curr->ToggleUnitSnap();
								break;
							case ID_UNITS_ALLOWSTACK:
								maps.curr->ToggleUnitStack();
								break;

							// Locations
							case ID_LOCATIONS_SNAPTOTILE:
								maps.curr->SetLocationSnap(SNAP_LOCATION_TO_TILE);
								break;
							case ID_LOCATIONS_SNAPTOACTIVEGRID:
								maps.curr->SetLocationSnap(SNAP_LOCATION_TO_GRID);
								break;
							case ID_LOCATIONS_NOSNAP:
								maps.curr->SetLocationSnap(NO_LOCATION_SNAP);
								break;
							case ID_LOCATIONS_LOCKANYWHERE:
								maps.curr->ToggleLockAnywhere();
								break;
							case ID_LOCATIONS_CLIPNAMES:
								maps.curr->ToggleLocationNameClip();
								break;

					// Scenario
							case ID_SCENARIO_DESCRIPTION: case ID_SCENARIO_FORCES: case ID_SCENARIO_UNITSETTINGS:
							case ID_SCENARIO_UPGRADESETTINGS: case ID_SCENARIO_TECHSETTINGS: case ID_SCENARIO_STRINGS:
							case ID_SCENARIO_SOUNDEDITOR:
								{
									if ( hMapSettings == nullptr )
										hMapSettings = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAPSETTINGS), hMain, MapSettingsProc);
									if ( hMapSettings != nullptr )
									{
										switch ( LOWORD(wParam) )
										{
											case ID_SCENARIO_DESCRIPTION: ChangeMapSettingsTab(hMapSettings, ID_TAB_MAPPROPERTIES); break;
											case ID_SCENARIO_FORCES: ChangeMapSettingsTab(hMapSettings, ID_TAB_FORCES); break;
											case ID_SCENARIO_UNITSETTINGS: ChangeMapSettingsTab(hMapSettings, ID_TAB_UNITSETTINGS); break;
											case ID_SCENARIO_UPGRADESETTINGS: ChangeMapSettingsTab(hMapSettings, ID_TAB_UPGRADESETTINGS); break;
											case ID_SCENARIO_TECHSETTINGS: ChangeMapSettingsTab(hMapSettings, ID_TAB_TECHSETTINGS); break;
											case ID_SCENARIO_STRINGS: ChangeMapSettingsTab(hMapSettings, ID_TAB_STRINGEDITOR); break;
											case ID_SCENARIO_SOUNDEDITOR: ChangeMapSettingsTab(hMapSettings, ID_TAB_WAVEDITOR); break;
										}
										ShowWindow(hMapSettings, SW_SHOW);
									}
								}
								break;

					// Tools
					case ID_TRIGGERS_TRIGGEREDITOR:
						if ( hTrigText = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TEXTTRIG), hMain, TextTrigProc) )
							ShowWindow(hTrigText, SW_SHOW);
						break;

					// Windows
					case ID_WINDOWS_CASCADE:
						SendMessage(hMaps, WM_MDICASCADE, 0, 0);
						break;
					case ID_WINDOWS_TILEHORIZONTALLY:
						SendMessage(hMaps, WM_MDITILE, MDITILE_HORIZONTAL, NULL);
						break;
					case ID_WINDOWS_TILEVERTICALLY:
						SendMessage(hMaps, WM_MDITILE, MDITILE_VERTICAL, NULL);
						break;
					case ID_WINDOW_CLOSE:
						SendMessage(hMaps, WM_MDIDESTROY, (WPARAM)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0), NULL);
						break;

					// Help
					case ID_HELP_STARCRAFT_WIKI:
						ShellExecute(NULL, "open", "http://www.staredit.net/starcraft/Main_Page", NULL, NULL, SW_SHOWNORMAL);
						break;
					case ID_HELP_SUPPORT_FORUM:
						ShellExecute(NULL, "open", "http://www.staredit.net/forums/", NULL, NULL, SW_SHOWNORMAL);
						break;
					case ID_HELP_CHKDRAFTTHREAD:
						ShellExecute(NULL, "open", "http://www.staredit.net/topic/15514/", NULL, NULL, SW_SHOWNORMAL);
						break;

					default:
						{
							switch ( HIWORD(wParam) )
							{
								case CBN_SETFOCUS:
									editFocused = true;
									break;
								case CBN_KILLFOCUS:
									editFocused = false;
									break;
								case CBN_EDITCHANGE:
									{
										u8 newPlayer;
										if ( GetPlayerNum((HWND)lParam, newPlayer) )
											maps.ChangePlayer(newPlayer);
									}
									break;
								case CBN_SELCHANGE:
									{
										int itemIndex = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
										switch ( LOWORD(wParam) )
										{
											case ID_COMBOBOX_LAYER: // Layer Changed
												maps.ChangeLayer(itemIndex);
												break;
											case ID_COMBOBOX_ZOOM: // Zoom Changed
												if ( itemIndex >= 0 && itemIndex < 10 )
													maps.curr->setZoom(zooms[itemIndex]);
												break;
											case ID_COMBOBOX_TERRAIN: // Terrain Placement Changed
												if ( itemIndex == 3 )
												{
													if ( hTilePal == nullptr )
														hTilePal = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INDEXED_TILESET), hMain, TerrainPaletteProc);
													ShowWindow(hTilePal, SW_SHOW);
												}
												break;
											case ID_COMBOBOX_PLAYER: // Player Changed
												maps.ChangePlayer(itemIndex);
												break;
										}
									}
									break;
								default:
									return DefFrameProc(hWnd, hMaps, msg, wParam, lParam);
									break;
							}
						}
						break;
				}
			}
			break;

		case WM_MENUCHAR:
			return MNC_CLOSE << 16;
			break;

		case WM_SIZE:
			{
				RECT rcMain, rcTool, rcStatus, rcLeftBar, rcMaps;
				
				//Autosize toolbar and status bar
					SendMessage(hTool, TB_AUTOSIZE, NULL, NULL);
					SendMessage(hStatus, WM_SIZE, 0, 0);

				//Get the size of the client area, toolbar, status bar, and left bar
					GetClientRect(hMain,   &rcMain);
					GetWindowRect(hTool,   &rcTool);
					GetWindowRect(hStatus, &rcStatus);
					GetWindowRect(hLeft,   &rcLeftBar);
					GetWindowRect(hMaps,   &rcMaps);

				int border = GetSystemMetrics(SM_CXSIZEFRAME)-1;
				//Fit plot to the area between the toolbar and statusbar
					SetWindowPos( hPlot,
								  NULL,
								  0,
								  rcTool.bottom-rcTool.top,
								  rcMain.right-rcMain.left,
								  rcMain.bottom-rcMain.top-rcTool.bottom+rcTool.top-rcStatus.bottom+rcStatus.top,
								  SWP_NOZORDER
								);
				//Fit left bar to the area between the toolbar and statusbar without changing width
					SetWindowPos( hLeft,
								  NULL,
								  -border,
								  -border,
								  rcLeftBar.right-rcLeftBar.left,
								  rcStatus.top-rcTool.bottom+border*2,
								  SWP_NOZORDER
								);
				//Fit the map MDI window to the area right of the left bar and between the toolbar and statusbar
					SetWindowPos( hMaps,
								  NULL,
								  rcLeftBar.right-rcLeftBar.left-border-2,
								  rcTool.bottom-rcTool.top,
								  rcMain.right-rcMain.left-rcLeftBar.right+rcLeftBar.left+border+2,
								  rcStatus.top-rcTool.bottom+2,
								  SWP_NOZORDER
								);

				RedrawWindow(hStatus, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_SIZING:
			SendMessage(hWnd, WM_SIZE, wParam, lParam);
			break;

		case WM_DROPFILES:
			char fileName[MAX_PATH];
			DragQueryFile((HDROP)wParam, 0, fileName, MAX_PATH);
			DragFinish((HDROP)wParam);
			maps.OpenMap(hMaps, fileName);
			maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
			break;

		case WM_CREATE:
			{
				hMainMenu = GetMenu(hWnd);

				DragAcceptFiles(hWnd, true);

				// Initilize primary toolbar
					hTool = CreateWindowEx(NULL, TOOLBARCLASSNAME, NULL,
							WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS,
							0, 0, 0, 0,
							hWnd, (HMENU)IDR_MAIN_TOOLBAR, GetModuleHandle(NULL), NULL);
					SendMessage(hTool, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
					SendMessage(hTool, TB_SETMAXTEXTROWS, 0, lParam);

				// Set toolbar to default button set
					TBADDBITMAP tbab;
					tbab.hInst = HINST_COMMCTRL;
					tbab.nID = IDB_STD_SMALL_COLOR;
					SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

				// Load Save-As image from resource and store the resulting index as STD_FILESAVEAS
					HIMAGELIST hImages = ImageList_Create(16, 16, NULL, 1, 1);
					HBITMAP hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAVEAS), IMAGE_BITMAP, 16, 16, LR_DEFAULTCOLOR);
					ImageList_Add(hImages, hBmp, NULL);
					TBADDBITMAP tb; tb.hInst = NULL; tb.nID = (UINT_PTR)hBmp;
					int STD_FILESAVEAS = SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tb);

				// Add buttons to toolbar
					TBBUTTON tbb[13];
					ZeroMemory(tbb, sizeof(TBBUTTON));
					tbb[ 0].iBitmap = STD_FILENEW;	  tbb[ 0].fsState = TBSTATE_ENABLED; tbb[ 0].fsStyle = TBSTYLE_BUTTON; tbb[ 0].idCommand = ID_FILE_NEW1;	   tbb[ 0].iString = (INT_PTR)"New";
					tbb[ 1].iBitmap = STD_FILEOPEN;   tbb[ 1].fsState = TBSTATE_ENABLED; tbb[ 1].fsStyle = TBSTYLE_BUTTON; tbb[ 1].idCommand = ID_FILE_OPEN1;	   tbb[ 1].iString = (INT_PTR)"Open";
					tbb[ 2].iBitmap = STD_FILESAVE;   tbb[ 2].fsState = NULL;			 tbb[ 2].fsStyle = TBSTYLE_BUTTON; tbb[ 2].idCommand = ID_FILE_SAVE1;	   tbb[ 2].iString = (INT_PTR)"Save";
					tbb[ 3].iBitmap = STD_FILESAVEAS; tbb[ 3].fsState = NULL;			 tbb[ 3].fsStyle = TBSTYLE_BUTTON; tbb[ 3].idCommand = ID_FILE_SAVEAS1;	   tbb[ 3].iString = (INT_PTR)"Save As";
					tbb[ 4].iBitmap = 8;			  tbb[ 4].fsState = NULL;			 tbb[ 4].fsStyle = BTNS_SEP;
					tbb[ 5].iBitmap = STD_UNDO;		  tbb[ 5].fsState = NULL;			 tbb[ 5].fsStyle = TBSTYLE_BUTTON; tbb[ 5].idCommand = ID_EDIT_UNDO1;	   tbb[ 5].iString = (INT_PTR)"Undo";
					tbb[ 6].iBitmap = STD_REDOW;	  tbb[ 6].fsState = NULL;			 tbb[ 6].fsStyle = TBSTYLE_BUTTON; tbb[ 6].idCommand = ID_EDIT_REDO1;	   tbb[ 6].iString = (INT_PTR)"Redo";
					tbb[ 7].iBitmap = STD_PROPERTIES; tbb[ 7].fsState = NULL;			 tbb[ 7].fsStyle = TBSTYLE_BUTTON; tbb[ 7].idCommand = ID_EDIT_PROPERTIES; tbb[ 7].iString = (INT_PTR)"Properties";
					tbb[ 8].iBitmap = STD_DELETE;	  tbb[ 8].fsState = NULL;			 tbb[ 8].fsStyle = TBSTYLE_BUTTON; tbb[ 8].idCommand = ID_EDIT_DELETE;	   tbb[ 8].iString = (INT_PTR)"Delete";
					tbb[ 9].iBitmap = 8;			  tbb[ 9].fsState = NULL;			 tbb[ 9].fsStyle = BTNS_SEP;
					tbb[10].iBitmap = STD_COPY;		  tbb[10].fsState = NULL;			 tbb[10].fsStyle = TBSTYLE_BUTTON; tbb[10].idCommand = ID_EDIT_COPY1;	   tbb[10].iString = (INT_PTR)"Copy";
					tbb[11].iBitmap = STD_CUT;		  tbb[11].fsState = NULL;			 tbb[11].fsStyle = TBSTYLE_BUTTON; tbb[11].idCommand = ID_EDIT_CUT1;	   tbb[11].iString = (INT_PTR)"Cut";
					tbb[12].iBitmap = STD_PASTE;	  tbb[12].fsState = NULL;			 tbb[12].fsStyle = TBSTYLE_BUTTON; tbb[12].idCommand = ID_EDIT_PASTE1;	   tbb[12].iString = (INT_PTR)"Paste";
					SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

				// Add layer ComboBox to toolbar
					hLayer = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_HASSTRINGS,
						277, 2, 110, 200, hTool, (HMENU)ID_COMBOBOX_LAYER, GetModuleHandle(NULL), NULL);
					// Add all the layers
						const char* layerStrings[] = { "Terrain", "Doodads", "Fog of War", "Locations", "Units", "Sprites", "Preview Fog",
													   "Copy\\Cut\\Paste\\Brush" };
						for ( int i=0; i<8; i++ )
							SendMessage(hLayer, CB_ADDSTRING, 0, (LPARAM)layerStrings[i]);
					SendMessage(hLayer, CB_SETCURSEL, 0, 0);
					SendMessage(hLayer, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
					ShowWindow(hLayer, false);

				// Add zoom ComboBox to toolbar
					hZoom = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_HASSTRINGS,
						395, 2, 80, 200,
						hTool, (HMENU)ID_COMBOBOX_ZOOM, GetModuleHandle(0), NULL);
					// Add the different magnifications
						const char* zoomStrings[] = { "400% Zoom", "300% Zoom", "200% Zoom", "150% Zoom", "100% Zoom", "66% Zoom",
													  "50% Zoom", "33% Zoom", "25% Zoom", "10% Zoom"};
						for ( int i=0; i<10; i++ )
							SendMessage(hZoom,CB_ADDSTRING,0,(LPARAM)zoomStrings[i]);	
					SendMessage(hZoom, CB_SETCURSEL, 4, 0);
					SendMessage(hZoom, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
					ShowWindow(hZoom, false);

				// Add player ComboBox to toolbar
					hPlayer = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWN | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_HASSTRINGS,
						483, 2, 75, 200,
						hTool, (HMENU)ID_COMBOBOX_PLAYER, GetModuleHandle(NULL), NULL);
					// Add players 1-12
						const char* playerStrings[] = { "Player 01", "Player 02", "Player 03", "Player 04", "Player 05", "Player 06",
														"Player 07", "Player 08", "Player 09", "Player 10", "Player 11", "Player 12 (Neutral)" };
						for ( int i=0; i<12; i++ )
							SendMessage(hPlayer, CB_ADDSTRING, 0, (LPARAM)playerStrings[i]);
					SendMessage(hPlayer, CB_SETCURSEL, 0, 0);
					PostMessage(hPlayer, CB_SETEDITSEL, 0, (-1, 0));
					SendMessage(hPlayer, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
					ShowWindow(hPlayer, false);

				// Add terrain ComboBox to toolbar

					hTerrainCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWN | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_HASSTRINGS,
						483, 2, 100, 200,
						hTool, (HMENU)ID_COMBOBOX_TERRAIN, GetModuleHandle(NULL), NULL);
					// Add terrain options
					const char* terrPalette[] = { "Isometrical", "Rectangular", "Subtile", "Tileset Indexed", "Copy/Cut/Paste" };
						for ( int i=0; i<5; i++ )
							SendMessage(hTerrainCombo, CB_ADDSTRING, 0, (LPARAM)terrPalette[i]);
					SendMessage(hTerrainCombo, CB_SETCURSEL, 0, 0);
					PostMessage(hTerrainCombo, CB_SETEDITSEL, 0, (-1, 0));
					SendMessage(hTerrainCombo, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
					ShowWindow(hTerrainCombo, SW_HIDE);

				// Create status bar
					int statwidths[] = {130, 205, 350, 450, 600, -1};
					hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
						WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
						0, 0, 0, 0,
						hWnd, (HMENU)IDR_MAIN_STATUS, GetModuleHandle(NULL), NULL);
					SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int),(LPARAM)statwidths);

				// Setup MDI Frame
					CLIENTCREATESTRUCT ccs;
					ccs.hWindowMenu  = GetSubMenu(GetMenu(hWnd), 6);
					ccs.idFirstChild = ID_MDI_FIRSTCHILD;

					hMaps = CreateWindow("MDICLIENT", (LPCTSTR)NULL,
						WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
						0, 0, 0, 0, hWnd, (HMENU)0xCAC, GetModuleHandle(NULL), (LPSTR)&ccs);

					ShowWindow(hMaps, SW_SHOW);

				// Setup the main plot (holds left bar)
					hPlot = CreateWindowEx(NULL, "Plot", NULL,
						WS_CHILD | WS_VISIBLE,
						0, 0, 162, 500,
						hWnd, (HMENU)IDR_MAIN_PLOT, GetModuleHandle(NULL), NULL);
			}
			break;

		case WM_SYSCOMMAND:
			{
				LRESULT result = DefWindowProc(hWnd, msg, wParam, lParam);
				switch ( wParam )
				{
					case SC_MINIMIZE:
						{
							ShowWindow(hUnit, SW_HIDE);
							ShowWindow(hLocation, SW_HIDE);
							ShowWindow(hTilePal, SW_HIDE);
							ShowWindow(hMapSettings, SW_HIDE);
						}
						break;
					case SC_RESTORE:
						{
							ShowWindow(hUnit, SW_SHOW);
							ShowWindow(hLocation, SW_SHOW);
							ShowWindow(hTilePal, SW_SHOW);
							ShowWindow(hMapSettings, SW_SHOW);
						}
						break;
				}
				return result;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			{
				if ( msg == WM_COPYDATA || (msg >= PLUGIN_MSG_START && msg <= PLUGIN_MSG_END) )
					return CallWindowProc(PluginProc, hWnd, msg, wParam, lParam);
				else
					return DefFrameProc(hWnd, hMaps, msg, wParam, lParam);
			}
			break;
	}
	return 0;
}

LRESULT CALLBACK PlotProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_CREATE:
			// Setup Left Bar (holds minimap and selection tree)
				hLeft = CreateWindowEx(WS_EX_CLIENTEDGE, "LeftBar", NULL,
					WS_CHILD | WS_VISIBLE | WS_THICKFRAME,
					0, 0, 162, 50,
					hWnd, (HMENU)IDR_LEFT_BAR, GetModuleHandle(NULL), NULL);

				ShowWindow(hLeft, SW_HIDE);
			break;

		case WM_ERASEBKGND:
			if ( maps.curr == nullptr )
				return DefWindowProc(hWnd, msg, wParam, lParam);
			else
				return true; // Prevent plot from drawing a background over maps
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

WNDPROC wpTree;
LRESULT CALLBACK TreeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_CHAR && (GetKeyState(VK_CONTROL) & 0x8000) )
		 return 0; // Prevent ctrl + key from causing beeps
	
	return CallWindowProc(wpTree, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK LeftBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_NOTIFY:
			if ( ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((LPNMTREEVIEW)lParam)->action != TVC_UNKNOWN )
			{
				if ( maps.curr != nullptr )
				{
					LPARAM itemType = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_TYPE,
						   itemData = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_DATA;
				
					switch ( itemType )
					{
						//case TREE_TYPE_ROOT: // Same as category
						case TREE_TYPE_CATEGORY: if ( maps.curr->currLayer() != itemData ) maps.ChangeLayer(u8(itemData)); break;
						case TREE_TYPE_ISOM: if ( maps.curr->currLayer() != LAYER_TERRAIN ) maps.ChangeLayer(LAYER_TERRAIN); break;
						case TREE_TYPE_UNIT: if ( maps.curr->currLayer() != LAYER_UNITS ) maps.ChangeLayer(LAYER_UNITS); break;
						case TREE_TYPE_LOCATION: if ( maps.curr->currLayer() != LAYER_LOCATIONS ) maps.ChangeLayer(LAYER_LOCATIONS); break;
						case TREE_TYPE_SPRITE: if ( maps.curr->currLayer() != LAYER_SPRITES ) maps.ChangeLayer(LAYER_SPRITES); break;
						case TREE_TYPE_DOODAD: if ( maps.curr->currLayer() != LAYER_DOODADS ) maps.ChangeLayer(LAYER_DOODADS); break;
					}

				
					switch ( itemType )
					{
						case TREE_TYPE_UNIT: // itemData == UnitID
							{
								maps.curr->selections().removeUnits();
								maps.endPaste();
								if ( maps.curr->currLayer() != LAYER_UNITS )
									maps.ChangeLayer(LAYER_UNITS);

								ChkUnit unit = { };
								unit.energy = 100;
								unit.hanger = 0;
								unit.hitpoints = 100;
								unit.id = u16(itemData);
								unit.link = 0;
								unit.linkType = 0;
								unit.owner = maps.curr->currPlayer();
								unit.resources = 0;
								unit.serial = 0;
								unit.shields = 100;
								unit.special = 0;
								unit.stateFlags = 0;
								unit.unused = 0;
								unit.validFlags = 0;
								unit.xc = 0;
								unit.yc = 0;
								maps.clipboard.addQuickUnit(&unit);
								maps.startPaste(true);
							}
							break;

						case TREE_TYPE_LOCATION: // itemData = location index
							maps.curr->selections().selectLocation(u16(itemData));
							if ( hLocation != nullptr )
								SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);
							maps.curr->viewLocation(u16(itemData));
							break;
					}
				}
			}
			else if ( ((NMHDR*)lParam)->code == NM_DBLCLK )
			{
				TVITEM item = { };
				item.mask = TVIF_PARAM;
				item.hItem = TreeView_GetSelection(((NMHDR*)lParam)->hwndFrom);
				if ( item.hItem != NULL && TreeView_GetItem(((NMHDR*)lParam)->hwndFrom, &item) == TRUE )
				{
					LPARAM itemType = item.lParam&TREE_ITEM_TYPE;
					if ( itemType == TREE_TYPE_LOCATION )
					{
						if ( hLocation == nullptr )
							hLocation = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOCPROP), hMain, LocationPropProc);
						else
							SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);

						ShowWindow(hLocation, SW_SHOW);
					}
				}
			}
			break;

		case WM_SIZE:
			{
				HWND hTree	  = GetDlgItem(hWnd , IDR_MAIN_TREE	 ), 
					 hMiniMap = GetDlgItem(hWnd , IDR_MINIMAP	 );
				RECT rcMain, rcTool, rcStatus, rcLeftBar;

				//Get the size of the client area, toolbar, status bar, and left bar
					GetClientRect(hMain	 , &rcMain	 );
					GetWindowRect(hTool	 , &rcTool	 );
					GetWindowRect(hStatus, &rcStatus );
					GetWindowRect(hLeft	 , &rcLeftBar);

				int border = GetSystemMetrics(SM_CXSIZEFRAME)-1;
				// Interrupt any attempt to vertically resize the left bar
					SetWindowPos( hLeft, NULL,
								  -border, -border,
								  rcLeftBar.right-rcLeftBar.left,
								  rcStatus.top-rcTool.bottom+border*2, 
								  SWP_NOZORDER
								);
				// Fit the map MDI window to the area right of the left bar and between the toolbar and statusbar
					SetWindowPos( hMaps, NULL,
								  rcLeftBar.right-rcLeftBar.left-border-2,
								  rcTool.bottom-rcTool.top,
								  rcMain.right-rcMain.left-rcLeftBar.right+rcLeftBar.left+border+2,
								  rcStatus.top-rcTool.bottom+2,
								  SWP_NOZORDER);
				// Fit the minimap to the center of the top part of the left bar
					SetWindowPos(hMiniMap, NULL, (rcLeftBar.right-rcLeftBar.left-(132+4+border*2))/2, 3, 132, 132, SWP_NOZORDER);
				//Fit the tree to the bottom part of the left bar
					GetClientRect(hLeft, &rcLeftBar);
					SetWindowPos(hTree, NULL, -2, 145, rcLeftBar.right-rcLeftBar.left+2, rcLeftBar.bottom-rcLeftBar.top-146/*rcStatus.top-rcTool.bottom-146*/, SWP_NOZORDER);
			}
			break;

		case WM_SIZING:
			SendMessage(hWnd, WM_SIZE, wParam, lParam);
			break;

		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO minmax = (LPMINMAXINFO)lParam;
							 minmax -> ptMinTrackSize.x = 151;
			}
			break;

		case WM_CREATE:
			{
				hMini = CreateWindowEx(WS_EX_CLIENTEDGE, "MiniMap", NULL,
					WS_VISIBLE | WS_CHILD,
					6, 3, 132, 132,
					hWnd, (HMENU)IDR_MINIMAP, GetModuleHandle(NULL), NULL);

				if ( !hMini )
					MessageBox(hWnd, "Failed to create minimap", "Message", MB_OK);

				hWndTV = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, "Tree View",
					WS_VISIBLE | WS_CHILD | TVS_NONEVENHEIGHT | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, 
					-2, 145, 162, 150,
					hWnd, (HMENU)IDR_MAIN_TREE, GetModuleHandle(NULL), NULL);

				if ( !hWndTV )
					Error("Failed to create TreeView");
				else
				{
					wpTree = (WNDPROC)SetWindowLong(hWndTV, GWL_WNDPROC, (LONG)&TreeProc);
					SendMessage(hWndTV, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
					scData.UpdateDisplayNames(DefaultUnitDisplayName);
					BuildMainTree(hWndTV);
				}
			}
			break;

		default:
			return DefFrameProc(hWnd, hMaps, msg, wParam, lParam);
			break;
	}
	return 0;
}

void MiniMapClick(LPARAM ClickPoints)
{
	if ( maps.curr != nullptr )
	{
		POINTS MiniClick = MAKEPOINTS(ClickPoints);

		RECT rect;
		GetClientRect(maps.curr->getHandle(), &rect);

		u16 xSize = maps.curr->XSize(),
			ySize = maps.curr->YSize();

		if ( xSize == 0 || ySize == 0 )
			return;

		HWND hMap = maps.curr->getHandle();
		RECT rcMap = { };
		GetClientRect(hMap, &rcMap);

		int screenWidth = rcMap.right-rcMap.left,
			screenHeight = rcMap.bottom-rcMap.top;

		float scale = 1.0f;

		if ( xSize >= ySize )
			if ( 128.0/xSize >= 1 )
				scale = (float)(128/xSize);
			else
				scale = 128.0f/xSize;
		else
			if ( 128.0/ySize >= 1 )
				scale = (float)(128/ySize);
			else
				scale = 128.0f/ySize;

		u16 xOffset = (u16)((128-xSize*scale)/2),
			yOffset = (u16)((128-ySize*scale)/2);

		maps.curr->display().x = (s32)((MiniClick.x-xOffset)*(32/scale)-screenWidth/2);
		maps.curr->display().y = (s32)((MiniClick.y-yOffset)*(32/scale)-screenHeight/2);

		maps.curr->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);

		RedrawWindow(maps.curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
	}
}

LRESULT CALLBACK MiniMapProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_LBUTTONDOWN:
			MiniMapClick(lParam);
			LockCursor(hWnd);
			break;

		case WM_LBUTTONUP:
			ClipCursor(NULL);
			break;

		case WM_MOUSEMOVE:
			if ( wParam == MK_LBUTTON )
				MiniMapClick(lParam);
			break;

		case WM_PAINT:
			{
				if ( maps.curr )
					maps.curr->PaintMiniMap(hWnd);
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void TrackMouse(HWND hWnd, DWORD hoverTime)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = hWnd;
	tme.dwHoverTime = hoverTime;
	TrackMouseEvent(&tme);
}

void EdgeDrag(HWND hWnd, LPARAM lParam, u8 layer, SELECTIONS& sel, GuiMap* map)
{
	if ( map->isDragging() )
	{
		POINTS currPoint = MAKEPOINTS(lParam);
		if ( currPoint.x < 0 )
			currPoint.x = 0;
		if ( currPoint.y < 0 )
			currPoint.y = 0;

		RECT rcMap;
		GetClientRect(hWnd, &rcMap);
		TrackMouse(hWnd, DEFAULT_HOVER_TIME);
		if ( currPoint.x == 0 ) // Cursor on the left
		{
			if ( (map->display().x+16)/32 > 0 )
				sel.setEndDrag( ((map->display().x+16)/32-1)*32, sel.getEndDrag().y );
			if ( map->display().x > 0 )
				map->display().x = sel.getEndDrag().x;
		}
		else if ( currPoint.x >= rcMap.right-2 ) // Cursor on the right
		{
			if ( (map->display().x+rcMap.right)/32 < map->XSize() )
				sel.setEndDrag( ((map->display().x+rcMap.right)/32+1)*32, sel.getEndDrag().y );
			map->display().x = sel.getEndDrag().x - rcMap.right;
		}
		if ( currPoint.y == 0 ) // Cursor on the top
		{
			if ( (map->display().y+16)/32 > 0 )
				sel.setEndDrag( sel.getEndDrag().x, ((map->display().y+16)/32-1)*32 );
			if ( map->display().y > 0 )
				map->display().y = sel.getEndDrag().y;
		}
		else if ( currPoint.y >= rcMap.bottom-2 ) // Cursor on the bottom
		{
			if ( (map->display().y+rcMap.bottom)/32 < map->YSize() )
				sel.setEndDrag( sel.getEndDrag().x, ((map->display().y+rcMap.bottom)/32+1)*32 );
			map->display().y = sel.getEndDrag().y - rcMap.bottom;
		}
		map->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
		map->Redraw(false);
	}
}

LRESULT CALLBACK MDIChildProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( maps.curr != nullptr )
	{
		GuiMap* map = maps.curr;

		switch ( msg )
		{
			case WM_PAINT:
				{
					// Paint the map given by hWnd regardless of whether it's the current map
					maps.GetMap(hWnd)->PaintMap( map,
												 maps.clipboard.isPasting(),
												 /*maps.cursor,*/
												 maps.clipboard
											   );
				}
				break;
	
			case WM_HSCROLL:
				{
					switch( LOWORD(wParam) )
					{
						case SB_LINELEFT:
							map->display().x -= 8;
							break;
						case SB_LINERIGHT:
							map->display().x += 8;
							break;
						case SB_PAGELEFT:
							{
								RECT rect;
								GetClientRect(hWnd, &rect);
								map->display().x -= (rect.right-rect.left)/2;
							}
							break;
						case SB_PAGERIGHT:
							{
								RECT rect;
								GetClientRect(hWnd, &rect);
								map->display().x += (rect.right-rect.left)/2;
							}
							break;
						case SB_THUMBPOSITION:
							map->display().x = HIWORD(wParam);
							break;
						case SB_THUMBTRACK:
							map->display().x = HIWORD(wParam);
							break;
						default:
							return DefMDIChildProc(hWnd, msg, wParam, lParam);
							break;
					}
					map->Scroll(SCROLL_X|VALIDATE_BORDER);
					map->Redraw(true);
				}
				break;
	
			case WM_VSCROLL:
				{
					switch( LOWORD(wParam) )
					{
						case SB_LINEUP:
							map->display().y -= 8;
							break;
						case SB_LINEDOWN:
							map->display().y += 8;
							break;
						case SB_PAGEUP:
							{
								RECT rect;
								GetClientRect(hWnd, &rect);
								map->display().y -= (rect.right-rect.left)/2;
							}
							break;
						case SB_PAGEDOWN:
							{
								RECT rect;
								GetClientRect(hWnd, &rect);
								map->display().y += (rect.right-rect.left)/2;
							}
							break;
						case SB_THUMBPOSITION:
							map->display().y = HIWORD(wParam);
							break;
						case SB_THUMBTRACK:
							map->display().y = HIWORD(wParam);
							break;
						default:
							return DefMDIChildProc(hWnd, msg, wParam, lParam);
							break;
					}
					map->Scroll(SCROLL_Y|VALIDATE_BORDER);
					map->Redraw(true);
				}
				break;
	
			case WM_SIZE:
				{
					if ( maps.GetMap(hWnd) != nullptr )
					{
						LRESULT result = DefMDIChildProc(hWnd, msg, wParam, lParam);
						map->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
						map->Redraw(false);
						return result;
					}
					else
						return DefMDIChildProc(hWnd, msg, wParam, lParam);
				}
				break;
	
			case WM_ERASEBKGND:
				break;
	
			case WM_MDIACTIVATE:
				{
					if ( hTerrainProp != nullptr )
						EndDialog(hTerrainProp, NULL);
	
					if ( hUnit != nullptr )
						EndDialog(hUnit, NULL);
	
					if ( hLocation != nullptr )
					{
						locProcLocIndex = NO_LOCATION;
						EndDialog(hLocation, NULL);
					}

					if ( hMapSettings != nullptr )
						EndDialog(hMapSettings, NULL);
	
					if ( hTilePal != nullptr )
						EndDialog(hTilePal, NULL);
	
					if ((HWND)lParam != NULL )
					{
						maps.Focus((HWND)lParam);
						maps.curr->Redraw(true);
						maps.UpdateTreeView();
					}
				}
				break;
	
			case WM_RBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_MOUSEMOVE:
			case WM_MOUSEHOVER:
			case WM_LBUTTONUP:
				return MapMouseProc(hWnd, msg, wParam, lParam, map);
				break;

			case WM_COMMAND:
				return DefMDIChildProc(hWnd, msg, wParam, lParam);
				break;

			case WM_DESTROY:
				maps.CloseMap(hWnd);
				maps.Focus((HWND)SendMessage(hMaps, WM_MDIGETACTIVE, 0, 0));
				RedrawWindow(hMini, NULL, NULL, RDW_INVALIDATE);
				SetFocus(hMain);
				return DefMDIChildProc(hWnd, msg, wParam, lParam);
				break;

			default:
				return DefMDIChildProc(hWnd, msg, wParam, lParam);
				break;
		}
	}
	else // maps.curr == nullptr
		return DefMDIChildProc(hWnd, msg, wParam, lParam);

	return 0;
}

LRESULT CALLBACK MapMouseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, GuiMap* map)
{
	u8 layer = map->currLayer();
	SELECTIONS& sel = map->selections();
	POINT& startDrag = sel.getStartDrag(),
		 & endDrag	 = sel.getEndDrag();

	switch ( msg )
	{
		case WM_RBUTTONUP:
			maps.endPaste();
			ClipCursor(NULL);
			map->Redraw(true);
			break;
	
		case WM_LBUTTONDBLCLK:
			if ( layer == LAYER_LOCATIONS )
				map->doubleClickLocation(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
	
		case WM_LBUTTONDOWN:
			{
				if ( editFocused )
					SetFocus(hMain);
	
				sel.resetMoved();
				POINTS click = MAKEPOINTS(lParam);
				s32 mapClickX = (s32(((double)click.x)/map->getZoom()) + map->display().x),
					mapClickY = (s32(((double)click.y)/map->getZoom()) + map->display().y);

				switch ( wParam )
				{
					case MK_SHIFT|MK_LBUTTON: // Shift + LClick
						if ( layer == LAYER_TERRAIN )
							map->openTileProperties(mapClickX, mapClickY);
						break;
	
					case MK_CONTROL|MK_LBUTTON: // Ctrl + LClick
						{
							if ( hTerrainProp )
								EndDialog(hTerrainProp, NULL);
							if ( !maps.clipboard.isPasting() )
							{
								if ( layer == LAYER_TERRAIN ) // Ctrl + Click tile
									sel.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
								else if ( layer == LAYER_DOODADS || layer == LAYER_UNITS || layer == LAYER_SPRITES )
									sel.setDrags(mapClickX, mapClickY);

								LockCursor(hWnd);
								TrackMouse(hWnd, DEFAULT_HOVER_TIME);
								map->setDragging(true);
							}
						}
						break;
	
					case MK_LBUTTON: // LClick
						{
							if ( hTerrainProp )
								EndDialog(hTerrainProp, NULL);
							if ( maps.clipboard.isPasting() )
								map->paste(mapClickX, mapClickY, maps.clipboard);
							else
							{
								if ( sel.hasTiles() )
									sel.removeTiles();
								
								sel.setDrags(mapClickX, mapClickY);
								if ( layer == LAYER_TERRAIN )
									sel.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
								else if ( layer == LAYER_LOCATIONS )
								{
									s32 x1 = mapClickX, y1 = mapClickY;
									if ( map->SnapLocationDimensions(x1, y1, x1, y1, SNAP_LOC_X1|SNAP_LOC_Y1) )
										sel.setDrags(x1, y1);
									sel.setLocationFlags(map->getLocSelFlags(mapClickX, mapClickY));
								}

								SetCapture(hWnd);
								TrackMouse(hWnd, DEFAULT_HOVER_TIME);
								map->setDragging(true);
								map->Redraw(false);
							}
						}
						break;
				}
			}
			break;
	
		case WM_MOUSEMOVE:
			{
				POINTS click = MAKEPOINTS(lParam);
				if ( click.x < 0 ) click.x = 0;
				if ( click.y < 0 ) click.y = 0;

				s32 mapHoverX = (s32(((double)click.x)/map->getZoom())) + map->display().x,
					mapHoverY = (s32(((double)click.y)/map->getZoom())) + map->display().y;

				if ( wParam & MK_LBUTTON ) // If click and dragging
				{
					maps.stickCursor(); // Stop cursor from reverting
					sel.setMoved();
				}
				else // If not click and dragging
					maps.updateCursor(mapHoverX, mapHoverY); // Determine proper hover cursor

				// Set status bar tracking pos
				char newPos[64];
				sprintf_s(newPos, 64, "%i, %i (%i, %i)", mapHoverX, mapHoverY, mapHoverX/32, mapHoverY/32);
				SendMessage(hStatus, SB_SETTEXT, 0, (INT_PTR)newPos);
	
				switch ( wParam )
				{
					case MK_CONTROL|MK_LBUTTON:
						{
							RECT rcMap;
							GetClientRect(hWnd, &rcMap);

							if ( click.x == 0 || click.y == 0 || click.x == rcMap.right-2 || click.y == rcMap.bottom-2 )
								EdgeDrag(hWnd, lParam, layer, sel, map);

							sel.setEndDrag(mapHoverX, mapHoverY);
							if ( layer == LAYER_TERRAIN )
								sel.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
							else if ( layer == LAYER_LOCATIONS )
							{
								s32 x2 = mapHoverX, y2 = mapHoverY;
								if ( map->SnapLocationDimensions(x2, y2, x2, y2, SNAP_LOC_X2|SNAP_LOC_Y2) )
									sel.setEndDrag(x2, y2);
							}
							else if ( layer == LAYER_UNITS )
							{
								s32 xc = mapHoverX, yc = mapHoverY;
								if ( map->snapUnitCoordinate(xc, yc) )
									sel.setEndDrag(xc, yc);
							}

							map->PaintMap(map, maps.clipboard.isPasting(), maps.clipboard);
						}
						break;

					case MK_LBUTTON:
						{
							// If pasting, move paste
							if ( maps.clipboard.isPasting() )
							{
								s32 xc = mapHoverX, yc = mapHoverY;
								if ( layer == LAYER_UNITS )
									map->snapUnitCoordinate(xc, yc);

								sel.setEndDrag(xc, yc);
								if ( !maps.clipboard.isPreviousPasteLoc(u16(xc), u16(yc)) )
									map->paste((s16)xc, (s16)yc, maps.clipboard);
							}

							if ( map->isDragging() )
							{
								RECT rcMap;
								GetClientRect(hWnd, &rcMap);
								if ( click.x == 0 || click.y == 0 || click.x >= rcMap.right-2 || click.y >= rcMap.bottom-2 )
									EdgeDrag(hWnd, lParam, layer, sel, map);

								sel.setEndDrag( mapHoverX, mapHoverY );
								if ( layer == LAYER_TERRAIN )
									sel.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
								else if ( layer == LAYER_LOCATIONS )
								{
									s32 x2 = mapHoverX, y2 = mapHoverY;
									if ( map->SnapLocationDimensions(x2, y2, x2, y2, SNAP_LOC_X2|SNAP_LOC_Y2) )
										sel.setEndDrag(x2, y2);
								}
								else if ( layer == LAYER_UNITS )
								{
									s32 xc = mapHoverX, yc = mapHoverY;
									if ( map->snapUnitCoordinate(xc, yc) )
										sel.setEndDrag(xc, yc);
								}
							}
							map->PaintMap(map, maps.clipboard.isPasting(), maps.clipboard);
						}
						break;

					default:
						{
							if ( maps.clipboard.isPasting() == true )
							{
								if ( GetKeyState(VK_SPACE) & 0x8000 )
								{
									RECT rcMap;
									GetClientRect(hWnd, &rcMap);
	
									if ( click.x == 0 || click.x == rcMap.right-2 || click.y == 0 || click.y == rcMap.bottom-2 )
									{
										if		( click.x == 0 )
											map->display().x -= 32;
										else if ( click.x == rcMap.right-2 )
											map->display().x += 32;
										if		( click.y == 0 )
											map->display().y -= 32;
										else if ( click.y == rcMap.bottom-2 )
											map->display().y += 32;
	
										map->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
									}
								}

								if ( layer == LAYER_UNITS )
									map->snapUnitCoordinate(mapHoverX, mapHoverY);

								sel.setEndDrag(mapHoverX, mapHoverY);
								map->PaintMap(map, maps.clipboard.isPasting(), maps.clipboard);
							}
						}
						break;
				}
			}
			break;
	
		case WM_MOUSEHOVER:
			{
				switch ( wParam )
				{
					case MK_CONTROL|MK_LBUTTON:
					case MK_LBUTTON:
						EdgeDrag(hWnd, lParam, layer, sel, map);
						break;
	
					default:
						{
							if ( maps.clipboard.isPasting() == true )
							{
								RECT rcMap;
								GetClientRect(hWnd, &rcMap);
								POINTS currPoint = MAKEPOINTS(lParam);
	
								if		( currPoint.x == 0				)
									map->display().x -= 8;
								else if ( currPoint.x >= rcMap.right-2	)
									map->display().x += 8;
								if		( currPoint.y == 0				)
									map->display().y -= 8;
								else if ( currPoint.y >= rcMap.bottom-2 )
									map->display().y += 8;
	
								map->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);
								RedrawWindow(map->getHandle(), NULL, NULL, RDW_INVALIDATE);

								s32 x = (s32(((double)currPoint.x)/map->getZoom())) + map->display().x,
									y = (s32(((double)currPoint.y)/map->getZoom())) + map->display().y;
								sel.setEndDrag(x, y);

								TrackMouse(hWnd, 100);
							}
						}
						break;
				}
			}
			break;

		case WM_LBUTTONUP:
			{
				ReleaseCapture();
				if ( map->isDragging() )
				{
					map->setDragging(false);
					POINTS endPoint = MAKEPOINTS(lParam);
					if ( endPoint.x < 0 )
						endPoint.x = 0;
					if ( endPoint.y < 0 )
						endPoint.y = 0;
					endPoint.x = s16(endPoint.x/map->getZoom() + map->display().x);
					endPoint.y = s16(endPoint.y/map->getZoom() + map->display().y);

					if ( maps.clipboard.isPasting() )
						map->paste((s16)endPoint.x, (s16)endPoint.y, maps.clipboard);
	
					if ( layer == LAYER_TERRAIN )
					{
						sel.setEndDrag( (endPoint.x+16)/32, (endPoint.y+16)/32 );
						sel.setStartDrag( sel.getStartDrag().x/32, sel.getStartDrag().y/32 );
						u16 width = map->XSize();
					
						if ( wParam == MK_CONTROL && sel.startEqualsEndDrag() )
						// Add/remove single tile to/front existing selection
						{
							sel.setEndDrag( (endPoint.x)/32, (endPoint.y)/32 );
							
							u16 tileValue;
							if ( map->MTXM().get<u16>(
														(u16 &)tileValue,
														(u32)((sel.getEndDrag().y*width+sel.getEndDrag().x)*2)
													 )
							   )
								sel.addTile(tileValue,
											(u16)sel.getEndDrag().x,
											(u16)sel.getEndDrag().y
										   );
						}
						else
						// Add/remove multiple tiles from selection
						{
							sel.sortDragPoints();
	
							if ( sel.getStartDrag().y < sel.getEndDrag().y &&
								 sel.getStartDrag().x < sel.getEndDrag().x )
							{
								bool multiAdd = sel.getStartDrag().x + 1 < sel.getEndDrag().x ||
												sel.getStartDrag().y + 1 < sel.getEndDrag().y;
	
								if ( sel.getEndDrag().x > map->XSize() )
									sel.setEndDrag(map->XSize(), sel.getEndDrag().y);
								if ( sel.getEndDrag().y > map->YSize() )
									sel.setEndDrag(sel.getEndDrag().x, map->YSize());
	
								for ( int yRow = sel.getStartDrag().y; yRow < sel.getEndDrag().y; yRow++ )
								{
									for ( int xRow = sel.getStartDrag().x; xRow < sel.getEndDrag().x; xRow++ )
									{
										u16 tileValue;
										if ( map->MTXM().get<u16>((u16 &)tileValue, (u32)((yRow*width+xRow)*2)) )
											sel.addTile(tileValue, xRow, yRow);
									}
								}
							}
						}
					}
					else if ( layer == LAYER_LOCATIONS )
					{	
						if ( sel.hasMoved() ) // attempt to move, resize, or create location
						{
							s32 startX = sel.getStartDrag().x,
								startY = sel.getStartDrag().y,
								endX = endPoint.x,//sel.getEndDrag().x,
								endY = endPoint.y;//sel.getEndDrag().y;
							s32 dragX = endX-startX;
							s32 dragY = endY-startY;

							if ( sel.getLocationFlags() == LOC_SEL_NOTHING ) // Create location
							{
								AscendingOrder(startX, endX);
								AscendingOrder(startY, endY);
								map->SnapLocationDimensions(startX, startY, endX, endY, SNAP_LOC_ALL);
									
								u16 locationIndex;
								if ( map->createLocation(startX, startY, endX, endY, locationIndex) )
								{
									map->undos().addUndoLocationCreate(locationIndex);
									map->undos().startNext(0);
									BuildLocationTree(map);
									map->Redraw(false);
								}
							}
							else // Move or resize location
							{
								u16 selectedLocation = sel.getSelectedLocation();
								ChkLocation* loc;
								if ( selectedLocation != NO_LOCATION && map->getLocation(loc, selectedLocation) )
								{
									u8 selFlags = sel.getLocationFlags();
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
											if ( map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2) )
											{
												loc->xc1 += loc->xc2 - xc2Preserve; // Maintain location width
												dragX += loc->xc1 - xc1Preserve;
											}
										}
										else if ( map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1) )
										{
											loc->xc2 += loc->xc1 - xc1Preserve; // Maintain location width
											dragX += loc->xc2 - xc2Preserve;
										}

										if ( yInverted )
										{
											if ( map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2) )
											{
												loc->yc1 += loc->yc2 - yc2Preserve; // Maintain location height
												dragY += loc->yc1 - yc1Preserve;
											}
										}
										else if ( map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1) )
										{
											loc->yc2 += loc->yc1 - yc1Preserve; // Maintain location height
											dragY += loc->yc2 - yc2Preserve;
										}
										
										map->undos().addUndoLocationMove(selectedLocation, dragX, dragY);
									}
									else // Resize location
									{
										if ( selFlags & LOC_SEL_NORTH )
										{
											if ( loc->yc1 <= loc->yc2 ) // Standard yc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_YC1, loc->yc1);
												loc->yc1 += dragY;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1);
											}
											else // Inverted yc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_YC2, loc->yc2);
												loc->yc2 += dragY;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2);
											}
											
										}
										else if ( selFlags & LOC_SEL_SOUTH )
										{
											if ( loc->yc1 <= loc->yc2 ) // Standard yc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_YC2, loc->yc2);
												loc->yc2 += dragY;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y2);
											}
											else // Inverted yc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_YC1, loc->yc1);
												loc->yc1 += dragY;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_Y1);
											}
										}
	
										if ( selFlags & LOC_SEL_WEST )
										{
											if ( loc->xc1 <= loc->xc2 ) // Standard xc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_XC1, loc->xc1);
												loc->xc1 += dragX;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1);
											}
											else // Inverted xc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_XC2, loc->xc2);
												loc->xc2 += dragX;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2);
											}
										}
										else if ( selFlags & LOC_SEL_EAST )
										{
											if ( loc->xc1 <= loc->xc2 ) // Standard xc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_XC2, loc->xc2);
												loc->xc2 += dragX;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X2);
											}
											else // Inverted xc
											{
												map->undos().addUndoLocationChange(selectedLocation, LOC_FIELD_XC1, loc->xc1);
												loc->xc1 += dragX;
												map->SnapLocationDimensions(loc->xc1, loc->yc1, loc->xc2, loc->yc2, SNAP_LOC_X1);
											}
										}
									}
									map->undos().startNext(0);
									map->Redraw(false);
									if ( hLocation != nullptr )
										SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);
								}
							}
						}
						else // attempt to select location, if you aren't resizing
						{
							sel.selectLocation(sel.getStartDrag().x, sel.getStartDrag().y, map, !map->LockAnywhere());
							if ( hLocation != nullptr )
								SendMessage(hLocation, REFRESH_LOCATION, NULL, NULL);
	
							map->Redraw(false);
						}
						sel.setLocationFlags(LOC_SEL_NOTHING);
					}
					else if ( ( layer == LAYER_DOODADS ||
								layer == LAYER_UNITS ||
								layer == LAYER_SPRITES ) )
					{
						sel.setEndDrag(endPoint.x, endPoint.y);
						sel.sortDragPoints();
	
						if ( layer == LAYER_UNITS )
						{
							if ( wParam != MK_CONTROL )
								// Remove selected units
							{
								if ( hUnit != nullptr )
								{
									changeHighlightOnly = true;
									UnitNode* curr = sel.getFirstUnit();
									HWND hUnitTree = GetDlgItem(hUnit, IDC_UNITLIST);
									while ( curr != nullptr )
									{
										LVFINDINFO findInfo = { };
										findInfo.flags = LVFI_PARAM;
										findInfo.lParam = curr->index;
												
										int lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
										ListView_SetItemState(hUnitTree, lvIndex, 0, LVIS_FOCUSED|LVIS_SELECTED);
										curr = curr->next;
									}
									changeHighlightOnly = false;
								}
								sel.removeUnits();
								SendMessage(hUnit, UPDATE_ENABLED_STATE, NULL, NULL);
							}
		
							u16 numUnits = map->numUnits();
							for ( int i=0; i<numUnits; i++ )
							{
								int unitLeft = 0, unitRight	 = 0,
									unitTop	 = 0, unitBottom = 0;
	
								ChkUnit* unit;
								if ( map->getUnit(unit, i) )
								{
									if ( unit->id < 228 )
									{
										unitLeft   = unit->xc - scData.units.UnitDat(unit->id)->UnitSizeLeft;
										unitRight  = unit->xc + scData.units.UnitDat(unit->id)->UnitSizeRight;
										unitTop	   = unit->yc - scData.units.UnitDat(unit->id)->UnitSizeUp;
										unitBottom = unit->yc + scData.units.UnitDat(unit->id)->UnitSizeDown;
									}
									else
									{
										unitLeft   = unit->xc - scData.units.UnitDat(0)->UnitSizeLeft;
										unitRight  = unit->xc + scData.units.UnitDat(0)->UnitSizeRight;
										unitTop	   = unit->yc - scData.units.UnitDat(0)->UnitSizeUp;
										unitBottom = unit->yc + scData.units.UnitDat(0)->UnitSizeDown;
									}
	
									if (	sel.getStartDrag().x <= unitRight  && sel.getEndDrag().x >= unitLeft
										 && sel.getStartDrag().y <= unitBottom && sel.getEndDrag().y >= unitTop )
									{
										bool wasSelected = sel.unitIsSelected(i);
										if ( wasSelected )
											sel.removeUnit(i);
										else
											sel.addUnit(i);
		
										if ( hUnit )
										{
											HWND hUnitTree = GetDlgItem(hUnit, IDC_UNITLIST);
											LVFINDINFO findInfo = { };
											findInfo.flags = LVFI_PARAM;
											findInfo.lParam = i;
		
											int lvIndex = ListView_FindItem(hUnitTree, -1, &findInfo);
											if ( wasSelected )
											{
												changeHighlightOnly = true;
												ListView_SetItemState(hUnitTree, lvIndex, 0, LVIS_FOCUSED|LVIS_SELECTED);
												changeHighlightOnly = false;
											}
											else
											{
												changeHighlightOnly = true;
												ListView_SetItemState(hUnitTree, lvIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
												changeHighlightOnly = false;
											}
										}
										SendMessage(hUnit, UPDATE_ENABLED_STATE, NULL, NULL);
									}
								}
							}
							map->Redraw(true);
						}
					}
					sel.setStartDrag(-1, -1);
					sel.setEndDrag(-1, -1);
					map->setDragging(false);
		
					RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
				}
				if ( !maps.clipboard.isPasting() )
					ClipCursor(NULL);
				else
					map->nextUndo();
			}
			break;
	}

	return 0;
}