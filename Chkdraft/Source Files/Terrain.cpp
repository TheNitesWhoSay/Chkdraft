#include "Terrain.h"
#include "Chkdraft.h"

#define START_TILES_YC 0
#define NUM_TILES 26640
#define TILES_PER_ROW 16
#define PIXELS_PER_TILE 33
#define PIXELS_PER_WHEEL 32

TerrainPaletteWindow::TerrainPaletteWindow() : tilesetIndexedYC(0)
{

}

bool TerrainPaletteWindow::CreateThis(HWND hParent)
{
	return getHandle() == NULL &&
		   ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_INDEXED_TILESET), hParent);
}

bool TerrainPaletteWindow::DestroyThis()
{
	return ClassWindow::DestroyDialog();
}

void TerrainPaletteWindow::DoScroll(HWND hWnd)
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

BOOL TerrainPaletteWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

					chkd.maps.clipboard.endPasting();
					chkd.maps.clipboard.addQuickTile(tileNum, -16, -16);
					chkd.maps.startPaste(true);

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
				if ( chkd.maps.curr != nullptr )
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
					chkd.maps.curr->ERA().get<u16>(tileset, 0);
					TileSet* tiles = &chkd.scData.tilesets.set[tileset];

					u16 tileValue = u16(tilesetIndexedYC/PIXELS_PER_TILE*TILES_PER_ROW);
					int yOffset = tilesetIndexedYC%PIXELS_PER_TILE;
					int numRows = height/PIXELS_PER_TILE+2;
					bool tileHighlighted = chkd.maps.clipboard.hasQuickTiles();
					u16 numHighlighted = 0;
					if ( tileHighlighted )
						numHighlighted = chkd.maps.clipboard.getFirstTile()->value;

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
								if ( chkd.maps.curr->DisplayingElevations() )
									DrawTileElevation(MemhDC, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE-yOffset), tileValue, bmi);
								else
									DrawTile(MemhDC, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE-yOffset), tileValue, bmi, 0, 0, 0);
								if ( chkd.maps.curr->DisplayingTileNums() )
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
			ClassWindow::DestroyDialog();
			break;

		case WM_DESTROY:
			ClassWindow::DestroyDialog();
			break;

		default:
			return false;
			break;
	}
	return true;
}
