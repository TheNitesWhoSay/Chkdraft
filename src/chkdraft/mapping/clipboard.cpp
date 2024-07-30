#include "clipboard.h"
#include "chkdraft/chkdraft.h"
#include "mapping/undos/chkd_undos/tile_change.h"
#include "mapping/undos/chkd_undos/mtxm_change.h"
#include "mapping/undos/chkd_undos/unit_create_del.h"
#include "mapping/undos/chkd_undos/unit_change.h"
#include "mapping/undos/chkd_undos/doodad_create_del.h"
#include "mapping/undos/chkd_undos/sprite_create_del.h"
#include "mapping/undos/chkd_undos/fog_change.h"
#include <set>

extern Logger logger;

void StringToWindowsClipboard(const std::string & str)
{
    if ( OpenClipboard(NULL) != 0 )
    {
        if ( EmptyClipboard() != 0 )
        {
            icux::uistring sysText = icux::toUistring(str);
            HGLOBAL globalData = GlobalAlloc(GMEM_MOVEABLE, sysText.size()*sizeof(icux::codepoint) + sizeof(icux::codepoint));
            if ( globalData != NULL )
            {
                LPVOID lockedData = GlobalLock(globalData);
                if ( lockedData != NULL )
                {
                    memcpy(lockedData, sysText.c_str(), sysText.size()*sizeof(icux::codepoint) + sizeof(icux::codepoint));
                    GlobalUnlock(lockedData);
                    SetClipboardData(CF_UNICODETEXT, globalData);
                }
                GlobalFree(globalData);
            }
        }
        CloseClipboard();
    }
}

bool WindowsClipboardToString(std::string & str)
{
    bool success = false;
    if ( OpenClipboard(NULL) != 0 )
    {
        HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
        if ( clipboardData != NULL )
        {
            LPVOID lockedData = GlobalLock(clipboardData);
            if ( lockedData != NULL )
            {
                SIZE_T maxStringSize = GlobalSize(lockedData);
                if ( maxStringSize != 0 )
                {
                    str = icux::toUtf8((wchar_t*)lockedData, (size_t)maxStringSize/sizeof(icux::codepoint));
                    success = true;
                }
                GlobalUnlock(lockedData);
            }
        }
        CloseClipboard();
    }
    return success;
}

PasteTileNode::~PasteTileNode()
{

}

PasteDoodadNode::PasteDoodadNode(u16 startTileGroup, s32 tileX, s32 tileY)
    : tileX(tileX), tileY(tileY)
{
    const auto & tileset = chkd.scData.terrain.get(CM->getTileset());
    const auto & doodad = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[startTileGroup];
    this->doodadId = doodad.ddDataIndex;
    this->tileWidth = doodad.tileWidth;
    this->tileHeight = doodad.tileHeight;
    this->tileX = tileX;
    this->tileY = tileY;
    this->owner = CM->getCurrPlayer();

    this->overlayIndex = doodad.overlayIndex;
    this->spriteFlags = doodad.flags;
    this->doodadPlacibility = &(tileset.doodadPlacibility[this->doodadId]);

    for ( u16 y=0; y<tileHeight; ++y )
    {
        u16 tileGroupIndex = startTileGroup + y;
        const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
        for ( u16 x=0; x<tileWidth; ++x )
        {
            if ( tileGroup.megaTileIndex[x] == 0 )
                this->tileIndex[x][y] = 0;
            else
                this->tileIndex[x][y] = 16*tileGroupIndex + x;
        }
    }
}

PasteDoodadNode::PasteDoodadNode(const Chk::Doodad & doodad)
{
    const auto & tileset = chkd.scData.terrain.get(CM->getTileset());
    if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
    {
        const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
        this->doodadId = doodadDat.ddDataIndex;
        this->tileWidth = doodadDat.tileWidth;
        this->tileHeight = doodadDat.tileHeight;
        this->tileX = doodad.xc/32;
        this->tileY = doodad.yc/32;
        this->owner = doodad.owner;

        this->overlayIndex = doodadDat.overlayIndex;
        this->spriteFlags = doodadDat.flags;
        this->doodadPlacibility = &(tileset.doodadPlacibility[this->doodadId]);

        for ( u16 y=0; y<tileHeight; ++y )
        {
            u16 tileGroupIndex = (*doodadGroupIndex) + y;
            const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
            for ( u16 x=0; x<tileWidth; ++x )
            {
                if ( tileGroup.megaTileIndex[x] == 0 )
                    this->tileIndex[x][y] = 0;
                else
                    this->tileIndex[x][y] = 16*tileGroupIndex + x;
            }
        }
    }
}

PasteDoodadNode::~PasteDoodadNode()
{

}

