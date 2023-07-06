#include "ClipBoard.h"
#include "../Chkdraft.h"
#include "../Mapping/Undos/ChkdUndos/TileChange.h"
#include "../Mapping/Undos/ChkdUndos/UnitCreateDel.h"
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
    ClearCopyTiles();
    ClearQuickItems();
}

bool Clipboard::hasTiles()
{
    return copyTiles.size() > 0;
}

void Clipboard::copy(GuiMap & map, Layer layer)
{
    Selections & selections = map.GetSelections();
    if ( layer == Layer::Terrain )
    {
        ClearCopyTiles(); // Clear whatever was previously copied
        if ( selections.hasTiles() )
        {
            TileNode firstTile = selections.getFirstTile();
            edges.left = firstTile.xc * 32;
            edges.right = firstTile.xc * 32 + 32;
            edges.top = firstTile.yc * 32;
            edges.bottom = firstTile.yc * 32 + 32;

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

            POINT middle; // Determine where the new middle of the paste is
            middle.x = edges.left+(edges.right-edges.left)/2;
            middle.y = edges.top+(edges.bottom-edges.top)/2; 

            for ( auto & tile : copyTiles ) // Update the tile's relative position to the cursor
            {
                tile.xc -= middle.x;
                tile.yc -= middle.y;
            }
        }
    }
    else if ( layer == Layer::Units )
    {
        ClearCopyUnits();
        if ( selections.hasUnits() )
        {
            u16 firstUnitIndex = selections.getFirstUnit();
            const Chk::Unit & currUnit = map.getUnit(firstUnitIndex);
            edges.left   = currUnit.xc;
            edges.right  = currUnit.xc;
            edges.top    = currUnit.yc;
            edges.bottom = currUnit.yc;

            auto & selectedUnits = selections.getUnits();
            for ( u16 & unitIndex : selectedUnits )
            {
                const Chk::Unit & currUnit = map.getUnit(unitIndex);
                PasteUnitNode add(currUnit);

                if      ( add.xc < edges.left   ) edges.left   = add.xc;
                else if ( add.xc > edges.right  ) edges.right  = add.xc;
                if      ( add.yc < edges.top    ) edges.top    = add.yc;
                else if ( add.yc > edges.bottom ) edges.bottom = add.yc;

                copyUnits.push_back(add);
            }

            POINT middle; // Determine where the new middle of the paste is
            middle.x = edges.left+(edges.right-edges.left)/2;
            middle.y = edges.top+(edges.bottom-edges.top)/2;
            
            // Update units position relative to the cursor
            for ( auto & unit : copyUnits )
            {
                unit.xc -= middle.x;
                unit.yc -= middle.y;
            }
        }
    }
}

void Clipboard::setQuickIsom(size_t terrainTypeIndex)
{
    this->terrainTypeIndex = terrainTypeIndex;
    this->prevIsomPaste = Chk::IsomDiamond::none();
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
        case Layer::Units:
            pasteUnits(mapClickX, mapClickY, map, undos, allowStack);
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

std::vector<PasteUnitNode> & Clipboard::getUnits()
{
    if ( quickPaste )
        return quickUnits;
    else
        return copyUnits;
}

void Clipboard::ClearCopyTiles()
{
    copyTiles.clear();
    edges.left = -1;
    edges.top = -1;
    edges.right = -1;
    edges.bottom = -1;
}

void Clipboard::ClearCopyUnits()
{
    copyUnits.clear();
    edges.left = -1;
    edges.top = -1;
    edges.right = -1;
    edges.bottom = -1;
}

void Clipboard::ClearQuickItems()
{
    this->terrainTypeIndex = 0;
    this->prevIsomPaste = Chk::IsomDiamond::none();
    quickTiles.clear();
    quickUnits.clear();
}

void Clipboard::toggleFillSimilarTiles()
{
    fillSimilarTiles = !fillSimilarTiles;
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_FILLSIMILARTILES, fillSimilarTiles);
}

