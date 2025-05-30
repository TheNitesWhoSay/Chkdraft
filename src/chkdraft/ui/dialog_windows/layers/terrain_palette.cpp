#include "terrain_palette.h"
#include "chkdraft/chkdraft.h"
#include "mapping/scr_graphics.h"
#include <WindowsX.h>

#define START_TILES_YC 0
#define NUM_TILES 32752
#define TILES_PER_ROW 16
#define PIXELS_PER_TILE 33
#define PIXELS_PER_WHEEL 32

TerrainPaletteWindow::TerrainPaletteWindow() : tilesetIndexedYC(0)
{

}

TerrainPaletteWindow::~TerrainPaletteWindow()
{

}

bool TerrainPaletteWindow::CreateThis(HWND hParent)
{
    return getHandle() == NULL &&
        ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_INDEXED_TILESET), hParent);
}

bool TerrainPaletteWindow::DestroyThis()
{
    if ( this->openGlDc != nullptr )
    {
        chkd.maps.releaseRenderContext(this->openGlDc);
        this->openGlDc = nullptr;
    }
    return ClassDialog::DestroyDialog();
}

void TerrainPaletteWindow::SelectTile(std::uint16_t tileValue)
{
    u32 newTileRow = u32(tileValue)/TILES_PER_ROW;
    int newTileOffsetY = newTileRow*PIXELS_PER_TILE;
    int numRows = (WindowsItem::cliHeight()-START_TILES_YC) / PIXELS_PER_TILE;
    
    u32 firstRow = tilesetIndexedYC/PIXELS_PER_TILE;
    int yOffset = tilesetIndexedYC%PIXELS_PER_TILE;
    u32 lastRow = firstRow+numRows;

    bool redraw = false;
    if ( newTileRow <= firstRow )
    {
        tilesetIndexedYC = newTileRow*PIXELS_PER_TILE;
        redraw = true;
    }
    else if ( newTileRow >= lastRow )
    {
        tilesetIndexedYC = (newTileRow-numRows)*PIXELS_PER_TILE+(PIXELS_PER_TILE-WindowsItem::cliHeight()%PIXELS_PER_TILE);
        redraw = true;
    }

    if ( !chkd.maps.clipboard.hasQuickTiles() || !chkd.maps.clipboard.isPasting() || chkd.maps.clipboard.getTiles()[0].value != tileValue )
    {
        chkd.maps.clipboard.endPasting();
        CM->setSubLayer(TerrainSubLayer::Rectangular);
        chkd.maps.clipboard.setQuickTile(tileValue, -16, -16);
        chkd.maps.startPaste(true);
        redraw = true;
    }

    if ( redraw )
        WindowsItem::RedrawThis();
}

void TerrainPaletteWindow::Activate(WPARAM wParam)
{
    if ( LOWORD(wParam) != WA_INACTIVE )
        chkd.SetCurrDialog(getHandle());
}

void TerrainPaletteWindow::DoScroll()
{
    SCROLLINFO scrollbars = { };
    scrollbars.cbSize = sizeof(SCROLLINFO);
    scrollbars.fMask = SIF_ALL;
    scrollbars.nMin = 0;
    scrollbars.nMax = (NUM_TILES / TILES_PER_ROW)*PIXELS_PER_TILE;
    scrollbars.nPage = cliHeight();
    scrollbars.nPos = tilesetIndexedYC;
    SetScrollInfo(getHandle(), SB_VERT, &scrollbars, TRUE);
}