bool PasteDoodadNode::isPlaceable(const Scenario & scenario, s32 xTileStart, s32 yTileStart) const
{
    s32 mapWidth = (s32)scenario.getTileWidth();
    s32 mapHeight = (s32)scenario.getTileHeight();

    if ( this->doodadPlacibility == nullptr || xTileStart < 0 || xTileStart+tileWidth > mapWidth || yTileStart < 0 || yTileStart+tileHeight > mapHeight )
        return false;

    for ( s32 y=0; y<this->tileHeight; ++y )
    {
        s32 tileYc = yTileStart + y;
        for ( s32 x=0; x<this->tileWidth; ++x )
        {
            s32 tileXc = xTileStart + x;
            auto existingTileGroup = scenario.getTile(tileXc, tileYc, Chk::Scope::Game) / 16;
            if ( doodadPlacibility->tileGroup[y*tileWidth+x] != 0 && existingTileGroup != doodadPlacibility->tileGroup[y*tileWidth+x] )
                return false;
        }
    }
    return true;
}

PasteSpriteNode::PasteSpriteNode(const Chk::Sprite & sprite)
{
    this->sprite = sprite;
    this->xc = sprite.xc;
    this->yc = sprite.yc;
}

PasteSpriteNode::~PasteSpriteNode()
{

}

PasteUnitNode::PasteUnitNode(const Chk::Unit & unit)
{
    this->unit = unit;
    this->xc = unit.xc;
    this->yc = unit.yc;
}

PasteUnitNode::~PasteUnitNode()
{

}

Clipboard::Clipboard() : pasting(false), quickPaste(false), fillSimilarTiles(false)
{
    edges.left = -1;
    edges.top = -1;
    edges.right = -1;
    edges.bottom = -1;
    prevPaste.x = -1;
    prevPaste.y = -1;
}

Clipboard::~Clipboard()
{

}

bool Clipboard::hasTiles()
{
    return copyTiles.size() > 0;
}

bool Clipboard::hasDoodads()
{
    return copyDoodads.size() > 0;
}

