#include "maps.h"
#include "chkdraft/chkdraft.h"
#include "mapping/undos/chkd_undos/unit_change.h"
#include "mapping/undos/chkd_undos/doodad_change.h"
#include "mapping/undos/chkd_undos/sprite_change.h"
#include <memory>
#include <string>
#include <utility>

Maps::Maps() : currentlyActiveMap(nullptr), mappingEnabled(false), UntitledNumber(0), lastUsedMapID(0),
    nonStandardCursor(false), currCursor(nullptr), standardCursor(NULL), sizeAllCursor(NULL),
    neswCursor(NULL), nwseCursor(NULL), nsCursor(NULL), weCursor(NULL)
{
    standardCursor = LoadCursor(NULL, IDC_ARROW);
    sizeAllCursor = LoadCursor(NULL, IDC_SIZEALL);
    neswCursor = LoadCursor(NULL, IDC_SIZENESW);
    nwseCursor = LoadCursor(NULL, IDC_SIZENWSE);
    nsCursor = LoadCursor(NULL, IDC_SIZENS);
    weCursor = LoadCursor(NULL, IDC_SIZEWE);
}

Maps::~Maps()
{

}

bool Maps::isInOpenMaps(std::shared_ptr<GuiMap> guiMap)
{
    for ( auto & pair : openMaps )
    {
        if ( guiMap == pair.second )
            return true;
    }
    return false;
}

bool Maps::Focus(HWND hGuiMap)
{
    for ( auto & pair : openMaps )
    {
        if ( hGuiMap == pair.second->getHandle() )
            return Focus(pair.second);
    }
    return false;
}

bool Maps::Focus(std::shared_ptr<GuiMap> guiMap)
{
    if ( guiMap != nullptr && isInOpenMaps(guiMap) )
    {
        currentlyActiveMap = guiMap;
        chkd.mainPlot.leftBar.mainTree.isomTree.UpdateIsomTree();
        chkd.mainPlot.leftBar.mainTree.doodadTree.UpdateDoodadTree();
        chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
        currentlyActiveMap->updateMenu();
        return true;
    }
    else
    {
        currentlyActiveMap = nullptr;
        return false;
    }
}

std::shared_ptr<GuiMap> Maps::GetMap(HWND hGuiMap)
{
    if ( currentlyActiveMap != nullptr && hGuiMap == currentlyActiveMap->getHandle() )
        return currentlyActiveMap;

    for ( auto & pair : openMaps)
    {
        if ( hGuiMap == pair.second->getHandle() )
            return pair.second;
    }

    return nullptr;
}

std::shared_ptr<GuiMap> Maps::GetMap(u16 mapId)
{
    if ( mapId == 0 || mapId == currentlyActiveMap->getMapId() )
        return currentlyActiveMap;

    auto it = openMaps.find(mapId);
    if ( it != openMaps.end() )
        return it->second;
    else
        return nullptr;
}

u16 Maps::GetMapID(std::shared_ptr<GuiMap> guiMap)
{
    if ( guiMap != nullptr )
        return guiMap->getMapId();
    else
        return 0;
}

GuiMapPtr Maps::NewMap(Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, DefaultTriggers defaultTriggers)
{
    if ( width == 0 || height == 0 )
    {
        Error("Invalid dimensions");
        return nullptr;
    }

    auto start = std::chrono::high_resolution_clock::now();
    GuiMapPtr newMap = GuiMapPtr(new GuiMap(clipboard, tileset, width, height, terrainTypeIndex, defaultTriggers));
    if ( newMap != nullptr )
    {
        try {
            AddMap(newMap);
            char title[256] = { "Untitled" };
            if ( UntitledNumber > 0 )
                std::snprintf(title, 256, "Untitled %d", UntitledNumber);
    
            if ( newMap->CreateThis(getHandle(), title) )
            {
                UntitledNumber++;
                Focus(newMap);
                EnableMapping();
                currentlyActiveMap->refreshScenario();
                currentlyActiveMap->Scroll(true, true, false);
                currentlyActiveMap->Redraw(true);
            
                auto finish = std::chrono::high_resolution_clock::now();
                logger.info() << "New map [ID:" << newMap->getMapId() << "] created in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                return newMap;
            }
            else
                Error("Failed to create MDI Child Window!");
        } catch ( AllMapIdsExausted & e ) {
            Error(e.what());
        } catch ( std::exception & e ) {
            Error(e.what());
        }
    }
    else
        Error("Failed to create new map!");

    RemoveMap(newMap);
    return nullptr;
}

