#include "Graphics.h"
#include "../CommonFiles/CommonFiles.h"
#include "../Chkdraft.h"
#include <algorithm>
#include <string>

DWORD ColorCycler::prevTickCount = 0;

enum_t(MaxUnitBounds, s32, {
    Left = 128, Right = 127, Up = 80, Down = 79
});

const size_t ColorCycler::TilesetRotationSet[Sc::Terrain::NumTilesets] = {
    0, // badlands uses set 0
    1, // platform uses set 1
    1, // installation uses set 1
    2, // ashworld uses set 2
    0, // jungle uses set 0
    3, // desert uses set 3
    3, // iceworld uses set 3
    3  // twilight uses set 3
};

ColorCycler::Rotator ColorCycler::NoRotators[MaxRotatersPerSet] = {};

ColorCycler::Rotator ColorCycler::RotatorSets[TotalRotatorSets][MaxRotatersPerSet] = {
    { // Rotator set 0: Three rotators act on badlands and jungle
        { Rotator::Enabled::Yes, 8, 0, 1, 6 }, // Every eight ticks rotate palette colors from indexes 1 to 6 (inclusive) rightwards
        { Rotator::Enabled::Yes, 8, 0, 7, 13 }, // Every eight ticks rotate palette colors from indexes 7 to 13 (inclusive) rightwards
        { Rotator::Enabled::Yes, 8, 0, 248, 254 } // Every eight ticks rotate palette colors from indexes 248 to 254 (inclusive) rightwards
    },
    {}, // Rotator set 1: No rotators act on platform or installation
    { // Rotator set 2: Three rotators act on ashworld
        { Rotator::Enabled::Yes, 8, 0, 1, 4 }, // Every eight ticks rotate palette colors from indexes 1 to 4 (inclusive) rightwards
        { Rotator::Enabled::Yes, 8, 0, 5, 8 }, // Every eight ticks rotate palette colors from indexes 5 to 8 (inclusive) rightwards
        { Rotator::Enabled::Yes, 8, 0, 9, 13 } // Every eight ticks rotate palette colors from indexes 9 to 13 (inclusive) rightwards
    },
    { // Rotator set 3: Two rotators act on desert, iceworld, and twilight
        { Rotator::Enabled::Yes, 8, 0, 1, 13 }, // Every eight ticks rotate palette colors from indexes 1 to 13 (inclusive) rightwards
        { Rotator::Enabled::Yes, 8, 0, 248, 254 } // Every eight ticks rotate palette colors from indexes 248 to 254 (inclusive) rightwards
    }
};

ColorCycler::~ColorCycler()
{

}

bool ColorCycler::CycleColors(const u16 tileset, ChkdPalette & palette)
{
    bool redraw = false;
    if ( GetTickCount() > prevTickCount )
    {
        size_t currentRotationSet = TilesetRotationSet[tileset];
        Rotator* rotatorSet = currentRotationSet < TotalRotatorSets ? RotatorSets[currentRotationSet] : NoRotators;
        for ( size_t rotatorIndex=0; rotatorIndex<8; rotatorIndex++ )
        {
            Rotator & rotator = rotatorSet[rotatorIndex];
            if ( rotator.enabled != Rotator::Enabled::No ) // Ensure this rotator is enabled/exists
            {
                if ( rotator.ticksRemaining == 0 )
                {
                    for ( u16 paletteIndex = rotator.paletteIndexMax; paletteIndex > rotator.paletteIndexMin; paletteIndex-- )
                        std::swap(palette[paletteIndex], palette[paletteIndex-1]); // Swap adjacent colors starting from max and ending at min, net effect is each rotates one to the right

                    rotator.ticksRemaining = rotator.ticksBetweenRotations; // Reset the timer
                    redraw = true; // Flag that anything using the palette should be redrawn
                }
                else
                    rotator.ticksRemaining --; // Decrement the timer
            }
        }

        prevTickCount = GetTickCount();
    }

    return redraw;
}

inline void BoundedAdjustPx(Sc::SystemColor & pixel, s16 redOffset, s16 greenOffset, s16 blueOffset)
{
    u8 result = ((u8*)&pixel)[0] + (u8)blueOffset;
    if ( blueOffset > 0 )
    {
        if ( result > ((u8*)&pixel)[0] ) // No overflow
            ((u8*)&pixel)[0] = result;
        else
            ((u8*)&pixel)[0] = 255;
    }
    else if ( blueOffset < 0 )
    {
        if ( result < ((u8*)&pixel)[0] ) // No underflow
            ((u8*)&pixel)[0] = result;
        else
            ((u8*)&pixel)[0] = 0;
    }

    result = ((u8*)&pixel)[1] + (u8)greenOffset;
    if ( greenOffset > 0 )
    {
        if ( result > ((u8*)&pixel)[1] ) // No overflow
            ((u8*)&pixel)[1] += (u8)greenOffset;
        else
            ((u8*)&pixel)[1] = 255;
    }
    else if ( greenOffset < 0 )
    {
        if ( result < ((u8*)&pixel)[1] ) // No underflow
            ((u8*)&pixel)[1] += (u8)greenOffset;
        else
            ((u8*)&pixel)[1] = 0;
    }

    result = ((u8*)&pixel)[2] + (u8)redOffset;
    if ( redOffset > 0 )
    {
        if ( result > ((u8*)&pixel)[2] ) // No overflow
            ((u8*)&pixel)[2] += (u8)redOffset;
        else
            ((u8*)&pixel)[2] = 255;
    }
    else if ( redOffset < 0 )
    {
        if ( result < ((u8*)&pixel)[2] ) // No underflow
            ((u8*)&pixel)[2] += (u8)redOffset;
        else
            ((u8*)&pixel)[2] = 0;
    }
}

Sc::SystemColor black = Sc::SystemColor();

Graphics::Graphics(GuiMap & map, Selections & selections) : map(map), selections(selections),
    displayingIsomTypes(false), displayingTileNums(false), tileNumsFromMTXM(false), displayingElevations(false), clipLocationNames(true), mapWidth(0), mapHeight(0), screenWidth(0), screenHeight(0), screenLeft(0), screenTop(0)
{
    if ( !map.empty() )
        updatePalette();
}

Graphics::~Graphics()
{

}

ChkdPalette & Graphics::getPalette()
{
    return palette;
}

void Graphics::updatePalette()
{
    palette = chkd.scData.terrain.getColorPalette(map.getTileset());
}

void Graphics::DrawMap(const WinLib::DeviceContext & dc, u16 bitWidth, u16 bitHeight, s32 screenLeft, s32 screenTop, ChkdBitmap & bitmap, bool showAnywhere)
{
    this->screenLeft = screenLeft;
    this->screenTop = screenTop;

    screenWidth = bitWidth;
    screenHeight = bitHeight;

    mapWidth = (u16)map.getTileWidth();
    mapHeight = (u16)map.getTileHeight();

    if ( displayingElevations )
        DrawTileElevations(bitmap);
    else
        DrawTerrain(bitmap);

    DrawGrid(bitmap);

    DrawUnits(bitmap);

    DrawSprites(bitmap);

    if ( map.getLayer() == Layer::Locations )
        DrawLocations(bitmap, showAnywhere);
    else if ( map.getLayer() == Layer::FogEdit )
        DrawFog(bitmap);

    BITMAPINFO bmi = GetBMI(screenWidth, screenHeight);
    dc.setBitsToDevice(0, 0, screenWidth, screenHeight, 0, 0, 0, screenHeight, bitmap.data(), &bmi);

    if ( map.getLayer() == Layer::Locations )
        DrawLocationNames(dc);

    if ( displayingTileNums )
        DrawTileNumbers(dc);

    if ( displayingIsomTypes )
        DrawIsomNumbers(dc);
}

void Graphics::DrawTerrain(ChkdBitmap & bitmap)
{
    Sc::Terrain::Tileset tileset = map.getTileset();
    const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);
    u32 maxRowY = screenHeight > (s32)mapHeight*32 || (screenTop+screenHeight)/32+1 > mapHeight ? mapHeight : (screenTop+screenHeight)/32+1;
    u32 maxRowX = screenWidth > (s32)mapWidth*32 || (screenLeft+screenWidth)/32+1 > mapWidth ? mapWidth : (screenLeft+screenWidth)/32+1;

    for ( u16 yTile = (u16)(screenTop/32); yTile < maxRowY; yTile++ ) // Cycle through all rows on the screen
    {
        for ( u16 xTile = (u16)(screenLeft/32); xTile < maxRowX; xTile++ ) // Cycle through all columns on the screen
        {
            TileToBits(bitmap, palette, tiles, s32(xTile)*32-screenLeft, s32(yTile)*32-screenTop,
                u16(screenWidth), u16(screenHeight), map.getTile(xTile, yTile));
        }
    }
}

void Graphics::DrawTileElevations(ChkdBitmap & bitmap)
{
    u32 maxRowX, maxRowY, xc, yc;

    if ( screenHeight > (s32)mapHeight*32 || (screenTop+screenHeight)/32+1 > mapHeight )
        maxRowY = mapHeight;
    else
        maxRowY = (screenTop+screenHeight)/32+1;

    if ( screenWidth > (s32)mapWidth*32 || (screenLeft+screenWidth)/32+1 > mapWidth )
        maxRowX = mapWidth;
    else
        maxRowX = (screenLeft+screenWidth)/32+1;

    BITMAPINFO bmi = GetBMI(32, 32);
    
    Sc::Terrain::Tileset tileset = map.getTileset();
    const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);

    for ( yc=screenTop/32; yc<maxRowY; yc++ )
    {
        for ( xc=screenLeft/32; xc<maxRowX; xc++ )
        {
            TileElevationsToBits(bitmap, screenWidth, screenHeight, tiles,
                s16(xc*32-screenLeft), s16(yc*32-screenTop), map.getTile(xc, yc), bmi, 0 );
        }
    }
}

void Graphics::DrawGrid(ChkdBitmap & bitmap)
{
    u16 gridXSize = 0,
        gridYSize = 0,
        x = 0, y = 0;

    for ( u32 i=0; i<2; i++ )
    {
        MapGrid currGrid = grids[i];
        gridXSize = currGrid.size.x;
        gridYSize = currGrid.size.y;

        if ( gridXSize > 0 )
        {
            for ( x = gridXSize-(screenLeft%gridXSize); x < screenWidth; x += gridXSize ) // Draw vertical lines
            {
                for ( y = 0; y < screenHeight; y++ )
                    bitmap[y*screenWidth + x] = currGrid.color;
            }
        }
            
        if ( gridYSize > 0 )
        {
            for ( y = gridYSize-(screenTop%gridYSize); y < screenHeight; y += gridYSize )
            {
                for ( x = 0; x < screenWidth; x++ )
                    bitmap[y*screenWidth + x] = currGrid.color;
            }
        }
    }
}