void Clipboard::copy(GuiMap & map, Layer layer)
{
    Selections & selections = map.GetSelections();
    edges.left = -1;
    edges.top = -1;
    edges.right = -1;
    edges.bottom = -1;
    bool initializedEdges = false;

    auto copyTerrain = [&]() {
        copyTiles.clear(); // Clear whatever was previously copied
        if ( selections.hasTiles() )
        {
            if ( !initializedEdges )
            {
                TileNode firstTile = selections.getFirstTile();
                edges.left = firstTile.xc * 32;
                edges.right = firstTile.xc * 32 + 32;
                edges.top = firstTile.yc * 32;
                edges.bottom = firstTile.yc * 32 + 32;
                initializedEdges = true;
            }

            auto & selTiles = selections.getTiles();
            for ( auto & selTile : selTiles ) // Traverse through all tiles
            {
                PasteTileNode tile(selTile.value, selTile.xc * 32, selTile.yc * 32, selTile.neighbors);

                // Record the outermost tile positions for determining their relation to the cursor
                if      ( tile.xc      < edges.left   ) edges.left   = tile.xc;
                else if ( tile.xc + 32 > edges.right  ) edges.right  = tile.xc + 32;
                if      ( tile.yc      < edges.top    ) edges.top    = tile.yc;
                else if ( tile.yc + 32 > edges.bottom ) edges.bottom = tile.yc + 32;

                copyTiles.insert(copyTiles.end(), tile);
            }
        }
    };
    auto copyDoodads = [&]() {
        this->copyDoodads.clear();
        if ( selections.hasDoodads() )
        {
            if ( !initializedEdges )
            {
                u16 firstDoodadIndex = selections.getFirstDoodad();
                const Chk::Doodad & currDoodad = map.getDoodad(firstDoodadIndex);
                edges.left = currDoodad.xc;
                edges.right = currDoodad.xc;
                edges.top = currDoodad.yc;
                edges.bottom = currDoodad.yc;
                initializedEdges = true;
            }

            const auto & selectedDoodads = selections.getDoodads();
            for ( size_t doodadIndex : selectedDoodads )
            {
                const Chk::Doodad & currDoodad = map.getDoodad(doodadIndex);
                PasteDoodadNode add(currDoodad);

                if      ( add.tileX*32 < edges.left   ) edges.left   = add.tileX*32;
                else if ( add.tileX*32 > edges.right  ) edges.right  = add.tileX*32+add.tileWidth*32;
                if      ( add.tileY*32 < edges.top    ) edges.top    = add.tileY*32;
                else if ( add.tileY*32 > edges.bottom ) edges.bottom = add.tileY*32+add.tileHeight*32;

                this->copyDoodads.push_back(add);
            }
        }
    };
    auto copyUnits = [&]() {
        this->copyUnits.clear();
        if ( selections.hasUnits() )
        {
            if ( !initializedEdges )
            {
                u16 firstUnitIndex = selections.getFirstUnit();
                const Chk::Unit & currUnit = map.getUnit(firstUnitIndex);
                edges.left   = currUnit.xc;
                edges.right  = currUnit.xc;
                edges.top    = currUnit.yc;
                edges.bottom = currUnit.yc;
                initializedEdges = true;
            }

            auto & selectedUnits = selections.getUnits();
            for ( u16 & unitIndex : selectedUnits )
            {
                const Chk::Unit & currUnit = map.getUnit(unitIndex);
                PasteUnitNode add(currUnit);

                if      ( add.xc < edges.left   ) edges.left   = add.xc;
                else if ( add.xc > edges.right  ) edges.right  = add.xc;
                if      ( add.yc < edges.top    ) edges.top    = add.yc;
                else if ( add.yc > edges.bottom ) edges.bottom = add.yc;

                this->copyUnits.push_back(add);
            }
        }
    };
    auto copySprites = [&]() {
        this->copySprites.clear();
        if ( selections.hasSprites() )
        {
            if ( !initializedEdges )
            {
                size_t firstSpriteIndex = selections.getFirstSprite();
                const Chk::Sprite & currSprite = map.getSprite(firstSpriteIndex);
                edges.left = currSprite.xc;
                edges.right = currSprite.xc;
                edges.top = currSprite.yc;
                edges.bottom = currSprite.yc;
                initializedEdges = true;
            }

            auto & selectedSprites = selections.getSprites();
            for ( size_t spriteIndex : selectedSprites )
            {
                const Chk::Sprite & currSprite = map.getSprite(spriteIndex);
                PasteSpriteNode add(currSprite);

                if      ( add.xc < edges.left   ) edges.left   = add.xc;
                else if ( add.xc > edges.right  ) edges.right  = add.xc;
                if      ( add.yc < edges.top    ) edges.top    = add.yc;
                else if ( add.yc > edges.bottom ) edges.bottom = add.yc;

                this->copySprites.push_back(add);
            }
        }
    };
    auto copyFog = [&]() {
        this->copyFogTiles.clear(); // Clear whatever was previously copied
        if ( selections.hasFogTiles() )
        {
            auto & selFogTiles = selections.getFogTiles();
            if ( !initializedEdges )
            {
                FogTile firstTile = selFogTiles.front();
                edges.left = firstTile.xc * 32;
                edges.right = firstTile.xc * 32 + 32;
                edges.top = firstTile.yc * 32;
                edges.bottom = firstTile.yc * 32 + 32;
            }

            for ( auto selFogTile : selFogTiles ) // Traverse through all selected fog tiles
            {
                u8 value = map.getFog(selFogTile.xc, selFogTile.yc);
                PasteFogTileNode fogTile(value, selFogTile.xc * 32, selFogTile.yc * 32, selFogTile.neighbors);

                // Record the outermost fog tile positions for determining their relation to the cursor
                if      ( fogTile.xc      < edges.left   ) edges.left   = fogTile.xc;
                else if ( fogTile.xc + 32 > edges.right  ) edges.right  = fogTile.xc + 32;
                if      ( fogTile.yc      < edges.top    ) edges.top    = fogTile.yc;
                else if ( fogTile.yc + 32 > edges.bottom ) edges.bottom = fogTile.yc + 32;

                copyFogTiles.push_back(fogTile);
            }
        }
    };
    auto calculateMiddle = [&]() {
        POINT middle {};
        middle.x = edges.left+(edges.right-edges.left)/2;
        middle.y = edges.top+(edges.bottom-edges.top)/2;
        if ( hasTiles() || hasDoodads() || hasFogTiles() )
        {
            middle.x = (middle.x+16)/32*32;
            middle.y = (middle.y+16)/32*32;
        }
        return middle;
    };

    if ( layer == Layer::Terrain )
    {
        chkd.maps.ChangeSubLayer(TerrainSubLayer::Rectangular);
        copyTerrain();
        updateTileMiddle(calculateMiddle());
    }
    else if ( layer == Layer::Doodads )
    {
        copyDoodads();
        updateDoodadMiddle(calculateMiddle());
    }
    else if ( layer == Layer::Units )
    {
        copyUnits();
        updateUnitMiddle(calculateMiddle());
    }
    else if ( layer == Layer::Sprites )
    {
        copySprites();
        updateSpriteMiddle(calculateMiddle());
    }
    else if ( layer == Layer::CutCopyPaste )
    {
        this->copyTiles.clear();
        this->copyDoodads.clear();
        this->copyUnits.clear();
        this->copySprites.clear();
        this->copyFogTiles.clear();
        if ( map.getCutCopyPasteTerrain() )
            copyTerrain();
        if ( map.getCutCopyPasteDoodads() )
            copyDoodads();
        if ( map.getCutCopyPasteUnits() )
            copyUnits();
        if ( map.getCutCopyPasteSprites() )
            copySprites();
        if ( map.getCutCopyPasteFog() )
            copyFog();

        auto middle = calculateMiddle();
        if ( map.getCutCopyPasteTerrain() )
            updateTileMiddle(middle);
        if ( map.getCutCopyPasteDoodads() )
            updateDoodadMiddle(middle);
        if ( map.getCutCopyPasteUnits() )
            updateUnitMiddle(middle);
        if ( map.getCutCopyPasteSprites() )
            updateSpriteMiddle(middle);
        if ( map.getCutCopyPasteFog() )
            updateFogMiddle(middle);
    }
}