bool Maps::OpenMap(const std::string & fileName)
{
    auto newMap = GuiMapPtr(new GuiMap(clipboard, fileName));
    if ( newMap != nullptr && !newMap->empty() )
    {
        try {
            AddMap(newMap);
            if ( newMap->CreateThis(getHandle(), fileName) )
            {
                newMap->SetWinText(fileName);
                Focus(newMap);
                EnableMapping();

                if ( newMap->isProtected() && newMap->hasPassword() )
                    chkd.enterPasswordWindow.CreateThis(chkd.getHandle());
                else if ( newMap->isProtected() )
                    mb("Map is protected and will be opened as view only");

                SetFocus(chkd.getHandle());
                currentlyActiveMap->Scroll(true, true, false);
                currentlyActiveMap->Redraw(true);
                currentlyActiveMap->refreshScenario();
                logger.info() << "Initialized map [ID:" << newMap->getMapId() << "] from " << newMap->getFilePath() << std::endl;
                return true;
            } 
            else
                Error("Failed to create MDI Child Window!");
        } catch ( AllMapIdsExausted & e ) {
            Error(e.what());
        }
    }
    else
        Error("Failed to open map!");

    RemoveMap(newMap);
    return false;
}

bool Maps::OpenMap(FileBrowserPtr<SaveType> fileBrowser)
{
    std::string browseFilePath {};
    SaveType saveType = SaveType::Unknown;
    if ( fileBrowser->browseForOpenPath(browseFilePath, saveType) )
        return OpenMap(browseFilePath);
    else
        logger.info() << "Map selection cancelled" << std::endl;

    return false;
}

bool Maps::SaveCurr(bool saveAs)
{
    if ( currentlyActiveMap->SaveFile(saveAs) )
    {
        currentlyActiveMap->SetWinText(currentlyActiveMap->getFilePath());
        if ( saveAs )
            currentlyActiveMap->refreshScenario();

        return true;
    }
    else
        return false;
}

void Maps::CloseMap(HWND hMap)
{
    std::shared_ptr<GuiMap> map = GetMap(hMap);
    if ( map != nullptr )
        RemoveMap(map);

    if ( openMaps.size() == 0 )
        DisableMapping();
}

bool Maps::CloseAll()
{
    while ( CM != nullptr )
    {
        auto mapId = CM->getMapId();
        if ( CM->CanExit() )
        {
            SendMessage(MdiClient::getHandle(), WM_MDIDESTROY, (WPARAM)CM->getHandle(), 0);
            openMaps.erase(mapId);
            CM = openMaps.empty() ? nullptr : openMaps.begin()->second;
        }
        else
            return false; // Abort close
    }
    return true;
}

void Maps::CloseActive()
{
    if ( currentlyActiveMap != nullptr )
    {
        if ( currentlyActiveMap->CanExit() )
            SendMessage(MdiClient::getHandle(), WM_MDIDESTROY, (WPARAM)MdiClient::getActive(), 0);
    }
}

void Maps::UpdateTreeView()
{
    if ( currentlyActiveMap != nullptr )
    {

    }
}

void Maps::SetGrid(s16 xSize, s16 ySize)
{
    if ( currentlyActiveMap != nullptr )
        currentlyActiveMap->SetGridSize(xSize, ySize);
}

