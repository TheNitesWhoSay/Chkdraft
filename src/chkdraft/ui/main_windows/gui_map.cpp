#include "gui_map.h"
#include "chkdraft/chkdraft.h"
#include "ui/chkd_controls/move_to.h"
#include "mapping/data_file_browsers.h"
#include "mapping/scr_graphics.h"
#include <cross_cut/logger.h>
#include <WindowsX.h>

bool GuiMap::doAutoBackups = false;

GuiMap::GuiMap(Clipboard & clipboard, const std::string & filePath) : MapFile(filePath),
    Chk::IsomCache(read.tileset, read.dimensions.tileWidth, read.dimensions.tileHeight, chkd.scData.terrain.get(read.tileset)),
    clipboard(clipboard), scrGraphics{std::make_unique<Scr::MapGraphics>(chkd.scData, *this)}
{
    SetWinText(MapFile::getFileName());
    int layerSel = chkd.mainToolbar.layerBox.GetSel();
    if ( layerSel != CB_ERR )
        currLayer = (Layer)layerSel;
}

GuiMap::GuiMap(Clipboard & clipboard, FileBrowserPtr<SaveType> fileBrowser) : MapFile(fileBrowser),
    Chk::IsomCache(read.tileset, read.dimensions.tileWidth, read.dimensions.tileHeight, chkd.scData.terrain.get(read.tileset)),
    clipboard(clipboard), scrGraphics{std::make_unique<Scr::MapGraphics>(chkd.scData, *this)}
{
    SetWinText(MapFile::getFileName());
    int layerSel = chkd.mainToolbar.layerBox.GetSel();
    if ( layerSel != CB_ERR )
        currLayer = (Layer)layerSel;
}

GuiMap::GuiMap(Clipboard & clipboard, Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, DefaultTriggers defaultTriggers)
    : MapFile(tileset, width, height, terrainTypeIndex, &chkd.scData.terrain.get(tileset)),
    Chk::IsomCache(read.tileset, read.dimensions.tileWidth, read.dimensions.tileHeight, chkd.scData.terrain.get(read.tileset)),
    clipboard(clipboard), scrGraphics{std::make_unique<Scr::MapGraphics>(chkd.scData, *this)}
{
    refreshTileOccupationCache();

    scGraphics.updatePalette();
    int layerSel = chkd.mainToolbar.layerBox.GetSel();
    if ( layerSel != CB_ERR )
        currLayer = (Layer)layerSel;
    
    auto addSetRes = [&](Sc::Player::Id player) {
        Chk::Trigger setRes {};
        setRes.owners[player] = Chk::Trigger::Owned::Yes;
        setRes.conditions[0].conditionType = Chk::Condition::Type::Always;
        setRes.actions[0].actionType = Chk::Action::Type::SetResources;
        setRes.actions[0].group = Sc::Player::Id::CurrentPlayer;
        setRes.actions[0].type2 = Chk::Trigger::ValueModifier::SetTo;
        setRes.actions[0].number = 50;
        setRes.actions[0].type = Chk::Trigger::ResourceType::Ore;
        this->addTrigger(setRes);
    };
    auto addDefeat = [&](Sc::Player::Id player) {
        Chk::Trigger defeat {};
        defeat.owners[player] = Chk::Trigger::Owned::Yes;
        defeat.conditions[0].conditionType = Chk::Condition::Type::Command;
        defeat.conditions[0].player = Sc::Player::Id::CurrentPlayer;
        defeat.conditions[0].comparison = Chk::Condition::Comparison::AtMost;
        defeat.conditions[0].amount = 0;
        defeat.conditions[0].unitType = Sc::Unit::Type::Buildings;
        defeat.actions[0].actionType = Chk::Action::Type::Defeat;
        this->addTrigger(defeat);
    };
    auto addVictory = [&](Sc::Player::Id player) {
        Chk::Trigger victory {};
        victory.owners[player] = Chk::Trigger::Owned::Yes;
        victory.conditions[0].conditionType = Chk::Condition::Type::Command;
        victory.conditions[0].player = Sc::Player::Id::NonAlliedVictoryPlayers;
        victory.conditions[0].comparison = Chk::Condition::Comparison::AtMost;
        victory.conditions[0].amount = 0;
        victory.conditions[0].unitType = Sc::Unit::Type::Buildings;
        victory.actions[0].actionType = Chk::Action::Type::Victory;
        this->addTrigger(victory);
    };
    auto addSharedVision = [&](Sc::Player::Id player) {
        Chk::Trigger addSharedVision {};
        addSharedVision.owners[player] = Chk::Trigger::Owned::Yes;
        addSharedVision.conditions[0].conditionType = Chk::Condition::Type::Always;
        addSharedVision.actions[0].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[1].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[2].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[3].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[4].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[5].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[6].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[7].actionType = Chk::Action::Type::RunAiScript;
        addSharedVision.actions[0].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer1);
        addSharedVision.actions[1].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer2);
        addSharedVision.actions[2].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer3);
        addSharedVision.actions[3].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer4);
        addSharedVision.actions[4].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer5);
        addSharedVision.actions[5].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer6);
        addSharedVision.actions[6].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer7);
        addSharedVision.actions[7].number = u32(Sc::Ai::Script::TurnONSharedVisionforPlayer8);
        this->addTrigger(addSharedVision);
    };

    switch ( defaultTriggers )
    {
    case DefaultTriggers::NoTriggers: break;
    case DefaultTriggers::DefaultMelee:
        addSetRes(Sc::Player::Id::AllPlayers);
        addDefeat(Sc::Player::Id::AllPlayers);
        addVictory(Sc::Player::Id::AllPlayers);
        break;
    case DefaultTriggers::TwoPlayerMeleeWithObs:
    case DefaultTriggers::ThreePlayerMeleeWithObs:
    case DefaultTriggers::FourPlayerMeleeWithObs:
    case DefaultTriggers::FivePlayerMeleeWithObs:
    case DefaultTriggers::SixPlayerMeleeWithObs:
    case DefaultTriggers::SevenPlayerMeleeWithObs:
        for ( size_t slot = size_t(defaultTriggers)-size_t(DefaultTriggers::TwoPlayerMeleeWithObs)+2; slot < 8; ++slot )
            this->setPlayerForce(slot, Chk::Force::Force2);
            
        addSetRes(Sc::Player::Id::Force1);
        addDefeat(Sc::Player::Id::Force1);
        addVictory(Sc::Player::Id::Force1);
        addSharedVision(Sc::Player::Id::Force2);
        break;
    }
}

GuiMap::~GuiMap()
{
    chkd.tilePropWindow.DestroyThis();
}

bool GuiMap::CanExit()
{
    if ( unsavedChanges )
    {
        WinLib::PromptResult result = WinLib::PromptResult::Unknown;
        if ( MapFile::getFilePath().length() <= 0 )
            result = WinLib::GetYesNoCancel("Save Changes?", "Untitled");
        else
            result = WinLib::GetYesNoCancel("Save Changes?", MapFile::getFilePath());

        if ( result == WinLib::PromptResult::Yes )
            SaveFile(false);
        else if ( result == WinLib::PromptResult::Cancel )
            return false;
    }
    return true;
}

bool GuiMap::SaveFile(bool saveAs)
{
    bool backupCopyFailed = false;
    TryBackup(backupCopyFailed);
    
    if ( backupCopyFailed && WinLib::GetYesNo("Backup failed, would you like to save anyway?", "Backup Failed!") == WinLib::PromptResult::No )
        return false;

    if ( MapFile::save(saveAs) )
    {
        lastSaveActionCursor = Tracked::getCursorIndex();
        nonSelChangeCursor = std::numeric_limits<std::size_t>::max();
        notifyNoUnsavedChanges();
        return true;
    }
    else
        return false;
}

bool GuiMap::setDoodadTile(size_t x, size_t y, u16 tileNum)
{
    if ( x > Scenario::getTileWidth() || y > Scenario::getTileHeight() )
        return false;

    Scenario::setTile(x, y, tileNum, Chk::Scope::Game);

    RECT rcTile;
    rcTile.left   = s32(x)*32-screenLeft;
    rcTile.right  = rcTile.left+32;
    rcTile.top    = s32(y)*32-screenTop;
    rcTile.bottom = rcTile.top+32;

    RedrawMap = true;
    RedrawMiniMap = true;
    InvalidateRect(getHandle(), &rcTile, true);
    return true;
}

void GuiMap::setTileValue(size_t tileX, size_t tileY, uint16_t tileValue)
{
    if ( tileX < read.dimensions.tileWidth && tileY < read.dimensions.tileHeight )
    {
        auto edit = createAction();
        edit->editorTiles[tileY*read.dimensions.tileWidth + tileX] = tileValue;
        if ( !tileOccupationCache.tileOccupied(tileX, tileY, read.dimensions.tileWidth) )
            edit->tiles[tileY*read.dimensions.tileWidth + tileX] = tileValue;
        else
            validateTileOccupiers(tileX, tileY, tileValue);
    }
}

void GuiMap::setFogValue(size_t tileX, size_t tileY, u8 fogValue)
{
    if ( tileX < read.dimensions.tileWidth && tileY < read.dimensions.tileHeight )
        Scenario::setFog(tileX, tileY, fogValue);
}

void GuiMap::beginTerrainOperation()
{
    refreshTileOccupationCache();
}

void GuiMap::finalizeTerrainOperation()
{
    clipboard.clearPreviousPasteLoc();
}

void GuiMap::finalizeFogOperation()
{
    clipboard.clearPreviousPasteLoc();
}

void GuiMap::validateTileOccupiers(size_t tileX, size_t tileY, uint16_t tileValue)
{
    auto edit = createAction();
    bool cacheRefreshNeeded = false;
    bool tileOccupiedByValidDoodad = false;
    const auto & tileset = chkd.scData.terrain.get(Scenario::getTileset());
    if ( !allowIllegalDoodads )
    {
        for ( int doodadIndex=int(read.doodads.size())-1; doodadIndex>=0; --doodadIndex )
        {
            const auto & doodad = read.doodads[doodadIndex];
            if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
            {
                const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
                const auto & placability = tileset.doodadPlacibility[doodad.type];

                bool evenWidth = doodadDat.tileWidth % 2 == 0;
                bool evenHeight = doodadDat.tileHeight % 2 == 0;
                size_t left = (size_t(doodad.xc) - 16*size_t(doodadDat.tileWidth))/32;
                size_t top = (size_t(doodad.yc) - 16*size_t(doodadDat.tileHeight))/32;
                size_t right = left + size_t(doodadDat.tileWidth);
                size_t bottom = top + size_t(doodadDat.tileHeight);

                if ( tileX >= left && tileX < right && tileY >= top && tileY < bottom ) // Tile within dimensions, though not necessarily part of the doodad
                {
                    size_t y = tileY-top;
                    const auto & tileGroup = tileset.tileGroups[(*doodadGroupIndex)+y];
                    size_t x = tileX-left;
                    if ( tileset.tileGroups[(*doodadGroupIndex)+y].megaTileIndex[x] != 0 ) // Tile is part of the doodad
                    {
                        auto doodadTilePlacability = placability.tileGroup[y*doodadDat.tileWidth+x];
                        if ( doodadTilePlacability != 0 && tileValue/16 != doodadTilePlacability ) // This doodad tile is illegal given the new tileValue
                        {
                            if ( !read.sprites.empty() )
                            {
                                for ( int i=int(read.sprites.size())-1; i>=0; --i )
                                {
                                    const auto & sprite = read.sprites[i];
                                    if ( sprite.type == doodadDat.overlayIndex && sprite.xc == doodad.xc && sprite.yc == doodad.yc )
                                        Scenario::deleteSprite(i);
                                }
                            }
                            for ( y=top; y<bottom; ++y )
                            {
                                for ( x=left; x<right; ++x )
                                    edit->tiles[y*size_t(read.dimensions.tileWidth)+x] = read.editorTiles[y*size_t(read.dimensions.tileWidth)+x];
                            }
                            Scenario::deleteDoodad(doodadIndex);
                            cacheRefreshNeeded = true;
                        }
                        else
                            tileOccupiedByValidDoodad = true;
                    }
                }
            }
        }
    }

    if ( !tileOccupiedByValidDoodad )
        edit->tiles[tileY*size_t(read.dimensions.tileWidth)+tileX] = tileValue;
    
    if ( !placeUnitsAnywhere || !placeBuildingsAnywhere )
    {
        for ( int unitIndex=int(read.units.size())-1; unitIndex>=0; --unitIndex )
        {
            const auto & unit = read.units[unitIndex];
            if ( unit.type < Sc::Unit::TotalTypes )
            {
                const auto & unitDat = chkd.scData.units.getUnit(unit.type);
                bool isBuilding = (unitDat.flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building;
                bool isFlyer = (unitDat.flags & Sc::Unit::Flags::Flyer) == Sc::Unit::Flags::Flyer;
                bool isFlyingBuilding = isBuilding &&
                    (unitDat.flags & Sc::Unit::Flags::FlyingBuilding) == Sc::Unit::Flags::FlyingBuilding &&
                    (unit.stateFlags & Chk::Unit::State::InTransit) == Chk::Unit::State::InTransit;

                if ( (isBuilding && !isFlyingBuilding && !placeBuildingsAnywhere) ||
                     (!isBuilding && !isFlyer && !placeUnitsAnywhere) )
                {
                    s32 left = s32(unit.xc) - s32(unitDat.unitSizeLeft);
                    s32 right = s32(unit.xc) + s32(unitDat.unitSizeRight);
                    s32 top = s32(unit.yc) - s32(unitDat.unitSizeUp);
                    s32 bottom = s32(unit.yc) + s32(unitDat.unitSizeDown);
            
                    s32 xTileMin = left/32;
                    s32 xTileMax = right/32;
                    s32 yTileMin = top/32;
                    s32 yTileMax = bottom/32;
                    if ( s32(tileX) >= xTileMin && s32(tileX) <= xTileMax && // Mini-tiles the unit overlaps must be examined
                         s32(tileY) >= yTileMin && s32(tileY) <= yTileMax )
                    {
                        size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileValue);
                        if ( groupIndex < tileset.tileGroups.size() )
                        {
                            const Sc::Terrain::TileGroup & tileGroup = tileset.tileGroups[groupIndex];
                            u16 megaTileIndex = tileGroup.megaTileIndex[tileset.getGroupMemberIndex(tileValue)];
                            s32 xTileStart = 32*s32(tileX);
                            s32 yTileStart = 32*s32(tileY);
                            s32 xMiniTileMin = left < xTileStart ? 0 : (left-xTileStart)/8;
                            s32 xMiniTileMax = right >= xTileStart+24 ? 4 : (right-xTileStart)/8+1;
                            s32 yMiniTileMin = top < yTileStart ? 0 : (top-yTileStart)/8;
                            s32 yMiniTileMax = bottom >= yTileStart+24 ? 4 : (bottom-yTileStart)/8+1;
                            [&]() {
                                for ( s32 yMiniTile = yMiniTileMin; yMiniTile < yMiniTileMax; ++yMiniTile )
                                {
                                    for ( s32 xMiniTile = xMiniTileMin; xMiniTile < xMiniTileMax; ++xMiniTile )
                                    {
                                        if ( !tileset.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile].isWalkable() ) // Unit is invalidated
                                        {
                                            unlinkAndDeleteUnit(unitIndex);
                                            cacheRefreshNeeded = true;
                                            return;
                                        }
                                    }
                                }
                            }();
                        }
                    }
                }
            }
        }
    }

    if ( cacheRefreshNeeded )
        refreshTileOccupationCache();
}

void GuiMap::setTileset(Sc::Terrain::Tileset tileset)
{
    Scenario::setTileset(tileset);
    scGraphics.updatePalette();
    (Chk::IsomCache &)(*this) = {read.tileset, Scenario::getTileWidth(), Scenario::getTileHeight(), chkd.scData.terrain.get(read.tileset)};
}

// From Scenario.cpp
void setTiledDimensions(std::vector<u8> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge);
void setTiledDimensions(std::vector<u16> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge);
template <typename T> std::vector<T> resizeAndOffset(const std::vector<T> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, std::ptrdiff_t leftEdge, std::ptrdiff_t topEdge);

struct SimpleCache : Chk::IsomCache
{
    Scenario & scenario;

    SimpleCache(Scenario & scenario, Sc::Terrain::Tileset tileset, u16 tileWidth, u16 tileHeight, const Sc::Terrain::Tiles & tiles)
        : Chk::IsomCache{tileset, tileWidth, tileHeight, tiles}, scenario(scenario) {}

    inline void setTileValue(size_t tileX, size_t tileY, uint16_t tileValue) final {
        auto edit = scenario.operator()();
        edit->editorTiles[tileY*scenario->dimensions.tileWidth + tileX] = tileValue;
        edit->tiles[tileY*scenario->dimensions.tileWidth + tileX] = tileValue;
    }
};

void GuiMap::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 sizeValidationFlags, s32 leftEdge, s32 topEdge, size_t newTerrainType)
{
    auto edit = createAction();

    // Selection indexes would be invalidated by a size change
    edit->tiles.clearSelections();
    edit->editorTiles.clearSelections();
    edit->isomRects.clearSelections();
    edit->tileFog.clearSelections();

    bool anywhereWasStandardDimensions = Scenario::anywhereIsStandardDimensions();

    auto destMapData = std::make_unique<Scenario>(this->getTileset(), u16(this->getTileWidth()), u16(this->getTileHeight()));
    Scenario & destMap = *destMapData;
    auto editDest = destMap.operator()();
    SimpleCache destMapCache(destMap, read.tileset, newTileWidth, newTileHeight, chkd.scData.terrain.get(read.tileset));
    
    editDest->tiles = read.tiles;
    editDest->editorTiles = read.editorTiles;
    editDest->tileFog = read.tileFog;

    editDest->tiles = ::resizeAndOffset(destMap->tiles, newTileWidth, newTileHeight, read.dimensions.tileWidth, read.dimensions.tileHeight, 0, 0);
    editDest->editorTiles = ::resizeAndOffset(destMap->editorTiles, newTileWidth, newTileHeight, read.dimensions.tileWidth, read.dimensions.tileHeight, 0, 0);
    editDest->tileFog = ::resizeAndOffset(destMap->tileFog, newTileWidth, newTileHeight, read.dimensions.tileWidth, read.dimensions.tileHeight, 0, 0);
    uint16_t isomValue = ((Chk::IsomCache::getTerrainTypeIsomValue(newTerrainType) << 4) | Chk::IsomRect::EditorFlag::Modified);

    editDest->dimensions = Chk::DIM{newTileWidth, newTileHeight};
    editDest->isomRects.assign((newTileWidth/2+1)*(newTileHeight+1), Chk::IsomRect{ isomValue, isomValue, isomValue, isomValue });
    
    destMap.copyIsomFrom(*this, leftEdge, topEdge, destMapCache);
    destMap.resizeIsom(leftEdge, topEdge, this->getTileWidth(), this->getTileHeight(), false, destMapCache);
    destMap.updateTilesFromIsom(destMapCache);

    Sc::BoundingBox tileRect { this->getTileWidth(), this->getTileHeight(), newTileWidth, newTileHeight, leftEdge, topEdge };
    size_t destStartX = leftEdge < 0 ? 0 : leftEdge;
    size_t destStartY = topEdge < 0 ? 0 : topEdge;
    size_t copyHeight = tileRect.bottom-tileRect.top;
    size_t copyWidth = tileRect.right-tileRect.left;
    if ( copyWidth+destStartX > newTileWidth )
        copyWidth = newTileWidth-destStartX;
    if ( copyHeight+destStartY > newTileHeight )
        copyHeight = newTileHeight-destStartY;

    for ( size_t y=0; y<copyHeight; ++y )
    {
        for ( size_t x=0; x<copyWidth; ++x )
        {
            editDest->editorTiles[(y+destStartY)*newTileWidth+(x+destStartX)] = read.editorTiles[(y+tileRect.top)*this->getTileWidth()+(x+tileRect.left)];
            editDest->tiles[(y+destStartY)*newTileWidth+(x+destStartX)] = read.tiles[(y+tileRect.top)*this->getTileWidth()+(x+tileRect.left)];
        }
    }

    edit->dimensions = destMap->dimensions;
    edit->isomRects = destMap->isomRects;
    edit->editorTiles = destMap->editorTiles;
    edit->tiles = destMap->tiles;
    edit->tileFog = destMap->tileFog;

    u16 pixelWidth = u16(Scenario::getPixelWidth());
    u16 pixelHeight = u16(Scenario::getPixelHeight());
    for ( int i=int(read.doodads.size()-1); i>=0; --i )
    {
        const auto & doodad = read.doodads[i];
        if ( u16(s32(doodad.xc) + 32*leftEdge) > pixelWidth || u16(s32(doodad.yc) + 32*topEdge) > pixelHeight )
            ((Scenario*)(this))->deleteDoodad(i);
        else
        {
            edit->doodads[i].xc += 32*leftEdge;
            edit->doodads[i].yc += 32*topEdge;
        }
    }
    for ( int i=int(read.sprites.size()-1); i>=0; --i )
    {
        const auto & sprite = read.sprites[i];
        if ( u16(s32(sprite.xc) + 32*leftEdge) > pixelWidth || u16(s32(sprite.yc) + 32*topEdge) > pixelHeight )
            ((Scenario*)(this))->deleteSprite(i);
        else
        {
            edit->sprites[i].xc += 32*leftEdge;
            edit->sprites[i].yc += 32*topEdge;
        }
    }
    for ( int i=int(read.units.size()-1); i>=0; --i )
    {
        const auto & unit = read.units[i];
        if ( u16(s32(unit.xc) + 32*leftEdge) > pixelWidth || u16(s32(unit.yc) + 32*topEdge) > pixelHeight )
            ((Scenario*)(this))->deleteUnit(i);
        else
        {
            edit->units[i].xc += 32*leftEdge;
            edit->units[i].yc += 32*topEdge;
        }
    }
    for ( std::size_t i=0; i<read.locations.size(); ++i )
    {
        const auto & location = read.locations[i];
        if ( location.isBlank() )
            continue;

        if ( leftEdge < 0 ) // Moving leftwards
        {
            if ( location.left + 32*leftEdge > location.left )
                edit->locations[i].left = 0;
            else
                edit->locations[i].left += 32*leftEdge;

            if ( location.right + 32*leftEdge > location.right )
                edit->locations[i].right = 0;
            else
                edit->locations[i].right += 32*leftEdge;
        }
        else // Moving rightwards
        {
            if ( location.left + 32*leftEdge > pixelWidth )
                edit->locations[i].left = pixelWidth;
            else
                edit->locations[i].left += 32*leftEdge;

            if ( location.right + 32*leftEdge > pixelWidth )
                edit->locations[i].right = pixelWidth;
            else
                edit->locations[i].right += 32*leftEdge;
        }

        if ( topEdge < 0 ) // Moving upwards
        {
            if ( location.top + 32*topEdge > location.top )
                edit->locations[i].top = 0;
            else
                edit->locations[i].top += 32*topEdge;

            if ( location.bottom + 32*topEdge > location.bottom )
                edit->locations[i].bottom = 0;
            else
                edit->locations[i].bottom += 32*topEdge;
        }
        else // Moving downwards
        {
            if ( location.top + 32*topEdge > pixelHeight )
                edit->locations[i].top = pixelHeight;
            else
                edit->locations[i].top += 32*topEdge;

            if ( location.bottom + 32*topEdge > pixelHeight )
                edit->locations[i].bottom = pixelHeight;
            else
                edit->locations[i].bottom += 32*topEdge;
        }
    }

    if ( anywhereWasStandardDimensions )
        Scenario::matchAnywhereToDimensions();

    (Chk::IsomCache &)(*this) = {read.tileset, Scenario::getTileWidth(), Scenario::getTileHeight(), chkd.scData.terrain.get(read.tileset)};
}