void Clipboard::setQuickIsom(size_t terrainTypeIndex)
{
    this->terrainTypeIndex = terrainTypeIndex;
    this->prevIsomPaste = Chk::IsomDiamond::none();
    quickPaste = true;
    pasting = true;
}

void Clipboard::setQuickDoodad(u16 doodadStartTileGroup)
{
    this->quickDoodads.clear();
    this->quickDoodads.push_back(PasteDoodadNode(doodadStartTileGroup, 0, 0));
    prevPaste.x = -1;
    prevPaste.y = -1;
    quickPaste = true;
    pasting = true;
}

void Clipboard::addQuickTile(u16 index, s32 xc, s32 yc)
{
    quickTiles.insert(quickTiles.end(), PasteTileNode(index, xc, yc, TileNeighbor::All));
}

void Clipboard::addQuickUnit(const Chk::Unit & unit)
{
    quickUnits.push_back(unit);
}

void Clipboard::addQuickSprite(const Chk::Sprite & sprite)
{
    quickSprites.push_back(sprite);
}

void Clipboard::setFogBrushSize(u32 width, u32 height)
{
    if ( width == 0 )
        this->fogBrush.width = 1;
    else if ( width > 256 )
        this->fogBrush.width = 256;
    else
        this->fogBrush.width = width;

    if ( height == 0 )
        this->fogBrush.height = 1;
    else if ( height > 256 )
        this->fogBrush.height = 256;
    else
        this->fogBrush.height = height;
}

void Clipboard::initFogBrush(s32 mapClickX, s32 mapClickY, const GuiMap & map, bool allPlayers)
{
    s32 tileClickX = mapClickX/32;
    s32 tileClickY = mapClickY/32;

    auto currPlayer = map.getCurrPlayer();
    if ( currPlayer >= u8(8) )
        currPlayer = u8(0);

    this->fogBrush.setFog = (map.getFog(tileClickX, tileClickY) & u8Bits[currPlayer]) == 0;
    this->fogBrush.allPlayers = allPlayers;

    pasting = true;
    quickPaste = true;
}

void Clipboard::beginPasting(bool isQuickPaste)
{
    quickPaste = isQuickPaste;
    pasting = true;
}

void Clipboard::endPasting()
{
    if ( pasting )
    {
        if ( quickPaste )
        {
            ClearQuickItems();
            quickPaste = false;
            
            if ( chkd.terrainPalWindow.getHandle() != nullptr )
                RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
        }

        prevPaste.x = -1;
        prevPaste.y = -1;
        prevIsomPaste = Chk::IsomDiamond::none();
        pasting = false;
    }
}

void Clipboard::doPaste(Layer layer, TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos, bool allowStack)
{
    switch ( layer )
    {
        case Layer::Terrain:
            pasteTerrain(terrainSubLayer, mapClickX, mapClickY, map, undos);
            break;
        case Layer::Doodads:
            pasteDoodads(mapClickX, mapClickY, map, undos);
            break;
        case Layer::Units:
            pasteUnits(mapClickX, mapClickY, map, undos, allowStack);
            break;
        case Layer::Sprites:
            pasteSprites(mapClickX, mapClickY, map, undos);
            break;
        case Layer::FogEdit:
            pasteBrushFog(mapClickX, mapClickY, map, undos);
            break;
        case Layer::CutCopyPaste:
            {
                bool pastingTerrain = map.getCutCopyPasteTerrain() && hasTiles();
                bool pastingDoodads = map.getCutCopyPasteDoodads() && hasDoodads();
                bool pastingFog = map.getCutCopyPasteFog() && hasFogTiles();
                if ( pastingTerrain || pastingDoodads || pastingFog ) // If paste includes tile-based entities, force paste onto tiles
                {
                    mapClickX = (mapClickX+16)/32*32;
                    mapClickY = (mapClickY+16)/32*32;
                }
                pasteTerrain(terrainSubLayer, mapClickX, mapClickY, map, undos);
                
                pasteDoodads(mapClickX, mapClickY, map, undos);
                pasteUnits(mapClickX, mapClickY, map, undos, allowStack);
                pasteSprites(mapClickX, mapClickY, map, undos);
                pasteFog(mapClickX, mapClickY, map, undos);
            }
            break;
    }
}

std::vector<PasteTileNode> & Clipboard::getTiles()
{
    if ( quickPaste )
        return quickTiles;
    else
        return copyTiles;
}

std::vector<PasteDoodadNode> & Clipboard::getDoodads()
{
    if ( quickPaste )
        return quickDoodads;
    else
        return copyDoodads;
}

std::vector<PasteUnitNode> & Clipboard::getUnits()
{
    if ( quickPaste )
        return quickUnits;
    else
        return copyUnits;
}

std::vector<PasteSpriteNode> & Clipboard::getSprites()
{
    if ( quickPaste )
        return quickSprites;
    else
        return copySprites;
}

std::vector<PasteFogTileNode> & Clipboard::getFogTiles()
{
    return copyFogTiles;
}