void Maps::ChangeLayer(Layer newLayer)
{
    if ( currentlyActiveMap != nullptr && currentlyActiveMap->getLayer() != newLayer )
    {
        currentlyActiveMap->clearSelectedTiles();

        if ( chkd.mainToolbar.layerBox.GetSel() != (int)newLayer )
            chkd.mainToolbar.layerBox.SetSel((int)newLayer);

        if ( newLayer == Layer::FogEdit || newLayer == Layer::Units || newLayer == Layer::Sprites ||
             newLayer == Layer::FogView || newLayer == Layer::Doodads || newLayer == Layer::CutCopyPaste )
            // Layers where player#'s are relevant
        {
            ChangePlayer(currentlyActiveMap->getCurrPlayer(), false);
            ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_SHOW);
        }
        else // Layers where player#'s are irrelevant
        {
            chkd.statusBar.SetText(2, "");
            ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
        }

        if ( newLayer == Layer::Terrain )
            ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_SHOW);
        else
            ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

        if ( newLayer == Layer::Doodads )
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CONVERTTOTERRAIN, MF_ENABLED);
        else
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CONVERTTOTERRAIN, MF_DISABLED);

        if ( newLayer == Layer::Units || newLayer == Layer::Sprites )
        {
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_STACKSELECTED, MF_ENABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATELOCATION, MF_ENABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATEINVERTEDLOCATION, MF_ENABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATEMOBILEINVERTEDLOCATION, MF_ENABLED);
        }
        else
        {
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_STACKSELECTED, MF_DISABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATELOCATION, MF_DISABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATEINVERTEDLOCATION, MF_DISABLED);
            EnableMenuItem(chkd.mainMenu.getHandle(), ID_EDIT_CREATEMOBILEINVERTEDLOCATION, MF_DISABLED);
        }

        if ( newLayer == Layer::FogEdit )
        {
            chkd.mainToolbar.brushWidth.Show();
            chkd.mainToolbar.brushHeight.Show();
        }
        else
        {
            chkd.mainToolbar.brushWidth.Hide();
            chkd.mainToolbar.brushHeight.Hide();
        }

        if ( newLayer == Layer::CutCopyPaste )
        {
            chkd.mainToolbar.checkTerrain.Show();
            chkd.mainToolbar.checkDoodads.Show();
            chkd.mainToolbar.checkSprites.Show();
            chkd.mainToolbar.checkUnits.Show();
            chkd.mainToolbar.checkFog.Show();
        }
        else
        {
            chkd.mainToolbar.checkTerrain.Hide();
            chkd.mainToolbar.checkDoodads.Hide();
            chkd.mainToolbar.checkSprites.Hide();
            chkd.mainToolbar.checkUnits.Hide();
            chkd.mainToolbar.checkFog.Hide();
        }

        currentlyActiveMap->setLayer(newLayer);

        chkd.tilePropWindow.DestroyThis();
        currentlyActiveMap->Redraw(false);
        std::string layerString;
        if ( chkd.mainToolbar.layerBox.GetItemText((int)newLayer, layerString) )
            chkd.statusBar.SetText(1, layerString);
        
        UpdatePlayerStatus();
    }
}

void Maps::ChangeSubLayer(TerrainSubLayer newSubLayer)
{
    if ( currentlyActiveMap != nullptr )
    {
        ChangeLayer(Layer::Terrain);
        if ( currentlyActiveMap->getSubLayer() != newSubLayer )
        {
            currentlyActiveMap->setSubLayer(newSubLayer);

            if ( newSubLayer == TerrainSubLayer::Isom && chkd.mainToolbar.terrainBox.GetSel() != 0 )
                chkd.mainToolbar.terrainBox.SetSel(0); // Isometrical
            else if ( newSubLayer == TerrainSubLayer::Rectangular && chkd.mainToolbar.terrainBox.GetSel() != 1 )
                chkd.mainToolbar.terrainBox.SetSel(1); // Rectangular
        }
    }
}