void Graphics::DrawLocations(ChkdBitmap & bitmap, bool showAnywhere)
{
    u32 bitMax = screenWidth*screenHeight;

    for ( size_t locationId=1; locationId<map.numLocations(); ++locationId )
    {
        const auto & location = map.locations[locationId];
        if ( locationId != Chk::LocationId::Anywhere || showAnywhere )
        {
            
            s32 leftMost = std::min(location.left, location.right);
            if ( leftMost < screenLeft+screenWidth )
            {
                s32 rightMost = std::max(location.left, location.right);
                if ( rightMost >= screenLeft )
                {
                    s32 topMost = std::min(location.top, location.bottom);
                    if ( topMost < screenTop+screenHeight )
                    {
                        s32 bottomMost = std::max(location.top, location.bottom);
                        if ( bottomMost >= screenTop )
                        {
                            bool leftMostOnScreen = true,
                                rightMostOnScreen = true,
                                topMostOnScreen = true,
                                bottomMostOnScreen = true,
                                inverted = (location.left > location.right || location.top > location.bottom);

                            if ( leftMost < screenLeft )
                            {
                                leftMost = screenLeft;
                                leftMostOnScreen = false;
                            }
                            if ( rightMost >= screenLeft+screenWidth )
                            {
                                rightMost = screenLeft+screenWidth-1;
                                rightMostOnScreen = false;
                            }
                            if ( topMost < screenTop )
                            {
                                topMost = screenTop;
                                topMostOnScreen = false;
                            }
                            if ( bottomMost >= screenTop+screenHeight )
                            {
                                bottomMost = screenTop+screenHeight-1;
                                bottomMostOnScreen = false;
                            }

                            leftMost -= screenLeft;
                            rightMost -= screenLeft;
                            topMost -= screenTop;
                            bottomMost -= screenTop;

                            if ( leftMostOnScreen )
                            {
                                for ( s32 y = topMost; y < bottomMost; y++ )
                                    bitmap[y*screenWidth + leftMost] = black;
                            }
                            if ( rightMostOnScreen )
                            {
                                for ( s32 y = topMost; y < bottomMost; y++ )
                                    bitmap[y*screenWidth + rightMost] = black;
                            }
                            if ( topMostOnScreen )
                            {
                                for ( s32 x = leftMost; x < rightMost; x++ )
                                    bitmap[topMost*screenWidth + x] = black;
                            }
                            if ( bottomMostOnScreen )
                            {
                                for ( s32 x = leftMost; x < rightMost; x++ )
                                    bitmap[bottomMost*screenWidth + x] = black;
                            }

                            if ( inverted )
                            {
                                // Base: +20, -10, -10

                                for ( s32 y = topMost; y<bottomMost; y++ )
                                {
                                    for ( s32 x = leftMost; x<rightMost; x++ )
                                        BoundedAdjustPx(bitmap[y*screenWidth + x], 20, -10, -10);
                                }
                            }
                            else
                            {
                                // Base: -10, +10, +15
                                for ( s32 y = topMost; y<bottomMost; y++ )
                                {
                                    for ( s32 x = leftMost; x<rightMost; x++ )
                                        BoundedAdjustPx(bitmap[y*screenWidth + x], -10, 10, 15);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    u16 selectedLoc = selections.getSelectedLocation();
    if ( selectedLoc != NO_LOCATION && selectedLoc < map.locations.size() )
    {
        const Chk::Location & loc = map.getLocation(selectedLoc);
        s32 leftMost = std::min(loc.left, loc.right);
        s32 rightMost = std::max(loc.left, loc.right);
        s32 topMost = std::min(loc.top, loc.bottom);
        s32 bottomMost = std::max(loc.top, loc.bottom);
        if ( leftMost < screenLeft+screenWidth && rightMost >= screenLeft && topMost < screenTop+screenHeight && bottomMost >= screenTop )
        {
            bool leftMostOnScreen = leftMost >= screenLeft,
                rightMostOnScreen = rightMost < screenLeft+screenWidth,
                topMostOnScreen = topMost >= screenTop,
                bottomMostOnScreen = bottomMost < screenTop+screenHeight;

            if ( !leftMostOnScreen )
                leftMost = 0;
            else
                leftMost -= screenLeft;

            if ( !rightMostOnScreen )
                rightMost = screenWidth;
            else
                rightMost -= screenLeft;

            if ( !topMostOnScreen )
                topMost = 0;
            else
                topMost -= screenTop;

            if ( !bottomMostOnScreen )
                bottomMost = screenHeight;
            else
                bottomMost -= screenTop;

            if ( leftMostOnScreen )
            {
                for ( s32 y = topMost; y < bottomMost; y++ )
                    bitmap[y*screenWidth + leftMost] = Sc::SystemColor(255, 255, 255);
            }
            if ( rightMostOnScreen )
            {
                for ( s32 y = topMost; y < bottomMost; y++ )
                    bitmap[y*screenWidth + rightMost] = Sc::SystemColor(255, 255, 255);
            }
            if ( topMostOnScreen )
            {
                for ( s32 x = leftMost; x < rightMost; x++ )
                    bitmap[topMost*screenWidth + x] = Sc::SystemColor(255, 255, 255);
            }
            if ( bottomMostOnScreen )
            {
                for ( s32 x = leftMost; x < rightMost; x++ )
                    bitmap[bottomMost*screenWidth + x] = Sc::SystemColor(255, 255, 255);
            }
        }
    }
}

void Graphics::DrawFog(ChkdBitmap & bitmap)
{
    u8 currPlayer = map.getCurrPlayer();
    if ( currPlayer >= 8 )
        currPlayer = 0;

    u8 currPlayerMask = u8Bits[currPlayer];
    Sc::Terrain::Tileset tileset = map.getTileset();
    const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);
    u32 maxRowY = screenHeight > (s32)mapHeight*32 || (screenTop+screenHeight)/32+1 > mapHeight ? mapHeight : (screenTop+screenHeight)/32+1;
    u32 maxRowX = screenWidth > (s32)mapWidth*32 || (screenLeft+screenWidth)/32+1 > mapWidth ? mapWidth : (screenLeft+screenWidth)/32+1;

    for ( u16 yTile = (u16)(screenTop/32); yTile < maxRowY; yTile++ ) // Cycle through all rows on the screen
    {
        for ( u16 xTile = (u16)(screenLeft/32); xTile < maxRowX; xTile++ ) // Cycle through all columns on the screen
        {
            if ( (map.getFog(xTile, yTile) & currPlayerMask) != 0 ) // Covered in fog
            {
                s64 xStart = s32(xTile)*32-screenLeft;
                s64 yStart = s32(yTile)*32-screenTop;
                s64 yEnd = std::min(yStart + 32, s64(screenHeight));
                s64 xEnd = std::min(xStart + 32, s64(screenWidth));
                for ( s64 yc = std::max(s64(0), yStart); yc < yEnd; yc++ )
                {
                    for ( s64 xc = std::max(s64(0), xStart); xc < xEnd; xc++ )
                        bitmap[size_t(yc*screenWidth + xc)].darken();
                }
            }
        }
    }
}

void Graphics::DrawUnits(ChkdBitmap & bitmap)
{
    s32 screenRight = screenLeft+screenWidth,
        screenBottom = screenTop+screenHeight;

    u16 tileset = map.getTileset();

    for ( u16 unitNum = 0; unitNum < map.numUnits(); unitNum++ )
    {
        const Chk::Unit & unit = map.getUnit(unitNum);
        if ( (s32)unit.xc + MaxUnitBounds::Right > screenLeft &&
            (s32)unit.xc - MaxUnitBounds::Left < screenRight )
            // If within screen x-bounds
        {
            if ( (s32)unit.yc + MaxUnitBounds::Down > screenTop &&
                (s32)unit.yc - MaxUnitBounds::Up < screenBottom )
                // If within screen y-bounds
            {
                u16 frame = 0;
                Chk::PlayerColor color = (unit.owner < Sc::Player::TotalSlots ?
                    map.getPlayerColor(unit.owner) : (Chk::PlayerColor)unit.owner);

                bool isSelected = selections.unitIsSelected(unitNum);

                UnitToBits(bitmap, palette, color, u16(screenWidth), u16(screenHeight),
                    screenLeft, screenTop, (u16)unit.type, unit.xc, unit.yc,
                    u16(frame), isSelected);
            }
        }
    }
}

void Graphics::DrawSprites(ChkdBitmap & bitmap)
{
    s32 screenRight = screenLeft + screenWidth,
        screenBottom = screenTop + screenHeight;

    u16 tileset = map.getTileset();

    for ( size_t spriteId = 0; spriteId < map.numSprites(); spriteId++ )
    {
        const Chk::Sprite & sprite = map.getSprite(spriteId);
        if ( (s32)sprite.xc + MaxUnitBounds::Right > screenLeft &&
            (s32)sprite.xc - MaxUnitBounds::Left < screenRight )
            // If within screen x-bounds
        {
            if ( (s32)sprite.yc + MaxUnitBounds::Down > screenTop &&
                (s32)sprite.yc - MaxUnitBounds::Up < screenBottom )
                // If within screen y-bounds
            {
                u16 frame = 0;
                bool isSprite = sprite.isDrawnAsSprite();
                Chk::PlayerColor color = (sprite.owner < Sc::Player::TotalSlots ?
                    map.getPlayerColor(sprite.owner) : (Chk::PlayerColor)sprite.owner);

                bool isSelected = selections.spriteIsSelected(spriteId);

                if ( isSprite )
                    SpriteToBits(bitmap, palette, color, u16(screenWidth), u16(screenHeight),
                        screenLeft, screenTop, (u16)sprite.type, sprite.xc, sprite.yc, false, isSelected);
                else
                    UnitToBits(bitmap, palette, color, u16(screenWidth), u16(screenHeight),
                        screenLeft, screenTop, (u16)sprite.type, sprite.xc, sprite.yc,
                        frame, isSelected);
            }
        }
    }
}

void Graphics::DrawLocationNames(const WinLib::DeviceContext & dc)
{
    s32 screenRight = screenLeft + screenWidth;
    s32 screenBottom = screenTop + screenHeight;

    dc.setDefaultFont();
    dc.setBkMode(TRANSPARENT);
    dc.setTextColor(RGB(255, 255, 0));

    for ( size_t locationId = 1; locationId <= map.numLocations(); locationId++ )
    {
        const auto & location = map.getLocation(locationId);
        if ( locationId != Chk::LocationId::Anywhere )
        {
            s32 leftMost = std::min(location.left, location.right);
            if ( leftMost < screenRight )
            {
                s32 rightMost = std::max(location.left, location.right);
                if ( rightMost > screenLeft )
                {
                    s32 topMost = std::min(location.top, location.bottom);
                    if ( topMost < screenBottom )
                    {
                        s32 bottomMost = std::max(location.top, location.bottom);
                        if ( bottomMost > screenTop )
                        {
                            if ( auto str = map.getLocationName<ChkdString>(locationId, Chk::StrScope::EditorOverGame) )
                            {
                                leftMost = leftMost - screenLeft + 2;
                                topMost = topMost - screenTop + 2;
                                RECT rect = {};
                                if ( auto textSize = dc.getTextExtentPoint32(icux::toUistring(*str)) )
                                {
                                    if ( clipLocationNames )
                                    {
                                        rect.left = (leftMost < 0) ? 0 : leftMost;
                                        rect.top = (topMost < 0) ? 0 : topMost;
                                        rect.bottom = bottomMost - screenTop - 1;
                                        rect.right = rightMost - screenLeft - 1;
                                        LONG rectWidth = rect.right - rect.left,
                                            rectHeight = rect.bottom - rect.top;

                                        if ( textSize->cx < rectWidth )
                                            dc.drawText(*str, leftMost, topMost, rect, true, false);
                                        else if ( rectHeight > textSize->cy ) // Can word wrap
                                        {
                                            size_t lastCharPos = str->size() - 1;
                                            s32 prevBottom = rect.top;

                                            while ( rect.bottom - prevBottom > textSize->cy && str->size() > 0 )
                                            {
                                                // Binary search for the character length of this line
                                                size_t floor = 0;
                                                size_t ceil = str->size();
                                                while ( ceil - 1 > floor )
                                                {
                                                    lastCharPos = (ceil - floor) / 2 + floor;
                                                    if ( textSize = dc.getTextExtentPoint32(str->substr(0, lastCharPos)) )
                                                    {
                                                        if ( textSize->cx > rectWidth )
                                                            ceil = lastCharPos;
                                                        else
                                                            floor = lastCharPos;
                                                    }
                                                }
                                                if ( textSize = dc.getTextExtentPoint32(str->substr(0, floor + 1)) ) // Correct last character if needed
                                                {
                                                    if ( textSize->cx > rectWidth )
                                                        lastCharPos = floor;
                                                    else
                                                        lastCharPos = ceil;
                                                }
                                                // End binary search

                                                dc.drawText(str->substr(0, lastCharPos), leftMost, prevBottom, rect, true, false);
                                                (*str) = str->substr(lastCharPos, str->size());
                                                prevBottom += textSize->cy;
                                            }
                                        }
                                    }
                                    else
                                        dc.drawText(*str, leftMost, topMost, rect, false, false);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Graphics::DrawIsomNumbers(const WinLib::DeviceContext & dc)
{
    u32 maxRowX, maxRowY, yc;

    if ( screenHeight > (s32)mapHeight*32 || (screenTop+screenHeight)/32+1 > mapHeight )
        maxRowY = mapHeight;
    else
        maxRowY = (screenTop+screenHeight)/32+1;

    if ( screenWidth > (s32)mapWidth*32 || (screenLeft+screenWidth)/32+1 > mapWidth )
        maxRowX = mapWidth;
    else
        maxRowX = (screenLeft+screenWidth)/32+1;

    dc.setFont(4, 14, "Microsoft Sans Serif");
    dc.setBkMode(TRANSPARENT);
    dc.setTextColor(RGB(255, 255, 0));
    dc.setPen(PS_SOLID, 0, RGB(255, 255, 255));

    RECT nullRect = { };
    std::string TileHex;

    for ( yc=screenTop/32; yc<maxRowY; yc++ )
    {
        u32 xc = screenLeft/32;
        if ( xc%2 > 0 )
            xc -= 1;
        for ( ; xc<maxRowX; xc+=2 )
        {
            auto i = yc*(u32(mapWidth)/2)+xc/2;
            auto isomRect = map.getIsomRect(yc*(u32(mapWidth)/2+1)+xc/2);

            auto str = to_hex_string((isomRect.left & 0x7FF0) >> 4, false);
            dc.drawText(str, xc * 32 - screenLeft + 2, yc * 32 - screenTop + 10, nullRect, false, true);

            str = to_hex_string((isomRect.right & 0x7FF0) >> 4, false);
            if ( auto textSize = dc.getTextExtentPoint32(str) )
                dc.drawText(str, xc*32-screenLeft+63-textSize->cx, yc*32-screenTop+10, nullRect, false, true);

            str = to_hex_string((isomRect.top & 0x7FF0) >> 4, false);
            if ( auto textSize = dc.getTextExtentPoint32(str) )
                dc.drawText(str, xc*32-screenLeft+32-textSize->cx/2, yc*32-screenTop+1, nullRect, false, true);

            str = to_hex_string((isomRect.bottom & 0x7FF0) >> 4, false);
            if ( auto textSize = dc.getTextExtentPoint32(str) )
                dc.drawText(str, xc*32-screenLeft+32-textSize->cx/2, yc*32-screenTop+20, nullRect, false, true);

            if ( (xc/2 + yc) % 2 == 0 )
            {
                dc.moveTo(xc*32-screenLeft+64, yc*32-screenTop);
                dc.lineTo(xc*32-screenLeft, yc*32-screenTop+32);
            }
            else
            {
                dc.moveTo(xc*32-screenLeft, yc*32-screenTop);
                dc.lineTo(xc*32-screenLeft+64, yc*32-screenTop+32);
            }
        }
    }
}

void Graphics::DrawTileNumbers(const WinLib::DeviceContext & dc)
{
    u32 maxRowX, maxRowY,
        xc, yc;

    u16 wTileHex;

    if ( screenHeight > (s32)mapHeight*32 || (screenTop+screenHeight)/32+1 > mapHeight )
        maxRowY = mapHeight;
    else
        maxRowY = (screenTop+screenHeight)/32+1;

    if ( screenWidth > (s32)mapWidth*32 || (screenLeft+screenWidth)/32+1 > mapWidth )
        maxRowX = mapWidth;
    else
        maxRowX = (screenLeft+screenWidth)/32+1;

    dc.setFont(4, 14, "Microsoft Sans Serif");
    dc.setBkMode(TRANSPARENT);
    dc.setTextColor(RGB(255, 255, 0));
    RECT nullRect = { };
    std::string TileHex;

    for ( yc=screenTop/32; yc<maxRowY; yc++ )
    {
        for ( xc=screenLeft/32; xc<maxRowX; xc++ )
        {
            wTileHex = map.getTile(xc, yc, tileNumsFromMTXM ? Chk::StrScope::Game : Chk::StrScope::Editor);
            TileHex = std::to_string(wTileHex);

            dc.drawText(TileHex, xc * 32 - screenLeft + 3, yc * 32 - screenTop + 2, nullRect, false, true);
        }
    }
}

void Graphics::ToggleDisplayIsomValues()
{
    displayingIsomTypes = !displayingIsomTypes;
}

void Graphics::ToggleTileNumSource(bool MTXMoverTILE)
{
    if ( !( displayingTileNums && tileNumsFromMTXM != MTXMoverTILE ) )
        displayingTileNums = !displayingTileNums;

    tileNumsFromMTXM = MTXMoverTILE;
}

bool Graphics::mtxmOverTile()
{
    return tileNumsFromMTXM;
}

bool Graphics::DisplayingIsomNums()
{
    return displayingIsomTypes;
}

bool Graphics::DisplayingTileNums()
{
    return displayingTileNums;
}

void Graphics::ToggleLocationNameClip()
{
    clipLocationNames = !clipLocationNames;
}

bool Graphics::ClippingLocationNames()
{
    return clipLocationNames;
}

void Graphics::ToggleDisplayElevations()
{
    displayingElevations = !displayingElevations;
}

bool Graphics::DisplayingElevations()
{
    return displayingElevations;
}

bool Graphics::GetGridSize(u32 gridNum, u16 & outWidth, u16 & outHeight)
{
    if ( gridNum >= 0 && gridNum < 2 )
    {
        outWidth = grids[gridNum].size.x;
        outHeight = grids[gridNum].size.y;
    }
    else
    {
        outWidth = grids[0].size.x;
        outHeight = grids[0].size.y;
    }
    return gridNum >= 0 && gridNum < 2;
}

bool Graphics::GetGridOffset(u32 gridNum, u16 & outX, u16 & outY)
{
    if ( gridNum >= 0 && gridNum < 2 )
    {
        outX = grids[gridNum].offset.x;
        outY = grids[gridNum].offset.y;
    }
    else
    {
        outX = grids[0].offset.x;
        outY = grids[0].offset.y;
    }
    return gridNum >= 0 && gridNum < 2;
}

bool Graphics::GetGridColor(u32 gridNum, u8 & red, u8 & green, u8 & blue)
{
    if ( gridNum >= 0 && gridNum < 2 )
    {
        red = grids[gridNum].color.red;
        green = grids[gridNum].color.green;
        blue = grids[gridNum].color.blue;
    }
    else
    {
        red = grids[0].color.red;
        green = grids[0].color.green;
        blue = grids[0].color.blue;
    }
    return gridNum >= 0 && gridNum < 2;
}

bool Graphics::SetGridSize(u32 gridNum, u16 xSize, u16 ySize)
{
    if ( gridNum >= 0 && gridNum < 2 )
    {
        grids[gridNum].size.x = xSize;
        grids[gridNum].size.y = ySize;
    }
    return gridNum >= 0 && gridNum < 2;
}

bool Graphics::SetGridColor(u32 gridNum, u8 red, u8 green, u8 blue)
{
    if ( gridNum >= 0 && gridNum < 2 )
    {
        grids[gridNum].color.red = red;
        grids[gridNum].color.green = green;
        grids[gridNum].color.blue = blue;
    }
    return gridNum >= 0 && gridNum < 2;
}


void Graphics::DrawTools(const WinLib::DeviceContext & dc, u16 width, u16 height, u32 screenLeft, u32 screenTop,
    Selections & selections, bool pasting, Clipboard & clipboard, GuiMap & map)
{
    ::DrawTools(dc, palette, width, height, screenLeft, screenTop, selections, pasting, clipboard, map);
}

BITMAPINFO GetBMI(s32 width, s32 height)
{
    BITMAPINFOHEADER bmiH = {};
    bmiH.biSize = sizeof(BITMAPINFOHEADER);
    bmiH.biWidth = width;
    bmiH.biHeight = -height;
    bmiH.biPlanes = 1;
    bmiH.biBitCount = 32;
    bmiH.biCompression = BI_RGB;
    bmiH.biXPelsPerMeter = 1;
    bmiH.biYPelsPerMeter = 1;
                    
    BITMAPINFO bmi = {};
    bmi.bmiHeader = bmiH;
    return bmi;
}

void TileElevationsToBits(ChkdBitmap & bitmap, s64 bitWidth, s64 bitHeight, const Sc::Terrain::Tiles & tiles,
                           s64 xOffset, s64 yOffset, u16 TileValue, BITMAPINFO & bmi, u8 miniTileSeparation )
{
    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(TileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        u16 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(TileValue)];
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                Sc::Terrain::TileFlags::MiniTileFlags miniTileFlags = tiles.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile];
                u8 red = 0, blue = 0, green = 100;
                switch ( miniTileFlags.getElevation() )
                {
                    //case Sc::Terrain::TileElevation::Low: break;
                    case Sc::Terrain::TileElevation::Mid: blue = 100; green = 0; break;
                    case Sc::Terrain::TileElevation::High: red = 100; green = 0; break;
                }
                if ( !miniTileFlags.isWalkable() )
                {
                    red /= 3;
                    green /= 3;
                    blue /= 3;
                }

                s64 miniTileYc = yOffset + 8*(yMiniTile + miniTileSeparation),
                    miniTileXc = xOffset + 8*(xMiniTile + miniTileSeparation);

                for ( s64 yc = 0; yc < 8; yc++ )
                {
                    if ( yc + miniTileYc >= 0 && yc + miniTileYc < bitHeight )
                    {
                        for ( s64 xc = 0; xc < 8; xc ++ )
                        {
                            if ( xc + miniTileXc >= 0 && xc + miniTileXc < bitWidth )
                                bitmap[size_t((yc + miniTileYc)*bitWidth + xc + miniTileXc)] = Sc::SystemColor(red, green, blue);
                        }
                    }
                }
            }
        }
    }
    else // No corresponding CV5 entry
    {
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                s64 miniTileYc = yOffset + yMiniTile * (8*miniTileSeparation),
                    miniTileXc = xOffset + xMiniTile * (8*miniTileSeparation);
                
                for ( s64 yc = 0; yc < 8; yc++ )
                {
                    if ( yc + miniTileYc >= 0 && yc + miniTileYc < bitHeight )
                    {
                        for ( s64 xc = 0; xc < 8; xc ++ )
                        {
                            if ( xc + miniTileXc >= 0 && xc + miniTileXc < bitWidth )
                                bitmap[size_t((yc + miniTileYc)*bitWidth + xc + miniTileXc)] = Sc::SystemColor(0, 0, 0);
                        }
                    }
                }
            }
        }
    }
}

void GrpToBits(ChkdBitmap & bitmap, ChkdPalette & palette, s64 bitWidth, s64 bitHeight, s64 xStart, s64 yStart,
               const Sc::Sprite::GrpFile & grpFile, s64 grpXc, s64 grpYc, u16 frame, u8 color, bool flipped) // TODO: Flipped GRP rendering
{
    if ( frame < grpFile.numFrames )
    {
        const Sc::Sprite::GrpFrameHeader & grpFrameHeader = grpFile.frameHeaders[frame];
        
        s64 xOffset = grpXc-xStart-s64(grpFile.grpWidth)/2+s64(grpFrameHeader.xOffset),
            yOffset = grpYc-yStart-s64(grpFile.grpHeight)/2+s64(grpFrameHeader.yOffset);

        s64 row = 0;
        if ( yOffset < 0 ) // Only draw visible rows by starting at row: |yOffset|, if yOffset is negative
            row = s64(-yOffset);

        s64 frameWidth = s64(grpFrameHeader.frameWidth);
        s64 frameHeight = s64(grpFrameHeader.frameHeight);

        if ( frameWidth == 0 || frameHeight == 0 ) // A dimension is zero, nothing to draw
            return;

        if ( yOffset+frameHeight >= bitHeight ) // Only draw visible rows by limiting row < bitHeight-yOffset if yOffset+frameHeight >= bitHeight
            frameHeight = bitHeight - yOffset;
        
        size_t frameOffset = size_t(grpFrameHeader.frameOffset);
        const Sc::Sprite::GrpFrame & grpFrame = (const Sc::Sprite::GrpFrame &)((u8*)&grpFile)[frameOffset];
        for ( ; row < frameHeight; row++ )
        {
            size_t rowOffset = size_t(grpFrame.rowOffsets[row]);
            const Sc::Sprite::PixelRow & grpPixelRow = (const Sc::Sprite::PixelRow &)((u8*)&grpFile)[frameOffset+rowOffset];
            const s64 rowStart = (row+yOffset)*bitWidth;
            const s64 rowLimit = (row+yOffset+1)*bitWidth;
            s64 currPixelIndex = rowStart + xOffset;
            size_t pixelLineOffset = 0;
            if ( currPixelIndex < (s64)bitmap.size() && currPixelIndex+frameWidth >= rowStart )
            {
                auto currPixel = currPixelIndex < rowStart ? bitmap.begin()+size_t(rowStart) : bitmap.begin()+size_t(currPixelIndex); // Start from the left-most pixel of this row of the frame
                auto frameEnd = currPixelIndex+frameWidth > rowLimit ? bitmap.begin()+size_t(rowLimit) : bitmap.begin()+size_t(currPixelIndex+frameWidth);
                while ( currPixelIndex < rowStart ) // Skip any pixels before the left-edge, draw visible parts of lines overlapping left edge
                {
                    const Sc::Sprite::PixelLine & pixelLine = (const Sc::Sprite::PixelLine &)((u8*)&grpPixelRow)[pixelLineOffset];
                    s64 lineLength = s64(pixelLine.lineLength());
                    s64 inBoundLength = currPixelIndex+lineLength-rowStart;
                    if ( rowStart+inBoundLength > rowLimit )
                        inBoundLength = bitWidth;
                    
                    if ( inBoundLength > 0 )
                    {
                        if ( pixelLine.isSpeckled() )
                        {
                            for ( s64 linePixel = lineLength-inBoundLength; linePixel<lineLength; linePixel++, ++currPixel )
                                *currPixel = palette[pixelLine.paletteIndex[linePixel]]; // Place color from palette index specified in the array at current pixel
                        }
                        else // Solid or transparent
                        {
                            if ( pixelLine.isSolidLine() )
                                std::fill_n(currPixel, inBoundLength, palette[pixelLine.paletteIndex[0]]); // Place single color across the entire line
                            
                            currPixel += size_t(inBoundLength);
                        }
                    }
                    currPixelIndex += lineLength;
                    pixelLineOffset += pixelLine.sizeInBytes();
                }

                while ( currPixel < frameEnd ) // Draw all remaining adjacent horizontal lines
                {
                    const Sc::Sprite::PixelLine & pixelLine = (const Sc::Sprite::PixelLine &)((u8*)&grpPixelRow)[pixelLineOffset];
                    s64 lineLength = s64(pixelLine.lineLength());
                    if ( std::distance(currPixel, frameEnd) < lineLength )
                        lineLength = frameEnd - currPixel;
                    
                    if ( pixelLine.isSpeckled() )
                    {
                        for ( s64 linePixel=0; linePixel<lineLength; linePixel++, ++currPixel ) // For every pixel in the line
                            *currPixel = palette[pixelLine.paletteIndex[linePixel]]; // Place color from palette index specified in the array at current pixel
                    }
                    else // Solid or transparent
                    {
                        if ( pixelLine.isSolidLine() )
                            std::fill_n(currPixel, lineLength, palette[pixelLine.paletteIndex[0]]); // Place single color across the entire line'

                        currPixel += size_t(lineLength);
                    }
                    pixelLineOffset += pixelLine.sizeInBytes();
                }
            }
        }
    }
}

void UnitToBits(ChkdBitmap & bitmap, ChkdPalette & palette, u8 color, u16 bitWidth, u16 bitHeight,
                 s32 & xStart, s32 & yStart, u16 unitID, u16 unitXC, u16 unitYC, u16 frame, bool selected )
{
    Sc::Unit::Type drawnUnitId = unitID < 228 ? (Sc::Unit::Type)unitID : Sc::Unit::Type::TerranMarine; // Extended units use ID:0's graphics (for now)
    u32 grpId = chkd.scData.sprites.getImage(chkd.scData.sprites.getSprite(chkd.scData.units.getFlingy(chkd.scData.units.getUnit(drawnUnitId).graphics).sprite).imageFile).grpFile;

    if ( (size_t)grpId < chkd.scData.sprites.numGrps() )
    {
        Sc::SystemColor remapped[8];
        if ( selected )
        {
            u32 selectionGrpId = chkd.scData.sprites.getImage(chkd.scData.sprites.getSprite(chkd.scData.units.getFlingy(chkd.scData.units.getUnit(drawnUnitId).graphics).sprite).selectionCircleImage+561).grpFile;
            if ( selectionGrpId < chkd.scData.sprites.numGrps() )
            {
                const Sc::Sprite::GrpFile & selCirc = chkd.scData.sprites.getGrp(selectionGrpId).get();
                u16 offsetY = unitYC + chkd.scData.sprites.getSprite(chkd.scData.units.getFlingy(chkd.scData.units.getUnit(drawnUnitId).graphics).sprite).selectionCircleOffset;
                std::memcpy(remapped, &palette[0], sizeof(remapped));
                std::memcpy(&palette[0], &chkd.scData.tselect.palette[0], sizeof(remapped));
                GrpToBits(bitmap, palette, bitWidth, bitHeight, xStart, yStart, selCirc, unitXC, offsetY, frame, 0, false);
                std::memcpy(&palette[0], remapped, sizeof(remapped));
            }
        }
        
        const Sc::Sprite::GrpFile & curr = chkd.scData.sprites.getGrp(grpId).get();
        std::memcpy(remapped, &palette[8], sizeof(remapped));
        std::memcpy(&palette[8], &chkd.scData.tunit.palette[color < 16 ? 8*color : 8*(color%16)], sizeof(remapped));
        GrpToBits(bitmap, palette, bitWidth, bitHeight, xStart, yStart, curr, unitXC, unitYC, frame, color, false);
        std::memcpy(&palette[8], remapped, sizeof(remapped));
    }
}

void SpriteToBits(ChkdBitmap & bitmap, ChkdPalette & palette, u8 color, u16 bitWidth, u16 bitHeight,
                   s32 xStart, s32 yStart, u16 spriteID, u16 spriteXC, u16 spriteYC, bool flipped, bool selected)
{
    const Sc::Sprite::GrpFile & curr = chkd.scData.sprites.getGrp(chkd.scData.sprites.getImage(chkd.scData.sprites.getSprite(spriteID).imageFile).grpFile).get();
    Sc::SystemColor remapped[8];
    if ( selected )
    {
        u32 selectionGrpId = spriteID >= 130 && spriteID <= 517 ?
            chkd.scData.sprites.getImage(chkd.scData.sprites.getSprite(spriteID).selectionCircleImage+561).grpFile :
            chkd.scData.sprites.getImage(chkd.scData.sprites.getSprite(130).selectionCircleImage+561).grpFile;

        if ( selectionGrpId < chkd.scData.sprites.numGrps() )
        {
            const Sc::Sprite::GrpFile & selCirc = chkd.scData.sprites.getGrp(selectionGrpId).get();
            u16 offsetY = spriteYC + chkd.scData.sprites.getSprite(spriteID).selectionCircleOffset;
            std::memcpy(remapped, &palette[0], sizeof(remapped));
            std::memcpy(&palette[0], &chkd.scData.tselect.palette[0], sizeof(remapped));
            GrpToBits(bitmap, palette, bitWidth, bitHeight, xStart, yStart, selCirc, spriteXC, offsetY, 0, 0, false);
            std::memcpy(&palette[0], remapped, sizeof(remapped));
        }
    }

    std::memcpy(remapped, &palette[8], sizeof(remapped));
    std::memcpy(&palette[8], &chkd.scData.tunit.palette[color < 16 ? 8*color : 8*(color%16)], sizeof(remapped));
    GrpToBits(bitmap, palette, bitWidth, bitHeight, xStart, yStart, curr, spriteXC, spriteYC, 0, color, flipped);
    std::memcpy(&palette[8], remapped, sizeof(remapped));
}

void TileToBits(ChkdBitmap & bitmap, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s64 xStart, s64 yStart, s64 width, s64 height, u16 TileValue)
{
    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(TileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(TileValue)];
        const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[megaTileIndex];
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            s64 yMiniOffset = yStart + yMiniTile*8;
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                const Sc::Terrain::TileGraphicsEx::MiniTileGraphics & miniTileGraphics = tileGraphics.miniTileGraphics[yMiniTile][xMiniTile];
                bool flipped = miniTileGraphics.isFlipped();
                size_t vr4Index = size_t(miniTileGraphics.vr4Index());
                
                s64 xMiniOffset = xStart + xMiniTile*8;
                s64 yMiniLimit = yMiniOffset+8 > height ? height-yMiniOffset : 8;
                s64 xMiniLimit = xMiniOffset+8 > width ? width-xMiniOffset : 8;
                for ( s64 yMiniPixel = yMiniOffset < 0 ? -yMiniOffset : 0; yMiniPixel < yMiniLimit; yMiniPixel++ )
                {
                    for ( s64 xMiniPixel = xMiniOffset < 0 ? -xMiniOffset : 0; xMiniPixel < xMiniLimit; xMiniPixel++ )
                    {
                        const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                        const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                        bitmap[size_t((yMiniOffset+yMiniPixel)*width + (xMiniOffset+xMiniPixel))] = palette[wpeIndex];
                    }
                }
            }
        }
    }
    else // No CV5 Reference
    {
        s64 yEnd = std::min(yStart + 32, height);
        s64 xEnd = std::min(xStart + 32, width);
        for ( s64 yc = std::max(s64(0), yStart); yc < yEnd; yc++ )
        {
            for ( s64 xc = std::max(s64(0), xStart); xc < xEnd; xc++ )
                bitmap[size_t(yc*width + xc)] = black;
        }
    }
}

void TileToBits(ChkdBitmap & bitmap, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s64 xStart, s64 yStart, s64 width, s64 height, u16 TileValue,
    u8 redOffset, u8 blueOffset, u8 greenOffset)
{
    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(TileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(TileValue)];
        const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[megaTileIndex];
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            s64 yMiniOffset = yStart + yMiniTile*8;
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                const Sc::Terrain::TileGraphicsEx::MiniTileGraphics & miniTileGraphics = tileGraphics.miniTileGraphics[yMiniTile][xMiniTile];
                bool flipped = miniTileGraphics.isFlipped();
                size_t vr4Index = size_t(miniTileGraphics.vr4Index());
                
                s64 xMiniOffset = xStart + xMiniTile*8;
                s64 yMiniLimit = yMiniOffset+8 > height ? height-yMiniOffset : 8;
                s64 xMiniLimit = xMiniOffset+8 > width ? width-xMiniOffset : 8;
                for ( s64 yMiniPixel = yMiniOffset < 0 ? -yMiniOffset : 0; yMiniPixel < yMiniLimit; yMiniPixel++ )
                {
                    for ( s64 xMiniPixel = xMiniOffset < 0 ? -xMiniOffset : 0; xMiniPixel < xMiniLimit; xMiniPixel++ )
                    {
                        const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                        const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                        bitmap[size_t((yMiniOffset+yMiniPixel)*width + (xMiniOffset+xMiniPixel))] = Sc::SystemColor(palette[wpeIndex], redOffset, greenOffset, blueOffset);
                    }
                }
            }
        }
    }
    else // No CV5 Reference
    {
        s64 yEnd = std::min(yStart + 32, height);
        s64 xEnd = std::min(xStart + 32, width);
        for ( s64 yc = std::max(s64(0), yStart); yc < yEnd; yc++ )
        {
            for ( s64 xc = std::max(s64(0), xStart); xc < xEnd; xc++ )
                bitmap[size_t(yc*width + xc)] = Sc::SystemColor(redOffset, greenOffset, blueOffset);
        }
    }
}

void DrawMiniTileElevation(const WinLib::DeviceContext & dc, const Sc::Terrain::Tiles & tiles, s64 xOffset, s64 yOffset, u16 tileValue, s64 miniTileX, s64 miniTileY, BITMAPINFO & bmi)
{
    ChkdBitmap graphicBits;
    graphicBits.resize(64);
    
    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileValue)];
        const Sc::Terrain::TileFlags::MiniTileFlags & miniTileFlags = tiles.tileFlags[megaTileIndex].miniTileFlags[miniTileY][miniTileX];
        u8 red = 0, blue = 0, green = 100;
        switch ( miniTileFlags.getElevation() )
        {
            //case Sc::Terrain::TileElevation::Low: break;
            case Sc::Terrain::TileElevation::Mid: blue = 100; green = 0; break;
            case Sc::Terrain::TileElevation::High: red = 100; green = 0; break;
        }

        for ( s64 yc=0; yc<8; yc++ )
        {
            for ( s64 xc = 0; xc < 8; xc++ )
                graphicBits[size_t(yc * 8 + xc)] = Sc::SystemColor(red, green, blue);
        }
    }
    dc.setBitsToDevice(int(xOffset), int(yOffset), 8, 8, 0, 0, 0, 8, &graphicBits[0], &bmi);
}

void DrawTileElevation(const WinLib::DeviceContext & dc, const Sc::Terrain::Tiles & tiles, s16 xOffset, s16 yOffset, u16 tileValue, BITMAPINFO & bmi)
{
    ChkdBitmap graphicBits;
    graphicBits.resize(1024);

    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileValue)];
        const Sc::Terrain::TileFlags & tileFlags = tiles.tileFlags[megaTileIndex];
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                const Sc::Terrain::TileFlags::MiniTileFlags & miniTileFlags = tileFlags.miniTileFlags[yMiniTile][xMiniTile];
                const Sc::Terrain::TileElevation elevation = miniTileFlags.getElevation();

                u8 red = 0, blue = 0, green = 100;
                switch ( elevation )
                {
                    case Sc::Terrain::TileElevation::Mid: blue = 100; green = 0; break;
                    case Sc::Terrain::TileElevation::High: red = 100; green = 0; break;
                }

                if ( !miniTileFlags.isWalkable() )
                {
                    red /= 3;
                    green /= 3;
                    blue /= 3;
                }

                size_t miniTileYC = yMiniTile*(8),
                    miniTileXC = xMiniTile*(8);

                for ( u32 yc=0; yc<8; yc++ )
                {
                    for ( u32 xc = 0; xc < 8; xc++ )
                        graphicBits[(yc + miniTileYC) * 32 + xc + miniTileXC] = Sc::SystemColor(red, green, blue);
                }
            }
        }
    }
    dc.setBitsToDevice(xOffset, yOffset, 32, 32, 0, 0, 0, 32, &graphicBits[0], &bmi);
}