bool Clipboard::isNearPrevPaste(s32 mapClickX, s32 mapClickY)
{
    auto xDiff = LONG(mapClickX) < prevPaste.x ? prevPaste.x-LONG(mapClickX) : LONG(mapClickX)-prevPaste.x;
    auto yDiff = LONG(mapClickY) < prevPaste.y ? prevPaste.y-LONG(mapClickY) : LONG(mapClickY)-prevPaste.y;
    return xDiff < 32 && yDiff < 32;
}

void Clipboard::ClearQuickItems()
{
    this->terrainTypeIndex = 0;
    this->prevIsomPaste = Chk::IsomDiamond::none();
    quickTiles.clear();
    quickUnits.clear();
    quickDoodads.clear();
    quickSprites.clear();
}

bool Clipboard::getFillSimilarTiles()
{
    return fillSimilarTiles;
}

void Clipboard::toggleFillSimilarTiles()
{
    fillSimilarTiles = !fillSimilarTiles;
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_FILLSIMILARTILES, fillSimilarTiles);
}

void Clipboard::pasteTerrain(TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    if ( terrainSubLayer == TerrainSubLayer::Isom && map.getLayer() != Layer::CutCopyPaste )
    {
        auto diamond = Chk::IsomDiamond::fromMapCoordinates(mapClickX, mapClickY);
        if ( prevIsomPaste != diamond )
        {
            map.placeIsomTerrain(Chk::IsomDiamond::fromMapCoordinates(mapClickX, mapClickY), terrainTypeIndex, 1);
            prevIsomPaste = diamond;
        }
    }
    else if ( fillSimilarTiles && getTiles().size() == 1 && map.getLayer() != Layer::CutCopyPaste )
    {
        fillPasteTerrain(mapClickX, mapClickY, map, undos);
    }
    else
    {
        mapClickX += 16;
        mapClickY += 16;

        if ( mapClickX/16 != prevPaste.x || mapClickY/16 != prevPaste.y )
        {
            prevPaste.x = mapClickX/16;
            prevPaste.y = mapClickY/16;
            u16 xSize = (u16)map.getTileWidth();
            u16 ySize = (u16)map.getTileHeight();

            map.beginTerrainOperation();
            auto & tiles = getTiles();
            for ( auto & tile : tiles )
            {
                s32 xc = (tile.xc + mapClickX) / 32;
                s32 yc = (tile.yc + mapClickY) / 32;

                // If within map boundaries
                if ( xc >= 0 && xc < xSize )
                {
                    if ( yc >= 0 && yc < ySize )
                    {
                        if ( map.getTile(xc, yc) != tile.value )
                            map.setTileValue(xc, yc, tile.value);
                    }
                }
            }
            map.finalizeTerrainOperation();
        }
    }
}

void Clipboard::pasteDoodads(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    const auto & doodads = getDoodads();
    if ( !doodads.empty() )
    {
        const auto & first = doodads[0];
        bool firstEvenWidth = first.tileWidth%2 == 0;
        bool firstEvenHeight = first.tileHeight%2 == 0;
        auto firstXStart = firstEvenWidth ? (mapClickX+16)/32 - first.tileWidth/2 : mapClickX/32 - (first.tileWidth-1)/2;
        auto secondYStart = firstEvenHeight ? (mapClickY+16)/32 - first.tileHeight/2 : mapClickY/32 - (first.tileHeight-1)/2;
        auto firstCenterX = 32*firstXStart + 16*first.tileWidth;
        auto firstCenterY = 32*secondYStart + 16*first.tileHeight;
        if ( firstCenterX != prevPaste.x || firstCenterY != prevPaste.y )
        {
            prevPaste = {firstCenterX, firstCenterY};
            auto doodadsUndo = ReversibleActions::Make();
            for ( const auto & doodad : doodads )
            {
                auto tileWidth = doodad.tileWidth;
                auto tileHeight = doodad.tileHeight;
                bool evenWidth = tileWidth%2 == 0;
                bool evenHeight = tileHeight%2 == 0;
                auto xStart = evenWidth ? (mapClickX+16)/32 - doodad.tileWidth/2 + doodad.tileX : mapClickX/32 - (doodad.tileWidth-1)/2 + doodad.tileX;
                auto yStart = evenHeight ? (mapClickY+16)/32 - doodad.tileHeight/2 + doodad.tileY : mapClickY/32 - (doodad.tileHeight-1)/2 + doodad.tileY;
                auto centerX = 32*xStart + 16*tileWidth;
                auto centerY = 32*yStart + 16*tileHeight;

                if ( map.AllowIllegalDoodadPlacement() || map.getLayer() == Layer::CutCopyPaste || doodad.isPlaceable(map, xStart, yStart) )
                {
                    if ( xStart >= 0 && yStart >= 0 && xStart+int(tileWidth) <= int(map.getTileWidth()) && yStart+int(tileHeight) <= int(map.getTileHeight()) )
                    {
                        doodadsUndo->Insert(DoodadCreateDel::Make(u16(map.numDoodads()), Sc::Terrain::Doodad::Type(doodad.doodadId), xStart, yStart, &map));
                        map.addDoodad(Chk::Doodad{Sc::Terrain::Doodad::Type(doodad.doodadId), u16(centerX), u16(centerY), doodad.owner, Chk::Doodad::Enabled::Enabled});
                        if ( doodad.overlayIndex != 0 )
                        {
                            map.addSprite(Chk::Sprite{Sc::Sprite::Type(doodad.overlayIndex), u16(centerX), u16(centerY), doodad.owner, 0,
                                u16(doodad.isSprite() ? Chk::Sprite::SpriteFlags::DrawAsSprite : 0)});
                        }
                        for ( int y=0; y<tileHeight; ++y )
                        {
                            for ( int x=0; x<tileWidth; ++x )
                            {
                                if ( doodad.tileIndex[x][y] != 0 )
                                    map.setDoodadTile(xStart+x, yStart+y, doodad.tileIndex[x][y]);
                            }
                        }
                    }
                }
            }
            map.AddUndo(doodadsUndo);
        }
    }
}