void TerrainPaletteWindow::DoPaint()
{
    if ( CM != nullptr )
    {
        if ( CM->GetSkin() == GuiMap::Skin::ClassicGDI )
        {
            WinLib::DeviceContext dc(WindowsItem::getHandle(), cliWidth(), cliHeight());
            dc.setFont(4, 14, "Microsoft Sans Serif");
            dc.setBkMode(TRANSPARENT);
            dc.setTextColor(RGB(255, 255, 0));
            BITMAPINFO bmi = GetBMI(32, 32);

            const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(CM->getTileset());

            u16 tileValue = u16(tilesetIndexedYC / PIXELS_PER_TILE*TILES_PER_ROW);
            int yOffset = tilesetIndexedYC%PIXELS_PER_TILE;
            int numRows = (WindowsItem::cliHeight()-START_TILES_YC) / PIXELS_PER_TILE + 2;
            bool tileHighlighted = chkd.maps.clipboard.hasQuickTiles();
            std::vector<PasteTileNode> & pasteTiles = chkd.maps.clipboard.getTiles();
            u16 numHighlighted = 0;
            if ( pasteTiles.size() > 0 )
                numHighlighted = pasteTiles[0].value;

            for ( s32 row = 0; row < numRows; row++ )
            {
                for ( s32 column = 0; column < TILES_PER_ROW; column++ )
                {
                    if ( tileHighlighted && tileValue == numHighlighted )
                    {
                        DrawTile(dc, CM->getPalette(), tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE - yOffset),
                            tileValue, bmi, 0, 0, 0);

                        COLORREF selClr = RGB(255, 255, 255);
                        int rectLeft = column*PIXELS_PER_TILE - 1,
                            rectTop = row*PIXELS_PER_TILE - yOffset - 1,
                            rectRight = rectLeft + PIXELS_PER_TILE,
                            rectBottom = rectTop + PIXELS_PER_TILE;

                        for ( int i = rectLeft; i<rectRight; i += 2 )
                            dc.setPixel(i, rectTop, selClr);
                        for ( int i = rectLeft + 1; i<rectRight; i += 2 )
                            dc.setPixel(i, rectBottom, selClr);
                        for ( int i = rectTop; i<rectBottom; i += 2 )
                            dc.setPixel(rectLeft, i, selClr);
                        for ( int i = rectTop + 1; i <= rectBottom; i += 2 )
                            dc.setPixel(rectRight, i, selClr);
                    }
                    else
                    {
                        if ( CM->DisplayingElevations() )
                            DrawTileElevation(dc, tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE - yOffset), tileValue, bmi);
                        else
                            DrawTile(dc, CM->getPalette(), tiles, s16(column*PIXELS_PER_TILE), s16(row*PIXELS_PER_TILE - yOffset), tileValue, bmi, 0, 0, 0);

                        if ( CM->DisplayingTileNums() )
                        {
                            char TileHex[8];
                            RECT nullRect {};
                            std::snprintf(TileHex, 8, "%hu", tileValue);
                            dc.drawText(TileHex, column*PIXELS_PER_TILE + 3, row*PIXELS_PER_TILE - yOffset + 2, nullRect, false, true);
                        }
                    }

                    tileValue++;
                }
            }
            dc.flushBuffer();
        }
        else // OpenGL rendering
        {
            RECT rcCli {};
            getClientRect(rcCli);
            GLint savedViewport[4] {};
            glGetIntegerv(GL_VIEWPORT, savedViewport);
            chkd.maps.setGlRenderTarget(this->openGlDc, *this);
            glViewport(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top);
            
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto selectedTile = chkd.maps.clipboard.hasQuickTiles() ?
                std::make_optional<u16>(chkd.maps.clipboard.getTiles()[0].value) : std::nullopt;

            CM->scrGraphics->drawTilesetIndexed(rcCli.left, rcCli.top, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top, tilesetIndexedYC, selectedTile);
            
            glFlush();
            SwapBuffers(openGlDc->getDcHandle());
            ValidateRect(getHandle(), &rcCli);
            
            glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
        }
    }
}

void TerrainPaletteWindow::LButtonDown(WPARAM wParam, int xc, int yc)
{
    if ( yc > START_TILES_YC )
    {
        yc -= START_TILES_YC;

        u16 xTileCoord = xc / PIXELS_PER_TILE,
            yTileCoord = (yc + tilesetIndexedYC) / PIXELS_PER_TILE,
            tileNum = yTileCoord*TILES_PER_ROW + xTileCoord;

        chkd.maps.clipboard.endPasting();
        CM->setSubLayer(TerrainSubLayer::Rectangular);
        chkd.maps.clipboard.addQuickTile(tileNum, -16, -16);
        chkd.maps.startPaste(true);

        WindowsItem::RedrawThis();
    }
}