void DrawTile(const WinLib::DeviceContext & dc, ChkdPalette & palette, const Sc::Terrain::Tiles & tiles, s16 xOffset, s16 yOffset, u16 TileValue, BITMAPINFO & bmi, u8 redOffset, u8 greenOffset, u8 blueOffset)
{
    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(TileValue);
    if ( groupIndex < tiles.tileGroups.size() )
    {
        ChkdBitmap graphicBits;
        graphicBits.resize(1024);

        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
        const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(TileValue)];
        const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[megaTileIndex];
        for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
        {
            s64 yMiniOffset = yMiniTile*8;
            for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
            {
                const Sc::Terrain::TileGraphicsEx::MiniTileGraphics & miniTileGraphics = tileGraphics.miniTileGraphics[yMiniTile][xMiniTile];
                bool flipped = miniTileGraphics.isFlipped();
                size_t vr4Index = size_t(miniTileGraphics.vr4Index());
                
                s64 xMiniOffset = xMiniTile*8;
                for ( s64 yMiniPixel = 0; yMiniPixel < 8; yMiniPixel++ )
                {
                    for ( s64 xMiniPixel = 0; xMiniPixel < 8; xMiniPixel++ )
                    {
                        const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                        const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                        graphicBits[size_t((yMiniOffset+yMiniPixel)*32 + (xMiniOffset+xMiniPixel))] = Sc::SystemColor(palette[wpeIndex], redOffset, greenOffset, blueOffset);
                    }
                }
            }
        }
        dc.setBitsToDevice(xOffset, yOffset, 32, 32, 0, 0, 0, 32, &graphicBits[0], &bmi);
    }
}