void Clipboard::fillPasteTerrain(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    mapClickX += 16;
    mapClickY += 16;

    if ( !( mapClickX/16 == prevPaste.x && mapClickY/16 == prevPaste.y ) )
    {
        prevPaste.x = mapClickX/16;
        prevPaste.y = mapClickY/16;
        u16 xSize = (u16)map.getTileWidth();
        u16 ySize = (u16)map.getTileHeight();

        map.beginTerrainOperation();
        if ( getTiles().size() == 1 )
        {
            PasteTileNode pasteTile = getTiles().at(0);
            s32 xc = (pasteTile.xc + mapClickX) / 32;
            s32 yc = (pasteTile.yc + mapClickY) / 32;
            u16 pasteTileValue = pasteTile.value;

            // If within map boundaries
            if ( xc >= 0 && xc < xSize && yc >= 0 && yc < ySize )
            {
                u16 filledTileValue = map.getTile(xc, yc);
                if ( filledTileValue != pasteTileValue )
                {
                    std::set<points> tilesProcessed;
                    std::stack<points> tilesToProcess;
                    const points originTile = points(xc, yc);
                    tilesToProcess.push(originTile);
                    while ( !tilesToProcess.empty() )
                    {
                        const points tile = tilesToProcess.top();
                        tilesToProcess.pop();
                        tilesProcessed.insert(tile);
                        xc = tile.x;
                        yc = tile.y;
                        if ( map.getTile(xc, yc) == filledTileValue )
                        {
                            map.setTileValue(xc, yc, pasteTileValue);
                            const points left = points(xc-1, yc);
                            const points right = points(xc+1, yc);
                            const points up = points(xc, yc-1);
                            const points down = points(xc, yc+1);
                            if ( xc >= 1 && tilesProcessed.find(left) == tilesProcessed.end() )
                                tilesToProcess.push(left);
                            if ( xc < xSize-1 && tilesProcessed.find(right) == tilesProcessed.end() )
                                tilesToProcess.push(right);
                            if ( yc >= 1 && tilesProcessed.find(up) == tilesProcessed.end() )
                                tilesToProcess.push(up);
                            if ( yc < ySize-1 && tilesProcessed.find(down) == tilesProcessed.end() )
                                tilesToProcess.push(down);
                        }
                    }
                }
            }
        }
        map.finalizeTerrainOperation();
    }
}