bool GuiMap::placeIsomTerrain(Chk::IsomDiamond isomDiamond, size_t terrainType, size_t brushExtent)
{
    if ( Scenario::placeIsomTerrain(isomDiamond, terrainType, brushExtent, *this) )
    {
        Scenario::updateTilesFromIsom(*this);
        return true;
    }
    return false;
}

void GuiMap::unlinkAndDeleteSelectedUnits()
{
    auto edit = createAction();
    if ( view.units.sel().size() == read.units.size() )
        edit->units.reset();
    else
    {
        for ( auto unitIndex : view.units.sel() )
        {
            const auto & toDelete = Scenario::getUnit(unitIndex);
            for ( size_t unitIndex = 0; unitIndex < read.units.size(); ++unitIndex )
            {
                const auto & unit = read.units[unitIndex];
                if ( unit.relationClassId == toDelete.classId )
                {
                    edit->units[unitIndex].relationClassId = 0;
                    edit->units[unitIndex].relationFlags = 0;
                    logger.info() << "Unit at index " << unitIndex << " unlinked from deleted unit" << std::endl;
                }
            }
        }
        edit->units.removeSelection();
    }
}

void GuiMap::unlinkAndDeleteUnit(size_t unitIndex)
{
    auto edit = createAction();
    const auto & toDelete = Scenario::getUnit(unitIndex);
    for ( size_t unitIndex = 0; unitIndex < read.units.size(); ++unitIndex )
    {
        const auto & unit = read.units[unitIndex];
        if ( unit.relationClassId == toDelete.classId )
        {
            edit->units[unitIndex].relationClassId = 0;
            edit->units[unitIndex].relationFlags = 0;
            logger.info() << "Unit at index " << unitIndex << " unlinked from deleted unit" << std::endl;
        }
    }
    Scenario::deleteUnit(unitIndex);
}

void GuiMap::changeUnitOwner(size_t unitIndex, u8 newPlayer)
{
    auto edit = createAction();
    const auto & toChange = Scenario::getUnit(unitIndex);
    if ( toChange.owner != newPlayer )
    {
        edit->units[unitIndex].owner = newPlayer;

        if ( toChange.relationClassId != 0 && addonAutoPlayerSwap )
        {
            bool mayHaveAddon =
                toChange.type == Sc::Unit::Type::TerranCommandCenter || toChange.type == Sc::Unit::Type::TerranScienceFacility ||
                toChange.type == Sc::Unit::Type::TerranFactory || toChange.type == Sc::Unit::Type::TerranStarport;

            if ( mayHaveAddon )
            {
                for ( size_t unitIndex = 0; unitIndex < read.units.size(); ++unitIndex )
                {
                    const auto & unit = read.units[unitIndex];
                    if ( toChange.relationClassId == unit.classId )
                    {
                        edit->units[unitIndex].owner = newPlayer;
                        edit->units[unitIndex].validFieldFlags |= Chk::Unit::ValidField::Owner;
                    }
                }
            }
        }
    }
}

Layer GuiMap::getLayer()
{
    return currLayer;
}

bool GuiMap::setLayer(Layer newLayer)
{
    chkd.maps.endPaste();
    ClipCursor(NULL);
    Redraw(false);
    selections.setDrags(-1, -1);
    selections.clear();
    currLayer = newLayer;
    return true;
}

TerrainSubLayer GuiMap::getSubLayer()
{
    return currTerrainSubLayer;
}

void GuiMap::setSubLayer(TerrainSubLayer newTerrainSubLayer)
{
    this->currTerrainSubLayer = newTerrainSubLayer;
}

double GuiMap::getZoom()
{
    return zoom;
}

void GuiMap::setZoom(double newScale)
{
    scrGraphics->setZoom(newScale);
    for ( u32 i=0; i<defaultZooms.size(); i++ )
    {
        if ( newScale == defaultZooms[i] )
            chkd.mainToolbar.zoomBox.SetSel(i);
    }
    zoom = newScale;
    RedrawMap = true;
    RedrawMiniMap = true;

    Scroll(true, true, true);
    UpdateZoomMenuItems();
    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

u8 GuiMap::getCurrPlayer() const
{
    return currPlayer;
}

bool GuiMap::setCurrPlayer(u8 newPlayer)
{
    currPlayer = newPlayer;
    return true;
}

bool GuiMap::getCutCopyPasteTerrain() const
{
    return cutCopyPasteTerrain;
}

void GuiMap::toggleCutCopyPasteTerrain()
{
    cutCopyPasteTerrain = !cutCopyPasteTerrain;
}

bool GuiMap::getCutCopyPasteDoodads() const
{
    return cutCopyPasteDoodads;
}

void GuiMap::toggleCutCopyPasteDoodads()
{
    cutCopyPasteDoodads = !cutCopyPasteDoodads;
}

bool GuiMap::getCutCopyPasteSprites() const
{
    return cutCopyPasteSprites;
}

void GuiMap::toggleCutCopyPasteSprites()
{
    cutCopyPasteSprites = !cutCopyPasteSprites;
}

bool GuiMap::getCutCopyPasteUnits() const
{
    return cutCopyPasteUnits;
}

void GuiMap::toggleCutCopyPasteUnits()
{
    cutCopyPasteUnits = !cutCopyPasteUnits;
}

bool GuiMap::getCutCopyPasteFog() const
{
    return cutCopyPasteFog;
}

void GuiMap::toggleCutCopyPasteFog()
{
    cutCopyPasteFog = !cutCopyPasteFog;
}

bool GuiMap::isDragging()
{
    return dragging;
}

void GuiMap::setDragging(bool bDragging)
{
    dragging = bDragging;
}

void GuiMap::convertSelectionToTerrain()
{
    if ( !view.doodads.sel().empty() )
    {
        auto edit = createAction(ActionDescriptor::ConvertDoodad);
        std::vector<size_t> selectedDoodads = view.doodads.sel();
        edit->doodads.clearSelections();

        std::sort(selectedDoodads.begin(), selectedDoodads.end(), [&](size_t lhs, size_t rhs) { return lhs > rhs; }); // Highest index to lowest
        for ( size_t doodadIndex : selectedDoodads ) {
            Chk::Doodad doodad = this->getDoodad(doodadIndex);
            Scenario::deleteDoodad(doodadIndex);

            const auto & tileset = chkd.scData.terrain.get(Scenario::getTileset());
            if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
            {
                const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
                bool evenWidth = doodadDat.tileWidth%2 == 0;
                bool evenHeight = doodadDat.tileHeight%2 == 0;
                auto xStart = evenWidth ? (doodad.xc+16)/32 - doodadDat.tileWidth/2 : doodad.xc/32 - (doodadDat.tileWidth-1)/2;
                auto yStart = evenHeight ? (doodad.yc+16)/32 - doodadDat.tileHeight/2 : doodad.yc/32 - (doodadDat.tileHeight-1)/2;

                for ( u16 y=0; y<doodadDat.tileHeight; ++y )
                {
                    auto yc = yStart+y;
                    u16 tileGroupIndex = *doodadGroupIndex + y;
                    const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
                    for ( u16 x=0; x<doodadDat.tileWidth; ++x )
                    {
                        auto xc = xStart+x;
                        if ( tileGroup.megaTileIndex[x] != 0 )
                        {
                            auto currDoodadTile = 16*tileGroupIndex + x;
                            auto currFinalTile = Scenario::getTile(xc, yc, Chk::Scope::Game);
                            auto underlyingTile = Scenario::getTile(xc, yc, Chk::Scope::Editor);
                            if ( currDoodadTile != underlyingTile ) // TILE
                                Scenario::setTile(xc, yc, currDoodadTile, Chk::Scope::Editor);

                            if ( currDoodadTile != currFinalTile ) // MTXM
                                Scenario::setTile(xc, yc, currDoodadTile, Chk::Scope::Game);
                        }
                    }
                }
            }
        }
        refreshTileOccupationCache();
    }
}

void GuiMap::stackSelected()
{
    int stackSize = 0;
    MoveToDialog<int>::GetIndex(stackSize, getHandle(), "Stack Selected...", "Stack Size:");
    if ( stackSize <= 0 )
        return;

    if ( currLayer == Layer::Units && selections.hasUnits() && Scenario::numUnits() + (size_t(stackSize)*selections.numUnits()) < 4000 )
    {
        auto edit = createAction(ActionDescriptor::StackUnits);
        for ( auto selectedUnit : view.units.sel() )
        {
            auto unit = CM->getUnit(selectedUnit);
            std::vector<Chk::Unit> addedUnits(stackSize, unit);
            edit->units.append(addedUnits);
        }

        if ( chkd.unitWindow.getHandle() != nullptr )
            chkd.unitWindow.RepopulateList();
    }
    else if ( currLayer == Layer::Sprites && selections.hasSprites() && Scenario::numSprites() + (size_t(stackSize)*selections.numSprites()) < 4000 )
    {
        auto edit = createAction(ActionDescriptor::StackSprites);
        for ( auto selectedSprite : view.sprites.sel() )
        {
            auto sprite = CM->getSprite(selectedSprite);
            std::vector<Chk::Sprite> addedSprites(stackSize, sprite);
            edit->sprites.append(addedSprites);
        }

        if ( chkd.spriteWindow.getHandle() != nullptr )
            chkd.spriteWindow.RepopulateList();
    }
}

void GuiMap::createLocation()
{
    if ( selections.hasUnits() )
    {
        auto edit = createAction(ActionDescriptor::CreateLocationForUnit);
        u16 firstUnitId = selections.getFirstUnit();
        const Chk::Unit & unit = Scenario::getUnit(firstUnitId);
        const auto & unitDat = chkd.scData.units.getUnit(unit.type);
        Chk::Location newLocation {};
        newLocation.left = unit.xc - unitDat.unitSizeLeft;
        newLocation.top = unit.yc - unitDat.unitSizeUp;
        newLocation.right = unit.xc + unitDat.unitSizeRight;
        newLocation.bottom = unit.yc + unitDat.unitSizeDown;
        newLocation.elevationFlags = 0;
        selections.setDrags(-1, -1);

        size_t newLocationId = Scenario::addLocation(newLocation);
        if ( newLocationId != Chk::LocationId::NoLocation )
        {
            CM->setLayer(Layer::Locations);
            Scenario::setLocationName<RawString>(newLocationId, "Location " + std::to_string(newLocationId), Chk::Scope::Game);
            Scenario::deleteUnusedStrings(Chk::Scope::Both);
            selections.selectLocation(u16(newLocationId));
            chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree(true);
            refreshScenario();
        }
        else
            Error("Max Locations Reached!");
    }
    else
        logger.warn("No units selected to create location");
}

void GuiMap::createInvertedLocation()
{
    if ( selections.hasUnits() )
    {
        auto edit = createAction(ActionDescriptor::CreateInvertedLocationForUnit);
        u16 firstUnitId = selections.getFirstUnit();
        const Chk::Unit & unit = Scenario::getUnit(firstUnitId);
        const auto & unitDat = chkd.scData.units.getUnit(unit.type);
        Chk::Location newLocation {};
        newLocation.left = unit.xc + unitDat.unitSizeRight;
        newLocation.top = unit.yc + unitDat.unitSizeDown;
        newLocation.right = unit.xc - unitDat.unitSizeLeft;
        newLocation.bottom = unit.yc - unitDat.unitSizeUp;
        newLocation.elevationFlags = 0;
        selections.setDrags(-1, -1);

        size_t newLocationId = Scenario::addLocation(newLocation);
        if ( newLocationId != Chk::LocationId::NoLocation )
        {
            CM->setLayer(Layer::Locations);
            Scenario::setLocationName<RawString>(newLocationId, "Location " + std::to_string(newLocationId), Chk::Scope::Game);
            Scenario::deleteUnusedStrings(Chk::Scope::Both);
            selections.selectLocation(u16(newLocationId));
            chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree(true);
            refreshScenario();
        }
        else
            Error("Max Locations Reached!");
    }
    else
        logger.warn("No units selected to create location");
}

void GuiMap::createMobileInvertedLocation()
{
    if ( selections.hasUnits() )
    {
        auto edit = createAction(ActionDescriptor::CreateMobileInvertedLocationForUnit);
        u16 firstUnitId = selections.getFirstUnit();
        const Chk::Unit & unit = Scenario::getUnit(firstUnitId);
        const auto & unitDat = chkd.scData.units.getUnit(unit.type);

        s32 width = (unitDat.unitSizeRight > unitDat.unitSizeLeft ? -1 : 0) - 2*std::min(unitDat.unitSizeLeft, unitDat.unitSizeRight);
        s32 height = (unitDat.unitSizeDown > unitDat.unitSizeUp ? -1 : 0) - 2*std::min(unitDat.unitSizeUp, unitDat.unitSizeDown);

        Chk::Location newLocation {};
        newLocation.right = u16(s32(unit.xc) + width/2);
        newLocation.bottom = u16(s32(unit.yc) + height/2);
        newLocation.left = newLocation.right - width;
        newLocation.top = newLocation.bottom - height;
        newLocation.elevationFlags = 0;
        selections.setDrags(-1, -1);

        size_t newLocationId = Scenario::addLocation(newLocation);
        if ( newLocationId != Chk::LocationId::NoLocation )
        {
            CM->setLayer(Layer::Locations);
            Scenario::setLocationName<RawString>(newLocationId, "Location " + std::to_string(newLocationId), Chk::Scope::Game);
            Scenario::deleteUnusedStrings(Chk::Scope::Both);
            selections.selectLocation(u16(newLocationId));
            chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree(true);
            refreshScenario();
        }
        else
            Error("Max Locations Reached!");
    }
    else
        logger.warn("No units selected to create location");
}

void GuiMap::viewUnit(u16 unitIndex)
{
    RECT rect {};
    if ( unitIndex < Scenario::numUnits() && GetClientRect(getHandle(), &rect) != 0 )
    {
        const Chk::Unit & unit = Scenario::getUnit(unitIndex);
        s32 width = rect.right - rect.left,
            height = rect.bottom - rect.top;

        screenLeft = unit.xc - width/2;
        screenTop = unit.yc - height/2;

        Scroll(true, true, true);
        Redraw(false);
    }
}

void GuiMap::viewSprite(u16 spriteIndex)
{
    RECT rect {};
    if ( spriteIndex < Scenario::numSprites() && GetClientRect(getHandle(), &rect) != 0 )
    {
        const Chk::Sprite & sprite = Scenario::getSprite(spriteIndex);
        s32 width = rect.right - rect.left,
            height = rect.bottom - rect.top;

        screenLeft = sprite.xc - width/2;
        screenTop = sprite.yc - height/2;

        Scroll(true, true, true);
        Redraw(false);
    }
}

void GuiMap::viewLocation(u16 locationId)
{
    RECT rect {};
    if ( locationId < Scenario::numLocations() && GetClientRect(getHandle(), &rect) != 0 )
    {
        const Chk::Location & location = Scenario::getLocation(locationId);
        s32 width = rect.right - rect.left,
            height = rect.bottom - rect.top,
            locLeft = std::min(location.left, location.right),
            locRight = std::max(location.left, location.right),
            locTop = std::min(location.top, location.bottom),
            locBottom = std::max(location.top, location.bottom),
            locCenterX = locLeft+(locRight-locLeft)/2,
            locCenterY = locTop+(locBottom-locTop)/2;

        screenLeft = locCenterX - width/2;
        screenTop = locCenterY - height/2;

        Scroll(true, true, true);
        Redraw(false);
    }
}

LocSelFlags GuiMap::getLocSelFlags(s32 xc, s32 yc)
{
    if ( currLayer == Layer::Locations )
    {
        u16 selectedLocation = selections.getSelectedLocation();
        if ( selectedLocation != NO_LOCATION && selectedLocation < Scenario::numLocations() ) // If location is selected, determine which part of it is hovered by mouse
        {
            const Chk::Location & loc = Scenario::getLocation(selectedLocation);
            s32 locationLeft = std::min(loc.left, loc.right),
                locationRight = std::max(loc.left, loc.right),
                locationTop = std::min(loc.top, loc.bottom),
                locationBottom = std::max(loc.top, loc.bottom),
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

                if ( xc >= leftInnerBound && xc <= rightInnerBound &&   // Location in absolute center
                        yc >= topInnerBound && yc <= bottomInnerBound    )
                {
                    return LocSelFlags::Middle;
                } // Invariant: not in center
                else if ( xc >= leftInnerBound && xc <= rightInnerBound ) // Location in horizontal center
                {
                    if ( yc > bottomInnerBound )
                        return LocSelFlags::South;
                    else
                        return LocSelFlags::North;
                } // Invariant: on west or east
                else if ( yc >= topInnerBound && yc <= bottomInnerBound )
                {
                    if ( xc > rightInnerBound )
                        return LocSelFlags::East;
                    else
                        return LocSelFlags::West;
                } // Invariant: is on a corner
                else if ( xc < leftInnerBound && yc < topInnerBound )
                    return LocSelFlags::NorthWest;
                else if ( xc > rightInnerBound && yc > bottomInnerBound )
                    return LocSelFlags::SouthEast;
                else if ( xc < leftInnerBound && yc > bottomInnerBound )
                    return LocSelFlags::SouthWest;
                else if ( xc > rightInnerBound && yc < topInnerBound )
                    return LocSelFlags::NorthEast;
            }
        }
    }
    return LocSelFlags::None;
}