void DrawTools(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop,
    Selections & selections, bool pasting, Clipboard & clipboard, GuiMap & map)
{
    if ( map.getLayer() == Layer::Terrain && selections.hasTiles() ) // Draw selected tiles
        DrawTileSel(dc, palette, width, height, screenLeft, screenTop, selections, map);
    else if ( map.getLayer() == Layer::Locations ) // Draw Location Creation/Movement Graphics
        DrawTempLocs(dc, screenLeft, screenTop, selections, map);
    else if ( map.getLayer() == Layer::Doodads ) // Draw selected doodads
        DrawDoodadSel(dc, width, height, screenLeft, screenTop, selections, map);

    if ( pasting || map.getLayer() == Layer::FogEdit ) // Draw paste graphics
        DrawPasteGraphics(dc, palette, width, height, screenLeft, screenTop, selections, clipboard, map, map.getLayer(), map.getSubLayer());
}

void DrawTileSel(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map)
{
    dc.setPen(PS_SOLID, 0, RGB(0, 255, 255));
    RECT rect, rcBorder;
    
    Sc::Terrain::Tileset tileset = map.getTileset();
    const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);

    BITMAPINFO bmi = GetBMI(32, 32);
    rcBorder.left   = (0      + screenLeft)/32 - 1;
    rcBorder.right  = (width  + screenLeft)/32 + 1;
    rcBorder.top    = (0      + screenTop)/32 - 1;
    rcBorder.bottom = (height + screenTop)/32 + 1;

    std::vector<TileNode> & selectedTiles = selections.getTiles();
    for ( auto & tile : selectedTiles )
    {
        if ( tile.xc > rcBorder.left &&
             tile.xc < rcBorder.right &&
             tile.yc > rcBorder.top &&
             tile.yc < rcBorder.bottom )
        // If tile is within current map border
        {
            // Draw tile with a blue haze, might replace with blending
                DrawTile( dc,
                          palette,
                          tiles,
                          s16(tile.xc*32-screenLeft),
                          s16(tile.yc*32-screenTop),
                          tile.value,
                          bmi,
                          0,
                          0,
                          32
                        );

            if ( tile.neighbors != TileNeighbor::None ) // if any edges need to be drawn
            {
                rect.left   = tile.xc*32 - screenLeft;
                rect.right  = tile.xc*32 - screenLeft + 32;
                rect.top    = tile.yc*32 - screenTop;
                rect.bottom = tile.yc*32 - screenTop + 32;

                if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                {
                    dc.moveTo(rect.left, rect.top);
                    dc.lineTo(rect.right, rect.top);
                }
                if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                {
                    if ( rect.right >= width )
                        rect.right --;

                    dc.moveTo(rect.right, rect.top);
                    dc.lineTo(rect.right, rect.bottom+1);
                }
                if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                {
                    if ( rect.bottom >= height )
                        rect.bottom --;

                    dc.moveTo(rect.left, rect.bottom);
                    dc.lineTo(rect.right, rect.bottom);
                }
                if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                {
                    dc.moveTo(rect.left, rect.bottom);
                    dc.lineTo(rect.left, rect.top-1);
                }
            }
        }
    }
}