void Clipboard::pasteUnits(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos, bool allowStack)
{
    auto currPasteTime = std::chrono::steady_clock::now();
    if ( allowStack && std::chrono::duration_cast<std::chrono::milliseconds>(currPasteTime - this->lastPasteTime).count() < 250 && isNearPrevPaste(mapClickX, mapClickY) )
        return; // Prevent unintentional repeat-pastes
    else
        this->lastPasteTime = currPasteTime;

    auto unitCreates = ReversibleActions::Make();
    auto & pasteUnits = getUnits();
    auto nextClassId = map.getNextClassId();
    std::vector<size_t> pastedAddons {};
    for ( auto & pasteUnit : pasteUnits )
    {
        pasteUnit.unit.xc = u16(mapClickX + pasteUnit.xc);
        pasteUnit.unit.yc = u16(mapClickY + pasteUnit.yc);
        if ( mapClickX + (s32(pasteUnit.xc)) >= 0 && mapClickY + (s32(pasteUnit.yc)) >= 0 &&
             map.isValidUnitPlacement(pasteUnit.unit.type, s32(pasteUnit.unit.xc), s32(pasteUnit.unit.yc)) )
        {
            prevPaste.x = pasteUnit.unit.xc;
            prevPaste.y = pasteUnit.unit.yc;
            size_t numUnits = map.numUnits();
            pasteUnit.unit.classId = nextClassId;
            ++nextClassId;
            pasteUnit.unit.relationClassId = 0;
            pasteUnit.unit.relationFlags = 0;
            if ( pasteUnit.unit.type < Sc::Unit::TotalTypes )
            {
                const auto & dat = chkd.scData.units.getUnit(pasteUnit.unit.type);
                if ( (dat.flags & Sc::Unit::Flags::Addon) == Sc::Unit::Flags::Addon ||
                    pasteUnit.unit.type == Sc::Unit::Type::TerranCommandCenter ||
                    pasteUnit.unit.type == Sc::Unit::Type::TerranScienceFacility ||
                    pasteUnit.unit.type == Sc::Unit::Type::TerranStarport ||
                    pasteUnit.unit.type == Sc::Unit::Type::TerranFactory )
                {
                    pastedAddons.push_back(numUnits);
                }
                else if ( pasteUnit.unit.type == Sc::Unit::Type::ZergNydusCanal && pasteUnits.size() == 1 )
                {
                    if ( lastPasteNydus )
                    {
                        auto & lastNydus = map.getUnit(*lastPasteNydus);
                        if ( pasteUnit.unit.owner == lastNydus.owner )
                        {
                            unitCreates->Insert(UnitChange::Make(*lastPasteNydus, Chk::Unit::Field::RelationClassId, lastNydus.relationClassId));
                            unitCreates->Insert(UnitChange::Make(*lastPasteNydus, Chk::Unit::Field::RelationFlags, lastNydus.relationFlags));
                            pasteUnit.unit.relationClassId = lastNydus.classId;
                            pasteUnit.unit.relationFlags = Chk::Unit::RelationFlag::NydusLink;
                            lastNydus.relationClassId = pasteUnit.unit.classId;
                            lastNydus.relationFlags = Chk::Unit::RelationFlag::NydusLink;
                            logger.info() << "Nydus at index " << numUnits << " linked to nydus at index " << *lastPasteNydus << std::endl;
                            lastPasteNydus = std::nullopt;
                        }
                        else
                            lastPasteNydus = u16(numUnits);
                    }
                    else
                        lastPasteNydus = u16(numUnits);
                }
                else
                    lastPasteNydus = std::nullopt;
            }
            map.addUnit(pasteUnit.unit);
            unitCreates->Insert(UnitCreateDel::Make((u16)numUnits));
            if ( chkd.unitWindow.getHandle() != nullptr )
                chkd.unitWindow.AddUnitItem((u16)numUnits, pasteUnit.unit);
        }
    }
    if ( !pastedAddons.empty() )
    {
        for ( auto pastedAddonIndex : pastedAddons )
        {
            auto & pastedAddon = map.getUnit(pastedAddonIndex);
            if ( auto buildingOpt = map.getLinkableUnitIndex(pastedAddon.type, pastedAddon.xc, pastedAddon.yc) )
            {
                auto & building = map.getUnit(*buildingOpt);
                if ( pastedAddon.owner == building.owner || map.autoSwappingAddonPlayers() )
                {
                    if ( map.autoSwappingAddonPlayers() && building.owner != pastedAddon.owner )
                    {
                        if ( (chkd.scData.units.getUnit(pastedAddon.type).flags & Sc::Unit::Flags::Addon) == Sc::Unit::Flags::Addon )
                        {
                            unitCreates->Insert(UnitChange::Make(u16(pastedAddonIndex), Chk::Unit::Field::Owner, pastedAddon.owner));
                            pastedAddon.owner = building.owner;
                        }
                        else
                        {
                            unitCreates->Insert(UnitChange::Make(*buildingOpt, Chk::Unit::Field::Owner, building.owner));
                            building.owner = pastedAddon.owner;
                        }
                    }
                    unitCreates->Insert(UnitChange::Make(*buildingOpt, Chk::Unit::Field::RelationClassId, building.relationClassId));
                    unitCreates->Insert(UnitChange::Make(*buildingOpt, Chk::Unit::Field::RelationFlags, building.relationFlags));
                    pastedAddon.relationClassId = building.classId;
                    pastedAddon.relationFlags = Chk::Unit::RelationFlag::AddonLink;
                    building.relationClassId = pastedAddon.classId;
                    building.relationFlags = Chk::Unit::RelationFlag::AddonLink;
                    logger.info() << "Addon pasted at index " << pastedAddonIndex << " linked to building at index " << *buildingOpt << std::endl;
                }
            }
        }
    }
    CM->AddUndo(unitCreates);
}