void Maps::ChangeZoom(bool increment)
{
    double zoom = currentlyActiveMap->getZoom();

    if ( increment )
    {
        for ( int i=1; i<10; i++ )
        {
            if ( zoom == defaultZooms[i] )
            {
                currentlyActiveMap->setZoom(defaultZooms[i-1]);
                chkd.mainToolbar.zoomBox.SetSel(i-1);
                break;
            }
        }
    }
    else // decrement
    {
        for ( int i=0; i<9; i++ )
        {
            if ( zoom == defaultZooms[i] )
            {
                currentlyActiveMap->setZoom(defaultZooms[i+1]);
                chkd.mainToolbar.zoomBox.SetSel(i+1);
                break;
            }
        }
    }
}

void Maps::ChangePlayer(u8 newPlayer, bool updateMapPlayers)
{
    currentlyActiveMap->setCurrPlayer(newPlayer);

    if ( updateMapPlayers )
    {
        if ( currentlyActiveMap->getLayer() == Layer::Units )
        {
            if ( clipboard.isPasting() )
            {
                auto & units = clipboard.getUnits();
                for ( auto & pasteUnit : units )
                    pasteUnit.unit.owner = newPlayer;
            }

            currentlyActiveMap->PlayerChanged(newPlayer);
        }
        else if ( currentlyActiveMap->getLayer() == Layer::Doodads )
        {
            if ( clipboard.isPasting() )
            {
                auto & doodads = clipboard.getDoodads();
                for ( auto & pasteDoodad : doodads )
                    pasteDoodad.owner = newPlayer;
            }
            else if ( currentlyActiveMap->selections.hasDoodads() )
            {
                const auto & tileset = chkd.scData.terrain.get(currentlyActiveMap->getTileset());
                for ( auto doodadIndex : currentlyActiveMap->view.doodads.sel() )
                {
                    const auto & selDoodad = currentlyActiveMap->getDoodad(doodadIndex);
                    if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(selDoodad.type) )
                    {
                        const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
                        if ( selDoodad.owner != newPlayer )
                        {
                            currentlyActiveMap->operator()()->doodads[doodadIndex].owner = newPlayer;
                            if ( !currentlyActiveMap->read.sprites.empty() )
                            {
                                for ( int i=int(currentlyActiveMap->read.sprites.size())-1; i>=0; --i )
                                {
                                    const auto & sprite = currentlyActiveMap->read.sprites[i];
                                    if ( sprite.type == doodadDat.overlayIndex && sprite.xc == selDoodad.xc && sprite.yc == selDoodad.yc )
                                        currentlyActiveMap->operator()()->sprites[i].owner = newPlayer;
                                }
                            }
                        }
                    }
                }
            }
            else if ( currentlyActiveMap->selections.hasSprites() )
            {
                if ( clipboard.isPasting() )
                {
                    auto & sprites = clipboard.getSprites();
                    for ( auto & pasteSprite : sprites )
                        pasteSprite.sprite.owner = newPlayer;
                }
            }

            currentlyActiveMap->PlayerChanged(newPlayer);
        }
        else if ( currentlyActiveMap->getLayer() == Layer::Sprites )
        {
            if ( clipboard.isPasting() )
            {
                auto & sprites = clipboard.getSprites();
                for ( auto & pasteSprite : sprites )
                    pasteSprite.sprite.owner = newPlayer;
            }

            currentlyActiveMap->PlayerChanged(newPlayer);
        }
    }
    UpdatePlayerStatus();
}