void TerrainPaletteWindow::MouseWheel(WPARAM wParam)
{
    int distanceScrolled = int((s16(HIWORD(wParam))) / WHEEL_DELTA);

    if ( distanceScrolled < 0 )
    {
        if ( u32(tilesetIndexedYC - distanceScrolled*PIXELS_PER_WHEEL) < u32(NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight()) )
            tilesetIndexedYC -= distanceScrolled*PIXELS_PER_WHEEL;
        else
            tilesetIndexedYC = NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight();
    }
    else if ( tilesetIndexedYC >= u32(distanceScrolled*PIXELS_PER_WHEEL) )
        tilesetIndexedYC -= distanceScrolled*PIXELS_PER_WHEEL;

    DoScroll();
    WindowsItem::RedrawThis();
}

void TerrainPaletteWindow::GetMinMaxInfo(LPARAM lParam)
{
    LONG windowWidth = WindowsItem::Width();
    ((MINMAXINFO*)lParam)->ptMaxSize.x = windowWidth;
    ((MINMAXINFO*)lParam)->ptMaxTrackSize.x = windowWidth;
    ((MINMAXINFO*)lParam)->ptMinTrackSize.x = windowWidth;
}

void TerrainPaletteWindow::VScroll(WPARAM wParam)
{
    switch ( LOWORD(wParam) )
    {
        case SB_LINEDOWN: VScrollLineDown(); break;
        case SB_LINEUP: VScrollLineUp(); break;
        case SB_PAGEDOWN: VScrollPageDown(); break;
        case SB_PAGEUP: VScrollPageUp(); break;
        case SB_THUMBPOSITION: VScrollThumbPos(); break;
        case SB_THUMBTRACK: VScrollThumbTrack(); break;
    }
    DoScroll();
    WindowsItem::RedrawThis();
}

void TerrainPaletteWindow::VScrollLineDown()
{
    if ( tilesetIndexedYC + 8 < u32(NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight()) )
        tilesetIndexedYC += 8;
    else
        tilesetIndexedYC = NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight();
}

void TerrainPaletteWindow::VScrollLineUp()
{
    if ( tilesetIndexedYC > 7 )
        tilesetIndexedYC -= 8;
    else
        tilesetIndexedYC = 0;
}

void TerrainPaletteWindow::VScrollPageDown()
{
    if ( u32(tilesetIndexedYC + cliHeight() / 2) < u32(NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight()) )
        tilesetIndexedYC += cliHeight() / 2;
    else
        tilesetIndexedYC = NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight();
}

void TerrainPaletteWindow::VScrollPageUp()
{
    if ( tilesetIndexedYC - cliHeight() / 2 > 0 )
        tilesetIndexedYC -= cliHeight() / 2;
    else
        tilesetIndexedYC = 0;
}

void TerrainPaletteWindow::VScrollThumbPos()
{
    SCROLLINFO scrollpos = {};
    scrollpos.cbSize = sizeof(SCROLLINFO);
    scrollpos.fMask = SIF_POS;
    if ( GetScrollInfo(getHandle(), SB_VERT, &scrollpos) != 0 )
    {
        if ( scrollpos.nPos < 0 )
            tilesetIndexedYC = 0;
        else if ( scrollpos.nPos > NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight() )
            tilesetIndexedYC = NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight();
        else
            tilesetIndexedYC = scrollpos.nPos;
    }
}

void TerrainPaletteWindow::VScrollThumbTrack()
{
    SCROLLINFO scrollpos = {};
    scrollpos.cbSize = sizeof(SCROLLINFO);
    scrollpos.fMask = SIF_TRACKPOS;
    if ( GetScrollInfo(getHandle(), SB_VERT, &scrollpos) != 0 )
    {
        if ( scrollpos.nTrackPos < 0 )
            tilesetIndexedYC = 0;
        else if ( scrollpos.nTrackPos > NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight() )
            tilesetIndexedYC = NUM_TILES / TILES_PER_ROW*PIXELS_PER_TILE - cliHeight();
        else
            tilesetIndexedYC = scrollpos.nTrackPos;
    }
}

BOOL TerrainPaletteWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE: Activate(wParam); break;
        case WM_INITDIALOG: DoScroll(); break;
        case WM_LBUTTONDOWN: LButtonDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_VSCROLL: VScroll(wParam); break;
        case WM_PAINT: DoPaint(); break;
        case WM_MOUSEWHEEL: MouseWheel(wParam); break;
        case WM_GETMINMAXINFO: GetMinMaxInfo(lParam); break;
        case WM_CLOSE: DestroyThis(); return TRUE; break;
        default: return ClassDialog::DlgProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