void Clipboard::pasteSprites(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    auto currPasteTime = std::chrono::steady_clock::now();
    if ( std::chrono::duration_cast<std::chrono::milliseconds>(currPasteTime - this->lastPasteTime).count() < 250 && isNearPrevPaste(mapClickX, mapClickY) )
        return; // Prevent unintentional repeat-pastes
    else
        this->lastPasteTime = currPasteTime;

    auto spriteCreates = ReversibleActions::Make();
    auto & pasteSprites = getSprites();
    for ( auto & pasteSprite : pasteSprites )
    {
        pasteSprite.sprite.xc = u16(mapClickX + pasteSprite.xc);
        pasteSprite.sprite.yc = u16(mapClickY + pasteSprite.yc);
        if ( mapClickX + (s32(pasteSprite.xc)) >= 0 && mapClickY + (s32(pasteSprite.yc)) >= 0 )
        {
            prevPaste.x = pasteSprite.sprite.xc;
            prevPaste.y = pasteSprite.sprite.yc;
            size_t numSprites = map.numSprites();
            map.addSprite(pasteSprite.sprite);
            spriteCreates->Insert(SpriteCreateDel::Make((u16)numSprites));
            if ( chkd.spriteWindow.getHandle() != nullptr )
                chkd.spriteWindow.AddSpriteItem((u16)numSprites, pasteSprite.sprite);
        }
    }
    CM->AddUndo(spriteCreates);
}

void Clipboard::pasteBrushFog(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    const auto width = this->fogBrush.width;
    const auto height = this->fogBrush.height;
    mapClickX += width % 2 == 0 ? 16 : 0;
    mapClickY += height % 2 == 0 ? 16 : 0;
    if ( mapClickX/16 != prevPaste.x || mapClickY/16 != prevPaste.y )
    {
        prevPaste.x = mapClickX/16;
        prevPaste.y = mapClickY/16;

        bool setFog = this->fogBrush.setFog;
        bool allPlayers = this->fogBrush.allPlayers;

        s32 brushWidth = s32(fogBrush.width);
        s32 brushHeight = s32(fogBrush.height);
        s32 hoverTileX = mapClickX/32;
        s32 hoverTileY = mapClickY/32;

        s32 startX = hoverTileX - brushWidth/2;
        s32 startY = hoverTileY - brushHeight/2;
        s32 endX = startX+brushWidth;
        s32 endY = startY+brushHeight;
        
        if ( startX < 0 )
            startX = 0;
        if ( startY < 0 )
            startY = 0;
        if ( endX > map.getTileWidth() )
            endX = (s32)map.getTileWidth();
        if ( endY > map.getTileHeight() )
            endY = (s32)map.getTileHeight();

        for ( s32 y=startY; y<endY; ++y )
        {
            for ( s32 x=startX; x<endX; ++x )
            {
                if ( allPlayers )
                {
                    if ( setFog )
                        map.setFogValue(x, y, u8(0xFF));
                    else
                        map.setFogValue(x, y, u8(0));
                }
                else
                {
                    u8 currPlayer = map.getCurrPlayer();
                    if ( currPlayer < 8 )
                    {
                        if ( setFog )
                            map.setFogValue(x, y, map.getFog(x, y) | u8Bits[currPlayer]);
                        else
                            map.setFogValue(x, y, map.getFog(x, y) & xU8Bits[currPlayer]);
                    }
                }
            }
        }
        CM->Redraw(true);
    }
}

void Clipboard::pasteFog(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    mapClickX += 16;
    mapClickY += 16;

    if ( mapClickX/16 != prevPaste.x || mapClickY/16 != prevPaste.y )
    {
        prevPaste.x = mapClickX/16;
        prevPaste.y = mapClickY/16;
        u16 xSize = (u16)map.getTileWidth();
        u16 ySize = (u16)map.getTileHeight();

        auto & fogTiles = getFogTiles();
        for ( auto & fogTile : fogTiles )
        {
            s32 xc = (fogTile.xc + mapClickX) / 32;
            s32 yc = (fogTile.yc + mapClickY) / 32;

            // If within map boundaries
            if ( xc >= 0 && xc < xSize )
            {
                if ( yc >= 0 && yc < ySize )
                {
                    if ( map.getFog(xc, yc) != fogTile.value )
                        map.setFogValue(xc, yc, fogTile.value);
                }
            }
        }
    }
}

void Clipboard::updateTileMiddle(POINT middle)
{
    // Update the tiles' relative position to the cursor
    for ( auto & tile : copyTiles )
    {
        tile.xc -= middle.x;
        tile.yc -= middle.y;
    }
}

void Clipboard::updateDoodadMiddle(POINT middle)
{
    // Update doodad position relative to the cursor
    for ( auto & doodad : this->copyDoodads )
    {
        doodad.tileX = doodad.tileX-(middle.x/32);
        doodad.tileY = doodad.tileY-(middle.y/32);
    }
}

void Clipboard::updateUnitMiddle(POINT middle)
{
    // Update units position relative to the cursor
    for ( auto & unit : this->copyUnits )
    {
        unit.xc -= middle.x;
        unit.yc -= middle.y;
    }
}

void Clipboard::updateSpriteMiddle(POINT middle)
{
    // Update sprites position relative to the cursor
    for ( auto & sprite : this->copySprites )
    {
        sprite.xc -= middle.x;
        sprite.yc -= middle.y;
    }
}

void Clipboard::updateFogMiddle(POINT middle)
{
    // Update the fog tiles' relative position to the cursor
    for ( auto & fogTile : copyFogTiles )
    {
        fogTile.xc -= middle.x;
        fogTile.yc -= middle.y;
    }
}