void Maps::UpdatePlayerStatus()
{
    if ( CM->getLayer() == Layer::Terrain || CM->getLayer() == Layer::Locations )
    {
        chkd.statusBar.SetText(2, "");
        chkd.statusBar.SetText(3, "");
        return;
    }

    char color[32] {}, race[32] {}, playerText[64] {}, forceText[32] {};
    auto slotIndex = CM->getCurrPlayer();
    if ( slotIndex < Sc::Player::TotalSlots )
    {
        switch ( CM->getPlayerColor(CM->getCurrPlayer()) )
        {
            case Chk::PlayerColor::Red: std::snprintf(color, sizeof(color), "Red"); break;
            case Chk::PlayerColor::Blue: std::snprintf(color, sizeof(color), "Blue"); break;
            case Chk::PlayerColor::Teal: std::snprintf(color, sizeof(color), "Teal"); break;
            case Chk::PlayerColor::Purple: std::snprintf(color, sizeof(color), "Purple"); break;
            case Chk::PlayerColor::Orange: std::snprintf(color, sizeof(color), "Orange"); break;
            case Chk::PlayerColor::Brown: std::snprintf(color, sizeof(color), "Brown"); break;
            case Chk::PlayerColor::White: std::snprintf(color, sizeof(color), "White"); break;
            case Chk::PlayerColor::Yellow: std::snprintf(color, sizeof(color), "Yellow"); break;
            case Chk::PlayerColor::Green: std::snprintf(color, sizeof(color), "Green"); break;
            case Chk::PlayerColor::PaleYellow: std::snprintf(color, sizeof(color), "PaleYellow"); break;
            case Chk::PlayerColor::Tan: std::snprintf(color, sizeof(color), "Tan"); break;
            case Chk::PlayerColor::Azure_NeutralColor: std::snprintf(color, sizeof(color), "Azure"); break;
            default: std::snprintf(color, sizeof(color), "CustomColor"); break;
        }
        switch ( CM->getPlayerForce(CM->getCurrPlayer()) )
        {
            case Chk::Force::Force1: std::snprintf(forceText, sizeof(forceText), "Force 1"); break;
            case Chk::Force::Force2: std::snprintf(forceText, sizeof(forceText), "Force 2"); break;
            case Chk::Force::Force3: std::snprintf(forceText, sizeof(forceText), "Force 3"); break;
            case Chk::Force::Force4: std::snprintf(forceText, sizeof(forceText), "Force 4"); break;
        }
    }
    switch ( CM->getPlayerRace(CM->getCurrPlayer()) )
    {
        case Chk::Race::Zerg: std::snprintf(race, sizeof(race), "Zerg"); break;
        case Chk::Race::Terran: std::snprintf(race, sizeof(race), "Terran"); break;
        case Chk::Race::Protoss: std::snprintf(race, sizeof(race), "Protoss"); break;
        case Chk::Race::Independent: std::snprintf(race, sizeof(race), "Independent"); break;
        case Chk::Race::Neutral: std::snprintf(race, sizeof(race), "Neutral"); break;
        case Chk::Race::UserSelectable: std::snprintf(race, sizeof(race), "UserSelect"); break;
        case Chk::Race::Random: std::snprintf(race, sizeof(race), "Random"); break;
        case Chk::Race::Inactive: std::snprintf(race, sizeof(race), "Inactive"); break;
        default: std::snprintf(race, sizeof(race), "UnknownRace"); break;
    }
    std::snprintf(playerText, sizeof(playerText), "Player %i: %s %s", currentlyActiveMap->getCurrPlayer() + 1, color, race);
    chkd.statusBar.SetText(2, playerText);
    chkd.statusBar.SetText(3, forceText);
}

bool Maps::toggleCutCopyPasteTerrain()
{
    if ( currentlyActiveMap != nullptr )
    {
        currentlyActiveMap->toggleCutCopyPasteTerrain();
        return currentlyActiveMap->getCutCopyPasteTerrain();
    }
    return false;
}

bool Maps::toggleCutCopyPasteDoodads()
{
    if ( currentlyActiveMap != nullptr )
    {
        currentlyActiveMap->toggleCutCopyPasteDoodads();
        return currentlyActiveMap->getCutCopyPasteDoodads();
    }
    return false;
}

bool Maps::toggleCutCopyPasteSprites()
{
    if ( currentlyActiveMap != nullptr )
    {
        currentlyActiveMap->toggleCutCopyPasteSprites();
        return currentlyActiveMap->getCutCopyPasteSprites();
    }
    return false;
}

bool Maps::toggleCutCopyPasteUnits()
{
    if ( currentlyActiveMap != nullptr )
    {
        currentlyActiveMap->toggleCutCopyPasteUnits();
        return currentlyActiveMap->getCutCopyPasteUnits();
    }
    return false;
}