void DrawDoodadSel(const WinLib::DeviceContext & dc, u16 width, u16 height, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map)
{
    dc.setPen(PS_SOLID, 0, RGB(255, 0, 0));
    const auto & selDoodads = selections.getDoodads();
    for ( auto index : selDoodads )
    {
        const auto & selDoodad = map.getDoodad(index);
        const auto & tileset = chkd.scData.terrain.get(CM->getTileset());
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(selDoodad.type) )
        {
            const auto & doodad = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[size_t(*doodadGroupIndex)];
            s32 doodadWidth = 32*s32(doodad.tileWidth);
            s32 doodadHeight = 32*s32(doodad.tileHeight);
            s32 left = s32(selDoodad.xc) - doodadWidth/2;
            s32 top = s32(selDoodad.yc) - doodadHeight/2;
            s32 right = s32(selDoodad.xc) + doodadWidth/2;
            s32 bottom = s32(selDoodad.yc) + doodadHeight/2;

            s32 screenRight = s32(screenLeft)+width;
            s32 screenBottom = s32(screenTop)+height;
            bool inBounds = left <= screenRight && top <= screenBottom && right >= s32(screenLeft) && bottom >= s32(screenTop);
            if ( inBounds )
            {            
                left -= screenLeft;
                right -= screenLeft;
                top -= screenTop;
                bottom -= screenTop;
                dc.moveTo(left, top);
                dc.lineTo(right, top);
                dc.lineTo(right, bottom);
                dc.lineTo(left, bottom);
                dc.lineTo(left, top);
            }
        }
    }
}