bool GuiMap::moveLocation(u32 downX, u32 downY, u32 upX, u32 upY)
{
    return false;
}

void GuiMap::doubleClickLocation(s32 xPos, s32 yPos)
{
    xPos = (s32(((double)xPos) / getZoom()) + screenLeft),
    yPos = (s32(((double)yPos) / getZoom()) + screenTop);
    u16 selectedLoc = selections.getSelectedLocation();
    if ( selectedLoc == NO_LOCATION )
    {
        selections.selectLocation(xPos, yPos, !lockAnywhere);
        this->Redraw(false);
        selectedLoc = selections.getSelectedLocation();
    }

    if ( selectedLoc != NO_LOCATION && selectedLoc < Scenario::numLocations() )
    {
        const Chk::Location & locRef = Scenario::getLocation(selectedLoc);
        s32 locLeft = std::min(locRef.left, locRef.right),
            locRight = std::max(locRef.left, locRef.right),
            locTop = std::min(locRef.top, locRef.bottom),
            locBottom = std::max(locRef.top, locRef.bottom);
    
        if ( xPos >= locLeft && xPos <= locRight && yPos >= locTop && yPos <= locBottom )
        {
            if ( chkd.locationWindow.getHandle() == NULL )
            {
                chkd.locationWindow.CreateThis(chkd.getHandle());
                ShowWindow(chkd.locationWindow.getHandle(), SW_SHOWNORMAL);
            }
            else
            {
                chkd.locationWindow.RefreshLocationInfo();
                ShowWindow(chkd.locationWindow.getHandle(), SW_SHOW);
            }
        }
    }
}

void GuiMap::openTileProperties(s32 xClick, s32 yClick)
{
    auto edit = createAction();
    std::size_t tileIndex = (yClick/32)*Scenario::getTileWidth()+(xClick/32);
    if ( !(view.tiles.sel().size() == 1 && view.tiles.sel().front() == tileIndex) )
    {
        edit->tiles.clearSelections();
        edit->tiles.select(tileIndex);
    }

    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
    if ( chkd.tilePropWindow.getHandle() != NULL )
        chkd.tilePropWindow.UpdateTile();
    else
        chkd.tilePropWindow.CreateThis(chkd.getHandle());

    ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
}

void GuiMap::EdgeDrag(HWND hWnd, int x, int y)
{
    if ( isDragging() )
    {
        s32 mapX = s32(x/zoom) + screenLeft;
        s32 mapY = s32(y/zoom) + screenTop;

        RECT rcMap;
        GetClientRect(hWnd, &rcMap);
        TrackMouse(defaultHoverTime);
        bool onEdge = false;
        if ( x <= 0 ) // Cursor on the left
        {
            mapX = s32(rcMap.left/zoom) + screenLeft;
            onEdge = true;
            if ( (screenLeft+16)/32 > 0 )
                selections.endDrag = { ((screenLeft +16)/32-1)*32, selections.endDrag.y };
            if ( screenLeft > 0 )
                screenLeft = selections.endDrag.x;
        }
        else if ( x >= rcMap.right-2 ) // Cursor on the right
        {
            mapX = s32((double(rcMap.right)-2)/zoom) + screenLeft;
            onEdge = true;
            selections.endDrag = { ((screenLeft+s32(rcMap.right/zoom))/32+1)*32, selections.endDrag.y };
            screenLeft = selections.endDrag.x - s32((double(rcMap.right)-2)/zoom);
        }

        if ( y <= 0 ) // Cursor on the top
        {
            mapY = s32(rcMap.top/zoom) + screenTop;
            onEdge = true;
            if ( (screenTop+16)/32 > 0 )
                selections.endDrag = { selections.endDrag.x, ((screenTop+16)/32-1)*32 };
            if ( screenTop > 0 )
                screenTop = selections.endDrag.y;
        }
        else if ( y >= rcMap.bottom-2 ) // Cursor on the bottom
        {
            mapY = s32((double(rcMap.bottom)-2)/zoom) + screenTop;
            onEdge = true;
            selections.endDrag = { selections.endDrag.x, ((screenTop+s32(rcMap.bottom/zoom))/32+1)*32 };
            screenTop = selections.endDrag.y - s32((double(rcMap.bottom)-2)/zoom);
        }

        if ( onEdge )
        {
            Scroll(true, true, true);
            if ( clipboard.isPasting() )
                paste(mapX, mapY);

            Redraw(false);
        }
    }
}

u8 GuiMap::GetPlayerOwnerStringId(u8 player)
{
    Sc::Player::SlotType slotType = Scenario::getSlotType(player);
    switch ( slotType )
    {
        case Sc::Player::SlotType::GameComputer: case Sc::Player::SlotType::Computer: return 2; // Computer
        case Sc::Player::SlotType::GameHuman: case Sc::Player::SlotType::Human: return 3; // Human
        case Sc::Player::SlotType::RescuePassive: return 1; // Rescuable
        case Sc::Player::SlotType::Neutral: return 4; // Neutral
    }
    return 0; // Unused
}

void GuiMap::refreshScenario(bool clearSelections)
{
    if ( clearSelections )
    {
        auto edit = createAction();
        edit->tiles.clearSelections();
        edit->doodads.clearSelections();
        edit->units.clearSelections();
        edit->sprites.clearSelections();
        edit->tileFog.clearSelections();
    }
    chkd.mainPlot.leftBar.blockSelections = true;
    chkd.mainPlot.leftBar.mainTree.isomTree.UpdateIsomTree();
    chkd.mainPlot.leftBar.mainTree.doodadTree.UpdateDoodadTree();
    chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree(currLayer == Layer::Locations);
    chkd.mainPlot.leftBar.blockSelections = false;
    
    if ( chkd.unitWindow.getHandle() != nullptr )
        chkd.unitWindow.RepopulateList();
    if ( chkd.spriteWindow.getHandle() != nullptr )
        chkd.spriteWindow.RepopulateList();
    if ( chkd.locationWindow.getHandle() != NULL )
    {
        if ( CM->numLocations() == 0 )
            chkd.locationWindow.DestroyThis();
        else
            chkd.locationWindow.RefreshLocationInfo();
    }
    if ( chkd.mapSettingsWindow.getHandle() != NULL )
        chkd.mapSettingsWindow.RefreshWindow();
    if ( chkd.dimensionsWindow.getHandle() != NULL )
        chkd.dimensionsWindow.RefreshWindow();
    chkd.trigEditorWindow.RefreshWindow();
    chkd.briefingTrigEditorWindow.RefreshWindow();
    
    scGraphics.updatePalette();
    Redraw(true);
}

void GuiMap::clearSelectedTiles()
{
    createAction(ActionDescriptor::ClearTileSel)->tiles.clearSelections();
}

void GuiMap::clearSelectedDoodads()
{
    createAction(ActionDescriptor::ClearDoodadSel)->doodads.clearSelections();
}

void GuiMap::clearSelectedUnits()
{
    createAction(ActionDescriptor::ClearUnitSel)->units.clearSelections();
}

void GuiMap::clearSelectedSprites()
{
    createAction(ActionDescriptor::ClearSpriteSel)->sprites.clearSelections();
}

void GuiMap::clearSelection()
{
    auto edit = createAction();
    edit->tiles.clearSelections();
    edit->doodads.clearSelections();
    edit->sprites.clearSelections();
    edit->units.clearSelections();
    edit->tileFog.clearSelections();
    selections.removeLocations();
}

void GuiMap::selectAll()
{
    auto edit = createAction();
    auto selectAllTiles = [&]() {
        edit->tiles.selectAll();
    };
    auto selectAllUnits = [&]() {
        chkd.unitWindow.SetChangeHighlightOnly(true);
        for ( u16 i=0; i<Scenario::numUnits(); i++ )
        {
            if ( !selections.unitIsSelected(i) )
            {
                if ( chkd.unitWindow.getHandle() != nullptr )
                    chkd.unitWindow.FocusAndSelectIndex(i);
            }
        }
        edit->units.selectAll();
        chkd.unitWindow.SetChangeHighlightOnly(false);
        Redraw(true);
    };
    auto selectAllDoodads = [&]() {
        for ( size_t i=0; i<Scenario::numDoodads(); ++i )
        {
            if ( !selections.doodadIsSelected(i) )
                edit->doodads.select(i);
        }
    };
    auto selectAllSprites = [&]() {
        chkd.spriteWindow.SetChangeHighlightOnly(true);

        for ( size_t i=0; i<Scenario::numSprites(); ++i )
        {
            if ( !selections.spriteIsSelected(i) )
            {
                edit->sprites.select(i);
                if ( chkd.spriteWindow.getHandle() != nullptr )
                    chkd.spriteWindow.FocusAndSelectIndex(u16(i));
            }
        }
        chkd.spriteWindow.SetChangeHighlightOnly(false);
    };
    auto selectAllFog = [&]() {
        edit->tileFog.selectAll();
    };
    switch ( currLayer )
    {
        case Layer::Terrain:
            selectAllTiles();
            Redraw(true);
            break;
        case Layer::Units:
            selectAllUnits();
            Redraw(true);
            break;
        case Layer::Doodads:
            selectAllDoodads();
            Redraw(true);
            break;
        case Layer::Sprites:
            selectAllSprites();
            Redraw(true);
            break;
        case Layer::CutCopyPaste:
            if ( cutCopyPasteTerrain )
                selectAllTiles();
            if ( cutCopyPasteUnits )
                selectAllUnits();
            if ( cutCopyPasteDoodads )
                selectAllDoodads();
            if ( cutCopyPasteSprites )
                selectAllSprites();
            if ( cutCopyPasteFog )
                selectAllFog();
            
            Redraw(true);
            break;
    }
}

void GuiMap::deleteSelection()
{
    auto edit = createAction();
    auto deleteTerrainSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteTiles);
        auto tileWidth = Scenario::getTileWidth();
        for ( auto tileIndex : view.tiles.sel() )
        {
            auto x = tileIndex % tileWidth;
            auto y = tileIndex / tileWidth;
            if ( read.tiles[tileIndex] != 0 )
                edit->tiles[tileIndex] = 0;
            if ( read.editorTiles[tileIndex] != 0 )
                edit->editorTiles[tileIndex] = 0;
        }
        edit->tiles.clearSelections();
        edit->editorTiles.clearSelections();
    };
    auto deleteDoodadSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteDoodad);
        for ( int i=0; i<view.doodads.sel().size(); ++i )
        {
            auto selDoodad = view.doodads.sel()[i];
            const auto & doodad = read.doodads[selDoodad];

            const auto & tileset = chkd.scData.terrain.get(Scenario::getTileset());
            if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
            {
                const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
                bool evenWidth = doodadDat.tileWidth%2 == 0;
                bool evenHeight = doodadDat.tileHeight%2 == 0;
                auto xStart = evenWidth ? (doodad.xc+16)/32 - doodadDat.tileWidth/2 : doodad.xc/32 - (doodadDat.tileWidth-1)/2;
                auto yStart = evenHeight ? (doodad.yc+16)/32 - doodadDat.tileHeight/2 : doodad.yc/32 - (doodadDat.tileHeight-1)/2;

                for ( u16 y=0; y<doodadDat.tileHeight; ++y )
                {
                    auto yc = yStart+y; 
                    u16 tileGroupIndex = *doodadGroupIndex + y;
                    const auto & tileGroup = tileset.tileGroups[tileGroupIndex];
                    for ( u16 x=0; x<doodadDat.tileWidth; ++x )
                    {
                        auto xc = xStart+x;
                        if ( tileGroup.megaTileIndex[x] != 0 )
                        {
                            auto currDoodadTile = 16*tileGroupIndex + x;
                            if ( xc < Scenario::getTileWidth() && yc < Scenario::getTileHeight() )
                            {
                                auto currFinalTile = Scenario::getTile(xc, yc, Chk::Scope::Game);
                                auto underlyingTile = Scenario::getTile(xc, yc, Chk::Scope::Editor);
                                if ( currDoodadTile == currFinalTile && currDoodadTile != underlyingTile )
                                    Scenario::setTile(xc, yc, underlyingTile, Chk::Scope::Game);
                            }
                        }
                    }
                }

                if ( !read.sprites.empty() )
                {
                    for ( int i=int(read.sprites.size())-1; i>=0; --i )
                    {
                        const auto & sprite = read.sprites[i];
                        if ( sprite.type == doodadDat.overlayIndex && sprite.xc == doodad.xc && sprite.yc == doodad.yc )
                            Scenario::deleteSprite(i);
                    }
                }
            }
        }
        edit->doodads.removeSelection();
    };
    auto deleteUnitSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteUnits);
        if ( chkd.unitWindow.getHandle() != nullptr )
            SendMessage(chkd.unitWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, NULL), 0);
        else
            unlinkAndDeleteSelectedUnits();
    };
    auto deleteLocationSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteLocations);
        if ( chkd.locationWindow.getHandle() != NULL )
            chkd.locationWindow.DestroyThis();
                
        u16 index = selections.getSelectedLocation();
        if ( index != NO_LOCATION && index < Scenario::numLocations() )
        {
            const Chk::Location & loc = Scenario::getLocation(index);

            chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

            edit->locations[index].elevationFlags = 0;
            edit->locations[index].left = 0;
            edit->locations[index].right = 0;
            edit->locations[index].top = 0;
            edit->locations[index].bottom = 0;
            u16 stringNum = loc.stringId;
            edit->locations[index].stringId = 0;
            if ( stringNum > 0 )
            {
                Scenario::deleteString(stringNum);
                refreshScenario();
            }

            selections.selectLocation(NO_LOCATION);
        }
    };
    auto deleteSpriteSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteSprites);
        if ( chkd.spriteWindow.getHandle() != nullptr )
            SendMessage(chkd.spriteWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, NULL), 0);
        else if ( selections.hasSprites() )
            edit->sprites.removeSelection();
    };
    auto deleteFogTileSelection = [&]() {
        setActionDescription(ActionDescriptor::DeleteFogTiles);
        auto tileWidth = Scenario::getTileWidth();
        for ( auto tileIndex : view.tileFog.sel() )
        {
            auto x = tileIndex % tileWidth;
            auto y = tileIndex / tileWidth;
            if ( read.tileFog[tileIndex] != 0 )
                edit->tileFog[tileIndex] = 0;
        }
        edit->tileFog.clearSelections();
    };
   switch ( currLayer )
    {
        case Layer::Terrain:
            beginTerrainOperation();
            deleteTerrainSelection();
            finalizeTerrainOperation();
            break;
        case Layer::Doodads:
            deleteDoodadSelection();
            break;
        case Layer::Units:
            deleteUnitSelection();
            break;
        case Layer::Locations:
            deleteLocationSelection();
            break;
        case Layer::Sprites:
            deleteSpriteSelection();
            break;
        case Layer::CutCopyPaste:
            setActionDescription(ActionDescriptor::DeleteSelection);
            refreshTileOccupationCache();
            deleteTerrainSelection();
            deleteDoodadSelection();
            deleteUnitSelection();
            deleteSpriteSelection();
            deleteFogTileSelection();
            finalizeTerrainOperation();
            finalizeFogOperation();
            break;
    }

    RedrawMap = true;
    RedrawMiniMap = true;
    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void GuiMap::paste(s32 mapClickX, s32 mapClickY)
{
    auto edit = createAction();
    edit->tiles.clearSelections();
    s32 xc = mapClickX, yc = mapClickY;
    selections.endDrag = {xc, yc};
    SnapSelEndDrag();
    xc = selections.endDrag.x;
    yc = selections.endDrag.y;
    clipboard.doPaste(currLayer, currTerrainSubLayer, xc, yc, *this, stackUnits);

    Redraw(true);
}

void GuiMap::PlayerChanged(u8 newPlayer)
{
    auto edit = createAction();
    if ( currLayer == Layer::Units )
    {
        for ( auto unitIndex : view.units.sel() )
        {
            CM->changeUnitOwner(unitIndex, newPlayer);

            if ( chkd.unitWindow.getHandle() != nullptr )
                chkd.unitWindow.ChangeUnitsDisplayedOwner(unitIndex, newPlayer);
        }
        chkd.unitWindow.ChangeDropdownPlayer(newPlayer);
        Redraw(true);
    }
    else if ( currLayer == Layer::Sprites )
    {
        for ( size_t spriteIndex : view.sprites.sel() )
        {
            const Chk::Sprite & sprite = Scenario::getSprite(spriteIndex);
            edit->sprites[spriteIndex].owner = newPlayer;

            if ( chkd.spriteWindow.getHandle() != nullptr )
                chkd.spriteWindow.ChangeSpritesDisplayedOwner(int(spriteIndex), newPlayer);
        }
        chkd.spriteWindow.ChangeDropdownPlayer(newPlayer);
        Redraw(true);
    }
}

u16 GuiMap::GetSelectedLocation()
{
    return selections.getSelectedLocation();
}

bool GuiMap::autoSwappingAddonPlayers()
{
    return addonAutoPlayerSwap;
}

void GuiMap::moveSelection(Direction direction, bool useGrid)
{
    auto edit = createAction();
    if ( useGrid )
    {
        u16 gridOffX = 0, gridOffY = 0;
        u16 gridWidth = 32, gridHeight = 32;
        if ( !scGraphics.GetGridSize(0, gridWidth, gridHeight) || gridWidth == 0 || gridHeight == 0 )
        {
            gridWidth = 32;
            gridHeight = 32;
        }
        scGraphics.GetGridOffset(0, gridOffX, gridOffY);
        switch ( currLayer )
        {
        case Layer::Units:
        {
            if ( selections.hasUnits() )
            {
                auto & firstUnit = Scenario::getUnit(selections.getFirstUnit());
                switch ( direction )
                {
                    case Direction::Left:
                    {
                        auto diff = ((firstUnit.xc-gridOffX) % gridWidth) > 0 ? ((firstUnit.xc-gridOffX) % gridWidth) : gridWidth;
                        for ( auto unitIndex : view.units.sel() )
                            edit->units[unitIndex].xc -= diff;
                    }
                    break;
                    case Direction::Up:
                    {
                        auto diff = ((firstUnit.yc-gridOffY) % gridHeight) > 0 ? ((firstUnit.yc-gridOffY) % gridHeight) : gridHeight;
                        for ( auto unitIndex : view.units.sel() )
                            edit->units[unitIndex].yc -= diff;
                    }
                    break;
                    case Direction::Right:
                    {
                        auto diff = ((firstUnit.xc-gridOffX) % gridWidth) > 0 ? (gridWidth-((firstUnit.xc-gridOffX) % gridWidth)) : gridWidth;
                        for ( auto unitIndex : view.units.sel() )
                            edit->units[unitIndex].xc += diff;
                    }
                    break;
                    case Direction::Down:
                    {
                        auto diff = ((firstUnit.yc-gridOffY) % gridHeight) > 0 ? (gridHeight-((firstUnit.yc-gridOffY) % gridHeight)) : gridHeight;
                        for ( auto unitIndex : view.units.sel() )
                            edit->units[unitIndex].yc += diff;
                    }
                    break;
                }
            }
        }
        break;
        case Layer::Sprites:
            if ( selections.hasSprites() )
            {
                auto & firstSprite = Scenario::getSprite(selections.getFirstSprite());
                switch ( direction )
                {
                    case Direction::Left:
                    {
                        auto diff = ((firstSprite.xc-gridOffX) % gridWidth) > 0 ? ((firstSprite.xc-gridOffX) % gridWidth) : gridWidth;
                        for ( auto spriteIndex : view.sprites.sel() )
                            edit->sprites[spriteIndex].xc -= diff;
                    }
                    break;
                    case Direction::Up:
                    {
                        auto diff = ((firstSprite.yc-gridOffY) % gridHeight) > 0 ? ((firstSprite.yc-gridOffY) % gridHeight) : gridHeight;
                        for ( auto spriteIndex : view.sprites.sel() )
                            edit->sprites[spriteIndex].yc -= diff;
                    }
                    break;
                    case Direction::Right:
                    {
                        auto diff = ((firstSprite.xc-gridOffX) % gridWidth) > 0 ? (gridWidth-((firstSprite.xc-gridOffX) % gridWidth)) : gridWidth;
                        for ( auto spriteIndex : view.sprites.sel() )
                            edit->sprites[spriteIndex].xc += diff;
                    }
                    break;
                    case Direction::Down:
                    {
                        auto diff = ((firstSprite.yc-gridOffY) % gridHeight) > 0 ? (gridHeight-((firstSprite.yc-gridOffY) % gridHeight)) : gridHeight;
                        for ( auto spriteIndex : view.sprites.sel() )
                            edit->sprites[spriteIndex].yc += diff;
                    }
                    break;
                }
            }
            break;
        }
    }
    else // No-grid
    {
        switch ( currLayer )
        {
        case Layer::Units:
            for ( auto unitIndex : view.units.sel() )
            {
                switch ( direction )
                {
                    case Direction::Left:
                        edit->units[unitIndex].xc -= 1;
                        break;
                    case Direction::Up:
                        edit->units[unitIndex].yc -= 1;
                        break;
                    case Direction::Right:
                        edit->units[unitIndex].xc += 1;
                        break;
                    case Direction::Down:
                        edit->units[unitIndex].yc += 1;
                        break;
                }
            }
            break;
        case Layer::Sprites:
            for ( auto spriteIndex : view.sprites.sel() )
            {
                auto & sprite = Scenario::getSprite(spriteIndex);
                switch ( direction )
                {
                    case Direction::Left:
                        edit->sprites[spriteIndex].xc -= 1;
                        break;
                    case Direction::Up:
                        edit->sprites[spriteIndex].yc -= 1;
                        break;
                    case Direction::Right:
                        edit->sprites[spriteIndex].xc += 1;
                        break;
                    case Direction::Down:
                        edit->sprites[spriteIndex].yc += 1;
                        break;
                }
            }
            break;
        }
    }
    Redraw(false);
}

