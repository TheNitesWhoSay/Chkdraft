#include "Terrain.h"
#include "Common Files/CommonFiles.h"
#include "Data.h"
#include "Graphics.h"
#include "Maps.h"
#include "GuiAccel.h"
#include <cstdio>
#include <iostream>
using namespace std;

extern Tiles tiles;
extern MAPS maps;
extern HWND hTilePal;
extern HWND hMain;

BOOL CALLBACK SetTileProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_TILEVALUE);
				SendMessage(hEdit, EM_SETLIMITTEXT, 10, 0);
				SendMessage(hWnd, WM_COMMAND, TILE_UPDATE, NULL);
				PostMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)hEdit, true);
				return true;
			}
			break;

		case WM_COMMAND:
			{
				switch ( HIWORD(wParam) )
				{
					case EN_UPDATE:
						RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
						break;
				}
				switch ( LOWORD(wParam) )
				{
					case IDOK:
						{
							TCHAR lpszTile[11];
							int TextLength = (WORD) SendDlgItemMessage(hWnd, IDC_EDIT_TILEVALUE, EM_LINELENGTH, 0, 0);
							*((LPWORD)lpszTile) = TextLength;
							SendDlgItemMessage(hWnd, IDC_EDIT_TILEVALUE, EM_GETLINE, 0, (LPARAM)lpszTile);	
							lpszTile[TextLength] = NULL;
	
							u16 tile = atoi(lpszTile);
							if ( tile > 65535 )
								tile %= 65536;
							
							TileNode* headTile = maps.curr->selections().getFirstTile();
							headTile->value = tile;
							maps.curr->SetTile(headTile->xc, headTile->yc, tile);
							maps.curr->nextUndo();
							EndDialog(hWnd, IDOK);
							break;
						}

					case TILE_UPDATE:
						{
							char title[256];
							TileNode* headTile = maps.curr->selections().getFirstTile();
							sprintf_s(title, 256, "Tile Properties (%d, %d)", headTile->xc, headTile->yc);
							SetWindowText(hWnd, title);

							HWND hEditTile = GetDlgItem(hWnd, IDC_EDIT_TILEVALUE);
							u16 currTile = headTile->value;
							char tileValue[32];
							_itoa_s(currTile, tileValue, 10);
							SetWindowText(hEditTile, tileValue);
						}
						break;

					case IDCANCEL:
						EndDialog(hWnd, IDCANCEL);
						break;
				}
			}
			break;

		case WM_PAINT:
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				int width = 32,
					height = 32;
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(hWnd, &ps),
					MemhDC = CreateCompatibleDC(hDC);
				HBITMAP Membitmap = CreateCompatibleBitmap(hDC, width, height);
				SelectObject(MemhDC, Membitmap);

				TCHAR lpszTile[11];
				int TextLength = WORD(SendDlgItemMessage(hWnd, IDC_EDIT_TILEVALUE, EM_LINELENGTH, 0, 0));
				*((LPWORD)lpszTile) = TextLength;
				SendDlgItemMessage(hWnd, IDC_EDIT_TILEVALUE, EM_GETLINE, 0, (LPARAM)lpszTile);	
				lpszTile[TextLength] = NULL;

				u16 tile = atoi(lpszTile), tileset;
				
				maps.curr->ERA().get<u16>(tileset, 0);
				TileSet* tiles = &scData.tilesets.set[tileset];

				HBRUSH brush = CreateSolidBrush(RGB(166, 156, 132));
				FillRect(MemhDC, &rect, brush);
				DeleteObject(brush);

				BITMAPINFO bmi = GetBMI(32, 32);
				DrawTile(MemhDC, tiles, 0, 0, tile, bmi, 0, 0, 0);
				BitBlt(hDC, 55, 50, width, height, MemhDC, 0, 0, SRCCOPY);

				BITMAPINFO bmiMini = GetBMI(8, 8);
				for ( int yMiniTile=0; yMiniTile<4; yMiniTile++ )
				{
					for ( int xMiniTile=0; xMiniTile<4; xMiniTile++ )
						DrawMiniTileElevation(hDC, tiles, 350+xMiniTile*9, 50+yMiniTile*9, tile, xMiniTile, yMiniTile, bmiMini);
				}
				
				DeleteObject(Membitmap);
				DeleteDC    (MemhDC);
				DeleteDC    (hDC);
			}
			break;

		case WM_DESTROY:
			EndDialog(hWnd, IDCANCEL);
			break;

		default:
			return false;
			break;
	}
	return 0;
}

#define START_TILES_YC 0
#define NUM_TILES 26640
#define TILES_PER_ROW 16
#define PIXELS_PER_TILE 33
#define PIXELS_PER_WHEEL 32
u32 tilesetIndexedYC(0);