void DrawPasteGraphics(const WinLib::DeviceContext & dc, ChkdPalette & palette, u16 width, u16 height, u32 screenLeft, u32 screenTop,
                        Selections & selections, Clipboard & clipboard, GuiMap & map, Layer layer, TerrainSubLayer subLayer)
{
    if ( layer == Layer::Terrain )
    {
        if ( subLayer == TerrainSubLayer::Isom )
        {
            dc.setPen(PS_SOLID, 0, RGB(255, 0, 0));
            auto diamond = Chk::IsomDiamond::fromMapCoordinates(selections.getEndDrag().x, selections.getEndDrag().y);

            s32 diamondCenterX = s32(diamond.x)*64-screenLeft;
            s32 diamondCenterY = s32(diamond.y)*32-screenTop;
            dc.moveTo(diamondCenterX-64, diamondCenterY); // Start from diamond left corner
            dc.lineTo(diamondCenterX, diamondCenterY-32); // Draw top-left
            dc.lineTo(diamondCenterX+64, diamondCenterY); // Draw top-right
            dc.lineTo(diamondCenterX, diamondCenterY+32); // Draw bottom-right
            dc.lineTo(diamondCenterX-64, diamondCenterY); // Draw bottom-left
        }
        else if ( subLayer == TerrainSubLayer::Rectangular )
        {
            dc.setPen(PS_SOLID, 0, RGB(0, 255, 255));
    
            RECT rect, rcShade;
            Sc::Terrain::Tileset tileset = map.getTileset();
            const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);
    
            BITMAPINFO bmi = GetBMI(32, 32);
            rcShade.left   = -32;
            rcShade.right  = width  + 32;
            rcShade.top    = -32;
            rcShade.bottom = height + 32;
    
            POINT center;
            center.x = selections.getEndDrag().x + 16;
            center.y = selections.getEndDrag().y + 16;
    
            std::vector<PasteTileNode> & pasteTiles = clipboard.getTiles();
            for ( auto & tile : pasteTiles )
            {
                rect.left   = ( tile.xc + center.x      )/32*32 - screenLeft;
                rect.top    = ( tile.yc + center.y      )/32*32 - screenTop;
                rect.right  = ( tile.xc + 32 + center.x )/32*32 - screenLeft;
                rect.bottom = ( tile.yc + 32 + center.y )/32*32 - screenTop;

                if ( rect.left > rcShade.left && rect.left < rcShade.right && rect.top > rcShade.top && rect.top < rcShade.bottom )
                // If tile is within current map border
                {
                    // Draw tile with a blue haze
                    DrawTile(dc, palette, tiles, (s16)rect.left, (s16)rect.top, tile.value, bmi, 0, 0, 32);

                    if ( tile.neighbors != TileNeighbor::None ) // if any edges need to be drawn
                    {
                        if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                        {
                            dc.moveTo(rect.left, rect.top);
                            dc.lineTo(rect.right, rect.top);
                        }
                        if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                        {
                            if ( rect.right >= width )
                                rect.right --;
    
                            dc.moveTo(rect.right, rect.top);
                            dc.lineTo(rect.right, rect.bottom+1);
                        }
                        if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                        {
                            if ( rect.bottom >= height )
                                rect.bottom --;

                            dc.moveTo(rect.left, rect.bottom);
                            dc.lineTo(rect.right, rect.bottom);
                        }
                        if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                        {
                            dc.moveTo(rect.left, rect.bottom);
                            dc.lineTo(rect.left, rect.top-1);
                        }
                    }
                }
            }
        }
    }
    else if ( layer == Layer::Doodads )
    {
        const auto & doodads = clipboard.getDoodads();
        if ( !doodads.empty() )
        {
            ChkdBitmap graphicBits {};
            graphicBits.resize(((size_t)width)*((size_t)height));

            BITMAPINFO bmi = GetBMI(width, height);
            dc.getDiBits(0, height, &graphicBits[0], &bmi);

            const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(map.getTileset());
            POINT center { selections.getEndDrag().x, selections.getEndDrag().y };
            for ( auto & doodad : doodads )
            {
                auto tileWidth = doodad.tileWidth;
                auto tileHeight = doodad.tileHeight;
                bool evenWidth = tileWidth%2 == 0;
                bool evenHeight = tileHeight%2 == 0;
                auto xStart = evenWidth ? -16*doodad.tileWidth - screenLeft + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) - screenLeft + (center.x+doodad.tileX*32)/32*32;
                auto yStart = evenHeight ? -16*doodad.tileHeight - screenTop + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) - screenTop + (center.y+doodad.tileY*32)/32*32;
                auto xTileStart = (screenLeft+xStart)/32;
                auto yTileStart = (screenTop+yStart)/32;
                const auto & placability = tiles.doodadPlacibility[doodad.doodadId];
                if ( xStart < width && yStart < height )
                {
                    for ( u16 y=0; y<tileHeight; ++y )
                    {
                        for ( u16 x=0; x<tileWidth; ++x )
                        {
                            if ( doodad.tileIndex[x][y] != 0 )
                            {
                                if ( placability.tileGroup[y*tileWidth+x] == 0 )
                                    TileToBits(graphicBits, palette, tiles, xStart+32*x, yStart+32*y, width, height, doodad.tileIndex[x][y]);
                                else
                                {
                                    size_t tileXc = xTileStart+x;
                                    size_t tileYc = yTileStart+y;
                                    if ( tileXc < map.dimensions.tileWidth && tileYc < map.dimensions.tileHeight )
                                    {
                                        u16 existingTileGroup = map.getTile(tileXc, tileYc) / 16;
                                        if ( existingTileGroup == placability.tileGroup[y*tileWidth+x] )
                                            TileToBits(graphicBits, palette, tiles, xStart+32*x, yStart+32*y, width, height, doodad.tileIndex[x][y], 0, 0, 25);
                                        else
                                            TileToBits(graphicBits, palette, tiles, xStart+32*x, yStart+32*y, width, height, doodad.tileIndex[x][y], 100, 0, 0);
                                    }
                                }
                            }
                        }
                    }
                    if ( doodad.overlayIndex != 0 )
                    {
                        if ( doodad.isSprite() )
                        {
                            SpriteToBits(graphicBits, palette, Chk::PlayerColor::Azure_NeutralColor, width, height,
                                0, 0, doodad.overlayIndex, u16(xStart+tileWidth*16), u16(yStart+tileHeight*16), doodad.overlayFlipped(), false);
                        }
                        else // Overlay is unit index
                        {
                            auto spriteIndex = chkd.scData.units.getFlingy(chkd.scData.units.getUnit(Sc::Unit::Type(doodad.overlayIndex)).graphics).sprite;
                            SpriteToBits(graphicBits, palette, Chk::PlayerColor::Azure_NeutralColor, width, height,
                                0, 0, spriteIndex, u16(xStart+tileWidth*16), u16(yStart+tileHeight*16), doodad.overlayFlipped(), false);
                        }
                    }
                }
            }

            dc.setBitsToDevice(0, 0, width, height, 0, 0, 0, height, &graphicBits[0], &bmi);
        }
    }
    else if ( layer == Layer::Units )
    {
        ChkdBitmap graphicBits;
        graphicBits.resize(((size_t)width)*((size_t)height));

        BITMAPINFO bmi = GetBMI(width, height);
        dc.getDiBits(0, height, &graphicBits[0], &bmi);

        u16 tileset = map.getTileset();

        s32 sScreenLeft = screenLeft;
        s32 sScreenTop = screenTop;

        POINT cursor = selections.getEndDrag();
        if ( cursor.x != -1 && cursor.y != -1 )
        {
            std::vector<PasteUnitNode> units = clipboard.getUnits();
            for ( auto & pasteUnit : units )
            {
                Chk::PlayerColor color = (pasteUnit.unit.owner < Sc::Player::TotalSlots ?
                    map.getPlayerColor(pasteUnit.unit.owner) : (Chk::PlayerColor)pasteUnit.unit.owner);
                if ( cursor.y+ pasteUnit.yc >= 0 )
                {
                    UnitToBits(graphicBits, palette, color, width, height, sScreenLeft, sScreenTop,
                        (u16)pasteUnit.unit.type, u16(cursor.x+ pasteUnit.xc), u16(cursor.y+ pasteUnit.yc), 0, false );
                }
            }
        }

        dc.setBitsToDevice(0, 0, width, height, 0, 0, 0, height, &graphicBits[0], &bmi);
    }
    else if ( layer == Layer::Sprites )
    {
        ChkdBitmap graphicBits;
        graphicBits.resize(((size_t)width)*((size_t)height));

        BITMAPINFO bmi = GetBMI(width, height);
        dc.getDiBits(0, height, &graphicBits[0], &bmi);

        u16 tileset = map.getTileset();

        s32 sScreenLeft = screenLeft;
        s32 sScreenTop = screenTop;

        POINT cursor = selections.getEndDrag();
        if ( cursor.x != -1 && cursor.y != -1 )
        {
            std::vector<PasteSpriteNode> sprites = clipboard.getSprites();
            for ( auto & pasteSprite : sprites )
            {
                Chk::PlayerColor color = (pasteSprite.sprite.owner < Sc::Player::TotalSlots ?
                    map.getPlayerColor(pasteSprite.sprite.owner) : (Chk::PlayerColor)pasteSprite.sprite.owner);

                if ( cursor.y + pasteSprite.yc >= 0 )
                {
                    SpriteToBits(graphicBits, palette, color, width, height, sScreenLeft, sScreenTop,
                        (u16)pasteSprite.sprite.type, u16(cursor.x + pasteSprite.xc), u16(cursor.y + pasteSprite.yc), false, false);
                }
            }
        }

        dc.setBitsToDevice(0, 0, width, height, 0, 0, 0, height, &graphicBits[0], &bmi);
    }
    else if ( layer == Layer::FogEdit )
    {
        const auto brushWidth = clipboard.getFogBrush().width;
        const auto brushHeight = clipboard.getFogBrush().height;
        s32 hoverTileX = (selections.getEndDrag().x + (brushWidth % 2 == 0 ? 16 : 0))/32;
        s32 hoverTileY = (selections.getEndDrag().y + (brushHeight % 2 == 0 ? 16 : 0))/32;

        const auto startX = 32*(hoverTileX - brushWidth/2) - screenLeft;
        const auto startY = 32*(hoverTileY - brushHeight/2) - screenTop;
        const auto endX = startX+32*brushWidth;
        const auto endY = startY+32*brushHeight;

        dc.setPen(PS_SOLID, 0, RGB(0, 255, 255));
        dc.moveTo(startX, startY); // From top-right...
        dc.lineTo(endX, startY); // Draw top
        dc.lineTo(endX, endY); // Draw right
        dc.lineTo(startX, endY); // Draw bottom
        dc.lineTo(startX, startY); // Draw left
    }
}