bool GuiMap::pastingToGrid()
{
    u16 gridWidth = 32, gridHeight = 32;
    if ( scGraphics.GetGridSize(0, gridWidth, gridHeight) && (gridWidth == 0 || gridHeight == 0) )
        return false;

    switch ( currLayer )
    {
        case Layer::Locations: return this->snapLocations;
        case Layer::Units: return this->snapUnits;
        case Layer::Sprites: return this->snapSprites;
        case Layer::CutCopyPaste: return this->snapCutCopyPasteSel;
    }
    return false;
}

void GuiMap::undo()
{
    Scenario::undoAction();
    checkSelChangeFlags();
    checkUnsavedChanges();
    chkd.mainPlot.leftBar.historyTree.RefreshActionHeaders();
    refreshScenario(false); // TODO: rely on change notifications instead of using hard refreshes
}

void GuiMap::redo()
{
    Scenario::redoAction();
    checkSelChangeFlags();
    checkUnsavedChanges();
    chkd.mainPlot.leftBar.historyTree.RefreshActionHeaders();
    refreshScenario(false); // TODO: rely on change notifications instead of using hard refreshes
}

void GuiMap::checkUnsavedChanges()
{
    auto cursorIndex = Tracked::getCursorIndex();
    if ( cursorIndex >= lastSaveActionCursor && cursorIndex < nonSelChangeCursor )
        notifyNoUnsavedChanges();
    else
        notifyUnsavedChanges();
}

float GuiMap::MiniMapScale(u16 xSize, u16 ySize)
{
    if ( xSize >= ySize && xSize > 0 )
    {
        if ( 128.0/xSize > 1 )
            return (float)(128/xSize);
        else
            return (float)(128.0/xSize);
    }
    else
    {
        if ( 128.0/ySize > 1 && ySize > 0 )
            return (float)(128/ySize);
        else
            return (float)(128.0/ySize);
    }
    return 1;
}

bool GuiMap::EnsureBitmapSize(u32 desiredWidth, u32 desiredHeight)
{
    if ( bitmapWidth == desiredWidth && bitmapHeight == desiredHeight )
        return true;
    else // ( bitmapWidth != desiredWidth || bitmapHeight != desiredHeight )
    {
        try
        {
            bitmapWidth = desiredWidth;
            bitmapHeight = desiredHeight;
            graphicBits.resize(bitmapWidth*bitmapHeight);
            return true;
        }
        catch ( ... ) {}
    }
    bitmapWidth = 0;
    bitmapHeight = 0;
    graphicBits.clear();
    return false;
}

