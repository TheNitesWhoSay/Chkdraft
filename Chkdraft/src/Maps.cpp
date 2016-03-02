#include "Maps.h"
#include "Chkdraft.h"
#include "UnitChange.h"
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
    for ( auto &pair : openMaps )
    {
        if ( guiMap == pair.second )
            return true;
    }
    return false;
}

bool Maps::Focus(HWND hGuiMap)
{
    for ( auto &pair : openMaps )
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
    if ( hGuiMap == currentlyActiveMap->getHandle() )
        return currentlyActiveMap;

    for ( auto &pair : openMaps)
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

bool Maps::NewMap(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers)
{
    if ( width == 0 || height == 0 )
    {
        Error("Invalid dimensions");
        return false;
    }

    std::shared_ptr<GuiMap> newMap = AddEmptyMap();

    if ( newMap->CreateNew(width, height, tileset, terrain, triggers) )
    {
        char title[256] = { "Untitled" };
        if ( UntitledNumber > 0 )
            std::snprintf(title, 256, "Untitled %d", UntitledNumber);

        if ( newMap->CreateThis(getHandle(), title) )
        {
            UntitledNumber++;
            EnableMapping();
            Focus(newMap);
            currentlyActiveMap->Redraw(true);
            return true;
        }
        else
            Error("Failed to create MDI Child Window!");
    }
    else
    {
        CHKD_ERR("Failed to create map!\n\nError in %s\n\n%s", LastErrorLoc, LastError);
        Error(LastError);
    }

    RemoveMap(newMap);
    return false;
}

bool Maps::OpenMap(const char* fileName)
{
    auto newMap = AddEmptyMap();

    if ( newMap->LoadFile(fileName) )
    {
        if ( newMap->CreateThis(getHandle(), fileName) )
        {
            SetWindowText(newMap->getHandle(), fileName);
            EnableMapping();
            Focus(newMap);

            if ( newMap->isProtected() && newMap->hasPassword() )
                chkd.enterPasswordWindow.CreateThis(chkd.getHandle());
            else if ( newMap->isProtected() )
                mb("Map is protected and will be opened as view only");

            SetFocus(chkd.getHandle());
            currentlyActiveMap->Scroll(true, true, false);
            currentlyActiveMap->Redraw(true);
            currentlyActiveMap->refreshScenario();
            return true;
        } 
        else
            Error("Failed to create MDI Child Window!");
    }

    RemoveMap(newMap);
    return false;
}

bool Maps::OpenMap()
{
    return OpenMap(nullptr);
}

bool Maps::SaveCurr(bool saveAs)
{
    if ( currentlyActiveMap->SaveFile(saveAs) )
    {
        SetWindowText(currentlyActiveMap->getHandle(), currentlyActiveMap->FilePath());
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

void Maps::CloseActive()
{
    SendMessage(MdiClient::getHandle(), WM_MDIDESTROY, (WPARAM)MdiClient::getActive(), 0);
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

        if ( newLayer == Layer::FogEdit || newLayer == Layer::Units ||
             newLayer == Layer::Sprites || newLayer == Layer::FogView )
            // Layers where player#'s are relevant
        {
            ChangePlayer(currentlyActiveMap->getCurrPlayer());
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

        currentlyActiveMap->setLayer(newLayer);

        chkd.tilePropWindow.DestroyThis();
        currentlyActiveMap->Redraw(false);
        std::string layerString;
        if ( chkd.mainToolbar.layerBox.GetItemText((int)newLayer, layerString) )
            chkd.statusBar.SetText(1, layerString.c_str());
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

void Maps::ChangePlayer(u8 newPlayer)
{
    currentlyActiveMap->setCurrPlayer(newPlayer);

    if ( currentlyActiveMap->getLayer() == Layer::Units )
    {
        if ( clipboard.isPasting() )
        {
            auto &units = clipboard.getUnits();
            for ( auto &pasteUnit : units )
                pasteUnit.unit.owner = newPlayer;
        }

        u16 numUnits = currentlyActiveMap->numUnits();
        currentlyActiveMap->PlayerChanged(newPlayer);
    }

    char color[32], race[32], playerText[64];
    std::snprintf(color, sizeof(color), "Red");
    std::snprintf(race, sizeof(race), "Terran");
    std::snprintf(playerText, sizeof(playerText), "Player %i: %s %s", currentlyActiveMap->getCurrPlayer() + 1, color, race);
    chkd.statusBar.SetText(2, playerText);
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
}

void Maps::endPaste()
{
    clipboard.endPasting();
    if ( currentlyActiveMap != nullptr )
        currentlyActiveMap->Redraw(false);
}

void Maps::properties()
{
    if ( currentlyActiveMap->getLayer() == Layer::Terrain )
    {
        Selections &selections = currentlyActiveMap->GetSelections();
        if ( selections.hasTiles() )
        {
            TileNode tile = selections.getFirstTile();
            selections.removeTiles();
            selections.addTile(tile.value, tile.xc, tile.yc, TileNeighbor::All);

            RedrawWindow(currentlyActiveMap->getHandle(), NULL, NULL, RDW_INVALIDATE);
            if ( chkd.tilePropWindow.getHandle() != NULL )
                chkd.tilePropWindow.UpdateTile();
            else
                chkd.tilePropWindow.CreateThis(chkd.getHandle());
            ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
        }
    }
}

void Maps::stickCursor()
{
    if ( nonStandardCursor && currCursor != nullptr )
        SetCursor(*currCursor);
}

void Maps::updateCursor(s32 xc, s32 yc)
{
    Selections &selections = currentlyActiveMap->GetSelections();
    if ( currentlyActiveMap->getLayer() == Layer::Locations )
    {
        u16 selectedLocation = selections.getSelectedLocation();
        if ( selectedLocation != NO_LOCATION )
        {
            ChkLocation* loc;
            if ( currentlyActiveMap->getLocation(loc, selectedLocation) )
            {
                s32 locationLeft = std::min(loc->xc1, loc->xc2),
                    locationRight = std::max(loc->xc1, loc->xc2),
                    locationTop = std::min(loc->yc1, loc->yc2),
                    locationBottom = std::max(loc->yc1, loc->yc2),
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

                    if ( xc >= leftInnerBound && xc <= rightInnerBound &&
                         yc >= topInnerBound && yc <= bottomInnerBound    )
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
    }
    else if ( nonStandardCursor )
        SetCursor(standardCursor);
}

u16 Maps::NextId()
{
    if ( lastUsedMapID < 65535 )
    {
        lastUsedMapID++;
        return lastUsedMapID;
    }
    else // Find one that isn't used, ambiguously use 65535 if all are used
    {
        u16 newMapId = 65535; // Use 65535 if all are used
        for ( u32 mapId = 0; mapId < 65536; mapId++ ) // Try every ID, break if you find one
        {
            bool used = false; // Assume it's not used until you find that it is
            auto it = openMaps.find(mapId);
            if ( it == openMaps.end() ) // mapId is unused
            {
                newMapId = mapId;
                break; // Use the current value of newMapId
            }
        }
        return newMapId; // Use the unused ID or 65535
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

        for (auto item : onOffMenuItems)
            EnableMenuItem(hMenu, item, MF_ENABLED);

        HWND hLeftBar = chkd.mainPlot.leftBar.getHandle();
        ShowWindow(hLeftBar, SW_SHOW);

        chkd.statusBar.SetText(1, "Terrain");
    }
}

void Maps::DisableMapping()
{
    if ( mappingEnabled )
    {
        mappingEnabled = false;

        int toolbarItems[] =
        {
            ID_FILE_SAVE1, ID_FILE_SAVEAS1, ID_EDIT_UNDO1, ID_EDIT_REDO1, ID_EDIT_PROPERTIES,
            ID_EDIT_DELETE, ID_EDIT_COPY1, ID_EDIT_CUT1, ID_EDIT_PASTE1
        };

        for ( int i = 0; i < sizeof(toolbarItems) / sizeof(int); i++ )
            SendMessage(chkd.mainToolbar.getHandle(), TB_ENABLEBUTTON, toolbarItems[i], false);

        ShowWindow(chkd.mainToolbar.layerBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.zoomBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.playerBox.getHandle(), SW_HIDE);
        ShowWindow(chkd.mainToolbar.terrainBox.getHandle(), SW_HIDE);

        HMENU hMenu = GetMenu(chkd.getHandle());

        for (auto item : onOffMenuItems)
            EnableMenuItem(hMenu, item, MF_DISABLED);

        chkd.mainPlot.leftBar.Hide();

        chkd.statusBar.SetText(0, "");
        chkd.statusBar.SetText(1, "");

        chkd.changePasswordWindow.Hide();
    }
}

std::shared_ptr<GuiMap> Maps::AddEmptyMap()
{
    u16 id = NextId();
    if ( id < u16_max )
    {
        auto it = openMaps.insert(std::pair<u16, std::shared_ptr<GuiMap>>(id, std::shared_ptr<GuiMap>(new GuiMap(clipboard))));
        if ( it != openMaps.end() )
        {
            it->second->setMapId(id);
            return it->second;
        }
    }
    return nullptr;
}

bool Maps::RemoveMap(std::shared_ptr<GuiMap> guiMap)
{
    if ( guiMap == nullptr )
        return false;

    u16 toDelete = u16_max;
    for ( auto &pair : openMaps )
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

        openMaps.erase(toDelete);
        return true;
    }

    return false;
}