bool Maps::toggleCutCopyPasteFog()
{
    if ( currentlyActiveMap != nullptr )
    {
        currentlyActiveMap->toggleCutCopyPasteFog();
        return currentlyActiveMap->getCutCopyPasteFog();
    }
    return false;
}

void Maps::cut()
{
    if ( currentlyActiveMap != nullptr )
    {
        if ( currentlyActiveMap->isProtected() )
            Error("Cannot copy from protected maps!");
        else
        {
            clipboard.copy(*currentlyActiveMap, currentlyActiveMap->getLayer());
            currentlyActiveMap->deleteSelection();
            if ( clipboard.isPasting() )
            {
                endPaste();
                RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);
            }
            ClipCursor(NULL);
        }
    }
}

void Maps::copy()
{
    if ( currentlyActiveMap != nullptr )
    {
        if ( currentlyActiveMap->isProtected() )
            Error("Cannot copy from protected maps!");
        else
        {
            clipboard.copy(*currentlyActiveMap, currentlyActiveMap->getLayer());
            if ( clipboard.isPasting() )
            {
                endPaste();
                RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);
            }
            ClipCursor(NULL);
        }
    }
}

void Maps::SetGridColor(u8 red, u8 green, u8 blue)
{
    if ( currentlyActiveMap != nullptr )
        currentlyActiveMap->SetGridColor(red, green, blue);
}

void Maps::startPaste(bool isQuickPaste)
{
    if ( currentlyActiveMap == nullptr )
        return;
    else if ( currentlyActiveMap->getLayer() == Layer::Terrain )
    {
        if ( clipboard.hasTiles() || clipboard.hasQuickTiles() )
        {
            currentlyActiveMap->clearSelectedTiles();
            clipboard.beginPasting(isQuickPaste);

            RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER;
            tme.hwndTrack = currentlyActiveMap->getHandle();
            tme.dwHoverTime = defaultHoverTime;
            TrackMouseEvent(&tme);
        }
    }
    else if ( currentlyActiveMap->getLayer() == Layer::Doodads )
    {
        if ( clipboard.hasDoodads() )
        {
            currentlyActiveMap->clearSelectedDoodads();
            clipboard.beginPasting(isQuickPaste);

            RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER;
            tme.hwndTrack = currentlyActiveMap->getHandle();
            tme.dwHoverTime = defaultHoverTime;
            TrackMouseEvent(&tme);
        }
    }
    else if ( currentlyActiveMap->getLayer() == Layer::Units )
    {
        if ( clipboard.hasUnits() || clipboard.hasQuickUnits() )
        {
            currentlyActiveMap->clearSelectedUnits();
            clipboard.beginPasting(isQuickPaste);

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER;
            tme.hwndTrack = currentlyActiveMap->getHandle();
            tme.dwHoverTime = defaultHoverTime;
            TrackMouseEvent(&tme);
        }
    }
    else if ( currentlyActiveMap->getLayer() == Layer::Sprites )
    {
        if ( clipboard.hasSprites() || clipboard.hasQuickSprites() )
        {
            currentlyActiveMap->clearSelectedSprites();
            clipboard.beginPasting(isQuickPaste);

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER;
            tme.hwndTrack = currentlyActiveMap->getHandle();
            tme.dwHoverTime = defaultHoverTime;
            TrackMouseEvent(&tme);
        }
    }
    else if ( currentlyActiveMap->getLayer() == Layer::CutCopyPaste && !isQuickPaste )
    {
        currentlyActiveMap->clearSelection();
        clipboard.beginPasting(false);
        RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);

        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_HOVER;
        tme.hwndTrack = currentlyActiveMap->getHandle();
        tme.dwHoverTime = defaultHoverTime;
        TrackMouseEvent(&tme);
    }
}

void Maps::endPaste()
{
    clipboard.endPasting();
    if ( currentlyActiveMap != nullptr )
        currentlyActiveMap->Redraw(false);
}