void GuiMap::SnapSelEndDrag()
{
    auto snapToGrid = [&]() {
        u16 gridWidth = 32, gridHeight = 32;
        if ( scGraphics.GetGridSize(0, gridWidth, gridHeight) )
            selections.snapEndDrag(gridWidth, gridHeight);
    };
    switch ( currLayer )
    {
        case Layer::Units:
            if ( clipboard.hasUnits() || clipboard.hasQuickUnits() )
            {
                const auto & firstUnit = clipboard.getUnits().begin()->unit;
                if ( firstUnit.type < Sc::Unit::TotalTypes )
                {
                    const auto & dat = chkd.scData.units.getUnit(firstUnit.type);
                    if ( (dat.flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building && buildingsSnapToTile )
                    {
                        bool oddTileWidth = (dat.starEditPlacementBoxWidth/32)%2 > 0;
                        bool oddTileHeight = (dat.starEditPlacementBoxHeight/32)%2 > 0;
                        point & endDrag = selections.endDrag;
                        if ( (endDrag.x % 32) != (oddTileWidth ? 16 : 0) )
                            endDrag.x = (endDrag.x + (oddTileWidth ? 16 : 16))/32*32 + (oddTileWidth ? (((endDrag.x+16) % 32) < 16 ? -16 : 16) : 0);
                        if ( (endDrag.y % 32) != (oddTileHeight ? 16 : 0) )
                            endDrag.y = (endDrag.y + (oddTileHeight ? 16 : 16))/32*32 + (oddTileHeight ? (((endDrag.y+16) % 32) < 16 ? -16 : 16) : 0);
                    }
                    else if ( snapUnits )
                        snapToGrid();
                }
                else if ( snapUnits )
                    snapToGrid();
            }
            break;
        case Layer::Sprites:
            if ( snapSprites )
                snapToGrid();
            break;
    }
}

bool GuiMap::UpdateGlGraphics()
{
    auto currTickCount = GetTickCount64();
    auto ticks = currTickCount - prevTickCount;
    if ( ticks > 0 )
    {
        prevTickCount = currTickCount;
        return scrGraphics->updateGraphics(ticks);
    }
    return false;
}

void GuiMap::PaintMap(GuiMapPtr currMap, bool pasting)
{
    if ( CM.get() == this && this->skin != GuiMap::Skin::ClassicGDI )
    {
        RECT cliRect {};
        if ( getClientRect(cliRect) )
        {
            chkd.maps.setGlRenderTarget(this->openGlDc, *this);

            glClearColor(0.0f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // TODO: scGraphics is likely being deprecated, this section is a temporary copy-over
            scrGraphics->useGameTileNums = scGraphics.mtxmOverTile();
            scrGraphics->displayIsomNums = scGraphics.DisplayingIsomNums();
            scrGraphics->displayTileNums = scGraphics.DisplayingTileNums();
            scrGraphics->displayBuildability = scGraphics.DisplayingBuildability();
            scrGraphics->displayElevations = scGraphics.DisplayingElevations();
            scrGraphics->clipLocationNames = scGraphics.ClippingLocationNames();
            
            SnapSelEndDrag();
            scrGraphics->render();
            
            glFlush();
            SwapBuffers(openGlDc->getDcHandle());
            ValidateRect(getHandle(), &cliRect);
        }
        // TODO: temp copy of minimap render code
        if ( currMap == nullptr || currMap.get() == this ) // Only redraw minimap for active window
        {
            u16 scaledWidth = cliRect.right-cliRect.left,
                scaledHeight = cliRect.bottom-cliRect.top;

            if ( zoom < 1 || zoom > 1 )
            {
                scaledWidth = u16(((double)scaledWidth) / zoom),
                scaledHeight = u16(((double)scaledHeight) / zoom);
            }
            EnsureBitmapSize(scaledWidth, scaledHeight);
            RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        return;
    }

    WinLib::DeviceContext dc(WindowsItem::getHandle());
    if ( dc )
    {
        u16 scaledWidth = dc.getWidth(),
            scaledHeight = dc.getHeight();

        if ( zoom < 1 || zoom > 1 )
        {
            scaledWidth = u16(((double)dc.getWidth()) / zoom),
            scaledHeight = u16(((double)dc.getHeight()) / zoom);
        }
        u32 bitmapSize = (u32)scaledWidth*(u32)scaledHeight * 4;

        if ( RedrawMap == true && EnsureBitmapSize(scaledWidth, scaledHeight) )
        {
            RedrawMap = false;

            mapBuffer.setBuffer(scaledWidth, scaledHeight, dc.getDcHandle());
            if ( currMap == nullptr || currMap.get() == this ) // Only redraw minimap for active window
                RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

            // Terrain, Grid, Units, Sprites, Debug
            scGraphics.DrawMap(mapBuffer, bitmapWidth, bitmapHeight, screenLeft, screenTop, graphicBits, !lockAnywhere);
        }

        if ( mapBuffer )
        {
            toolsBuffer.setBuffer(scaledWidth, scaledHeight, dc.getDcHandle());
            mapBuffer.flushBuffer(toolsBuffer);
            if ( currMap == nullptr || currMap.get() == this )
            { // Drag, paste & hover graphics
                SnapSelEndDrag();
                scGraphics.DrawTools(toolsBuffer, scaledWidth, scaledHeight,
                    screenLeft, screenTop, selections, pasting, clipboard, *this);

                if ( currLayer != Layer::Locations && dragging && !clipboard.isPasting() )
                    DrawSelectingFrame(toolsBuffer, selections, screenLeft, screenTop, bitmapWidth, bitmapHeight, zoom);
            }
            SetStretchBltMode(dc.getDcHandle(), HALFTONE);
            if ( zoom == 1 )
                toolsBuffer.bitBlt(dc.getDcHandle(), 0, 0, dc.getWidth(), dc.getHeight(), 0, 0, SRCCOPY);
            else
                toolsBuffer.stretchBlt(dc.getDcHandle(), 0, 0, dc.getWidth(), dc.getHeight(), 0, 0, scaledWidth, scaledHeight, SRCCOPY);
        }
        dc.validateRect();
    }
}

void GuiMap::PaintMiniMap(const WinLib::DeviceContext & dc)
{
    if ( dc )
    {
        if ( RedrawMiniMap && miniMapBuffer.setBuffer(dc.getWidth(), dc.getHeight(), dc.getDcHandle()) )
        {
            RedrawMiniMap = false;
            DrawMiniMap(miniMapBuffer, getStaticPalette(), (u16)Scenario::getTileWidth(), (u16)Scenario::getTileHeight(),
                MiniMapScale((u16)Scenario::getTileWidth(), (u16)Scenario::getTileHeight()), *this);
        }

        if ( miniMapBuffer )
        {
            miniMapBuffer.flushBuffer(dc);
            DrawMiniMapBox(dc, screenLeft, screenTop, bitmapWidth, bitmapHeight,
                (u16)Scenario::getTileWidth(), (u16)Scenario::getTileHeight(),
                MiniMapScale((u16)Scenario::getTileWidth(), (u16)Scenario::getTileHeight()));
            dc.flushBuffer();
        }
    }
}

void GuiMap::Redraw(bool includeMiniMap)
{
    RedrawMap = true;
    if ( includeMiniMap )
        RedrawMiniMap = true;

    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void GuiMap::ValidateBorder(s32 screenWidth, s32 screenHeight, s32 newLeft, s32 newTop)
{
    if ( newLeft == -1 )
        newLeft = screenLeft;
    if ( newTop == -1 )
        newTop = screenTop;
    
    if ( newLeft < 0 )
        screenLeft = 0;
    else if ( newLeft > ((s32)Scenario::getTileWidth())*32-screenWidth )
    {
        if ( screenWidth > s32(Scenario::getTileWidth())*32 )
            screenLeft = 0;
        else
            screenLeft = s32(Scenario::getTileWidth())*32-screenWidth;
    }
    else
        screenLeft = newLeft;

    if ( newTop < 0 )
        screenTop = 0;
    else if ( newTop > s32(Scenario::getTileHeight())*32-screenHeight )
    {
        if ( screenHeight > s32(Scenario::getTileHeight())*32 )
            screenTop = 0;
        else
            screenTop = s32(Scenario::getTileHeight())*32-screenHeight;
    }
    else
        screenTop = newTop;
}

bool GuiMap::SetGridSize(s16 xSize, s16 ySize)
{

    bool success = false;
    u16 oldXSize = 0, oldYSize = 0;
    if ( scGraphics.GetGridSize(0, oldXSize, oldYSize) )
    {
        if ( oldXSize == xSize && oldYSize == ySize )
        {
            success = scGraphics.SetGridSize(0, 0, 0);
            scrGraphics->setGridSize(0); // TODO: interface shouldn't have varying sizes if it's not used
        }
        else
        {
            success = scGraphics.SetGridSize(0, xSize, ySize);
            scrGraphics->setGridSize(xSize); // TODO: interface shouldn't have varying sizes if it's not used
        }

        if ( success )
        {
            UpdateGridSizesMenu();
            Redraw(false);
        }
    }
    return success;
}

bool GuiMap::SetGridColor(u8 red, u8 green, u8 blue)
{
    scrGraphics->setGridColor(u32(red) | (u32(green) << 8) | (u32(blue) << 16));

    bool success = false;
    if ( scGraphics.SetGridColor(0, red, green, blue) )
    {
        u16 xSize = 0, ySize = 0;
        if ( scGraphics.GetGridSize(0, xSize, ySize) && (xSize == 0 || ySize == 0) )
            success = SetGridSize(32, 32);

        UpdateGridColorMenu();
        Redraw(false);
    }
    return success;
}

void GuiMap::ToggleDisplayBuildability()
{
    scGraphics.ToggleDisplayBuildability();
    UpdateTerrainViewMenuItems();
    Redraw(false);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingBuildability()
{
    return scGraphics.DisplayingBuildability();
}

void GuiMap::ToggleDisplayElevations()
{
    scGraphics.ToggleDisplayElevations();
    UpdateTerrainViewMenuItems();
    Redraw(false);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingElevations()
{
    return scGraphics.DisplayingElevations();
}

void GuiMap::ToggleDisplayIsomValues()
{
    scGraphics.ToggleDisplayIsomValues();
    UpdateTerrainViewMenuItems();
    Redraw(false);
}

void GuiMap::ToggleTileNumSource(bool MTXMoverTILE)
{
    scGraphics.ToggleTileNumSource(MTXMoverTILE);
    UpdateTerrainViewMenuItems();
    Redraw(false);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingTileNums()
{
    return scGraphics.DisplayingTileNums();
}

void GuiMap::ToggleDisplayFps()
{
    scrGraphics->fpsEnabled = !scrGraphics->fpsEnabled;
    UpdateBaseViewMenuItems();
}

u32 GuiMap::getNextClassId()
{
    u32 maxClassId = 0;
    for ( const auto & unit : read.units ) {
        if ( unit.classId > maxClassId )
            maxClassId = unit.classId;
    }
    return maxClassId+1;
}

bool GuiMap::isValidUnitPlacement(Sc::Unit::Type unitType, s32 x, s32 y)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        return true; // Extended units placement isn't checked for validity

    bool placementIsBuilding = (chkd.scData.units.getUnit(unitType).flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building;
    bool placementIsFlyer = (chkd.scData.units.getUnit(unitType).flags & Sc::Unit::Flags::Flyer) == Sc::Unit::Flags::Flyer;
    bool placementIsResourceDepot = (chkd.scData.units.getUnit(unitType).flags & Sc::Unit::Flags::ResourceDepot) == Sc::Unit::Flags::ResourceDepot;

    bool validateOnWalkableTerrain = !placementIsFlyer && !placementIsBuilding && !placeUnitsAnywhere;
    bool validateOnBuildableTerrain = placementIsBuilding && !placeBuildingsAnywhere;
    bool validatePlacableOnTerrain = validateOnWalkableTerrain || validateOnBuildableTerrain;
    bool validateNotGroundStacked = !placementIsFlyer && !stackUnits;
    bool validateNotAirStacked = placementIsFlyer && !stackAirUnits;
    bool validateNotStacked = validateNotGroundStacked || validateNotAirStacked;
    bool validateMineralDistance = requireMineralDistance && placementIsResourceDepot;
    bool performValidation = validatePlacableOnTerrain || validateNotStacked || validateMineralDistance;

    if ( performValidation )
    {
        const auto & placementDat = chkd.scData.units.getUnit(unitType);
        s32 placementLeft   = x - s32(placementDat.unitSizeLeft),
            placementRight  = x + s32(placementDat.unitSizeRight),
            placementTop    = y - s32(placementDat.unitSizeUp),
            placementBottom = y + s32(placementDat.unitSizeDown);

        if ( validatePlacableOnTerrain )
        {
            if ( placementLeft < 0 ||
                 placementTop < 0 ||
                 placementRight > s32(Scenario::getPixelWidth()) ||
                 placementBottom > s32(Scenario::getPixelHeight()) ||
                 placementLeft >= 32*s32(read.dimensions.tileWidth) ||
                 placementTop >= 32*s32(read.dimensions.tileHeight) )
            {
                return false;
            }

            const auto & tileset = chkd.scData.terrain.get(this->getTileset());
            s32 xTileMin = std::max(0, placementLeft/32);
            s32 xTileMax = std::min(s32(read.dimensions.tileWidth-1), placementRight/32);
            s32 yTileMin = std::max(0, placementTop/32);
            s32 yTileMax = std::min(s32(read.dimensions.tileHeight-1), placementBottom/32);
            for ( s32 yTile = yTileMin; yTile <= yTileMax; ++yTile )
            {
                for ( s32 xTile = xTileMin; xTile <= xTileMax; ++xTile )
                {
                    auto tileValue = read.tiles[size_t(yTile)*size_t(read.dimensions.tileWidth)+xTile];
                    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileValue);
                    if ( groupIndex < tileset.tileGroups.size() )
                    {
                        const Sc::Terrain::TileGroup & tileGroup = tileset.tileGroups[groupIndex];
                        if ( validateOnBuildableTerrain && !tileGroup.isBuildable() )
                            return false; // Overlaps unbuildable tile

                        if ( validatePlacableOnTerrain )
                        {
                            u16 megaTileIndex = tileGroup.megaTileIndex[tileset.getGroupMemberIndex(tileValue)];
                            s32 xTileStart = 32*s32(xTile);
                            s32 yTileStart = 32*s32(yTile);
                            s32 xMiniTileMin = placementLeft < xTileStart ? 0 : (placementLeft-xTileStart)/8;
                            s32 xMiniTileMax = placementRight >= xTileStart+24 ? 4 : (placementRight-xTileStart)/8+1;
                            s32 yMiniTileMin = placementTop < yTileStart ? 0 : (placementTop-yTileStart)/8;
                            s32 yMiniTileMax = placementBottom >= yTileStart+24 ? 4 : (placementBottom-yTileStart)/8+1;
                            for ( s32 yMiniTile = yMiniTileMin; yMiniTile < yMiniTileMax; ++yMiniTile )
                            {
                                for ( s32 xMiniTile = xMiniTileMin; xMiniTile < xMiniTileMax; ++xMiniTile )
                                {
                                    if ( !tileset.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile].isWalkable() )
                                        return false; // Overlaps unwalkable mini-tile
                                }
                            }
                        }
                    }
                }
            }
        }
        if ( validateNotStacked )
        {
            for ( const auto & unit : read.units )
            {
                if ( unit.type < Sc::Unit::TotalTypes )
                {
                    const auto & unitDat = chkd.scData.units.getUnit(unit.type);
                    auto datFlags = unitDat.flags;
                    bool isBuilding = (datFlags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building;
                    bool isFlyer = (datFlags & Sc::Unit::Flags::Flyer) == Sc::Unit::Flags::Flyer;
                    bool isFlyingBuilding = isBuilding &&
                        (datFlags & Sc::Unit::Flags::FlyingBuilding) == Sc::Unit::Flags::FlyingBuilding &&
                        (unit.stateFlags & Chk::Unit::State::InTransit) == Chk::Unit::State::InTransit;
                    bool isFlying = isFlyer || isFlyingBuilding;

                    if ( (isFlying && validateNotAirStacked) || (!isFlying && validateNotGroundStacked) )
                    {
                        s32 left   = unit.xc - unitDat.unitSizeLeft,
                            right  = unit.xc + unitDat.unitSizeRight,
                            top    = unit.yc - unitDat.unitSizeUp,
                            bottom = unit.yc + unitDat.unitSizeDown;

                        if ( placementRight >= left && placementLeft <= right && placementBottom >= top && placementTop <= bottom )
                            return false; // placement would be stacked on this unit
                    }
                }
            }
        }
        if ( validateMineralDistance )
        {
            const auto & placementDat = chkd.scData.units.getUnit(unitType);
            s32 xPlacementTileMin = std::max(0, (x - s32(placementDat.unitSizeLeft))/32),
                xPlacementTileMax = std::min(s32(read.dimensions.tileWidth-1), (x + s32(placementDat.unitSizeRight))/32),
                yPlacementTileMin = std::max(0, (y - s32(placementDat.unitSizeUp))/32),
                yPlacementTileMax = std::min(s32(read.dimensions.tileHeight-1), (y + s32(placementDat.unitSizeDown))/32);

            for ( const auto & unit : read.units )
            {
                if ( unit.type < Sc::Unit::TotalTypes )
                {
                    const auto & unitDat = chkd.scData.units.getUnit(unit.type);
                    bool isResource = (unitDat.flags & Sc::Unit::Flags::ResourceContainer) == Sc::Unit::Flags::ResourceContainer;

                    if ( isResource )
                    {
                        s32 xTileMin = std::max(0, (s32(unit.xc) - s32(unitDat.unitSizeLeft))/32-3);
                        s32 xTileMax = std::min(s32(read.dimensions.tileWidth-1), (s32(unit.xc) + s32(unitDat.unitSizeRight))/32+3);
                        s32 yTileMin = std::max(0, (s32(unit.yc) - s32(unitDat.unitSizeUp))/32-3);
                        s32 yTileMax = std::min(s32(read.dimensions.tileHeight-1), (s32(unit.yc) + s32(unitDat.unitSizeDown))/32+3);

                        if ( xPlacementTileMax >= xTileMin && xPlacementTileMin <= xTileMax && yPlacementTileMax >= yTileMin && yPlacementTileMin <= yTileMax )
                            return false; // Placement violates mineral distance
                    }
                }
            }
        }
    }
    return true;
}

bool GuiMap::isLinkable(const Chk::Unit & first, const Chk::Unit & second)
{
    switch ( first.type )
    {
        case Sc::Unit::Type::TerranControlTower: return second.type == Sc::Unit::Type::TerranStarport;
        case Sc::Unit::Type::TerranMachineShop: return second.type == Sc::Unit::Type::TerranFactory;
        case Sc::Unit::Type::TerranComsatStation: return second.type == Sc::Unit::Type::TerranCommandCenter;
        case Sc::Unit::Type::TerranNuclearSilo: return second.type == Sc::Unit::Type::TerranCommandCenter;
        case Sc::Unit::Type::TerranCovertOps: return second.type == Sc::Unit::Type::TerranScienceFacility;
        case Sc::Unit::Type::TerranPhysicsLab: return second.type == Sc::Unit::Type::TerranScienceFacility;
        case Sc::Unit::Type::TerranStarport: return second.type == Sc::Unit::Type::TerranControlTower;
        case Sc::Unit::Type::TerranFactory: return second.type == Sc::Unit::Type::TerranMachineShop;
        case Sc::Unit::Type::TerranCommandCenter:
            return second.type == Sc::Unit::Type::TerranComsatStation || second.type == Sc::Unit::Type::TerranNuclearSilo;
        case Sc::Unit::Type::TerranScienceFacility:
            return second.type == Sc::Unit::Type::TerranCovertOps || second.type == Sc::Unit::Type::TerranPhysicsLab;
        case Sc::Unit::Type::ZergNydusCanal: return second.type == Sc::Unit::Type::ZergNydusCanal;
        default: return false;
    }
}

std::optional<u16> GuiMap::getLinkableUnitIndex(Sc::Unit::Type unitType, s32 x, s32 y)
{
    bool isAddon = false;
    Sc::Unit::Type unitTypeToLocate = Sc::Unit::Type::AnyUnit;
    Sc::Unit::Type otherUnitTypeToLocate = Sc::Unit::Type::AnyUnit;
    switch ( unitType ) {
        case Sc::Unit::Type::TerranControlTower:
            unitTypeToLocate = Sc::Unit::Type::TerranStarport;
            isAddon = true;
            break;
        case Sc::Unit::Type::TerranMachineShop:
            unitTypeToLocate = Sc::Unit::Type::TerranFactory;
            isAddon = true;
            break;
        case Sc::Unit::Type::TerranComsatStation:
        case Sc::Unit::Type::TerranNuclearSilo:
            unitTypeToLocate = Sc::Unit::Type::TerranCommandCenter;
            isAddon = true;
            break;
        case Sc::Unit::Type::TerranCovertOps:
        case Sc::Unit::Type::TerranPhysicsLab:
            unitTypeToLocate = Sc::Unit::Type::TerranScienceFacility;
            isAddon = true;
            break;
        case Sc::Unit::Type::TerranStarport:
            unitTypeToLocate = Sc::Unit::Type::TerranControlTower;
            break;
        case Sc::Unit::Type::TerranFactory:
            unitTypeToLocate = Sc::Unit::Type::TerranMachineShop;
            break;
        case Sc::Unit::Type::TerranCommandCenter:
            unitTypeToLocate = Sc::Unit::Type::TerranComsatStation;
            otherUnitTypeToLocate = Sc::Unit::Type::TerranNuclearSilo;
            break;
        case Sc::Unit::Type::TerranScienceFacility:
            unitTypeToLocate = Sc::Unit::Type::TerranCovertOps;
            otherUnitTypeToLocate = Sc::Unit::Type::TerranPhysicsLab;
            break;
        default:
            return std::nullopt;
    }

    const auto & checkDat = chkd.scData.units.getUnit(unitType);
    for ( size_t unitIndex = 0; unitIndex < numUnits(); ++unitIndex )
    {
        const auto & unit = read.units[unitIndex];
        if ( unit.relationClassId == 0 && (unit.type == unitTypeToLocate || unit.type == otherUnitTypeToLocate ) )
        { // Correct unit type which doesn't already have a relation
            const auto & dat = chkd.scData.units.getUnit(unit.type);
            auto unitOriginX = unit.xc - dat.starEditPlacementBoxWidth/2;
            auto unitOriginY = unit.yc - dat.starEditPlacementBoxHeight/2;
            auto checkOriginX = x - checkDat.starEditPlacementBoxWidth/2;
            auto checkOriginY = y - checkDat.starEditPlacementBoxHeight/2;
            if ( isAddon && (unitOriginX + checkDat.addonHorizontal == checkOriginX && unitOriginY + checkDat.addonVertical == checkOriginY) )
                return u16(unitIndex);
            else if ( !isAddon && (checkOriginX + dat.addonHorizontal == unitOriginX && checkOriginY + dat.addonVertical == unitOriginY) )
                return u16(unitIndex);
        }
    }
    return std::nullopt;
}

void GuiMap::ToggleBuildingsSnapToTile()
{
    buildingsSnapToTile = !buildingsSnapToTile;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleUnitSnap()
{
    snapUnits = !snapUnits;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleUnitStack()
{
    stackUnits = !stackUnits;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleEnableAirStack()
{
    stackAirUnits = !stackAirUnits;
    UpdateUnitMenuItems();
}

void GuiMap::TogglePlaceUnitsAnywhere()
{
    placeUnitsAnywhere = !placeUnitsAnywhere;
    UpdateUnitMenuItems();
}

void GuiMap::TogglePlaceBuildingsAnywhere()
{
    placeBuildingsAnywhere = !placeBuildingsAnywhere;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleAddonAutoPlayerSwap()
{
    addonAutoPlayerSwap = !addonAutoPlayerSwap;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleRequireMineralDistance()
{
    requireMineralDistance = !requireMineralDistance;
    UpdateUnitMenuItems();
}

void GuiMap::ToggleSpriteSnap()
{
    snapSprites = !snapSprites;
    UpdateSpriteMenuItems();
}

void GuiMap::ToggleLocationNameClip()
{
    scGraphics.ToggleLocationNameClip();
    chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, scGraphics.ClippingLocationNames());
    Redraw(false);
}

void GuiMap::SetLocationSnap(Snap locationSnap)
{
    bool prevSnapLocations = snapLocations;
    if ( locationSnap == Snap::NoSnap )
        snapLocations = false;
    else
        snapLocations = true;

    if ( locationSnap == Snap::SnapToTile )
    {
        if ( prevSnapLocations && locSnapTileOverGrid )
            snapLocations = false;
        else
            locSnapTileOverGrid = true;
    }
    else if ( locationSnap == Snap::SnapToGrid )
    {
        if ( prevSnapLocations && !locSnapTileOverGrid )
            snapLocations = false;
        else
            locSnapTileOverGrid = false;
    }

    UpdateLocationMenuItems();
}

void GuiMap::ToggleLockAnywhere()
{
    lockAnywhere = !lockAnywhere;
    UpdateLocationMenuItems();
    if ( selections.getSelectedLocation() == Chk::LocationId::Anywhere )
        selections.selectLocation(NO_LOCATION);
    chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
    Redraw(false);
}

bool GuiMap::LockAnywhere()
{
    return lockAnywhere;
}

void GuiMap::ToggleAllowIllegalDoodadPlacement()
{
    allowIllegalDoodads = !allowIllegalDoodads;
    UpdateDoodadMenuItems();
}

bool GuiMap::AllowIllegalDoodadPlacement()
{
    return allowIllegalDoodads;
}

bool GuiMap::GetSnapIntervals(u32 & x, u32 & y, u32 & xOffset, u32 & yOffset)
{
    if ( snapLocations )
    {
        if ( locSnapTileOverGrid )
        {
            x = 32;
            y = 32;
            xOffset = 0;
            yOffset = 0;
            return true;
        }
        else
        {
            u16 gridWidth = 0, gridHeight = 0, gridXOffset = 0, gridYOffset = 0;
            if ( scGraphics.GetGridSize(0, gridWidth, gridHeight) &&
                 scGraphics.GetGridOffset(0, gridXOffset, gridYOffset) &&
                 (gridWidth > 0 || gridHeight > 0) )
            {
                x = gridWidth;
                y = gridHeight;
                xOffset = gridXOffset;
                yOffset = gridYOffset;
                return true;
            }
        }
    }
    return false;
}

bool GuiMap::SnapLocationDimensions(u32 & x1, u32 & y1, u32 & x2, u32 & y2, LocSnapFlags locSnapFlags)
{
    u32 lengthX, lengthY, startSnapX, startSnapY;
    if ( GetSnapIntervals(lengthX, lengthY, startSnapX, startSnapY) )
    {
        double intervalNum = (double(x1-startSnapX))/lengthX;
        u32 xStartIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(y1-startSnapY))/lengthY;
        u32 yStartIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(x2-startSnapX))/lengthX;
        u32 xEndIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(y2-startSnapY))/lengthY;
        u32 yEndIntervalNum = (u32)round(intervalNum);

        if ( (locSnapFlags & LocSnapFlags::SnapX1) == LocSnapFlags::SnapX1 )
            x1 = xStartIntervalNum*lengthX + startSnapX;
        if ( (locSnapFlags & LocSnapFlags::SnapY1) == LocSnapFlags::SnapY1 )
            y1 = yStartIntervalNum*lengthY + startSnapY;
        if ( (locSnapFlags & LocSnapFlags::SnapX2) == LocSnapFlags::SnapX2 )
            x2 = xEndIntervalNum*lengthX + startSnapX;
        if ( (locSnapFlags & LocSnapFlags::SnapY2) == LocSnapFlags::SnapY2 )
            y2 = yEndIntervalNum*lengthY + startSnapY;

        return locSnapFlags != LocSnapFlags::None;
    }
    else
        return false;
}

bool GuiMap::SnapLocationDimensions(std::size_t locationIndex, LocSnapFlags locSnapFlags)
{
    auto edit = createAction();
    u32 lengthX, lengthY, startSnapX, startSnapY;
    if ( GetSnapIntervals(lengthX, lengthY, startSnapX, startSnapY) )
    {
        const auto & loc = read.locations[locationIndex];
        double intervalNum = (double(loc.left-startSnapX))/lengthX;
        u32 xStartIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(loc.top-startSnapY))/lengthY;
        u32 yStartIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(loc.right-startSnapX))/lengthX;
        u32 xEndIntervalNum = (u32)round(intervalNum);
        intervalNum = (double(loc.bottom-startSnapY))/lengthY;
        u32 yEndIntervalNum = (u32)round(intervalNum);

        if ( (locSnapFlags & LocSnapFlags::SnapX1) == LocSnapFlags::SnapX1 )
            edit->locations[locationIndex].left = xStartIntervalNum*lengthX + startSnapX;
        if ( (locSnapFlags & LocSnapFlags::SnapY1) == LocSnapFlags::SnapY1 )
            edit->locations[locationIndex].top = yStartIntervalNum*lengthY + startSnapY;
        if ( (locSnapFlags & LocSnapFlags::SnapX2) == LocSnapFlags::SnapX2 )
            edit->locations[locationIndex].right = xEndIntervalNum*lengthX + startSnapX;
        if ( (locSnapFlags & LocSnapFlags::SnapY2) == LocSnapFlags::SnapY2 )
            edit->locations[locationIndex].bottom = yEndIntervalNum*lengthY + startSnapY;

        return locSnapFlags != LocSnapFlags::None;
    }
    else
        return false;
}

void GuiMap::SetCutCopyPasteSnap(Snap cutCopyPasteSnap)
{
    bool prevSnapCutCopyPaste = snapCutCopyPasteSel;
    if ( cutCopyPasteSnap == Snap::NoSnap )
        snapCutCopyPasteSel = false;
    else
        snapCutCopyPasteSel = true;

    if ( cutCopyPasteSnap == Snap::SnapToTile )
    {
        if ( prevSnapCutCopyPaste && cutCopyPasteSnapTileOverGrid )
            snapCutCopyPasteSel = false;
        else
            cutCopyPasteSnapTileOverGrid = true;
    }
    else if ( cutCopyPasteSnap == Snap::SnapToGrid )
    {
        if ( prevSnapCutCopyPaste && !cutCopyPasteSnapTileOverGrid )
            snapCutCopyPasteSel = false;
        else
            cutCopyPasteSnapTileOverGrid = false;
    }

    UpdateCutCopyPasteMenuItems();
}

void GuiMap::ToggleIncludeDoodadTiles()
{
    cutCopyPasteIncludeDoodadTiles = !cutCopyPasteIncludeDoodadTiles;
    UpdateCutCopyPasteMenuItems();
}

bool GuiMap::GetIncludeDoodadTiles()
{
    return cutCopyPasteIncludeDoodadTiles;
}

void GuiMap::UpdateLocationMenuItems()
{
    chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, scGraphics.ClippingLocationNames());
    chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOTILE, snapLocations && locSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOACTIVEGRID, snapLocations && !locSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_NOSNAP, !snapLocations);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_LOCKANYWHERE, lockAnywhere);
}

void GuiMap::UpdateDoodadMenuItems()
{
    chkd.mainMenu.SetCheck(ID_DOODADS_ALLOWILLEGALPLACEMENT, allowIllegalDoodads);
}

void GuiMap::UpdateZoomMenuItems()
{
    u32 zoomNum;
    for ( zoomNum=0; zoomNum<defaultZooms.size(); zoomNum++ )
    {
        if ( zoom == defaultZooms[zoomNum] )
            break;
    }

    chkd.mainToolbar.zoomBox.SetSel(zoomNum);

    chkd.mainMenu.SetCheck(ID_ZOOM_10, zoom == .10);
    chkd.mainMenu.SetCheck(ID_ZOOM_25, zoom == .25);
    chkd.mainMenu.SetCheck(ID_ZOOM_33, zoom == .33);
    chkd.mainMenu.SetCheck(ID_ZOOM_50, zoom == .50);
    chkd.mainMenu.SetCheck(ID_ZOOM_66, zoom == .66);
    chkd.mainMenu.SetCheck(ID_ZOOM_100, zoom == 1.00);
    chkd.mainMenu.SetCheck(ID_ZOOM_150, zoom == 1.50);
    chkd.mainMenu.SetCheck(ID_ZOOM_200, zoom == 2.00);
    chkd.mainMenu.SetCheck(ID_ZOOM_300, zoom == 3.00);
    chkd.mainMenu.SetCheck(ID_ZOOM_400, zoom == 4.00);
}

void GuiMap::UpdateGridSizesMenu()
{
    u16 gridWidth = 0, gridHeight = 0;
    scGraphics.GetGridSize(0, gridWidth, gridHeight);

    chkd.mainMenu.SetCheck(ID_GRID_ULTRAFINE, false);
    chkd.mainMenu.SetCheck(ID_GRID_FINE, false);
    chkd.mainMenu.SetCheck(ID_GRID_NORMAL, false);
    chkd.mainMenu.SetCheck(ID_GRID_LARGE, false);
    chkd.mainMenu.SetCheck(ID_GRID_EXTRALARGE, false);
    chkd.mainMenu.SetCheck(ID_GRID_CUSTOM, false);
    chkd.mainMenu.SetCheck(ID_GRID_DISABLED, false);
    
    if ( gridWidth == 0 && gridHeight == 0 )
        chkd.mainMenu.SetCheck(ID_GRID_DISABLED, true);
    else if ( gridWidth == 8 && gridHeight == 8 )
        chkd.mainMenu.SetCheck(ID_GRID_ULTRAFINE, true);
    else if ( gridWidth == 16 && gridHeight == 16 )
        chkd.mainMenu.SetCheck(ID_GRID_FINE, true);
    else if ( gridWidth == 32 && gridHeight == 32 )
        chkd.mainMenu.SetCheck(ID_GRID_NORMAL, true);
    else if ( gridWidth == 64 && gridHeight == 64 )
        chkd.mainMenu.SetCheck(ID_GRID_LARGE, true);
    else if ( gridWidth == 128 && gridHeight == 128 )
        chkd.mainMenu.SetCheck(ID_GRID_EXTRALARGE, true);
    else
        chkd.mainMenu.SetCheck(ID_GRID_CUSTOM, true);
}

void GuiMap::UpdateGridColorMenu()
{
    u8 red = 0, green = 0, blue = 0;
    scGraphics.GetGridColor(0, red, green, blue);

    chkd.mainMenu.SetCheck(ID_COLOR_BLACK, false);
    chkd.mainMenu.SetCheck(ID_COLOR_GREY, false);
    chkd.mainMenu.SetCheck(ID_COLOR_WHITE, false);
    chkd.mainMenu.SetCheck(ID_COLOR_GREEN, false);
    chkd.mainMenu.SetCheck(ID_COLOR_RED, false);
    chkd.mainMenu.SetCheck(ID_COLOR_BLUE, false);
    chkd.mainMenu.SetCheck(ID_COLOR_OTHER, false);
    
    if ( red == 0 && green == 0 && blue == 0 )
        chkd.mainMenu.SetCheck(ID_COLOR_BLACK, true);
    else if ( red == 72 && green == 72 && blue == 88 )
        chkd.mainMenu.SetCheck(ID_COLOR_GREY, true);
    else if ( red == 255 && green == 255 && blue == 255 )
        chkd.mainMenu.SetCheck(ID_COLOR_WHITE, true);
    else if ( red == 16 && green == 252 && blue == 24 )
        chkd.mainMenu.SetCheck(ID_COLOR_GREEN, true);
    else if ( red == 244 && green == 4 && blue == 4 )
        chkd.mainMenu.SetCheck(ID_COLOR_RED, true);
    else if ( red == 36 && green == 36 && blue == 252 )
        chkd.mainMenu.SetCheck(ID_COLOR_BLUE, true);
    else
        chkd.mainMenu.SetCheck(ID_COLOR_OTHER, true);
}

void GuiMap::UpdateSkinMenuItems()
{
    chkd.mainMenu.SetCheck(ID_SKIN_CLASSICGDI, skin == GuiMap::Skin::ClassicGDI);
    chkd.mainMenu.SetCheck(ID_SKIN_CLASSICOPENGL, skin == GuiMap::Skin::ClassicGL);
    
    chkd.mainMenu.SetCheck(ID_SKIN_REMASTEREDSD, skin == GuiMap::Skin::ScrSD);
    chkd.mainMenu.SetCheck(ID_SKIN_REMASTEREDHD2, skin == GuiMap::Skin::ScrHD2);
    chkd.mainMenu.SetCheck(ID_SKIN_REMASTEREDHD, skin == GuiMap::Skin::ScrHD);
    
    chkd.mainMenu.SetCheck(ID_SKIN_CARBOTHD2, skin == GuiMap::Skin::CarbotHD2);
    chkd.mainMenu.SetCheck(ID_SKIN_CARBOTHD, skin == GuiMap::Skin::CarbotHD);
}

void GuiMap::UpdateTerrainViewMenuItems()
{
    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEBUILDABILITY, scGraphics.DisplayingBuildability());
    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEELEVATIONS, scGraphics.DisplayingElevations());
    
    if ( scGraphics.DisplayingIsomNums() )
        chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYISOMVALUES, true);
    else
        chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYISOMVALUES, false);

    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, false);
    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, false);
    if ( scGraphics.DisplayingTileNums() )
    {
        if ( scGraphics.mtxmOverTile() )
            chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, true);
        else
            chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, true);
    }
}