void DrawTempLocs(const WinLib::DeviceContext & dc, u32 screenLeft, u32 screenTop, Selections & selections, GuiMap & map)
{
    POINT start = selections.getStartDrag();
    POINT end = selections.getEndDrag();
    if ( selections.getLocationFlags() == LocSelFlags::None ) // Draw location creation preview
    {
        s32 left = start.x-screenLeft,
            top = start.y-screenTop,
            right = end.x-screenLeft,
            bottom = end.y-screenTop;
        DrawLocationFrame(dc, start.x-screenLeft, start.y-screenTop, end.x-screenLeft, end.y-screenTop);
    }
    else 
    {
        u16 selectedLocation = selections.getSelectedLocation();
        if ( selectedLocation != NO_LOCATION && selectedLocation < map.numLocations() ) // Draw location resize/movement graphics
        {
            const Chk::Location & loc = map.getLocation((size_t)selectedLocation);
            s32 locLeft = loc.left-screenLeft;
            s32 locRight = loc.right-screenLeft;
            s32 locTop = loc.top-screenTop;
            s32 locBottom = loc.bottom-screenTop;
            s32 dragX = end.x-start.x;
            s32 dragY = end.y-start.y;

            LocSelFlags locFlags = selections.getLocationFlags();
            if ( locFlags != LocSelFlags::Middle )
            {
                if ( locTop > locBottom )
                {
                    if ( (locFlags & LocSelFlags::North) == LocSelFlags::North )
                        locFlags = LocSelFlags(locFlags&(~LocSelFlags::North)|LocSelFlags::South);
                    else if ( (locFlags & LocSelFlags::South) == LocSelFlags::South )
                        locFlags = LocSelFlags(locFlags&(~LocSelFlags::South)|LocSelFlags::North);
                }

                if ( locLeft > locRight )
                {
                    if ( (locFlags & LocSelFlags::West) == LocSelFlags::West )
                        locFlags = LocSelFlags(locFlags&(~LocSelFlags::West)|LocSelFlags::East);
                    else if ( (locFlags & LocSelFlags::East) == LocSelFlags::East )
                        locFlags = LocSelFlags(locFlags&(~LocSelFlags::East)|LocSelFlags::West);
                }
            }

            switch ( locFlags )
            {
                case LocSelFlags::North: DrawLocationFrame(dc, locLeft, locTop+dragY, locRight, locBottom); break;
                case LocSelFlags::South: DrawLocationFrame(dc, locLeft, locTop, locRight, locBottom+dragY); break;
                case LocSelFlags::East: DrawLocationFrame(dc, locLeft, locTop, locRight+dragX, locBottom); break;
                case LocSelFlags::West: DrawLocationFrame(dc, locLeft+dragX, locTop, locRight, locBottom); break;
                case LocSelFlags::NorthWest: DrawLocationFrame(dc, locLeft+dragX, locTop+dragY, locRight, locBottom); break;
                case LocSelFlags::NorthEast: DrawLocationFrame(dc, locLeft, locTop+dragY, locRight+dragX, locBottom); break;
                case LocSelFlags::SouthWest: DrawLocationFrame(dc, locLeft+dragX, locTop, locRight, locBottom+dragY); break;
                case LocSelFlags::SouthEast: DrawLocationFrame(dc, locLeft, locTop, locRight+dragX, locBottom+dragY); break;
                case LocSelFlags::Middle:
                    DrawLocationFrame(dc, locLeft+dragX, locTop+dragY, locRight+dragX, locBottom+dragY);
                    break;
            }
        }
    }
}

void DrawSelectingFrame(const WinLib::DeviceContext & dc, Selections & selections, u32 screenLeft, u32 screenTop, s32 width,  s32 height, double scale)
{
    if ( !selections.selectionAreaIsNull() )
    {
        POINT startDrag = selections.getStartDrag(),
              endDrag = selections.getEndDrag();

        RECT rect;
        rect.left   = s32(((s32)startDrag.x-(s32)screenLeft)/**scale*/)  ;
        rect.right  = s32(((s32)  endDrag.x-(s32)screenLeft)/**scale*/)+1;
        rect.top    = s32(((s32)startDrag.y-(s32)screenTop )/**scale*/)  ;
        rect.bottom = s32(((s32)  endDrag.y-(s32)screenTop )/**scale*/)+1;
    
        if ( rect.right < rect.left )
        {
            s32 xSave   = rect.left     ;
            rect.left   = rect.right - 1;
            rect.right  = xSave      + 1; 
        }
        if ( rect.bottom < rect.top )
        {
            s32 ySave   = rect.top       ;
            rect.top    = rect.bottom - 1;
            rect.bottom = ySave       + 1;
        }
        if ( rect.right >= width )
            rect.right --;
        if ( rect.bottom >= height )
            rect.bottom --;

        dc.frameRect(rect, RGB(255, 0, 0));
    }
}

void DrawLocationFrame(const WinLib::DeviceContext & dc, s32 left, s32 top, s32 right, s32 bottom)
{
    RECT rect;
    if ( left < right ) {
        rect.left = left;
        rect.right = right;
    } else {
        rect.left = right;
        rect.right = left;
    } if ( top < bottom ) {
        rect.top = top;
        rect.bottom = bottom;
    } else {
        rect.top = bottom;
        rect.bottom = top;
    }

    dc.frameRect(rect, RGB(255, 0, 0));

    rect.left --;
    rect.top --;
    rect.right ++;
    rect.bottom ++;

    dc.frameRect(rect, RGB(255, 0, 0));
}

void DrawMiniMapTiles(ChkdBitmap & bitmap, const ChkdPalette & palette, s64 bitWidth, s64 bitHeight, s64 xSize, s64 ySize,
                       s64 xOffset, s64 yOffset, float scale, const Sc::Terrain::Tiles & tiles, GuiMap & map )
{
    for ( s64 yc=0; yc<128-2*yOffset; yc++ ) // Cycle through all minimap pixel rows
    {
        s64 yMiniTile = 0;
        s64 yTile = (s64)(yc/scale); // Get the yc of tile used for the pixel
        if ( scale > 1 )
        {
            yMiniTile = yc%(int)scale; // Get the y-minitile used for the pixel
            if ( yMiniTile > 3 )
                yMiniTile %= 4; // Correct for invalid y-minitiles
        }
    
        for ( s64 xc=0; xc<128-2*xOffset; xc++ ) // Cycle through all minimap pixel columns
        {
            s64 xMiniTile = 0;
            s64 xTile = (s64)(xc/scale); // Get the xc of the tile used for the pixel
            if ( scale > 1 )
            {
                xMiniTile = xc%(int)scale; // Get the x-minitile used for the pixel
                if ( xMiniTile > 3 )
                    xMiniTile %= 4; // Correct for invalid x-minitiles
            }

            u16 tileIndex = map.getTile(size_t(xTile), size_t(yTile));
            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
            if ( groupIndex < tiles.tileGroups.size() )
            {
                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                const u16 & megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[megaTileIndex];

                const size_t vr4Index = size_t(tileGraphics.miniTileGraphics[yMiniTile][xMiniTile].vr4Index());
                const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                const u8 & wpeIndex = miniTilePixels.wpeIndex[6][7];
                
                bitmap[size_t((yc + yOffset) * 128 + xc + xOffset)] = palette[wpeIndex];
            }
        }
    }
}

#define MINI_MAP_MAXBIT 65536 // Maximum graphicBits position

void DrawMiniMapUnits(ChkdBitmap & bitmap, u16 bitWidth, u16 bitHeight, u16 xSize, u16 ySize,
                       u16 xOffset, u16 yOffset, float scale, const Sc::Terrain::Tiles & tiles, GuiMap & map )
{
    for ( const auto & unit : map.units )
    {
        Chk::PlayerColor color = (unit.owner < Sc::Player::TotalSlots ?
            map.getPlayerColor(unit.owner) : (Chk::PlayerColor)unit.owner);

        if ( color > Chk::TotalColors )
            color = Chk::PlayerColor(color % Chk::TotalColors);

        u32 bitIndex = (
            ((u32)((unit.yc / 32)*scale) + yOffset) * 128
            + (u32)((unit.xc / 32)*scale) + xOffset
            );

        if ( bitIndex < MINI_MAP_MAXBIT )
            bitmap[bitIndex] = chkd.scData.tminimap.palette[color];
    }
    
    for ( const auto & sprite : map.sprites )
    {
        if ( sprite.isDrawnAsSprite() )
        {
            Chk::PlayerColor color = (sprite.owner < Sc::Player::TotalSlots ?
                map.getPlayerColor(sprite.owner) : Chk::PlayerColor(sprite.owner%16));

            if ( color > 16 )
                color = Chk::PlayerColor(color % 16);

            u32 bitIndex = (((u32)((sprite.yc / 32)*scale) + yOffset) * 128
                + (u32)((sprite.xc / 32)*scale) + xOffset);

            if ( bitIndex < MINI_MAP_MAXBIT )
                bitmap[bitIndex] = chkd.scData.tminimap.palette[color];
        }
    }
}

void DrawMiniMapFog(ChkdBitmap & bitmap, const ChkdPalette & palette, s64 bitWidth, s64 bitHeight, s64 xSize, s64 ySize,
                       s64 xOffset, s64 yOffset, float scale, const Sc::Terrain::Tiles & tiles, GuiMap & map)
{
    u8 currPlayer = map.getCurrPlayer();
    if ( currPlayer >= 8 )
        currPlayer = 0;

    u8 currPlayerMask = u8Bits[currPlayer];
    for ( s64 yc=0; yc<128-2*yOffset; yc++ ) // Cycle through all minimap pixel rows
    {
        s64 yTile = (s64)(yc/scale); // Get the yc of tile used for the pixel
        for ( s64 xc=0; xc<128-2*xOffset; xc++ ) // Cycle through all minimap pixel columns
        {
            s64 xTile = (s64)(xc/scale); // Get the xc of the tile used for the pixel

            u8 fog = map.getFog(size_t(xTile), size_t(yTile));
            if ( (fog & currPlayerMask) != 0 )
                bitmap[size_t((yc + yOffset) * 128 + xc + xOffset)].darken();
        }
    }
}

void DrawMiniMap(const WinLib::DeviceContext & dc, const ChkdPalette & palette, u16 xSize, u16 ySize, float scale, GuiMap & map)
{
    ChkdBitmap graphicBits;
    graphicBits.resize(65536);

    BITMAPINFO bmi = GetBMI(128, 128);

    u16 xOffset = (u16)((128-xSize*scale)/2),
        yOffset = (u16)((128-ySize*scale)/2);
    
    Sc::Terrain::Tileset tileset = map.getTileset();
    const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(tileset);
    DrawMiniMapTiles(graphicBits, palette, 128, 128, xSize, ySize, xOffset, yOffset, scale, tiles, map);
    DrawMiniMapUnits(graphicBits, 128, 128, xSize, ySize, xOffset, yOffset, scale, tiles, map);
    if ( map.getLayer() == Layer::FogEdit )
        DrawMiniMapFog(graphicBits, palette, 128, 128, xSize, ySize, xOffset, yOffset, scale, tiles, map);
    dc.setBitsToDevice(xOffset, yOffset, 128-2*xOffset, 128-2*yOffset, xOffset, yOffset, 0, 128, &graphicBits[0], &bmi);

    // Draw Map Borders
    dc.setPen(PS_SOLID, 0, RGB(255, 255, 255));
    dc.moveTo(xOffset-1, yOffset-1);
    dc.lineTo(128-xOffset, yOffset-1);
    dc.lineTo(128-xOffset, 128-yOffset);
    dc.lineTo(xOffset-1, 128-yOffset);
    dc.lineTo(xOffset-1, yOffset-1);
}