void Clipboard::pasteTerrain(TerrainSubLayer terrainSubLayer, s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos)
{
    if ( terrainSubLayer == TerrainSubLayer::Isom )
    {
        auto diamond = Chk::IsomDiamond::fromMapCoordinates(mapClickX, mapClickY);
        if ( prevIsomPaste != diamond )
        {
            map.placeIsomTerrain(Chk::IsomDiamond::fromMapCoordinates(mapClickX, mapClickY), terrainTypeIndex, 1);
            prevIsomPaste = diamond;
        }
    }
    else if ( fillSimilarTiles && getTiles().size() == 1 )
    {
        fillPasteTerrain(mapClickX, mapClickY, map, undos);
    }
    else
    {
        mapClickX += 16;
        mapClickY += 16;

        if ( !( mapClickX/16 == prevPaste.x && mapClickY/16 == prevPaste.y ) )
        {
            prevPaste.x = mapClickX/16;
            prevPaste.y = mapClickY/16;
            u16 xSize = (u16)map.getTileWidth();
            u16 ySize = (u16)map.getTileHeight();

            auto tileChanges = ReversibleActions::Make();
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
                        {
                            tileChanges->Insert(TileChange::Make(xc, yc, map.getTile(xc, yc)));
                            map.setTile(xc, yc, tile.value);
                        }
                    }
                }
            }
            undos.AddUndo(tileChanges);
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

        auto tileChanges = ReversibleActions::Make();
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
                            tileChanges->Insert(TileChange::Make(xc, yc, map.getTile(xc, yc)));
                            map.setTile(xc, yc, pasteTileValue);
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
        undos.AddUndo(tileChanges);
    }
}

void Clipboard::pasteUnits(s32 mapClickX, s32 mapClickY, GuiMap & map, Undos & undos, bool allowStack)
{
    auto unitCreates = ReversibleActions::Make();
    auto & pasteUnits = getUnits();
    for ( auto & pasteUnit : pasteUnits )
    {
        pasteUnit.unit.xc = u16(mapClickX + pasteUnit.xc);
        pasteUnit.unit.yc = u16(mapClickY + pasteUnit.yc);
        if ( mapClickX + (s32(pasteUnit.xc)) >= 0 && mapClickY + (s32(pasteUnit.yc)) >= 0 )
        {
            bool canPaste = true;
            if ( allowStack == false )
            {
                s32 unitLeft   = pasteUnit.unit.xc - chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeLeft,
                    unitRight  = pasteUnit.unit.xc + chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeRight,
                    unitTop    = pasteUnit.unit.yc - chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeUp,
                    unitBottom = pasteUnit.unit.yc + chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeDown;

                size_t numUnits = map.numUnits();
                for ( size_t i=0; i<numUnits; i++ )
                {
                    const Chk::Unit & unit = map.getUnit(i);
                    s32 left   = unit.xc - chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeLeft,
                        right  = unit.xc + chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeRight,
                        top    = unit.yc - chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeUp,
                        bottom = unit.yc + chkd.scData.units.getUnit(pasteUnit.unit.type).unitSizeDown;

                    if ( unitRight >= left && unitLeft <= right && unitBottom >= top && unitTop <= bottom )
                    {
                        canPaste = false;
                        break;
                    }
                }
            }

            if ( canPaste )
            {
                prevPaste.x = pasteUnit.unit.xc;
                prevPaste.y = pasteUnit.unit.yc;
                size_t numUnits = map.numUnits();
                map.addUnit(pasteUnit.unit);
                unitCreates->Insert(UnitCreateDel::Make((u16)numUnits));
                if ( chkd.unitWindow.getHandle() != nullptr )
                    chkd.unitWindow.AddUnitItem((u16)numUnits, pasteUnit.unit);
            }
        }
    }
    CM->AddUndo(unitCreates);
}