void GuiMap::UpdateBaseViewMenuItems()
{
    chkd.mainMenu.SetCheck(ID_VIEW_DISPLAYFPS, scrGraphics->fpsEnabled);
}

void GuiMap::UpdateUnitMenuItems()
{
    chkd.mainMenu.SetCheck(ID_UNITS_BUILDINGSSNAPTOTILE, buildingsSnapToTile);
    chkd.mainMenu.SetCheck(ID_UNITS_UNITSSNAPTOGRID, snapUnits);
    chkd.mainMenu.SetCheck(ID_UNITS_ALLOWSTACK, stackUnits);
    chkd.mainMenu.SetCheck(ID_UNITS_ENABLEAIRSTACK, stackAirUnits);
    chkd.mainMenu.SetCheck(ID_UNITS_PLACEUNITSANYWHERE, placeUnitsAnywhere);
    chkd.mainMenu.SetCheck(ID_UNITS_PLACEBUILDINGSANYWHERE, placeBuildingsAnywhere);
    chkd.mainMenu.SetCheck(ID_UNITS_ADDONAUTOPLAYERSWAP, addonAutoPlayerSwap);
    chkd.mainMenu.SetCheck(ID_UNITS_REQUIREMINERALDISTANCE, requireMineralDistance);
}

void GuiMap::UpdateSpriteMenuItems()
{
    chkd.mainMenu.SetCheck(ID_SPRITES_SNAPTOGRID, snapSprites);
}

void GuiMap::UpdateCutCopyPasteMenuItems()
{
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_SNAPSELECTIONTOTILES, snapCutCopyPasteSel && cutCopyPasteSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_SNAPSELECTIONTOGRID, snapCutCopyPasteSel && !cutCopyPasteSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_NOSNAP, !snapCutCopyPasteSel);
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_INCLUDEDOODADTILES, cutCopyPasteIncludeDoodadTiles);
    chkd.mainMenu.SetCheck(ID_CUTCOPYPASTE_FILLSIMILARTILES, clipboard.getFillSimilarTiles());
}

void GuiMap::Scroll(bool scrollX, bool scrollY, bool validateBorder, s32 newLeft, s32 newTop)
{
    SCROLLINFO scrollbars = { };
    scrollbars.cbSize = sizeof(SCROLLINFO);
    scrollbars.fMask = SIF_ALL;
    scrollbars.nMin = 0;

    RECT rcMap;
    GetClientRect(getHandle(), &rcMap);
    s32 screenWidth  = s32((double(rcMap.right-rcMap.left))/zoom),
        screenHeight = s32((double(rcMap.bottom-rcMap.top))/zoom);

    if ( validateBorder )
        ValidateBorder(screenWidth, screenHeight, newLeft, newTop);

    if ( scrollX )
    {
        scrollbars.nPos = screenLeft; 
        scrollbars.nMax = (int)Scenario::getPixelWidth();
        scrollbars.nPage = screenWidth;
        SetScrollInfo(getHandle(), SB_HORZ, &scrollbars, true);
    }
    if ( scrollY )
    {
        scrollbars.nPos = screenTop;
        scrollbars.nMax = (int)Scenario::getPixelHeight();
        scrollbars.nPage = screenHeight;
        SetScrollInfo(getHandle(), SB_VERT, &scrollbars, true);
    }
    
    scrGraphics->windowBoundsChanged({
        .left = screenLeft,
        .top = screenTop,
        .right = screenLeft + rcMap.right-rcMap.left,
        .bottom = screenTop + rcMap.bottom-rcMap.top
    });
    Redraw(true);
}

void GuiMap::setMapId(u16 mapId)
{
    this->mapId = mapId;
}

u16 GuiMap::getMapId()
{
    return this->mapId;
}

void GuiMap::notifyUnsavedChanges()
{
    if ( !unsavedChanges )
    {
        unsavedChanges = true;
        if ( auto windowTitle = WindowsItem::GetWinText() )
            WindowsItem::SetWinText(*windowTitle + "*");
    }
}

void GuiMap::notifyNoUnsavedChanges()
{
    if ( unsavedChanges )
    {
        unsavedChanges = false;
        if ( auto windowTitle = WindowsItem::GetWinText() )
        {
            if ( windowTitle->back() == '*' )
            {
                windowTitle->pop_back();
                WindowsItem::SetWinText(*windowTitle);
            }
        }
    }
}

void GuiMap::updateMenu()
{
    UpdateLocationMenuItems();
    UpdateDoodadMenuItems();
    chkd.mainToolbar.layerBox.SetSel((int)currLayer);
    UpdateZoomMenuItems();
    chkd.mainToolbar.playerBox.SetSel(currPlayer);
    UpdateGridSizesMenu();
    UpdateGridColorMenu();
    UpdateSkinMenuItems();
    UpdateTerrainViewMenuItems();
    UpdateBaseViewMenuItems();
    UpdateUnitMenuItems();
    UpdateSpriteMenuItems();
    UpdateCutCopyPasteMenuItems();
    chkd.mainToolbar.checkTerrain.SetCheck(cutCopyPasteTerrain);
    chkd.mainToolbar.checkDoodads.SetCheck(cutCopyPasteDoodads);
    chkd.mainToolbar.checkSprites.SetCheck(cutCopyPasteSprites);
    chkd.mainToolbar.checkUnits.SetCheck(cutCopyPasteUnits);
    chkd.mainToolbar.checkFog.SetCheck(cutCopyPasteFog);
}

bool GuiMap::CreateThis(HWND hClient, const std::string & title)
{
    if ( !WindowClassIsRegistered("MdiChild") )
    {
        UINT style = CS_DBLCLKS;
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);

        if ( !RegisterWindowClass(CS_DBLCLKS, NULL, hCursor, hBackground, NULL, "MdiChild", NULL, true) )
            return false;
    }

    return CreateMdiChild(title, WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hClient);
}

void GuiMap::ReturnKeyPress()
{
    if ( currLayer == Layer::Terrain && selections.hasTiles() )
        openTileProperties((view.tiles.sel().front()%getTileWidth())*32, (view.tiles.sel().front()/getTileWidth())*32);
    else if ( currLayer == Layer::Units )
    {
        if ( selections.hasUnits() )
        {
            if ( chkd.unitWindow.getHandle() == nullptr )
                chkd.unitWindow.CreateThis(chkd.getHandle());
            ShowWindow(chkd.unitWindow.getHandle(), SW_SHOW);
        }
    }
    else if ( currLayer == Layer::Sprites )
    {
        if ( selections.hasSprites() )
        {
            if ( chkd.spriteWindow.getHandle() == nullptr )
                chkd.spriteWindow.CreateThis(chkd.getHandle());
            ShowWindow(chkd.spriteWindow.getHandle(), SW_SHOW);
        }
    }
    else if ( currLayer == Layer::Locations )
    {
        if ( selections.getSelectedLocation() != NO_LOCATION )
        {
            if ( chkd.locationWindow.getHandle() == NULL )
            {
                if ( chkd.locationWindow.CreateThis(chkd.getHandle()) )
                    ShowWindow(chkd.locationWindow.getHandle(), SW_SHOWNORMAL);
            }
            else
                ShowWindow(chkd.locationWindow.getHandle(), SW_SHOW);
        }
    }
}

void GuiMap::SetAutoBackup(bool doAutoBackups)
{
    GuiMap::doAutoBackups = doAutoBackups;
}

ChkdPalette & GuiMap::getPalette()
{
    return scGraphics.getPalette();
}

ChkdPalette & GuiMap::getStaticPalette()
{
    return scGraphics.getStaticPalette();
}

LRESULT GuiMap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_CONTEXTMENU: ContextMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_PAINT: PaintMap(CM, chkd.maps.clipboard.isPasting()); break;
        case WM_MDIACTIVATE: ActivateMap((HWND)wParam, (HWND)lParam); return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
        case WM_ERASEBKGND: return 1; break; // Prevent background from showing
        case WM_HSCROLL: return HorizontalScroll(hWnd, msg, wParam, lParam); break;
        case WM_VSCROLL: return VerticalScroll(hWnd, msg, wParam, lParam); break;
        case WM_CHAR: return 0; break;
        case WM_SIZE: return DoSize(hWnd, wParam, lParam); break;
        case WM_KILLFOCUS: destroyBrush(); break;
        case WM_CLOSE: return ConfirmWindowClose(hWnd); break;
        case WM_DESTROY: return DestroyWindow(hWnd); break;
        case WM_RBUTTONUP: RButtonUp(hWnd, wParam, lParam); break;
        case WM_LBUTTONDBLCLK: LButtonDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_LBUTTONDOWN: LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MBUTTONDOWN: MButtonDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MBUTTONUP: MButtonUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MOUSEMOVE: MouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MOUSEHOVER: MouseHover(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_LBUTTONUP: LButtonUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MOUSEWHEEL: MouseWheel(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_WHEEL_DELTA_WPARAM(wParam), wParam); break;
        case WM_TIMER: PanTimerTimeout(); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}

LRESULT GuiMap::HorizontalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( LOWORD(wParam) )
    {
        case SB_LINELEFT      : screenLeft -= 8;             break;
        case SB_LINERIGHT     : screenLeft += 8;             break;
        case SB_THUMBPOSITION : screenLeft = HIWORD(wParam); break;
        case SB_THUMBTRACK    : screenLeft = HIWORD(wParam); break;
        case SB_PAGELEFT: case SB_PAGERIGHT:
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                if ( LOWORD(wParam) == SB_PAGELEFT )
                    screenLeft -= (rect.right-rect.left)/2;
                else
                    screenLeft += (rect.right-rect.left)/2;
            }
            break;
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    Scroll(true, false, true);
    Redraw(true);
    return 0;
}

LRESULT GuiMap::VerticalScroll(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
        case SB_LINEUP        : screenTop -= 8;             break;
        case SB_LINEDOWN      : screenTop += 8;             break;
        case SB_THUMBPOSITION : screenTop = HIWORD(wParam); break;
        case SB_THUMBTRACK    : screenTop = HIWORD(wParam); break;
        case SB_PAGEUP: case SB_PAGEDOWN:
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                if ( LOWORD(wParam) == SB_PAGEUP )
                    screenTop -= (rect.right-rect.left)/2;
                else
                    screenTop += (rect.right-rect.left)/2;
            }
            break;
        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    Scroll(false, true, true);
    Redraw(true);
    return 0;
}

void GuiMap::ActivateMap(HWND deactivate, HWND activate)
{
    if ( getHandle() == deactivate )
        destroyBrush();

    chkd.tilePropWindow.DestroyThis();
    chkd.unitWindow.DestroyThis();
    chkd.spriteWindow.DestroyThis();
    chkd.locationWindow.DestroyThis();
    chkd.mapSettingsWindow.DestroyThis();
    chkd.terrainPalWindow.DestroyThis();
    chkd.trigEditorWindow.DestroyThis();
    chkd.briefingTrigEditorWindow.DestroyThis();
    
    if ( activate != NULL )
    {
        chkd.maps.Focus(activate);
        Redraw(true);
        chkd.maps.UpdateTreeView();
    }
}

LRESULT GuiMap::DoSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = ClassWindow::WndProc(hWnd, WM_SIZE, wParam, lParam);
    Scroll(true, true, true);
    if ( skin != GuiMap::Skin::ClassicGDI )
    {
        RECT rcCli {};
        ClassWindow::getClientRect(rcCli);
        glViewport(0, 0, GLsizei(rcCli.right-rcCli.left), GLsizei(rcCli.bottom-rcCli.top));

        SwapBuffers(openGlDc->getDcHandle());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    return result;
}

LRESULT GuiMap::DestroyWindow(HWND hWnd)
{
    if ( brushAction )
        brushAction = std::nullopt;

    if ( panTimerID != 0 ) {
        KillTimer(hWnd, panTimerID);
        panTimerID = 0;
    }
    LRESULT destroyResult = ClassWindow::WndProc(hWnd, WM_DESTROY, 0, 0);
    PostMessage(chkd.getHandle(), CLOSE_MAP, u16(mapId), NULL);
    return destroyResult;
}

void GuiMap::ContextMenu(int x, int y)
{
    WinLib::ContextMenu menu {};
    menu.append("Undo\tCtrl+Z", [&](){ undo(); })
        .append("Redo\tCtrl+Y", [&](){ redo(); })
        .appendSeparator()
        .append("Cut\tCtrl+X", [&](){ chkd.maps.cut(); })
        .append("Copy\tCtrl+C", [&](){ chkd.maps.copy(); })
        .append("Paste\tCtrl+V", [&](){ chkd.maps.startPaste(false); })
        .appendSeparator()
        .append("Select All\tCtrl+A", [&](){ selectAll(); })
        .append("Delete\tDel", [&](){ deleteSelection(); })
        .append("Clear Selections\tEsc", [&](){ clearSelection(); })
        .append("Convert to terrain", [&](){ convertSelectionToTerrain(); }, false, currLayer != Layer::Doodads)
        .append("Stack Selected", [&](){ stackSelected(); }, false, currLayer != Layer::Units && currLayer != Layer::Sprites)
        .appendSeparator()
        .append("Create Location", [&](){ createLocation(); }, false, currLayer != Layer::Units && currLayer != Layer::Sprites)
        .append("Create Inverted Location", [&](){ createInvertedLocation(); }, false, currLayer != Layer::Units && currLayer != Layer::Sprites)
        .append("Create Mobile Inverted Location", [&]() { createMobileInvertedLocation(); }, false, currLayer != Layer::Units && currLayer != Layer::Sprites)
        .appendSeparator();

    if ( auto & layerMenu = menu.appendSubmenu("Layer") )
    {
        layerMenu.append("Terrain\tCtrl+T", [&](){ chkd.maps.ChangeLayer(Layer::Terrain); }, currLayer == Layer::Terrain)
            .append("Doodads\tCtrl+D", [&](){ chkd.maps.ChangeLayer(Layer::Doodads); }, currLayer == Layer::Doodads)
            .append("Fog Of War\tCtrl+F", [&](){ chkd.maps.ChangeLayer(Layer::FogEdit); }, currLayer == Layer::FogEdit)
            .append("Locations\tCtrl+L", [&](){ chkd.maps.ChangeLayer(Layer::Locations); }, currLayer == Layer::Locations)
            .append("Units\tCtrl+U", [&](){ chkd.maps.ChangeLayer(Layer::Units); }, currLayer == Layer::Units)
            .append("Sprites\tCtrl+R", [&](){ chkd.maps.ChangeLayer(Layer::Sprites); }, currLayer == Layer::Sprites)
            .append("Preview Fog of War", [&](){ chkd.maps.ChangeLayer(Layer::FogView); }, currLayer == Layer::FogView)
            .append("Cut Copy Paste\\Brushes", [&](){ chkd.maps.ChangeLayer(Layer::CutCopyPaste); }, currLayer == Layer::CutCopyPaste);
    }

    menu.appendSeparator()
        .append("Properties\tEnter", [&](){ ReturnKeyPress(); });

    menu.select(getHandle(), x, y);
}

void GuiMap::RButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ScopedBrushDestructor destroyBrush{brushAction};
    bool wasPasting = clipboard.isPasting();
    if ( wasPasting )
        chkd.maps.endPaste();

    ClipCursor(NULL);
    Redraw(true);
    
    if ( !wasPasting )
        ClassWindow::WndProc(hWnd, WM_RBUTTONUP, wParam, lParam); // May continue on to context menu
}

void GuiMap::LButtonDoubleClick(int x, int y, WPARAM wParam)
{
    if ( currLayer == Layer::Locations )
        doubleClickLocation(x, y);
    else
        ReturnKeyPress();
}

void GuiMap::LButtonDown(int x, int y, WPARAM wParam)
{
    if ( brushAction )
        brushAction = std::nullopt;

    if ( (wParam & MK_LBUTTON) != MK_LBUTTON )
        return; // LButton not down

    FocusThis();
    selections.moved = false;
    u16 gridWidth = 32, gridHeight = 32;
    u32 mapClickX = (s32(((double)x)/getZoom()) + screenLeft),
        mapClickY = (s32(((double)y)/getZoom()) + screenTop);
    
    bool shift = (wParam & MK_SHIFT) == MK_SHIFT;
    bool ctrl = (wParam & MK_CONTROL) == MK_CONTROL;
    bool shiftOrCtrl = shift || ctrl;
    bool shiftLeftClick = shift && !ctrl;
    bool ctrlLeftClick = ctrl && !shift;
    bool nonZeroDragSnap = ctrlLeftClick;
    
    brushAction.emplace(std::move(createAction()));
    auto & edit = *brushAction;

    if ( shiftLeftClick && currLayer == Layer::Terrain )
    {
        setActionDescription(ActionDescriptor::OpenTileProperties);
        openTileProperties(mapClickX, mapClickY);
        return;
    }

    chkd.tilePropWindow.DestroyThis();
    
    if ( currLayer == Layer::FogEdit )
    {
        selections.setDrags(mapClickX, mapClickY);
        setActionDescription(ActionDescriptor::BrushFog);
        clipboard.initFogBrush(mapClickX, mapClickY, *this, shiftOrCtrl);
        clipboard.doPaste(currLayer, currTerrainSubLayer, mapClickX, mapClickY, *this, false);
    }
    if ( chkd.maps.clipboard.isPasting() )
    {
        if ( currLayer == Layer::Terrain || currLayer == Layer::CutCopyPaste )
            refreshTileOccupationCache();

        paste(mapClickX, mapClickY);
    }
    else if ( currLayer == Layer::Terrain )
    {
        selections.setDrags((mapClickX+16)/32*32, (mapClickY+16)/32*32);
        if ( !ctrl )
        {
            setActionDescription(ActionDescriptor::ClearTileSel);
            edit->tiles.clearSelections();
        }
    }
    else
    {
        selections.setDrags(mapClickX, mapClickY);
        if ( currLayer == Layer::CutCopyPaste )
        {
            setActionDescription(ActionDescriptor::UpdateMiscSelection);
            if ( snapCutCopyPasteSel && (cutCopyPasteSnapTileOverGrid || scGraphics.GetGridSize(0, gridWidth, gridHeight)) )
                selections.snapDrags(gridWidth, gridHeight, nonZeroDragSnap);
        }
        else if ( currLayer == Layer::Locations )
        {
            u32 x1 = mapClickX, y1 = mapClickY;
            if ( SnapLocationDimensions(x1, y1, x1, y1, LocSnapFlags(LocSnapFlags::SnapX1|LocSnapFlags::SnapY1)) )
                selections.setDrags(x1, y1);

            selections.setLocationFlags(getLocSelFlags(mapClickX, mapClickY));
            if ( selections.getSelectedLocation() != Chk::LocationId::NoLocation && !selections.selFlagsIndicateInside() )
                selections.clear();
        }
        else if ( currLayer == Layer::Doodads )
            setActionDescription(ActionDescriptor::UpdateDoodadSel);
    }

    SetCapture(getHandle());
    TrackMouse(defaultHoverTime);
    setDragging(true);
    Redraw(false);
}