void DrawMiniMapBox(const WinLib::DeviceContext & dc, u32 screenLeft, u32 screenTop, u16 screenWidth, u16 screenHeight, u16 xSize, u16 ySize, float scale)
{
    u16 xOffset = (u16)((128-xSize*scale)/2),
        yOffset = (u16)((128-ySize*scale)/2);

    // Draw Current Area Box
    u16 X1 = (u16)((screenLeft+16)/32*scale + xOffset),
        Y1 = (u16)((screenTop +16)/32*scale + yOffset),

        X2 = (u16)(((screenLeft+16)/32
            + ((screenWidth - 16)/32)+1)*scale + xOffset),

        Y2 = (u16)(((screenTop +16)/32
            + ((screenHeight - 16)/32)+1)*scale + yOffset);

    if ( X2 > 127-xOffset )
        X2 = 127-xOffset;

    if ( Y2 > 127-yOffset )
        Y2 = 127-yOffset;

    dc.setPen(PS_SOLID, 0, RGB(255, 255, 255));
    dc.moveTo(X1, Y1);
    dc.lineTo(X2, Y1);
    dc.lineTo(X2, Y2);
    dc.lineTo(X1, Y2);
    dc.lineTo(X1, Y1);
}

void DrawStringLine(const WinLib::DeviceContext & dc, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str)
{
    COLORREF lastColor = defaultColor;
    const char* cStr = str.c_str();
    size_t size = str.size();
    size_t chunkStartChar = 0;
    bool center = false,
         right = false;
    for ( size_t i=0; i<size; i++ )
    {
        if ( u8(cStr[i]) < 32 && cStr[i] != '\11' ) // Not tab, must be color or alignment
        {
            if ( i > chunkStartChar ) // Output everything prior to this...
            {
                std::string chunk = str.substr(chunkStartChar, i-chunkStartChar);
                if ( center )
                {
                    UINT chunkWidth, chunkHeight;
                    if ( GetStringDrawSize(dc, chunkWidth, chunkHeight, chunk) )
                    {
                        xPos = UINT(width)/2-chunkWidth/2;
                        if ( xPos >= UINT(width) )
                            xPos = 0;
                    }
                }
                else if ( right )
                {
                    UINT chunkWidth, chunkHeight;
                    if ( GetStringDrawSize(dc, chunkWidth, chunkHeight, chunk) )
                    {
                        xPos = UINT(width)-chunkWidth-1;
                        if ( xPos >= UINT(width) )
                            xPos = 0;
                    }
                }
                dc.drawTabbedText(chunk, xPos, yPos); // Output everything before the color/alignment
                xPos += UINT(dc.getTextWidth(chunk));
                chunkStartChar = i+1;
            }
            else if ( i == chunkStartChar )
                chunkStartChar ++;

            if ( cStr[i] < '\x20' ) // Possible special character, change color/alignment
            {
                switch ( ((u8)cStr[i]) )
                {
                    case 0x01: lastColor = RGB(184, 184, 232); break;
                    case 0x02: lastColor = RGB(184, 184, 232); break;
                    case 0x03: lastColor = RGB(220, 220,  60); break;
                    case 0x04: lastColor = RGB(255, 255, 255); break;
                    case 0x05: lastColor = RGB(132, 116, 116); break;
                    case 0x06: lastColor = RGB(200,  24,  24); break;
                    case 0x07: lastColor = RGB( 16, 252,  24); break;
                    case 0x08: lastColor = RGB(244,   4,   4); break;
                    case 0x0B: lastColor = defaultColor; break;
                    case 0x0C: lastColor = defaultColor; break;
                    case 0x0E: lastColor = RGB( 12,  72, 204); break;
                    case 0x0F: lastColor = RGB( 44, 180, 148); break;
                    case 0x10: lastColor = RGB(136,  64, 156); break;
                    case 0x11: lastColor = RGB(248, 140,  20); break;
                    case 0x12: right = true; center = false; break;
                    case 0x13: if ( !right ) center = true; break;
                    case 0x14: lastColor = defaultColor; break;
                    case 0x15: lastColor = RGB(112,  48,  20); break;
                    case 0x16: lastColor = RGB(204, 224, 208); break;
                    case 0x17: lastColor = RGB(252, 252,  56); break;
                    case 0x18: lastColor = RGB(  8, 128,   8); break;
                    case 0x19: lastColor = RGB(252, 252, 124); break;
                    case 0x1A: lastColor = RGB(184, 184, 232); break;
                    case 0x1B: lastColor = RGB(236, 196, 176); break;
                    case 0x1C: lastColor = RGB( 64, 104, 212); break;
                    case 0x1D: lastColor = RGB(116, 164, 124); break;
                    case 0x1E: lastColor = RGB(144, 144, 184); break;
                    case 0x1F: lastColor = RGB(  0, 228, 252); break;
                }
                dc.setTextColor(lastColor);
            }
        }
    }

    if ( chunkStartChar < size )
    {
        std::string chunk = str.substr(chunkStartChar, size-chunkStartChar);
        if ( center )
        {
            UINT chunkWidth, chunkHeight;
            if ( GetStringDrawSize(dc, chunkWidth, chunkHeight, chunk) )
            {
                xPos = UINT(width)/2-chunkWidth/2;
                if ( xPos >= UINT(width) )
                    xPos = 0;
            }
        }
        else if ( right )
        {
            UINT chunkWidth, chunkHeight;
            if ( GetStringDrawSize(dc, chunkWidth, chunkHeight, chunk) )
            {
                xPos = (UINT(width))-chunkWidth-1;
                if ( (LONG(xPos)) >= width )
                    xPos = 0;
            }
        }
        dc.drawTabbedText(chunk, xPos, yPos);
    }
}

bool GetStringDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, const std::string & str)
{
    // This method is very time sensative and should be optimized as much as possible
    width = 0;
    height = 0;
    if ( str.empty() )
        return false;

    s32 lineWidth = 0, lineHeight = 0;
    
    constexpr icux::codepoint carriageReturn = (icux::codepoint)'\r';
    constexpr icux::codepoint lineFeed = (icux::codepoint)'\n';
    constexpr icux::codepoint emptyLine[] = { carriageReturn, lineFeed, icux::nullChar };
    constexpr size_t emptyLineLength = sizeof(emptyLine)/sizeof(icux::codepoint);
    
    auto sysStr = icux::toUistring(str);
    const size_t codepointCount = sysStr.length();
    size_t lineStart = 0;
    icux::codepoint* rawStr = &sysStr[0];
    icux::codepoint* currLine = rawStr;

    for ( size_t i=0; i<codepointCount; i++ )
    {
        if ( rawStr[i] == carriageReturn && i+1 < codepointCount && rawStr[i+1] == lineFeed ) // Line ending found
        {
            rawStr[i] = icux::nullChar; // Null-terminate currLine, replacing the carriageReturn
            if ( dc.getTabTextExtent(currLine, i-lineStart, lineWidth, lineHeight) || // Regular line
                 (i == lineStart && dc.getTabTextExtent(&emptyLine[0], emptyLineLength, lineWidth, lineHeight)) ) // Empty line
            {
                height += lineHeight;
                if ( lineWidth > s32(width) )
                    width = UINT(lineWidth);
            }
            else
                return false;

            i++; // Advance from position of carriageReturn to position of lineFeed
            lineStart = i+1;
            currLine = lineStart < codepointCount ? &rawStr[lineStart] : nullptr; // Set currLine to begin at the character after lineFeed if present
        }
    }

    // If currLine != nullptr, then either the last character(s) were not line endings or there were no line endings, process last line
    if ( currLine != nullptr )
    {
        if ( !dc.getTabTextExtent(currLine, codepointCount-lineStart, lineWidth, lineHeight) )
            return false;

        height += lineHeight;
        if ( lineWidth > s32(width) )
            width = UINT(lineWidth);
    }
    return true;
}

bool GetStringDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, const std::string & str, std::unordered_multimap<size_t, WinLib::LineSize> & lineCache)
{
    // This method is very time sensative and should be optimized as much as possible
    width = 0;
    height = 0;
    if ( str.empty() )
        return false;

    s32 lineWidth = 0, lineHeight = 0;
    
    constexpr icux::codepoint carriageReturn = (icux::codepoint)'\r';
    constexpr icux::codepoint lineFeed = (icux::codepoint)'\n';
    
    auto sysStr = icux::toUistring(str);
    const size_t codepointCount = sysStr.length();
    size_t lineStart = 0;
    icux::codepoint* rawStr = &sysStr[0];
    icux::codepoint* currLine = rawStr;

    for ( size_t i=0; i<codepointCount; i++ )
    {
        if ( rawStr[i] == carriageReturn && i+1 < codepointCount && rawStr[i+1] == lineFeed ) // Line ending found
        {
            rawStr[i] = icux::nullChar; // Null-terminate currLine, replacing the carriageReturn
            if ( dc.getTabTextExtent(currLine, i-lineStart, lineWidth, lineHeight, lineCache) )
            {
                height += lineHeight;
                if ( lineWidth > s32(width) )
                    width = UINT(lineWidth);
            }
            else
                return false;

            i++; // Advance from position of carriageReturn to position of lineFeed
            lineStart = i+1;
            currLine = lineStart < codepointCount ? &rawStr[lineStart] : nullptr; // Set currLine to begin at the character after lineFeed if present
        }
    }

    // If currLine != nullptr, then either the last character(s) were not line endings or there were no line endings, process last line
    if ( currLine != nullptr )
    {
        if ( !dc.getTabTextExtent(currLine, codepointCount-lineStart, lineWidth, lineHeight, lineCache) )
            return false;

        height += lineHeight;
        if ( lineWidth > s32(width) )
            width = UINT(lineWidth);
    }
    return true;
}

void DrawString(const WinLib::DeviceContext & dc, UINT xPos, UINT yPos, LONG width, COLORREF defaultColor, std::string str)
{
    dc.setTextColor(defaultColor);
    if ( dc.setDefaultFont() )
    {
        size_t start = 0,
               loc,
               max = str.size(),
               npos = std::string::npos;

        s32 lineWidth = 0, lineHeight = 0;

        loc = str.find("\r\n");
        if ( loc != npos ) // Has new lines
        {
            // Do first line
            std::string firstLine = str.substr(0, loc);
            DrawStringLine(dc, xPos, yPos, width, defaultColor, firstLine);
            if ( dc.getTabTextExtent(firstLine, lineWidth, lineHeight) ||
                (loc == 0 && str.length() > 0 && dc.getTabTextExtent(std::string("\r\n"), lineWidth,  lineHeight)) )
            {
                start = loc+2;
                yPos += lineHeight;

                do // Do subsequent lines
                {
                    loc = str.find("\r\n", start);
                    if ( loc != npos )
                    {
                        std::string line = " ";
                        if ( loc-start > 0 )
                        {
                            line = str.substr(start, loc-start);
                            DrawStringLine(dc, xPos, yPos, width, defaultColor, line);
                        }

                        start = loc+2;
                        if ( dc.getTabTextExtent(line, lineWidth, lineHeight) )
                            yPos += lineHeight;
                        else
                            break;
                    }
                    else
                    {
                        // Do last line
                        std::string lastLine = str.substr(start, max-start);
                        if ( dc.getTabTextExtent(lastLine, lineWidth, lineHeight) )
                        {
                            DrawStringLine(dc, xPos, yPos, width, defaultColor, lastLine);
                            yPos += lineHeight;
                        }
                        break;
                    }
                } while ( start < max );
            }
        }
        else if ( dc.getTabTextExtent(str, lineWidth, lineHeight) )
        {
            DrawStringLine(dc, xPos, yPos, width, defaultColor, str);
            yPos += lineHeight;
        }
    }
}