void Maps::properties()
{
    auto edit = currentlyActiveMap->operator()();
    if ( currentlyActiveMap->getLayer() == Layer::Terrain )
    {
        Selections & selections = currentlyActiveMap->selections;
        if ( selections.hasTiles() )
        {
            auto numSelected = currentlyActiveMap->view.tiles.sel().size();
            if ( numSelected > 1 )
            {
                std::vector<std::size_t> indexesRemoved(numSelected-1, 0);
                std::iota(indexesRemoved.begin(), indexesRemoved.end(), 1);
                edit->tiles.deselect(indexesRemoved);
            }

            RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);
            if ( chkd.tilePropWindow.getHandle() != NULL )
                chkd.tilePropWindow.UpdateTile();
            else
                chkd.tilePropWindow.CreateThis(chkd.getHandle());
            ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
        }
    }
    else
        CM->ReturnKeyPress();
}

void Maps::stickCursor()
{
    if ( nonStandardCursor && currCursor != nullptr )
        SetCursor(*currCursor);
}

void Maps::updateCursor(s32 xc, s32 yc)
{
    Selections & selections = currentlyActiveMap->selections;
    if ( currentlyActiveMap->getLayer() == Layer::Locations )
    {
        u16 selectedLocation = selections.getSelectedLocation();
        if ( selectedLocation != NO_LOCATION && selectedLocation < currentlyActiveMap->numLocations() )
        {
            const Chk::Location & loc = currentlyActiveMap->getLocation(selectedLocation);
            s32 locationLeft = std::min(loc.left, loc.right),
                locationRight = std::max(loc.left, loc.right),
                locationTop = std::min(loc.top, loc.bottom),
                locationBottom = std::max(loc.top, loc.bottom),
                leftOuterBound = locationLeft-5,
                rightOuterBound = locationRight+5,
                topOuterBound = locationTop-5,
                bottomOuterBound = locationBottom+5;

            if ( xc >= leftOuterBound && xc <= rightOuterBound &&
                 yc >= topOuterBound && yc <= bottomOuterBound )
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
                     yc >= topInnerBound && yc <= bottomInnerBound )
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
                else if ( ( xc < leftInnerBound && yc < topInnerBound ) ||
                          ( xc > rightInnerBound && yc > bottomInnerBound ) )
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
    else if ( nonStandardCursor )
        SetCursor(standardCursor);
}

void Maps::releaseRenderContext(std::shared_ptr<WinLib::DeviceContext> & deviceContext)
{
    if ( openGlRenderContext && deviceContext != nullptr )
        openGlRenderContext->releaseDeviceContext(deviceContext);
}

void Maps::setGlRenderTarget(std::shared_ptr<WinLib::DeviceContext> & deviceContext, WinLib::WindowsItem & windowsItem)
{
    if ( !deviceContext )
    {
        deviceContext = std::make_shared<WinLib::DeviceContext>(windowsItem.getHandle());
        deviceContext->useOpenGlPixelFormat();
    }

    if ( openGlRenderContext )
        openGlRenderContext->setDeviceContext(deviceContext);
    else
        openGlRenderContext.emplace(deviceContext);
}

gl::ContextSemaphore* Maps::getContextSemaphore()
{
    return &openGlRenderContext.value();
}

u16 Maps::NextId()
{
    if ( lastUsedMapID < 65535 )
    {
        lastUsedMapID++;
        return lastUsedMapID;
    }
    else // Find one that isn't used, else throw an exception
    {
        for ( u32 mapId = 0; mapId < 65536; mapId++ ) // Try every ID, break if you find one
        {
            auto it = openMaps.find(mapId);
            if ( it == openMaps.end() ) // mapId is unused
                return mapId;
        }
        throw AllMapIdsExausted();
    }
}

void Maps::EnableMapping()
{
    if ( !mappingEnabled ) // Enable mapping functionality
    {
        mappingEnabled = true;

        int toolbarItems[] =
        {
            ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
            ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
        };

        for ( int i = 0; i < sizeof(toolbarItems) / sizeof(int); i++ )
            SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], true);

        ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_SHOW);
        ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_SHOW);
        ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_SHOW);

        HMENU hMenu = GetMenu(chkd.getHandle());

        for ( auto item : onOffMenuItems )
        {
            if ( item != ID_EDIT_CONVERTTOTERRAIN && item != ID_EDIT_STACKSELECTED && item != ID_EDIT_CREATELOCATION &&
                 item != ID_EDIT_CREATEINVERTEDLOCATION && item != ID_EDIT_CREATEMOBILEINVERTEDLOCATION )
                EnableMenuItem(hMenu, item, MF_ENABLED);
        }

        HWND hLeftBar = chkd.mainPlot.leftBar.getHandle();
        ShowWindow(hLeftBar, SW_SHOW);
        
        std::string layerString;
        if ( chkd.mainToolbar.layerBox.GetItemText((int)CM->getLayer(), layerString) )
            chkd.statusBar.SetText(1, layerString);

        UpdatePlayerStatus();
    }
}