void GuiMap::MouseMove(HWND hWnd, int x, int y, WPARAM wParam)
{
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;

    panCurrentX = x;
    panCurrentY = y;
    s32 mapHoverX = (s32(((double)x)/getZoom())) + screenLeft,
        mapHoverY = (s32(((double)y)/getZoom())) + screenTop;

    bool lButtonDown = (wParam & MK_LBUTTON) == MK_LBUTTON;
    bool ctrl = (wParam & MK_CONTROL) == MK_CONTROL;

    if ( lButtonDown ) // If click and dragging
    {
        chkd.maps.stickCursor(); // Stop cursor from reverting
        selections.moved = true;
    }
    else // If not click and dragging
        chkd.maps.updateCursor(mapHoverX, mapHoverY); // Determine proper hover cursor

    // Set status bar tracking pos
    char newPos[64];
    std::snprintf(newPos, 64, "%i, %i (%i, %i)", mapHoverX, mapHoverY, mapHoverX / 32, mapHoverY / 32);
    chkd.statusBar.SetText(0, newPos);

    lastMousePosition = {mapHoverX, mapHoverY};
    
    if ( lButtonDown && ctrl && currLayer == Layer::Terrain && chkd.terrainPalWindow.getHandle() != nullptr )
        chkd.terrainPalWindow.SelectTile(Scenario::getTilePx((std::size_t)mapHoverX, (std::size_t)mapHoverY));
    
    if ( currLayer == Layer::FogEdit )
    {
        selections.endDrag = {mapHoverX, mapHoverY};
        if ( dragging )
            clipboard.doPaste(currLayer, currTerrainSubLayer, mapHoverX, mapHoverY, *this, false);
        
        Redraw(dragging);
    }
    else if ( lButtonDown && isDragging() )
    {
        RECT rcMap {};
        GetClientRect(hWnd, &rcMap);

        // If pasting, move paste
        if ( chkd.maps.clipboard.isPasting() )
        {
            s32 xc = mapHoverX, yc = mapHoverY;
            if ( panCurrentX <= rcMap.left )
                xc = s32(rcMap.left/zoom) + screenLeft;
            else if ( panCurrentX > rcMap.right-2 )
                xc = s32((double(rcMap.right)-2)/zoom) + screenLeft;
            if ( panCurrentY <= rcMap.top )
                yc = s32(rcMap.top/zoom) + screenTop;
            else if ( panCurrentY > rcMap.bottom-2 )
                yc = s32((double(rcMap.bottom)-2)/zoom) + screenTop;

            selections.endDrag = {xc, yc};
            SnapSelEndDrag();
            if ( !chkd.maps.clipboard.isPreviousPasteLoc(u16(xc), u16(yc)) )
                paste((s16)xc, (s16)yc);
        }

        if ( x <= 0 || y <= 0 || x >= rcMap.right-2 || y >= rcMap.bottom-2 )
        {
            if ( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-lastMoveEdgeDrag).count() > 20 )
            {
                lastMoveEdgeDrag = std::chrono::system_clock::now();
                EdgeDrag(hWnd, x, y);
            }
        }

        selections.endDrag = { mapHoverX, mapHoverY };
        if ( currLayer == Layer::Terrain && !chkd.maps.clipboard.isPasting() )
            selections.endDrag = { (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 };
        else if ( currLayer == Layer::Locations )
        {
            u32 x2 = mapHoverX, y2 = mapHoverY;
            if ( SnapLocationDimensions(x2, y2, x2, y2, LocSnapFlags(LocSnapFlags::SnapX2|LocSnapFlags::SnapY2)) )
                selections.endDrag = {s32(x2), s32(y2)};
        }
        else if ( currLayer == Layer::CutCopyPaste && snapCutCopyPasteSel )
        {
            u16 gridWidth = 32, gridHeight = 32;
            if ( cutCopyPasteSnapTileOverGrid || scGraphics.GetGridSize(0, gridWidth, gridHeight) )
                selections.snapDrags(gridWidth, gridHeight, false);
        }
        else if ( currLayer == Layer::Units )
        {
            s32 xc = mapHoverX, yc = mapHoverY;
            selections.endDrag = {xc, yc};
            SnapSelEndDrag();
        }
        PaintMap(nullptr, chkd.maps.clipboard.isPasting());
    }
    else if ( chkd.maps.clipboard.isPasting() == true ) // Not fog, and lButton is not down
    {
        if ( GetKeyState(VK_SPACE) & 0x8000 )
        {
            RECT rcMap;
            GetClientRect(hWnd, &rcMap);
    
            if ( x <= 0 || x <= rcMap.right-2 || y >= 0 || y >= rcMap.bottom-2 )
            {
                Scroll(true, true, true,
                    (x <= 0 ? screenLeft-32 : (x >= rcMap.right-2 ? screenLeft+32 : -1)),
                    (y <= 0 ? screenTop-32 : (y >= rcMap.bottom-2 ? screenTop+32 : -1)));
            }
        }
                        
        if ( currLayer == Layer::CutCopyPaste )
        {
            selections.endDrag = {mapHoverX, mapHoverY};
            u16 gridWidth = 32, gridHeight = 32;
            if ( snapCutCopyPasteSel && (cutCopyPasteSnapTileOverGrid || scGraphics.GetGridSize(0, gridWidth, gridHeight))
                && gridWidth > 0 && gridHeight > 0 )
            {
                selections.snapDrags(gridWidth, gridHeight, false);
            }
        }
        else
            selections.endDrag = {mapHoverX, mapHoverY};
                        
        PaintMap(nullptr, chkd.maps.clipboard.isPasting());
    }
}

void GuiMap::MouseHover(HWND hWnd, int x, int y, WPARAM wParam)
{
    switch ( wParam )
    {
        case MK_CONTROL|MK_LBUTTON:
        case MK_LBUTTON:
            EdgeDrag(hWnd, x, y);
            break;
    
        default:
            if ( chkd.maps.clipboard.isPasting() == true || (CM->getLayer() == Layer::FogEdit && dragging) )
            {
                RECT rcMap;
                GetClientRect(hWnd, &rcMap);

                Scroll(true, true, true,
                    (x <= 0 ? screenLeft-8 : (x >= rcMap.right-2 ? screenLeft+8 : -1)),
                    (y <= 0 ? screenTop-8 : (y >= rcMap.bottom-2 ? screenTop+8 : -1)));
                RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);

                x = (s32(((double)x)/getZoom())) + screenLeft,
                y = (s32(((double)y)/getZoom())) + screenTop;
                selections.endDrag = {x, y};
                TrackMouse(100);
            }
            break;
    }
}

void GuiMap::MouseWheel(HWND hWnd, int x, int y, int z, WPARAM wParam)
{
    if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
        return;

    double scale = getZoom();
    u32 scaleIndex = -1;
    for ( u32 i = 0; i < defaultZooms.size(); i++ )
    {
        if ( scale == defaultZooms[i] )
        {
            if ( z > 0 && i > 0 )
                scaleIndex = i - 1;
            else if ( z < 0 && i < defaultZooms.size() - 1 )
                scaleIndex = i + 1;

            break;
        }
    }

    if ( scaleIndex != -1 )
        setZoom(defaultZooms[scaleIndex]);
}

void GuiMap::LButtonUp(HWND hWnd, int x, int y, WPARAM wParam)
{
    ScopedBrushDestructor destroyBrush{brushAction};
    finalizeTerrainOperation();
    ReleaseCapture();
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    x = s16(x/getZoom() + screenLeft);
    y = s16(y/getZoom() + screenTop);
    if ( wParam == MK_CONTROL && currLayer == Layer::Terrain && chkd.terrainPalWindow.getHandle() != nullptr )
    {
        chkd.terrainPalWindow.SelectTile(Scenario::getTilePx((std::size_t)x, (std::size_t)y));
        UnlockCursor();
    }

    if ( isDragging() )
    {
        setDragging(false);
        if ( !clipboard.isPasting() )
        {
            if ( currLayer == Layer::Terrain )
                FinalizeTerrainSelection(hWnd, x, y, wParam);
            else if ( currLayer == Layer::Locations )
                FinalizeLocationDrag(hWnd, x, y, wParam);
            else if ( currLayer == Layer::Units )
                FinalizeUnitSelection(hWnd, x, y, wParam);
            else if ( currLayer == Layer::Doodads )
                FinalizeDoodadSelection(hWnd, x, y, wParam);
            else if ( currLayer == Layer::Sprites )
                FinalizeSpriteSelection(hWnd, x, y, wParam);
            else if ( currLayer == Layer::CutCopyPaste )
                FinalizeCutCopyPasteSelection(hWnd, x, y, wParam);
        }
        else if ( currLayer == Layer::CutCopyPaste )
            finalizeFogOperation();

        if ( currLayer == Layer::FogEdit )
        {
            clipboard.doPaste(currLayer, currTerrainSubLayer, x, y,*this, false);
            finalizeFogOperation();
        }

        selections.endDrag = {-1, -1};
        setDragging(false);
        Redraw(false);
    }

    if ( !chkd.maps.clipboard.isPasting() || (currLayer == Layer::FogEdit && !dragging) )
        ClipCursor(NULL);
}

void GuiMap::MButtonDown(HWND hWnd, int x, int y, WPARAM wParam)
{
    if ( panStartX == -1 && panStartY == -1 ) {
        panStartX = x;
        panStartY = y;
        panTimerID = SetTimer(hWnd, 1, 1000/30, NULL);
    }
    SetCapture(hWnd);
}

void GuiMap::MButtonUp(HWND hWnd, int x, int y, WPARAM wParam)
{
    if ( panTimerID != 0 ) {
        KillTimer(hWnd, panTimerID);
        panTimerID = 0;
    }
    panStartX = panStartY = -1;
    ReleaseCapture();
}

void GuiMap::PanTimerTimeout()
{
    if ( panStartX == -1 || panStartY == -1 ) return;    
    int xDelta = std::min((panStartX - panCurrentX) / 4, 64);
    int yDelta = std::min((panStartY - panCurrentY) / 4, 64);
    Scroll(true, true, true, screenLeft-xDelta, screenTop-yDelta);
}

void GuiMap::FinalizeTerrainSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction();
    selections.endDrag = {(mapX+16)/32, (mapY+16)/32};
    selections.startDrag = {selections.startDrag.x/32, selections.startDrag.y/32};
    u16 width = (u16)Scenario::getTileWidth();
                    
    if ( wParam == MK_CONTROL && selections.startEqualsEndDrag() ) // Add/remove single tile to/front existing selection
    {
        setActionDescription(ActionDescriptor::UpdateTileSel);
        selections.endDrag = {mapX/32, mapY/32};
        edit->tiles.toggleSelected(selections.endDrag.y * getTileWidth() + selections.endDrag.x);
    }
    else // Add/remove multiple tiles from selection
    {
        selections.sortDragPoints();
    
        if ( selections.startDrag.y < selections.endDrag.y &&
             selections.startDrag.x < selections.endDrag.x )
        {
            setActionDescription(ActionDescriptor::UpdateTileSel);
            if ( selections.endDrag.x > LONG(Scenario::getTileWidth()) )
                selections.endDrag = {(s32)Scenario::getTileWidth(), selections.endDrag.y};
            if ( selections.endDrag.y > LONG(Scenario::getTileHeight()) )
                selections.endDrag = {selections.endDrag.x, (s32)Scenario::getTileHeight()};
    
            for ( int yRow = selections.startDrag.y; yRow < selections.endDrag.y; yRow++ )
            {
                for ( int xRow = selections.startDrag.x; xRow < selections.endDrag.x; xRow++ )
                    edit->tiles.toggleSelected(yRow * getTileWidth() + xRow);
            }
        }
    }
}

void GuiMap::FinalizeLocationDrag(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction();
    if ( selections.moved ) // attempt to move, resize, or create location
    {
        u32 startX = selections.startDrag.x,
            startY = selections.startDrag.y,
            endX = mapX,
            endY = mapY;
        s32 dragX = endX-startX;
        s32 dragY = endY-startY;

        if ( selections.getLocationFlags() == LocSelFlags::None ) // Create location
        {
            setActionDescription(ActionDescriptor::CreateLocation);
            ascendingOrder(startX, endX);
            ascendingOrder(startY, endY);
            SnapLocationDimensions(startX, startY, endX, endY, LocSnapFlags::SnapAll);

            if ( startX != endX && startY != endY )
            {
                Chk::Location newLocation {};
                newLocation.left = startX;
                newLocation.top = startY;
                newLocation.top = startY;
                newLocation.right = endX;
                newLocation.bottom = endY;
                newLocation.elevationFlags = 0;

                size_t newLocationId = Scenario::addLocation(newLocation);
                if ( newLocationId != Chk::LocationId::NoLocation )
                {
                    Scenario::setLocationName<RawString>(newLocationId, "Location " + std::to_string(newLocationId), Chk::Scope::Game);
                    Scenario::deleteUnusedStrings(Chk::Scope::Both);
                    selections.selectLocation(u16(newLocationId));
                    chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree(true);
                    refreshScenario();
                }
                else
                    Error("Max Locations Reached!");
            }
            selections.setDrags(-1, -1);
        }
        else // Move or resize location
        {
            setActionDescription(ActionDescriptor::MoveLocation);
            u16 selectedLocation = selections.getSelectedLocation();
            if ( selectedLocation != NO_LOCATION && selectedLocation < Scenario::numLocations() )
            {
                const Chk::Location & loc = Scenario::getLocation(selectedLocation);
                LocSelFlags selFlags = selections.getLocationFlags();
                if ( selFlags == LocSelFlags::Middle ) // Move location
                {
                    bool xInverted = loc.right < loc.left,
                         yInverted = loc.bottom < loc.top;

                    if ( s32(loc.left + dragX) < 0 && !xInverted )
                    {
                        edit->locations[selectedLocation].right -= loc.left;
                        edit->locations[selectedLocation].left = 0;
                    }
                    else if ( s32(loc.right + dragX) < 0 && xInverted )
                    {
                        edit->locations[selectedLocation].left -= loc.right;
                        edit->locations[selectedLocation].right = 0;
                    }
                    else
                    {
                        edit->locations[selectedLocation].left += dragX;
                        edit->locations[selectedLocation].right += dragX;
                    }

                    if ( s32(loc.top + dragY) < 0 && !yInverted )
                    {
                        edit->locations[selectedLocation].bottom -= loc.top;
                        edit->locations[selectedLocation].top = 0;
                    }
                    else if ( s32(loc.bottom + dragY) < 0 && yInverted )
                    {
                        edit->locations[selectedLocation].top -= loc.bottom;
                        edit->locations[selectedLocation].bottom = 0;
                    }
                    else
                    {
                        edit->locations[selectedLocation].top += dragY;
                        edit->locations[selectedLocation].bottom += dragY;
                    }
                    
                    s32 xc1Preserve = loc.left,
                        yc1Preserve = loc.top,
                        xc2Preserve = loc.right,
                        yc2Preserve = loc.bottom;
                                        
                    if ( xInverted )
                    {
                        if ( SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX2) )
                        {
                            edit->locations[selectedLocation].left += loc.right - xc2Preserve; // Maintain location width
                            dragX += loc.left - xc1Preserve;
                        }
                    }
                    else if ( SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX1) )
                    {
                        edit->locations[selectedLocation].right += loc.left - xc1Preserve; // Maintain location width
                        dragX += loc.right - xc2Preserve;
                    }

                    if ( yInverted )
                    {
                        if ( SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY2) )
                        {
                            edit->locations[selectedLocation].top += loc.bottom - yc2Preserve; // Maintain location height
                            dragY += loc.top - yc1Preserve;
                        }
                    }
                    else if ( SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY1) )
                    {
                        edit->locations[selectedLocation].bottom += loc.top - yc1Preserve; // Maintain location height
                        dragY += loc.bottom - yc2Preserve;
                    }
                }
                else // Resize location
                {
                    if ( (selFlags & LocSelFlags::North) == LocSelFlags::North )
                    {
                        if ( loc.top <= loc.bottom ) // Standard yc
                        {
                            edit->locations[selectedLocation].top += dragY;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY1);
                        }
                        else // Inverted yc
                        {
                            edit->locations[selectedLocation].bottom += dragY;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY2);
                        }
                                    
                    }
                    else if ( (selFlags & LocSelFlags::South) == LocSelFlags::South )
                    {
                        if ( loc.top <= loc.bottom ) // Standard yc
                        {
                            edit->locations[selectedLocation].bottom += dragY;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY2);
                        }
                        else // Inverted yc
                        {
                            edit->locations[selectedLocation].top += dragY;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapY1);
                        }
                    }
    
                    if ( (selFlags & LocSelFlags::West) == LocSelFlags::West )
                    {
                        if ( loc.left <= loc.right ) // Standard xc
                        {
                            edit->locations[selectedLocation].left += dragX;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX1);
                        }
                        else // Inverted xc
                        {
                            edit->locations[selectedLocation].right += dragX;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX2);
                        }
                    }
                    else if ( (selFlags & LocSelFlags::East) == LocSelFlags::East )
                    {
                        if ( loc.left <= loc.right ) // Standard xc
                        {
                            edit->locations[selectedLocation].right += dragX;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX2);
                        }
                        else // Inverted xc
                        {
                            edit->locations[selectedLocation].left += dragX;
                            SnapLocationDimensions(selectedLocation, LocSnapFlags::SnapX1);
                        }
                    }
                }
                Redraw(false);
                if ( chkd.locationWindow.getHandle() != NULL )
                    chkd.locationWindow.RefreshLocationInfo();

                selections.setDrags(-1, -1);
            }
        }
    }
    else // attempt to select location, if you aren't resizing
    {
        selections.selectLocation(selections.startDrag.x, selections.startDrag.y, !LockAnywhere());
        selections.setDrags(-1, -1);
        if ( chkd.locationWindow.getHandle() != NULL )
            chkd.locationWindow.RefreshLocationInfo();
    
        Redraw(false);
    }
    selections.setLocationFlags(LocSelFlags::None);
}

void GuiMap::FinalizeUnitSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction(ActionDescriptor::UpdateUnitSel);
    selections.endDrag = {mapX, mapY};
    selections.sortDragPoints();
    if ( wParam != MK_CONTROL )
        // Remove selected units
    {
        if ( chkd.unitWindow.getHandle() != nullptr )
        {
            chkd.unitWindow.SetChangeHighlightOnly(true);
            for ( auto unitIndex : view.units.sel() )
                chkd.unitWindow.DeselectIndex(unitIndex);
            
            chkd.unitWindow.SetChangeHighlightOnly(false);
        }
        edit->units.clearSelections();
        chkd.unitWindow.UpdateEnabledState();
    }
        
    size_t numUnits = Scenario::numUnits();
    for ( size_t i=0; i<numUnits; i++ )
    {
        int unitLeft = 0, unitRight  = 0,
            unitTop  = 0, unitBottom = 0;
    
        const Chk::Unit & unit = Scenario::getUnit(i);
        
        if ( (u16)unit.type < (u16)Sc::Unit::TotalTypes )
        {
            unitLeft = unit.xc - chkd.scData.units.getUnit(unit.type).unitSizeLeft;
            unitRight = unit.xc + chkd.scData.units.getUnit(unit.type).unitSizeRight;
            unitTop = unit.yc - chkd.scData.units.getUnit(unit.type).unitSizeUp;
            unitBottom = unit.yc + chkd.scData.units.getUnit(unit.type).unitSizeDown;
        }
        else
        {
            unitLeft = unit.xc - chkd.scData.units.getUnit(Sc::Unit::Type::TerranMarine).unitSizeLeft;
            unitRight = unit.xc + chkd.scData.units.getUnit(Sc::Unit::Type::TerranMarine).unitSizeRight;
            unitTop = unit.yc - chkd.scData.units.getUnit(Sc::Unit::Type::TerranMarine).unitSizeUp;
            unitBottom = unit.yc + chkd.scData.units.getUnit(Sc::Unit::Type::TerranMarine).unitSizeDown;
        }
    
        if ( selections.startDrag.x <= unitRight && selections.endDrag.x >= unitLeft
            && selections.startDrag.y <= unitBottom && selections.endDrag.y >= unitTop )
        {
            bool wasSelected = selections.unitIsSelected((u16)i);
            if ( wasSelected )
                edit->units.deselect(i);
            else
                edit->units.select(i);

            if ( chkd.unitWindow.getHandle() != nullptr )
            {
                chkd.unitWindow.SetChangeHighlightOnly(true);
                if ( wasSelected )
                    chkd.unitWindow.DeselectIndex((u16)i);
                else
                    chkd.unitWindow.FocusAndSelectIndex((u16)i);
                
                chkd.unitWindow.SetChangeHighlightOnly(false);
            }
            chkd.unitWindow.UpdateEnabledState();
        }
    }
}

void GuiMap::FinalizeDoodadSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction(ActionDescriptor::UpdateDoodadSel);
    selections.endDrag = {mapX, mapY};
    selections.sortDragPoints();
    if ( wParam != MK_CONTROL ) // Remove selected doodads
        edit->doodads.clearSelections();
        
    size_t numDoodads = Scenario::numDoodads();
    for ( size_t i=0; i<numDoodads; i++ )
    {
        const Chk::Doodad & doodad = Scenario::getDoodad(i);
        const auto & tileset = chkd.scData.terrain.get(getTileset());
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
        {
            const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[size_t(*doodadGroupIndex)];
            s32 doodadWidth = 32*s32(doodadDat.tileWidth);
            s32 doodadHeight = 32*s32(doodadDat.tileHeight);
            s32 left = s32(doodad.xc) - doodadWidth/2;
            s32 top = s32(doodad.yc) - doodadHeight/2;
            s32 right = s32(doodad.xc) + doodadWidth/2;
            s32 bottom = s32(doodad.yc) + doodadHeight/2;

            s32 selLeft = selections.startDrag.x;
            s32 selTop = selections.startDrag.y;
            s32 selRight = selections.endDrag.x;
            s32 selBottom = selections.endDrag.y;
            bool inBounds = left <= selRight && top <= selBottom && right >= selLeft && bottom >= selTop;
            if ( inBounds )
            {
                if ( selections.doodadIsSelected(i) )
                    edit->doodads.deselect(i);
                else
                    edit->doodads.select(i);
            }
        }
    }
}