void DoScroll(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	int screenHeight = rect.bottom-rect.top;

	SCROLLINFO scrollbars = { };
	scrollbars.cbSize = sizeof(SCROLLINFO);
	scrollbars.fMask = SIF_ALL;
	scrollbars.nMin = 0;

	scrollbars.nPos = tilesetIndexedYC;
	scrollbars.nMax = (NUM_TILES/TILES_PER_ROW)*PIXELS_PER_TILE;
	scrollbars.nPage = screenHeight;
	SetScrollInfo(hWnd, SB_VERT, &scrollbars, TRUE);
}

BOOL CALLBACK TerrainPaletteProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			DoScroll(hWnd);
			break;

		case WM_LBUTTONDOWN:
			{
				s16 xPos = LOWORD(lParam),
					yPos = HIWORD(lParam);

				if ( yPos > START_TILES_YC )
				{
					yPos -= START_TILES_YC;

					unsigned int xTileCoord = int(xPos)/PIXELS_PER_TILE,
								 yTileCoord = (int(yPos)+tilesetIndexedYC)/PIXELS_PER_TILE;
					
					u16 tileNum = yTileCoord*TILES_PER_ROW + xTileCoord;

					maps.clipboard.endPasting();
					maps.clipboard.addQuickTile(tileNum, -16, -16);
					maps.startPaste(true);

					RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
				}
			}
			break;

		case WM_VSCROLL:
			{
				switch( LOWORD(wParam) )
				{
					case SB_LINEUP:
						if ( tilesetIndexedYC > 7 )
							tilesetIndexedYC -= 8;
						else
							tilesetIndexedYC = 0;
						break;
					case SB_LINEDOWN:
						{
							RECT rect;
							GetClientRect(hWnd, &rect);
							int screenHeight = rect.bottom-rect.top;

							if ( tilesetIndexedYC + 8 < (u32(NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight)) )
								tilesetIndexedYC += 8;
							else
								tilesetIndexedYC = NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight;
						}
						break;
					case SB_PAGEUP:
						{
							RECT rect;
							GetClientRect(hWnd, &rect);
							int screenHeight = rect.bottom-rect.top;

							if ( tilesetIndexedYC - screenHeight/2 > 0 )
								tilesetIndexedYC -= screenHeight/2;
							else
								tilesetIndexedYC = 0;
						}
						break;
					case SB_PAGEDOWN:
						{
							RECT rect;
							GetClientRect(hWnd, &rect);
							int screenHeight = rect.bottom-rect.top;

							if ( (u32(tilesetIndexedYC + screenHeight/2)) < (u32(NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight)) )
								tilesetIndexedYC += screenHeight/2;
							else
								tilesetIndexedYC = NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight;
						}
						break;
					case SB_THUMBPOSITION:
						{
							RECT rect;
							GetClientRect(hWnd, &rect);
							int screenHeight = rect.bottom-rect.top;

							SCROLLINFO scrollpos = { };
							scrollpos.cbSize = sizeof(SCROLLINFO);
							scrollpos.fMask = SIF_POS;
							GetScrollInfo(hWnd, SB_VERT, &scrollpos);

							if ( scrollpos.nPos < 0 )
								tilesetIndexedYC = 0;
							else if ( scrollpos.nPos > NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight )
								tilesetIndexedYC = NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight;
							else
								tilesetIndexedYC = scrollpos.nPos;
						}
						break;
					case SB_THUMBTRACK:
						{
							RECT rect;
							GetClientRect(hWnd, &rect);
							int screenHeight = rect.bottom-rect.top;

							SCROLLINFO scrollpos = { };
							scrollpos.cbSize = sizeof(SCROLLINFO);
							scrollpos.fMask = SIF_TRACKPOS;
							GetScrollInfo(hWnd, SB_VERT, &scrollpos);

							if ( scrollpos.nTrackPos < 0 )
								tilesetIndexedYC = 0;
							else if ( scrollpos.nTrackPos > NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight )
								tilesetIndexedYC = NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight;
							else
								tilesetIndexedYC = scrollpos.nTrackPos;
						}
						break;
				}
				DoScroll(hWnd);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_PAINT:
			{
				if ( maps.curr != nullptr )
				{
					RECT rect;
					GetClientRect(hWnd, &rect);
					int width = rect.right-rect.left,
						height = rect.bottom-rect.top-START_TILES_YC;

					PAINTSTRUCT ps;
					HDC hDC, MemhDC;
					HBITMAP Membitmap;

					hDC = BeginPaint(hWnd, &ps);
					MemhDC = CreateCompatibleDC(hDC);
					Membitmap = CreateCompatibleBitmap(hDC, width, height);
					SelectObject(MemhDC, Membitmap);

					BITMAPINFO bmi = GetBMI(32, 32);

					u16 tileset;
					maps.curr->ERA().get<u16>(tileset, 0);
					TileSet* tiles = &scData.tilesets.set[tileset];

					u16 tileValue = u16(tilesetIndexedYC/PIXELS_PER_TILE*TILES_PER_ROW);
					int yOffset = tilesetIndexedYC%PIXELS_PER_TILE;
					int numRows = height/PIXELS_PER_TILE+2;
					bool tileHighlighted = maps.clipboard.hasQuickTiles();
					u16 numHighlighted = 0;
					if ( tileHighlighted )
						numHighlighted = maps.clipboard.getFirstTile()->value;

					for ( s32 row = 0; row < numRows; row++ )
					{
						for ( s32 column = 0; column < TILES_PER_ROW; column++ )
						{
							if ( tileHighlighted && tileValue == numHighlighted )
							{	
								DrawTile(MemhDC, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE-yOffset), tileValue, bmi, 0, 0, 0);

								COLORREF selClr = RGB(255, 255, 255);
								int rectLeft = column*PIXELS_PER_TILE-1,
									rectTop = row*PIXELS_PER_TILE-yOffset-1,
									rectRight = rectLeft+PIXELS_PER_TILE,
									rectBottom = rectTop+PIXELS_PER_TILE;

								for ( int i=rectLeft; i<rectRight; i+=2 )
									SetPixel(MemhDC, i, rectTop, selClr);
								for ( int i=rectLeft+1; i<rectRight; i+=2 )
									SetPixel(MemhDC, i, rectBottom, selClr);
								for ( int i=rectTop; i<rectBottom; i+=2 )
									SetPixel(MemhDC, rectLeft, i, selClr);
								for ( int i=rectTop+1; i<=rectBottom; i+=2 )
									SetPixel(MemhDC, rectRight, i, selClr);
							}
							else
							{
								if ( maps.curr->DisplayingElevations() )
									DrawTileElevation(MemhDC, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE-yOffset), tileValue, bmi);
								else
									DrawTile(MemhDC, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE-yOffset), tileValue, bmi, 0, 0, 0);
								if ( maps.curr->DisplayingTileNums() )
								{
									HFONT NumFont = CreateFont(14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Microsoft Sans Serif");
									SelectObject(MemhDC, NumFont);
									SetBkMode(MemhDC, TRANSPARENT);
									SetTextColor(MemhDC, RGB(255, 255, 0));

									char TileHex[32];
									RECT nullRect = { };
									sprintf_s(TileHex, 8, "%hu", tileValue);
									ExtTextOut(MemhDC, column*PIXELS_PER_TILE+3, row*PIXELS_PER_TILE-yOffset+2, ETO_OPAQUE, &nullRect, TileHex, strlen(TileHex), 0);

									DeleteObject(NumFont);
								}
							}

							tileValue++;
						}
					}

					BitBlt(hDC, 0, START_TILES_YC, width, height, MemhDC, 0, 0, SRCCOPY);
					DeleteObject(Membitmap);
					DeleteDC    (MemhDC);
					DeleteDC    (hDC);
				}
				return false;
			}
			break;

		case WM_MOUSEWHEEL:
			{
				int distanceScrolled = int((s16(HIWORD(wParam)))/WHEEL_DELTA);

				if ( distanceScrolled < 0 )
				{
					RECT rect;
					GetClientRect(hWnd, &rect);
					int screenHeight = rect.bottom-rect.top;

					if ( u32(tilesetIndexedYC - distanceScrolled*PIXELS_PER_WHEEL) < u32(NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight) )
						tilesetIndexedYC -= distanceScrolled*PIXELS_PER_WHEEL;
					else
						tilesetIndexedYC = NUM_TILES/TILES_PER_ROW*PIXELS_PER_TILE-screenHeight;
				}
				else if ( tilesetIndexedYC >= u32(distanceScrolled*PIXELS_PER_WHEEL) )
					tilesetIndexedYC -= distanceScrolled*PIXELS_PER_WHEEL;

				DoScroll(hWnd);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_GETMINMAXINFO:
			{
				RECT winRect = { };
				GetWindowRect(hWnd, &winRect);
				int width = winRect.right-winRect.left;

				((MINMAXINFO*)lParam)->ptMaxSize.x = width;
				((MINMAXINFO*)lParam)->ptMaxTrackSize.x = width;
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = width;
			}
			break;

		case WM_CLOSE:
			hTilePal = nullptr;
			EndDialog(hWnd, IDCANCEL);
			break;

		case WM_DESTROY:
			hTilePal = nullptr;
			EndDialog(hWnd, IDCANCEL);
			break;

		default:
			return false;
			break;
	}
	return true;
}