void Maps::DisableMapping()
{
    if ( mappingEnabled )
    {
        mappingEnabled = false;
        
        chkd.unitWindow.DestroyThis();
        chkd.spriteWindow.DestroyThis();
        chkd.locationWindow.DestroyThis();
        chkd.terrainPalWindow.DestroyThis();
        chkd.tilePropWindow.DestroyThis();
        chkd.textTrigWindow.DestroyThis();
        chkd.mapSettingsWindow.DestroyThis();
        chkd.trigEditorWindow.DestroyThis();
        chkd.briefingTrigEditorWindow.DestroyThis();
        chkd.changePasswordWindow.DestroyThis();
        chkd.enterPasswordWindow.DestroyThis();
        chkd.dimensionsWindow.DestroyThis();

        int toolbarItems[] =
        {
            ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
            ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
        };

        for ( int i = 0; i < sizeof(toolbarItems) / sizeof(int); i++ )
            SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], false);

        chkd.mainToolbar.layerBox.SetSel(0);
        chkd.mainToolbar.zoomBox.SetSel(4);
        chkd.mainToolbar.playerBox.SetSel(0);
        chkd.mainToolbar.terrainBox.SetSel(0);
        ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

        HMENU hMenu = GetMenu(chkd.getHandle());

        for (auto item : onOffMenuItems)
            EnableMenuItem(hMenu, item, MF_DISABLED);

        EnableMenuItem(hMenu, ID_EDIT_CONVERTTOTERRAIN, MF_DISABLED);
        EnableMenuItem(hMenu, ID_EDIT_STACKSELECTED, MF_DISABLED);
        EnableMenuItem(hMenu, ID_EDIT_CREATELOCATION, MF_DISABLED);
        EnableMenuItem(hMenu, ID_EDIT_CREATEINVERTEDLOCATION, MF_DISABLED);
        EnableMenuItem(hMenu, ID_EDIT_CREATEMOBILEINVERTEDLOCATION, MF_DISABLED);

        chkd.mainPlot.leftBar.Hide();

        chkd.statusBar.SetText(0, "");
        chkd.statusBar.SetText(1, "");
        chkd.statusBar.SetText(2, "");
        chkd.statusBar.SetText(3, "");

        chkd.changePasswordWindow.Hide();
    }
}

void Maps::AddMap(GuiMapPtr map)
{
    if ( map != nullptr )
    {
        u16 id = NextId();
        auto it = openMaps.insert(std::pair(id, map));
        if ( it != openMaps.end() )
            it->second->setMapId(id);
    }
}

bool Maps::RemoveMap(std::shared_ptr<GuiMap> guiMap)
{
    if ( guiMap == nullptr )
        return false;

    u16 toDelete = u16_max;
    for ( auto & pair : openMaps )
    {
        if ( guiMap == pair.second )
        {
            toDelete = pair.first;
            break;
        }
    }

    if ( toDelete != u16_max )
    {
        if ( guiMap == currentlyActiveMap )
            currentlyActiveMap = nullptr;

        std::string mapFilePath = guiMap->getFilePath();
        openMaps.erase(toDelete);
        logger.info() << "Closed map [ID:" << toDelete << "] " << (mapFilePath.empty() ? "(Untitled)" : mapFilePath.c_str()) << std::endl;
        return true;
    }

    return false;
}