void GuiMap::FinalizeSpriteSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction(ActionDescriptor::UpdateSpriteSel);
    selections.endDrag = {mapX, mapY};
    selections.sortDragPoints();
    if ( wParam != MK_CONTROL )
    {
        if ( chkd.spriteWindow.getHandle() != nullptr )
        {
            chkd.spriteWindow.SetChangeHighlightOnly(true);
            for ( auto spriteIndex : view.sprites.sel() )
                chkd.spriteWindow.DeselectIndex(u16(spriteIndex));
            
            chkd.spriteWindow.SetChangeHighlightOnly(false);
        }
        edit->sprites.clearSelections();
        chkd.spriteWindow.UpdateEnabledState();
    }

    size_t numSprites = Scenario::numSprites();
    for ( size_t i=0; i<numSprites; ++i )
    {
        int spriteLeft = 0, spriteRight = 0,
            spriteTop = 0, spriteBottom = 0;

        const Chk::Sprite & sprite = Scenario::getSprite(i);
        if ( selections.startDrag.x <= sprite.xc && selections.endDrag.x >= sprite.xc &&
             selections.startDrag.y <= sprite.yc && selections.endDrag.y >= sprite.yc )
        {
            bool wasSelected = selections.spriteIsSelected(i);
            if ( wasSelected )
                edit->sprites.deselect(i);
            else
                edit->sprites.select(i);

            if ( chkd.spriteWindow.getHandle() != nullptr )
            {
                chkd.spriteWindow.SetChangeHighlightOnly(true);
                if ( wasSelected )
                    chkd.spriteWindow.DeselectIndex((u16)i);
                else
                    chkd.spriteWindow.FocusAndSelectIndex((u16)i);
                
                chkd.spriteWindow.SetChangeHighlightOnly(false);
            }
            chkd.spriteWindow.UpdateEnabledState();
        }
    }
}

void GuiMap::FinalizeFogSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction();
    s32 startTileX = (selections.startDrag.x+16)/32;
    s32 startTileY =  (selections.startDrag.y+16)/32;
    s32 endTileX = (selections.endDrag.x+16)/32;
    s32 endTileY = (selections.endDrag.y+16)/32;

    bool startEqualsEnd = startTileX == endTileX && startTileY == endTileY;
    if ( wParam == MK_CONTROL && startEqualsEnd ) // Add/remove single fog tile to/front existing selection
    {
        s32 endTileX = (selections.endDrag.x)/32;
        s32 endTileY =  (selections.endDrag.y)/32;
        edit->tileFog.toggleSelected(endTileY * getTileWidth() + endTileX);
    }
    else if ( startTileX < endTileX && startTileY < endTileY ) // Add/remove multiple fog tiles from selection
    {
        if ( endTileX > LONG(Scenario::getTileWidth()) )
            endTileX = s32(Scenario::getTileWidth());
        if ( endTileY > LONG(Scenario::getTileHeight()) )
            endTileY = s32(Scenario::getTileHeight());
    
        for ( int yRow = startTileY; yRow < endTileY; yRow++ )
        {
            for ( int xRow = startTileX; xRow < endTileX; xRow++ )
                edit->tileFog.toggleSelected(yRow * getTileWidth() + xRow);
        }
    }
}

void GuiMap::FinalizeCutCopyPasteSelection(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    auto edit = createAction();
    bool snapped = false;
    selections.endDrag = {mapX, mapY};
    if ( snapCutCopyPasteSel )
    {
        u16 gridWidth = 32, gridHeight = 32;
        if ( cutCopyPasteSnapTileOverGrid || scGraphics.GetGridSize(0, gridWidth, gridHeight) )
        {
            selections.snapDrags(gridWidth, gridHeight, (wParam & MK_CONTROL) == MK_CONTROL);
            snapped = true;
        }
    }
    selections.sortDragPoints();
    if ( (wParam & MK_CONTROL) != MK_CONTROL )
    {
        edit->tiles.clearSelections();
        edit->doodads.clearSelections();
        edit->sprites.clearSelections();
        edit->units.clearSelections();
        edit->tileFog.clearSelections();
    }

    if ( cutCopyPasteTerrain )
    {
        s32 startTileX = (selections.startDrag.x+16)/32;
        s32 startTileY =  (selections.startDrag.y+16)/32;
        s32 endTileX = (selections.endDrag.x+16)/32;
        s32 endTileY = (selections.endDrag.y+16)/32;

        bool startEqualsEnd = startTileX == endTileX && startTileY == endTileY;
        if ( wParam == MK_CONTROL && startEqualsEnd ) // Add/remove single tile to/front existing selection
            edit->tiles.toggleSelected(startTileY*getTileWidth()+startTileX);
        else if ( startTileX < endTileX && startTileY < endTileY ) // Add/remove multiple tiles from selection
        {
            if ( endTileX > LONG(Scenario::getTileWidth()) )
                endTileX = s32(Scenario::getTileWidth());
            if ( endTileY > LONG(Scenario::getTileHeight()) )
                endTileY = s32(Scenario::getTileHeight());
    
            for ( int yRow = startTileY; yRow < endTileY; yRow++ )
            {
                for ( int xRow = startTileX; xRow < endTileX; xRow++ )
                    edit->tiles.toggleSelected(yRow*getTileWidth()+xRow);
            }
        }
    }

    if ( cutCopyPasteUnits )
        FinalizeUnitSelection(hWnd, selections.endDrag.x, selections.endDrag.y, wParam);

    if ( cutCopyPasteDoodads )
        FinalizeDoodadSelection(hWnd, selections.endDrag.x, selections.endDrag.y, wParam);

    if ( cutCopyPasteSprites )
        FinalizeSpriteSelection(hWnd, selections.endDrag.x, selections.endDrag.y, wParam);

    if ( cutCopyPasteFog )
        FinalizeFogSelection(hWnd, selections.endDrag.x, selections.endDrag.y, wParam);
}

LRESULT GuiMap::ConfirmWindowClose(HWND hWnd)
{
    if ( CanExit() )
        return ClassWindow::WndProc(hWnd, WM_CLOSE, 0, 0);
    else
        return 0;
}

bool GuiMap::GetBackupPath(time_t currTime, std::string & outFilePath)
{
    if ( auto moduleDirectory = getModuleDirectory() )
    {
        tm* currTimes = localtime(&currTime);
        int year = currTimes->tm_year + 1900, month = currTimes->tm_mon + 1, day = currTimes->tm_mday,
            hour = currTimes->tm_hour, minute = currTimes->tm_min, seconds = currTimes->tm_sec;

        makeDirectory(*moduleDirectory + "\\chkd");
        makeDirectory(*moduleDirectory + "\\chkd\\Backups");

        outFilePath = *moduleDirectory + "\\chkd\\Backups\\" +
            std::to_string(year) + std::string(month <= 9 ? "-0" : "-") + std::to_string(month) +
            std::string(day <= 9 ? "-0" : "-") + std::to_string(day) + std::string(hour <= 9 ? " 0" : " ") +
            std::to_string(hour) + std::string(minute <= 9 ? "h0" : "h") + std::to_string(minute) +
            std::string(seconds <= 9 ? "m0" : "m") + std::to_string(seconds) + std::string("s ") + std::string(getFileName());

        return true;
    }
    outFilePath.clear();
    return false;
}

bool GuiMap::TryBackup(bool & outCopyFailed)
{
    outCopyFailed = false;
    if ( doAutoBackups && MapFile::getFilePath().length() > 0 )
    {
        time_t currTime = time(0);
        // If there are no previous backups or enough time has elapsed since the last...
        if ( (lastBackupTime == -1 || difftime(lastBackupTime, currTime) >= minSecondsBetweenBackups) )
        {
            std::string backupPath;
            if ( GetBackupPath(currTime, backupPath) && makeFileCopy(MapFile::getFilePath(), backupPath) )
            {
                lastBackupTime = currTime;
                return true;
            }
            else
                outCopyFailed = true;
        }
    }
    return false;
}

GuiMap::Skin GuiMap::GetSkin()
{
    return this->skin;
}

std::string_view getSkinName(GuiMap::Skin skin)
{
    switch ( skin )
    {
        case GuiMap::Skin::ClassicGDI: return "Classic GDI";
        case GuiMap::Skin::ClassicGL: return "Classic OpenGL";
        case GuiMap::Skin::ScrSD: return "Remastered SD";
        case GuiMap::Skin::ScrHD2: return "Remastered HD2";
        case GuiMap::Skin::ScrHD: return "Remastered HD";
        case GuiMap::Skin::CarbotHD2: return "Carbot HD2";
        case GuiMap::Skin::CarbotHD: return "Carbot HD";
        default: throw std::logic_error("Invalid skin passed to getSkinName");
    }
}

void GuiMap::SetSkin(GuiMap::Skin skin)
{
    if ( skin == this->skin )
        return;

    // Validate the skin and if remastered, turn it into Scr::GraphicsData::LoadSettings
    Scr::GraphicsData::LoadSettings loadSettings {
        .visualQuality = Scr::VisualQuality::SD,
        .skinId = Scr::Skin::Id::Classic,
        .tileset = Sc::Terrain::Tileset(MapFile::getTileset() % Sc::Terrain::NumTilesets),
        .forceShowStars = false
    };
    switch ( skin )
    {
        case Skin::ClassicGDI: loadSettings.visualQuality = Scr::VisualQuality::SD; loadSettings.skinId = Scr::Skin::Id::Classic; break;
        case Skin::ClassicGL: loadSettings.visualQuality = Scr::VisualQuality::SD; loadSettings.skinId = Scr::Skin::Id::Classic; break;
        case Skin::ScrSD: loadSettings.visualQuality = Scr::VisualQuality::SD; loadSettings.skinId = Scr::Skin::Id::Remastered; break;
        case Skin::ScrHD2: loadSettings.visualQuality = Scr::VisualQuality::HD2; loadSettings.skinId = Scr::Skin::Id::Remastered; break;
        case Skin::ScrHD: loadSettings.visualQuality = Scr::VisualQuality::HD; loadSettings.skinId = Scr::Skin::Id::Remastered; break;
        case Skin::CarbotHD2: loadSettings.visualQuality = Scr::VisualQuality::HD2; loadSettings.skinId = Scr::Skin::Id::Carbot; break;
        case Skin::CarbotHD: loadSettings.visualQuality = Scr::VisualQuality::HD; loadSettings.skinId = Scr::Skin::Id::Carbot; break;
        default: throw std::logic_error("Unrecognized skin!");
    }

    if ( skin != Skin::ClassicGDI ) // Prepare OpenGL
    {
        chkd.maps.setGlRenderTarget(this->openGlDc, *this);

        RECT rcCli {};
        ClassWindow::getClientRect(rcCli);
        glViewport(0, 0, GLsizei(rcCli.right-rcCli.left), GLsizei(rcCli.bottom-rcCli.top));

        SwapBuffers(openGlDc->getDcHandle());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if ( chkd.scrData == nullptr )
        {
            chkd.scrData = std::make_unique<Scr::GraphicsData>();
            chkd.scrData->openGlContextSemaphore = chkd.maps.getContextSemaphore();
        }
        
        if ( chkd.scrData->defaultFont == nullptr )
        {
            setDefaultFont(false);
            auto dc = this->getDeviceContext();
            dc.setDefaultFont();
            DWORD bufferSize = GetFontData(dc.getDcHandle(), 0, 0, NULL, 0);
            auto fontMemory = std::make_shared<gl::Font::Memory>(size_t(bufferSize));
            GetFontData(dc.getDcHandle(), 0, 0, &fontMemory->data[0], bufferSize);
            chkd.scrData->defaultFont = gl::Font::load(fontMemory, 0, 12);
            chkd.scrData->defaultFont->setColor(0.f, 1.f, 1.f);
        }
        this->scrGraphics->setFont(chkd.scrData->defaultFont.get());
    }

    bool isRemastered = skin != Skin::ClassicGDI && skin != Skin::ClassicGL;
    if ( skin != Skin::ClassicGDI )
    {
        // Perform the data load (requires an OpenGL context)
        if ( chkd.scrData->isLoaded(loadSettings) )
        {
            auto fileData = ByteBuffer(4);
            ArchiveCluster archiveCluster {std::vector<ArchiveFilePtr>{}};
            this->scrGraphics->load(*chkd.scrData, loadSettings, archiveCluster, fileData);
            logger.info() << "Switched to skin: " << getSkinName(skin) << '\n';
        }
        else
        {
            logger.info() << "Loading " + std::string(getSkinName(skin)) + " skin...\n";
            auto begin = std::chrono::high_resolution_clock::now();
            bool includesRemastered = false;
            
            std::shared_ptr<ArchiveCluster> archiveCluster = nullptr;
            if ( skin == Skin::ClassicGL ) // ClassicGL relies on chkd.scData loaded earlier and does not require the dat files at this point
                archiveCluster = std::make_shared<ArchiveCluster>(std::vector<ArchiveFilePtr>{}); // TODO: If eliminating GDI, move load of GRPs/tilesets here
            else
            {
                archiveCluster = ChkdDataFileBrowser{}.openScDataFiles(
                    includesRemastered, ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory());

                if ( !includesRemastered )
                {
                    logger.error("Could not locate remastered data files, Chkdraft may be configured against an earlier StarCraft version.");
                    return;
                }
            }

            auto fileData = ByteBuffer(1024*1024*120); // 120MB
            this->scrGraphics->load(*chkd.scrData, loadSettings, *archiveCluster, fileData);
            auto end = std::chrono::high_resolution_clock::now();
            logger.info() << "New skin loaded in " << std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count() << "ms\n";
        }
    }
    else
        logger.info() << "Switched to skin: " << getSkinName(skin) << '\n';

    // Set the maps skin
    this->skin = skin;
    
    UpdateSkinMenuItems();
    windowBoundsChanged();
    this->Redraw(true);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        chkd.terrainPalWindow.RedrawThis();
}

void GuiMap::destroyBrush()
{
    if ( brushAction )
        brushAction = std::nullopt;
}

void GuiMap::refreshTileOccupationCache()
{
    auto newTileOccupationCache = Scenario::getTileOccupationCache(chkd.scData.terrain.get(Scenario::getTileset()), chkd.scData.units);
    this->tileOccupationCache.swap(newTileOccupationCache);
}

void GuiMap::windowBoundsChanged()
{
    RECT cliRect {};
    WindowsItem::getClientRect(cliRect);
    scrGraphics->windowBoundsChanged({
        .left = screenLeft,
        .top = screenTop,
        .right = screenLeft + cliRect.right-cliRect.left,
        .bottom = screenTop + cliRect.bottom-cliRect.top
    });
}

void GuiMap::tileSelectionsChanged()
{
    using neighbor_int = std::underlying_type_t<TileNeighbor>;
    constexpr neighbor_int no_neighbors = neighbor_int(TileNeighbor::None);
    constexpr neighbor_int left_neighbor = neighbor_int(TileNeighbor::Left);
    constexpr neighbor_int top_neighbor = neighbor_int(TileNeighbor::Top);
    constexpr neighbor_int right_neighbor = neighbor_int(TileNeighbor::Right);
    constexpr neighbor_int bottom_neighbor = neighbor_int(TileNeighbor::Bottom);

    std::size_t tileWidth = getTileWidth();
    std::size_t tileHeight = getTileHeight();
    selections.renderTiles.xBegin = std::numeric_limits<std::size_t>::max();
    selections.renderTiles.xEnd = 0;
    selections.renderTiles.yBegin = std::numeric_limits<std::size_t>::max();
    selections.renderTiles.yEnd = 0;
    selections.renderTiles.tiles.assign(tileWidth * tileHeight, std::nullopt);
    const auto & tileSel = view.tiles.sel();
    if ( !tileSel.empty() )
    {
        for ( auto tileIndex : tileSel )
            selections.renderTiles.tiles[tileIndex] = std::make_optional(TileNeighbor::None);

        for ( auto tileIndex : tileSel )
        {
            std::size_t x = tileIndex % tileWidth;
            std::size_t y = tileIndex / tileWidth;
            selections.renderTiles.tiles[tileIndex] = TileNeighbor(
                (y > 0 && !selections.renderTiles.tiles[(y-1)*tileWidth + x] ? top_neighbor : no_neighbors) |
                (y+1 < tileHeight && !selections.renderTiles.tiles[(y+1)*tileWidth + x] ? bottom_neighbor : no_neighbors) |
                (x > 0 && !selections.renderTiles.tiles[y*tileWidth + x - 1] ? left_neighbor : no_neighbors) |
                (x+1 < tileWidth && !selections.renderTiles.tiles[y*tileWidth + x + 1] ? right_neighbor : no_neighbors)
            );
            if ( x < selections.renderTiles.xBegin )
                selections.renderTiles.xBegin = x;
            if ( x > selections.renderTiles.xEnd )
                selections.renderTiles.xEnd = x;
            if ( y < selections.renderTiles.yBegin )
                selections.renderTiles.yBegin = y;
            if ( y > selections.renderTiles.yEnd )
                selections.renderTiles.yEnd = y;
        }

        ++(selections.renderTiles.xEnd);
        ++(selections.renderTiles.yEnd);
    }
}

void GuiMap::tileFogSelectionsChanged()
{
    using neighbor_int = std::underlying_type_t<TileNeighbor>;
    constexpr neighbor_int no_neighbors = neighbor_int(TileNeighbor::None);
    constexpr neighbor_int left_neighbor = neighbor_int(TileNeighbor::Left);
    constexpr neighbor_int top_neighbor = neighbor_int(TileNeighbor::Top);
    constexpr neighbor_int right_neighbor = neighbor_int(TileNeighbor::Right);
    constexpr neighbor_int bottom_neighbor = neighbor_int(TileNeighbor::Bottom);

    std::size_t tileWidth = getTileWidth();
    std::size_t tileHeight = getTileHeight();
    selections.renderFogTiles.xBegin = std::numeric_limits<std::size_t>::max();
    selections.renderFogTiles.xEnd = 0;
    selections.renderFogTiles.yBegin = std::numeric_limits<std::size_t>::max();
    selections.renderFogTiles.yEnd = 0;
    selections.renderFogTiles.tiles.assign(tileWidth * tileHeight, std::nullopt);
    const auto & tileFogSel = view.tileFog.sel();
    if ( !tileFogSel.empty() )
    {
        for ( auto tileIndex : tileFogSel )
            selections.renderFogTiles.tiles[tileIndex] = std::make_optional(TileNeighbor::None);

        for ( auto tileIndex : tileFogSel )
        {
            std::size_t x = tileIndex % tileWidth;
            std::size_t y = tileIndex / tileWidth;
            selections.renderFogTiles.tiles[tileIndex] = TileNeighbor(
                (y > 0 && !selections.renderFogTiles.tiles[(y-1)*tileWidth + x] ? top_neighbor : no_neighbors) |
                (y+1 < tileHeight && !selections.renderFogTiles.tiles[(y+1)*tileWidth + x] ? bottom_neighbor : no_neighbors) |
                (x > 0 && !selections.renderFogTiles.tiles[y*tileWidth + x - 1] ? left_neighbor : no_neighbors) |
                (x+1 < tileWidth && !selections.renderFogTiles.tiles[y*tileWidth + x + 1] ? right_neighbor : no_neighbors)
            );
            if ( x < selections.renderFogTiles.xBegin )
                selections.renderFogTiles.xBegin = x;
            if ( x > selections.renderFogTiles.xEnd )
                selections.renderFogTiles.xEnd = x;
            if ( y < selections.renderFogTiles.yBegin )
                selections.renderFogTiles.yBegin = y;
            if ( y > selections.renderFogTiles.yEnd )
                selections.renderFogTiles.yEnd = y;
        }

        ++(selections.renderFogTiles.xEnd);
        ++(selections.renderFogTiles.yEnd);
    }
}

void GuiMap::checkSelChangeFlags()
{
    if ( clearTileSelChanged() )
        this->tileSelectionsChanged();

    if ( clearFogSelChanged() )
        this->tileFogSelectionsChanged();
}
