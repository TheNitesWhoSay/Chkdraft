#include "GuiMap.h"
#include "../../Chkdraft.h"
#include "../../Mapping/Undos/ChkdUndos/TileChange.h"
#include "../../Mapping/Undos/ChkdUndos/UnitChange.h"
#include "../../Mapping/Undos/ChkdUndos/UnitCreateDel.h"
#include "../../Mapping/Undos/ChkdUndos/LocationCreateDel.h"
#include "../../Mapping/Undos/ChkdUndos/LocationMove.h"
#include "../../Mapping/Undos/ChkdUndos/LocationChange.h"
#include "../../../CommanderLib/Logger.h"

bool GuiMap::doAutoBackups = false;

GuiMap::GuiMap(Clipboard &clipboard) : clipboard(clipboard), selections(*this), graphics(*this, selections),
             screenLeft(0), screenTop(0),
             bitmapHeight(0), bitmapWidth(0), currLayer(Layer::Terrain), currPlayer(0), zoom(1), RedrawMiniMap(true), RedrawMap(true),
             dragging(false), snapLocations(true), locSnapTileOverGrid(true), lockAnywhere(true),
             snapUnits(true), stackUnits(false),
             /*MemhDC(NULL),*/ mapId(0),
             /*MemBitmap(NULL),*/ unsavedChanges(false), changeLock(false), undos(*this),
             minSecondsBetweenBackups(1800), lastBackupTime(-1)
{
    int layerSel = chkd.mainToolbar.layerBox.GetSel();
    if ( layerSel != CB_ERR )
        currLayer = (Layer)layerSel;
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

    if ( MapFile::SaveFile(saveAs) )
    {
        changeLock = false;
        if ( unsavedChanges )
        {
            unsavedChanges = false;
            removeAsterisk();
            undos.ResetChangeCount();
        }
        return true;
    }
    else
        return false;
}

bool GuiMap::SetTile(s32 x, s32 y, u16 tileNum)
{
    u16 xSize = Scenario::getWidth();
    if ( x > xSize || y > Scenario::getHeight() )
        return false;

    undos.AddUndo(TileChange::Make((u16)x, (u16)y, getTile((u16)x, (u16)y)));

    setTile(x, y, tileNum);

    RECT rcTile;
    rcTile.left   = x*32-screenLeft;
    rcTile.right  = rcTile.left+32;
    rcTile.top    = y*32-screenTop;
    rcTile.bottom = rcTile.top+32;

    RedrawMap = true;
    RedrawMiniMap = true;
    InvalidateRect(getHandle(), &rcTile, true);
    return true;
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
    currLayer = newLayer;
    return true;
}

double GuiMap::getZoom()
{
    return zoom;
}

void GuiMap::setZoom(double newScale)
{
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

u8 GuiMap::getCurrPlayer()
{
    return currPlayer;
}

bool GuiMap::setCurrPlayer(u8 newPlayer)
{
    currPlayer = newPlayer;
    return true;
}

bool GuiMap::isDragging()
{
    return dragging;
}

void GuiMap::setDragging(bool bDragging)
{
    dragging = bDragging;
}

void GuiMap::viewLocation(u16 index)
{
    Chk::LocationPtr location = layers.getLocation(index);
    RECT rect;
    if ( location != nullptr && GetClientRect(getHandle(), &rect) != 0 )
    {
        s32 width = rect.right - rect.left,
            height = rect.bottom - rect.top,
            locLeft = std::min(location->left, location->right),
            locRight = std::max(location->left, location->right),
            locTop = std::min(location->top, location->bottom),
            locBottom = std::max(location->top, location->bottom),
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
        if ( selectedLocation != NO_LOCATION ) // If location is selected, determine which part of it is hovered by mouse
        {
            Chk::LocationPtr loc = layers.getLocation(selectedLocation);
            if ( loc != nullptr )
            {
                s32 locationLeft = std::min(loc->left, loc->right),
                    locationRight = std::max(loc->left, loc->right),
                    locationTop = std::min(loc->top, loc->bottom),
                    locationBottom = std::max(loc->top, loc->bottom),
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

    if ( selectedLoc != NO_LOCATION )
    {
        Chk::LocationPtr locRef = layers.getLocation(selectedLoc);
        if ( locRef != nullptr )
        {
            s32 locLeft = std::min(locRef->left, locRef->right),
                locRight = std::max(locRef->left, locRef->right),
                locTop = std::min(locRef->top, locRef->bottom),
                locBottom = std::max(locRef->top, locRef->bottom);
    
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
}

void GuiMap::openTileProperties(s32 xClick, s32 yClick)
{
    u16 xSize = Scenario::getWidth();
    u16 currTile;
    
    if ( getTile(xClick/32, yClick/32, currTile) )
    {
        if ( selections.hasTiles() )
            selections.removeTiles();
                            
        selections.addTile(currTile, u16(xClick/32), u16(yClick/32), TileNeighbor::All);
        RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
        if ( chkd.tilePropWindow.getHandle() != NULL )
            chkd.tilePropWindow.UpdateTile();
        else
            chkd.tilePropWindow.CreateThis(chkd.getHandle());

        ShowWindow(chkd.tilePropWindow.getHandle(), SW_SHOW);
    }
}

void GuiMap::EdgeDrag(HWND hWnd, int x, int y)
{
    if ( isDragging() )
    {
        if ( x < 0 )
            x = 0;
        if ( y < 0 )
            y = 0;

        RECT rcMap;
        GetClientRect(hWnd, &rcMap);
        TrackMouse(defaultHoverTime);
        if ( x == 0 ) // Cursor on the left
        {
            if ( (screenLeft+16)/32 > 0 )
                selections.setEndDrag( ((screenLeft +16)/32-1)*32, selections.getEndDrag().y );
            if ( screenLeft > 0 )
                screenLeft = selections.getEndDrag().x;
        }
        else if ( x >= rcMap.right-2 ) // Cursor on the right
        {
            if ( (screenLeft+rcMap.right)/32 < Scenario::getWidth() )
                selections.setEndDrag( ((screenLeft+rcMap.right)/32+1)*32, selections.getEndDrag().y );
            screenLeft = selections.getEndDrag().x - rcMap.right;
        }
        if ( y == 0 ) // Cursor on the top
        {
            if ( (screenTop+16)/32 > 0 )
                selections.setEndDrag( selections.getEndDrag().x, ((screenTop+16)/32-1)*32 );
            if ( screenTop > 0 )
                screenTop = selections.getEndDrag().y;
        }
        else if ( y >= rcMap.bottom-2 ) // Cursor on the bottom
        {
            if ( (screenTop+rcMap.bottom)/32 < Scenario::getHeight() )
                selections.setEndDrag( selections.getEndDrag().x, ((screenTop+rcMap.bottom)/32+1)*32 );
            screenTop = selections.getEndDrag().y - rcMap.bottom;
        }
        Scroll(true, true, true);
        Redraw(false);
    }
}

u8 GuiMap::GetPlayerOwnerStringId(u8 player)
{
    u8 owner = 0;
    if ( getPlayerOwner(player, owner) )
    {
        switch ( owner )
        {
            case 1: case 5: return 2; break; // Computer
            case 2: case 6: return 3; break; // Human
            case 3: return 1; break; // Rescuable
            case 7: return 4; break; // Neutral
        }
    }
    return 0; // Unused
}

void GuiMap::refreshScenario()
{
    selections.removeTiles();
    selections.removeUnits();
    chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

    if ( chkd.unitWindow.getHandle() != nullptr )
        chkd.unitWindow.RepopulateList();
    if ( chkd.locationWindow.getHandle() != NULL )
    {
        if ( CM->numLocations() == 0 )
            chkd.locationWindow.DestroyThis();
        else
            chkd.locationWindow.RefreshLocationInfo();
    }
    if ( chkd.mapSettingsWindow.getHandle() != NULL )
        chkd.mapSettingsWindow.RefreshWindow();
    chkd.trigEditorWindow.RefreshWindow();

    Redraw(true);
}

void GuiMap::clearSelectedTiles()
{
    selections.removeTiles();
}

void GuiMap::clearSelectedUnits()
{
    selections.removeUnits();
}

void GuiMap::clearSelection()
{
    if ( this != nullptr )
    {
        selections.removeTiles();
        selections.removeUnits();
    }
}

void GuiMap::selectAll()
{
    if ( this != nullptr )
    {
        switch ( currLayer )
        {
            case Layer::Terrain:
                {
                    if ( selections.hasTiles() )
                        selections.removeTiles();

                    u16 tileValue,
                        width = Scenario::getWidth(), height = Scenario::getHeight(),
                        x=0, y=0;

                    for ( x=0; x<width; x++ ) // Add the top row
                    {
                        if ( getTile(x, 0, tileValue) )
                            selections.addTile(tileValue, x, y, TileNeighbor::Top);
                    }

                    for ( y=0; y<height-1; y++ ) // Add the middle rows
                    {
                        if ( getTile(0, y, tileValue) )
                            selections.addTile(tileValue, 0, y, TileNeighbor::Left); // Add the left tile

                        for ( x=1; x<width-1; x++ )
                        {
                            if ( getTile(x, y, tileValue) )
                                selections.addTile(tileValue, x, y, TileNeighbor::None); // Add the middle portion of the row
                        }
                        if ( getTile(width-1, y) )
                            selections.addTile(tileValue, width-1, y, TileNeighbor::Right); // Add the right tile
                    }

                    if ( getTile(0, height-1) )
                        selections.addTile(tileValue, 0, height-1, (TileNeighbor)((u8)TileNeighbor::Left|(u8)TileNeighbor::Bottom));

                    for ( x=1; x<width-1; x++ ) // Add the bottom row
                    {
                        if ( getTile(x, height-1) )
                            selections.addTile(tileValue, x, height-1, TileNeighbor::Bottom);
                    }
                    if ( getTile(width-1, height-1) )
                        selections.addTile(tileValue, width-1, height-1, (TileNeighbor)((u8)TileNeighbor::Right|(u8)TileNeighbor::Bottom));

                    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
                }
                break;
            case Layer::Units:
                {
                    chkd.unitWindow.SetChangeHighlightOnly(true);
                    for ( u16 i=0; i<numUnits(); i++ )
                    {
                        if ( !selections.unitIsSelected(i) )
                        {
                            selections.addUnit(i);
                            if ( chkd.unitWindow.getHandle() != nullptr )
                                chkd.unitWindow.FocusAndSelectIndex(i);
                        }
                    }
                    chkd.unitWindow.SetChangeHighlightOnly(false);
                    Redraw(true);
                }
                break;
        }
    }
}

void GuiMap::deleteSelection()
{
    if ( this != nullptr )
    {
        switch ( currLayer )
        {
            case Layer::Terrain:
                {
                    u16 xSize = Scenario::getWidth();

                    auto &selTiles = selections.getTiles();
                    for ( auto &tile : selTiles )
                    {
                        undos.AddUndo(TileChange::Make(tile.xc, tile.yc, getTile(tile.xc, tile.yc)));
                        setTile(tile.xc, tile.yc, 0);
                    }

                    selections.removeTiles();
                }
                break;

            case Layer::Units:
                {
                    if ( chkd.unitWindow.getHandle() != nullptr )
                        SendMessage(chkd.unitWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, NULL), 0);
                    else
                    {
                        auto deletes = ReversibleActions::Make();
                        while ( selections.hasUnits() )
                        {
                            // Get the highest index in the selection
                                u16 index = selections.getHighestIndex();
                                selections.removeUnit(index);
                            
                                Chk::UnitPtr delUnit = layers.getUnit(index);
                                deletes->Insert(UnitCreateDel::Make(index, *delUnit));
                                deleteUnit(index);
                        }
                        undos.AddUndo(deletes);
                    }
                }
                break;

            case Layer::Locations:
                {
                    if ( chkd.locationWindow.getHandle() != NULL )
                        chkd.locationWindow.DestroyThis();
                
                    u16 index = selections.getSelectedLocation();
                    Chk::LocationPtr loc = index != NO_LOCATION ? layers.getLocation(index) : nullptr;
                    if ( loc != nullptr )
                    {
                        undos.AddUndo(LocationCreateDel::Make(index));

                        chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

                        loc->elevationFlags = 0;
                        loc->left = 0;
                        loc->right = 0;
                        loc->top = 0;
                        loc->bottom = 0;
                        u16 stringNum = loc->stringId;
                        loc->stringId = 0;
                        if ( stringNum > 0 )
                        {
                            removeUnusedString(stringNum);
                            refreshScenario();
                        }

                        selections.selectLocation(NO_LOCATION);
                    }
                }
                break;
        }

        RedrawMap = true;
        RedrawMiniMap = true;
        RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
    }
}

void GuiMap::paste(s32 mapClickX, s32 mapClickY)
{
    s32 xc = mapClickX, yc = mapClickY;
    if ( currLayer == Layer::Units )
        this->snapUnitCoordinate(xc, yc);

    clipboard.doPaste(currLayer, xc, yc, *this, undos, stackUnits);

    Redraw(true);
}

void GuiMap::PlayerChanged(u8 newPlayer)
{
    auto unitChanges = ReversibleActions::Make();
    auto &selUnits = selections.getUnits();
    for ( u16 unitIndex : selUnits )
    {
        Chk::UnitPtr unit = layers.getUnit(unitIndex);
        unitChanges->Insert(UnitChange::Make(unitIndex, Chk::Unit::Field::Owner, unit->owner));
        unit->owner = newPlayer;

        if ( chkd.unitWindow.getHandle() != nullptr )
            chkd.unitWindow.ChangeUnitsDisplayedOwner(unitIndex, newPlayer);
    }
    chkd.unitWindow.ChangeDropdownPlayer(newPlayer);
    undos.AddUndo(unitChanges);
    Redraw(true);
}

Selections &GuiMap::GetSelections()
{
    return selections;
}

u16 GuiMap::GetSelectedLocation()
{
    return selections.getSelectedLocation();
}

void GuiMap::AddUndo(ReversiblePtr action)
{
    undos.AddUndo(action);
}

void GuiMap::undo()
{
    switch ( currLayer )
    {
        case Layer::Terrain:
            undos.doUndo((int32_t)UndoTypes::TileChange, this);
            //undoStacks.doUndo(UNDO_TERRAIN, scenario(), selections());
            break;
        case Layer::Units:
            undos.doUndo((int32_t)UndoTypes::UnitChange, this);
            //undoStacks.doUndo(UNDO_UNIT, scenario(), selections());
            if ( chkd.unitWindow.getHandle() != nullptr )
                chkd.unitWindow.RepopulateList();
            break;
        case Layer::Locations:
            {
                undos.doUndo((int32_t)UndoTypes::LocationChange, this);
                //undoStacks.doUndo(UNDO_LOCATION, scenario(), selections());
                if ( chkd.locationWindow.getHandle() != NULL )
                {
                    if ( CM->numLocations() > 0 )
                        chkd.locationWindow.RefreshLocationInfo();
                    else
                        chkd.locationWindow.DestroyThis();
                }
                refreshScenario();
                chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
            }
            break;
    }
    Redraw(true);
}

void GuiMap::redo()
{
    switch ( currLayer )
    {
        case Layer::Terrain:
            undos.doRedo((int32_t)UndoTypes::TileChange, this);
            //undoStacks.doRedo(UNDO_TERRAIN, scenario(), selections());
            break;
        case Layer::Units:
            undos.doRedo((int32_t)UndoTypes::UnitChange, this);
            //undoStacks.doRedo(UNDO_UNIT, scenario(), selections());
            if ( chkd.unitWindow.getHandle() != nullptr )
                chkd.unitWindow.RepopulateList();
            break;
        case Layer::Locations:
            undos.doRedo((int32_t)UndoTypes::LocationChange, this);
            //undoStacks.doRedo(UNDO_LOCATION, scenario(), selections());
            if ( chkd.locationWindow.getHandle() != NULL )
            {
                if ( CM->numLocations() == 0 )
                    chkd.locationWindow.DestroyThis();
                else
                    chkd.locationWindow.RefreshLocationInfo();
            }
            refreshScenario();
            break;
    }
    Redraw(true);
}

void GuiMap::ChangesMade()
{
    notifyChange(true);
}

void GuiMap::ChangesReversed()
{
    changesUndone();
}

void GuiMap::SetScreenLeft(s32 newScreenLeft)
{
    screenLeft = newScreenLeft;
}

void GuiMap::SetScreenTop(s32 newScreenTop)
{
    screenTop = newScreenTop;
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
        catch ( std::bad_alloc ) {}
    }
    bitmapWidth = 0;
    bitmapHeight = 0;
    graphicBits.clear();
    return false;
}

void GuiMap::PaintMap(GuiMapPtr currMap, bool pasting)
{
    if ( WindowsItem::StartSimplePaint() != NULL )
    {
        u16 scaledWidth = PaintWidth(),
            scaledHeight = PaintHeight();

        if ( zoom < 1 || zoom > 1 )
        {
            scaledWidth = (u16(((double)PaintWidth()) / zoom)) / 32 * 32,
            scaledHeight = (u16(((double)PaintHeight()) / zoom)) / 32 * 32;
        }
        u32 bitmapSize = (u32)scaledWidth*(u32)scaledHeight * 4;

        if ( RedrawMap == true && EnsureBitmapSize(scaledWidth, scaledHeight) )
        {
            RedrawMap = false;

            mapBuffer.SetSize(GetPaintDc(), scaledWidth, scaledHeight);
            if ( currMap == nullptr || currMap.get() == this ) // Only redraw minimap for active window
                RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

            // Terrain, Grid, Units, Sprites, Debug
            graphics.DrawMap(bitmapWidth, bitmapHeight, screenLeft, screenTop, graphicBits, mapBuffer.GetPaintDc(), !lockAnywhere);
        }

        toolsBuffer.SetSize(GetPaintDc(), scaledWidth, scaledHeight);
        BitBlt(toolsBuffer.GetPaintDc(), 0, 0, scaledWidth, scaledHeight, mapBuffer.GetPaintDc(), 0, 0, SRCCOPY);
        if ( currMap == nullptr || currMap.get() == this )
        { // Drag and paste graphics
            DrawTools(toolsBuffer.GetPaintDc(), toolsBuffer.GetPaintBitmap(), scaledWidth, scaledHeight,
                screenLeft, screenTop, selections, pasting, clipboard, *this);

            if ( currLayer != Layer::Locations )
                DrawSelectingFrame(toolsBuffer.GetPaintDc(), selections, screenLeft, screenTop, bitmapWidth, bitmapHeight, zoom);
        }
        SetStretchBltMode(GetPaintDc(), HALFTONE);
        if ( zoom == 1 )
            BitBlt(GetPaintDc(), 0, 0, PaintWidth(), PaintHeight(), toolsBuffer.GetPaintDc(), 0, 0, SRCCOPY);
        else
            StretchBlt(GetPaintDc(), 0, 0, PaintWidth(), PaintHeight(), toolsBuffer.GetPaintDc(), 0, 0, scaledWidth, scaledHeight, SRCCOPY);
    }
    WindowsItem::EndPaint();
}

void GuiMap::PaintMiniMap(HDC miniMapDc, int miniMapWidth, int miniMapHeight)
{
    if ( this != nullptr && miniMapDc != NULL )
    {
        if ( RedrawMiniMap && miniMapBuffer.SetSize(miniMapDc, miniMapWidth, miniMapHeight) )
        {
            RedrawMiniMap = false;
            DrawMiniMap(miniMapBuffer.GetPaintDc(), Scenario::getWidth(), Scenario::getHeight(), MiniMapScale(Scenario::getWidth(), Scenario::getHeight()), *this);
        }

        if ( miniMapBuffer.GetPaintDc() != NULL )
        {
            BitBlt(miniMapDc, 0, 0, miniMapWidth, miniMapHeight, miniMapBuffer.GetPaintDc(), 0, 0, SRCCOPY);
            DrawMiniMapBox(miniMapDc, screenLeft, screenTop, bitmapWidth, bitmapHeight, Scenario::getWidth(), Scenario::getHeight(),
                MiniMapScale(Scenario::getWidth(), Scenario::getHeight()));
        }
    }
}

void GuiMap::Redraw(bool includeMiniMap)
{
    if ( this != nullptr )
    {
        RedrawMap = true;
        if ( includeMiniMap )
            RedrawMiniMap = true;

        RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);
    }
}

void GuiMap::ValidateBorder(s32 screenWidth, s32 screenHeight)
{
    if ( screenLeft < 0 )
        screenLeft = 0;
    else if ( screenLeft > ((s32)Scenario::getWidth())*32-screenWidth )
    {
        if ( screenWidth > ((s32)Scenario::getWidth())*32 )
            screenLeft = 0;
        else
            screenLeft = Scenario::getWidth()*32-screenWidth;
    }

    if ( screenTop < 0 )
        screenTop = 0;
    else if ( screenTop > Scenario::getHeight()*32-screenHeight )
    {
        if ( screenHeight > Scenario::getHeight()*32 )
            screenTop = 0;
        else
            screenTop = Scenario::getHeight()*32-screenHeight;
    }
}

bool GuiMap::SetGridSize(s16 xSize, s16 ySize)
{
    bool success = false;
    u16 oldXSize = 0, oldYSize = 0;
    if ( graphics.GetGridSize(0, oldXSize, oldYSize) )
    {
        if ( oldXSize == xSize && oldYSize == ySize )
            success = graphics.SetGridSize(0, 0, 0);
        else
            success = graphics.SetGridSize(0, xSize, ySize);

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
    bool success = false;
    if ( graphics.SetGridColor(0, red, green, blue) )
    {
        u16 xSize = 0, ySize = 0;
        if ( graphics.GetGridSize(0, xSize, ySize) && (xSize == 0 || ySize == 0) )
            success = SetGridSize(32, 32);

        UpdateGridColorMenu();
        Redraw(false);
    }
    return success;
}

void GuiMap::ToggleDisplayElevations()
{
    graphics.ToggleDisplayElevations();
    UpdateTerrainViewMenuItems();
    Redraw(false);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingElevations()
{
    return graphics.DisplayingElevations();
}

void GuiMap::ToggleTileNumSource(bool MTXMoverTILE)
{
    graphics.ToggleTileNumSource(MTXMoverTILE);
    UpdateTerrainViewMenuItems();
    Redraw(false);
    if ( chkd.terrainPalWindow.getHandle() != NULL )
        RedrawWindow(chkd.terrainPalWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

bool GuiMap::DisplayingTileNums()
{
    return graphics.DisplayingTileNums();
}

bool GuiMap::snapUnitCoordinate(s32& x, s32& y)
{
    if ( snapUnits )
    {
        u16 gridWidth = 0, gridHeight = 0, offsetX = 0, offsetY = 0;
        if ( graphics.GetGridSize(0, gridWidth, gridHeight) && graphics.GetGridOffset(0, offsetX, offsetY) &&
             gridWidth > 0 && gridHeight > 0 )
        {
            double intervalNum = (double(x - offsetX)) / gridWidth;
            s32 xIntervalNum = (s32)round(intervalNum);
            x = xIntervalNum*gridWidth;
            intervalNum = (double(y - offsetY)) / gridHeight;
            s32 yIntervalNum = (s32)round(intervalNum);
            y = yIntervalNum*gridHeight;
        }
    }
    return false;
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

void GuiMap::ToggleLocationNameClip()
{
    if ( this != nullptr )
    {
        graphics.ToggleLocationNameClip();
        chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, graphics.ClippingLocationNames());
        Redraw(false);
    }
}

void GuiMap::SetLocationSnap(LocationSnap locationSnap)
{
    if ( this != nullptr )
    {
        bool prevSnapLocations = snapLocations;
        if ( locationSnap == LocationSnap::NoSnap )
            snapLocations = false;
        else
            snapLocations = true;

        if ( locationSnap == LocationSnap::SnapToTile )
        {
            if ( prevSnapLocations && locSnapTileOverGrid )
                snapLocations = false;
            else
                locSnapTileOverGrid = true;
        }
        else if ( locationSnap == LocationSnap::SnapToGrid )
        {
            if ( prevSnapLocations && !locSnapTileOverGrid )
                snapLocations = false;
            else
                locSnapTileOverGrid = false;
        }

        UpdateLocationMenuItems();
    }
}

void GuiMap::ToggleLockAnywhere()
{
    lockAnywhere = !lockAnywhere;
    UpdateLocationMenuItems();
    if ( selections.getSelectedLocation() == 63 )
        selections.selectLocation(NO_LOCATION);
    chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
    Redraw(false);
}

bool GuiMap::LockAnywhere()
{
    return lockAnywhere;
}

bool GuiMap::GetSnapIntervals(u32& x, u32& y, u32& xOffset, u32& yOffset)
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
            if ( graphics.GetGridSize(0, gridWidth, gridHeight) &&
                 graphics.GetGridOffset(0, gridXOffset, gridYOffset) &&
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

bool GuiMap::SnapLocationDimensions(u32& x1, u32& y1, u32& x2, u32& y2, LocSnapFlags locSnapFlags)
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

        if ( ((u32)locSnapFlags & (u32)LocSnapFlags::SnapX1) == (u32)LocSnapFlags::SnapX1 )
            x1 = xStartIntervalNum*lengthX + startSnapX;
        if ( ((u32)locSnapFlags & (u32)LocSnapFlags::SnapY1) == (u32)LocSnapFlags::SnapY1 )
            y1 = yStartIntervalNum*lengthY + startSnapY;
        if ( ((u32)locSnapFlags & (u32)LocSnapFlags::SnapX2) == (u32)LocSnapFlags::SnapX2 )
            x2 = xEndIntervalNum*lengthX + startSnapX;
        if ( ((u32)locSnapFlags & (u32)LocSnapFlags::SnapY2) == (u32)LocSnapFlags::SnapY2 )
            y2 = yEndIntervalNum*lengthY + startSnapY;

        return locSnapFlags != LocSnapFlags::None;
    }
    else
        return false;
}

void GuiMap::UpdateLocationMenuItems()
{
    chkd.mainMenu.SetCheck(ID_LOCATIONS_CLIPNAMES, graphics.ClippingLocationNames());
    chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOTILE, snapLocations && locSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_SNAPTOACTIVEGRID, snapLocations && !locSnapTileOverGrid);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_NOSNAP, !snapLocations);
    chkd.mainMenu.SetCheck(ID_LOCATIONS_LOCKANYWHERE, lockAnywhere);
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
    graphics.GetGridSize(0, gridWidth, gridHeight);

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
    graphics.GetGridColor(0, red, green, blue);

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

void GuiMap::UpdateTerrainViewMenuItems()
{
    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEELEVATIONS, graphics.DisplayingElevations());

    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, false);
    chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, false);
    if ( graphics.DisplayingTileNums() )
    {
        if ( graphics.mtxmOverTile() )
            chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUESMTXM, true);
        else
            chkd.mainMenu.SetCheck(ID_TERRAIN_DISPLAYTILEVALUES, true);
    }
}

void GuiMap::UpdateUnitMenuItems()
{
    chkd.mainMenu.SetCheck(ID_UNITS_UNITSSNAPTOGRID, snapUnits);
    chkd.mainMenu.SetCheck(ID_UNITS_ALLOWSTACK, stackUnits);
}

void GuiMap::Scroll(bool scrollX, bool scrollY, bool validateBorder)
{
    SCROLLINFO scrollbars = { };
    scrollbars.cbSize = sizeof(SCROLLINFO);
    scrollbars.fMask = SIF_ALL;
    scrollbars.nMin = 0;

    RECT rcMap;
    GetClientRect(getHandle(), &rcMap);
    s32 screenWidth  = (s32)((double(rcMap.right-rcMap.left))/zoom),
        screenHeight = (s32)((double(rcMap.bottom-rcMap.top))/zoom);

    if ( validateBorder )
        ValidateBorder(screenWidth, screenHeight);

    if ( scrollX )
    {
        scrollbars.nPos = screenLeft; 
        scrollbars.nMax = Scenario::getWidth()*32;
        scrollbars.nPage = screenWidth;
        SetScrollInfo(getHandle(), SB_HORZ, &scrollbars, true);
    }
    if ( scrollY )
    {
        scrollbars.nPos = screenTop;
        scrollbars.nMax = Scenario::getHeight()*32;
        scrollbars.nPage = screenHeight;
        SetScrollInfo(getHandle(), SB_VERT, &scrollbars, true);
    }
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

void GuiMap::notifyChange(bool undoable)
{
    if ( changeLock == false && undoable == false )
        changeLock = true;

    if ( !unsavedChanges )
    {
        unsavedChanges = true;
        addAsterisk();
    }
}

void GuiMap::changesUndone()
{
    if ( !changeLock )
    {
        unsavedChanges = false;
        removeAsterisk();
    }
}

bool GuiMap::changesLocked()
{
    return changeLock;
}

void GuiMap::addAsterisk()
{
    std::string windowTitle;
    if ( WindowsItem::GetWinText(windowTitle) )
        WindowsItem::SetWinText(windowTitle + "*");
}

void GuiMap::removeAsterisk()
{
    std::string windowTitle;
    if ( WindowsItem::GetWinText(windowTitle) )
    {
        if ( windowTitle.back() == '*' )
        {
            windowTitle.pop_back();
            WindowsItem::SetWinText(windowTitle);
        }
    }
}

void GuiMap::updateMenu()
{
    UpdateLocationMenuItems();
    chkd.mainToolbar.layerBox.SetSel((int)currLayer);
    UpdateZoomMenuItems();
    chkd.mainToolbar.playerBox.SetSel(currPlayer);
    UpdateGridSizesMenu();
    UpdateGridColorMenu();
    UpdateTerrainViewMenuItems();
    UpdateUnitMenuItems();
}

bool GuiMap::CreateThis(HWND hClient, const std::string &title)
{
    if ( !WindowClassIsRegistered("MdiChild") )
    {
        UINT style = CS_DBLCLKS;
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);

        if ( !RegisterWindowClass(CS_DBLCLKS, NULL, hCursor, hBackground, NULL, "MdiChild", NULL, true) )
            DestroyCursor(hCursor);
    }

    return CreateMdiChild(title, WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hClient);
}

void GuiMap::ReturnKeyPress()
{
    if ( this != nullptr )
    {
        if ( currLayer == Layer::Units )
        {
            if ( selections.hasUnits() )
            {
                if ( chkd.unitWindow.getHandle() == nullptr )
                    chkd.unitWindow.CreateThis(chkd.getHandle());
                ShowWindow(chkd.unitWindow.getHandle(), SW_SHOW);
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
}

void GuiMap::SetAutoBackup(bool doAutoBackups)
{
    GuiMap::doAutoBackups = doAutoBackups;
}

LRESULT GuiMap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_PAINT: PaintMap(CM, chkd.maps.clipboard.isPasting()); break;
        case WM_MDIACTIVATE: ActivateMap((HWND)lParam); return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
        case WM_ERASEBKGND: return 1; break; // Prevent background from showing
        case WM_HSCROLL: return HorizontalScroll(hWnd, msg, wParam, lParam); break;
        case WM_VSCROLL: return VerticalScroll(hWnd, msg, wParam, lParam); break;
        case WM_CHAR: return 0; break;
        case WM_SIZE: return DoSize(hWnd, wParam, lParam); break;
        case WM_CLOSE: return ConfirmWindowClose(hWnd); break;
        case WM_DESTROY: return DestroyWindow(hWnd); break;
        case WM_RBUTTONUP: RButtonUp(); break;
        case WM_LBUTTONDBLCLK: LButtonDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONDOWN: LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MOUSEMOVE: MouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_MOUSEHOVER: MouseHover(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
        case WM_LBUTTONUP: LButtonUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam); break;
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

void GuiMap::ActivateMap(HWND hWnd)
{
    chkd.tilePropWindow.DestroyThis();
    chkd.unitWindow.DestroyThis();
    chkd.locationWindow.DestroyThis();
    chkd.mapSettingsWindow.DestroyThis();
    chkd.terrainPalWindow.DestroyThis();
    chkd.trigEditorWindow.DestroyThis();
    
    if ( hWnd != NULL )
    {
        chkd.maps.Focus(hWnd);
        Redraw(true);
        chkd.maps.UpdateTreeView();
    }
}

LRESULT GuiMap::DoSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = ClassWindow::WndProc(hWnd, WM_SIZE, wParam, lParam);
    Scroll(true, true, true);
    return result;
}

LRESULT GuiMap::DestroyWindow(HWND hWnd)
{
    chkd.maps.CloseMap(hWnd);
    RedrawWindow(chkd.mainPlot.leftBar.miniMap.getHandle(), NULL, NULL, RDW_INVALIDATE);
    return ClassWindow::WndProc(hWnd, WM_DESTROY, 0, 0);
}

void GuiMap::RButtonUp()
{
    chkd.maps.endPaste();
    ClipCursor(NULL);
    Redraw(true);
}

void GuiMap::LButtonDoubleClick(int x, int y)
{
    if ( currLayer == Layer::Locations )
        doubleClickLocation(x, y);
}

void GuiMap::LButtonDown(int x, int y, WPARAM wParam)
{
    selections.resetMoved();
    u32 mapClickX = (s32(((double)x)/getZoom()) + screenLeft),
        mapClickY = (s32(((double)y)/getZoom()) + screenTop);

    switch ( wParam )
    {
        case MK_SHIFT|MK_LBUTTON: // Shift + LClick
            if ( currLayer == Layer::Terrain )
                openTileProperties(mapClickX, mapClickY);
            break;
    
        case MK_CONTROL|MK_LBUTTON: // Ctrl + LClick
            {
                chkd.tilePropWindow.DestroyThis();
                if ( !chkd.maps.clipboard.isPasting() )
                {
                    if ( currLayer == Layer::Terrain ) // Ctrl + Click tile
                        selections.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
                    else if ( currLayer == Layer::Doodads || currLayer == Layer::Units || currLayer == Layer::Sprites )
                        selections.setDrags(mapClickX, mapClickY);

                    LockCursor();
                    TrackMouse(defaultHoverTime);
                    setDragging(true);
                }
            }
            break;
    
        case MK_LBUTTON: // LClick
            {
                chkd.tilePropWindow.DestroyThis();
                if ( chkd.maps.clipboard.isPasting() )
                    paste(mapClickX, mapClickY);
                else
                {
                    if ( selections.hasTiles() )
                        selections.removeTiles();
                                
                    selections.setDrags(mapClickX, mapClickY);
                    if ( currLayer == Layer::Terrain )
                        selections.setDrags( (mapClickX+16)/32*32, (mapClickY+16)/32*32 );
                    else if ( currLayer == Layer::Locations )
                    {
                        u32 x1 = mapClickX, y1 = mapClickY;
                        if ( SnapLocationDimensions(x1, y1, x1, y1, (LocSnapFlags)((u32)LocSnapFlags::SnapX1|(u32)LocSnapFlags::SnapY1)) )
                            selections.setDrags(x1, y1);
                        selections.setLocationFlags(getLocSelFlags(mapClickX, mapClickY));
                    }

                    SetCapture(getHandle());
                    TrackMouse(defaultHoverTime);
                    setDragging(true);
                    Redraw(false);
                }
            }
            break;
    }
}

void GuiMap::MouseMove(HWND hWnd, int x, int y, WPARAM wParam)
{
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;

    s32 mapHoverX = (s32(((double)x)/getZoom())) + screenLeft,
        mapHoverY = (s32(((double)y)/getZoom())) + screenTop;

    if ( wParam & MK_LBUTTON ) // If click and dragging
    {
        chkd.maps.stickCursor(); // Stop cursor from reverting
        selections.setMoved();
    }
    else // If not click and dragging
        chkd.maps.updateCursor(mapHoverX, mapHoverY); // Determine proper hover cursor

    // Set status bar tracking pos
    char newPos[64];
    std::snprintf(newPos, 64, "%i, %i (%i, %i)", mapHoverX, mapHoverY, mapHoverX / 32, mapHoverY / 32);
    chkd.statusBar.SetText(0, newPos);
    
    switch ( wParam )
    {
        case MK_CONTROL|MK_LBUTTON:
            {
                RECT rcMap;
                GetClientRect(hWnd, &rcMap);

                if ( x == 0 || y == 0 || x == rcMap.right-2 || y == rcMap.bottom-2 )
                    EdgeDrag(hWnd, x, y);

                selections.setEndDrag(mapHoverX, mapHoverY);
                if ( currLayer == Layer::Terrain )
                    selections.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
                else if ( currLayer == Layer::Locations )
                {
                    u32 x2 = mapHoverX, y2 = mapHoverY;
                    if ( SnapLocationDimensions(x2, y2, x2, y2, (LocSnapFlags)((u32)LocSnapFlags::SnapX2|(u32)LocSnapFlags::SnapY2)) )
                        selections.setEndDrag(x2, y2);
                }
                else if ( currLayer == Layer::Units )
                {
                    s32 xc = mapHoverX, yc = mapHoverY;
                    if ( snapUnitCoordinate(xc, yc) )
                        selections.setEndDrag(xc, yc);
                }

                PaintMap(nullptr, chkd.maps.clipboard.isPasting());
            }
            break;

        case MK_LBUTTON:
            {
                // If pasting, move paste
                if ( chkd.maps.clipboard.isPasting() )
                {
                    s32 xc = mapHoverX, yc = mapHoverY;
                    if ( currLayer == Layer::Units )
                        snapUnitCoordinate(xc, yc);

                    selections.setEndDrag(xc, yc);
                    if ( !chkd.maps.clipboard.isPreviousPasteLoc(u16(xc), u16(yc)) )
                        paste((s16)xc, (s16)yc);
                }

                if ( isDragging() )
                {
                    RECT rcMap;
                    GetClientRect(hWnd, &rcMap);
                    if ( x == 0 || y == 0 || x >= rcMap.right-2 || y >= rcMap.bottom-2 )
                        EdgeDrag(hWnd, x, y);

                    selections.setEndDrag( mapHoverX, mapHoverY );
                    if ( currLayer == Layer::Terrain )
                        selections.setEndDrag( (mapHoverX+16)/32*32, (mapHoverY+16)/32*32 );
                    else if ( currLayer == Layer::Locations )
                    {
                        u32 x2 = mapHoverX, y2 = mapHoverY;
                        if ( SnapLocationDimensions(x2, y2, x2, y2, (LocSnapFlags)((u32)LocSnapFlags::SnapX2|(u32)LocSnapFlags::SnapY2)) )
                            selections.setEndDrag(x2, y2);
                    }
                    else if ( currLayer == Layer::Units )
                    {
                        s32 xc = mapHoverX, yc = mapHoverY;
                        if ( snapUnitCoordinate(xc, yc) )
                            selections.setEndDrag(xc, yc);
                    }
                }
                PaintMap(nullptr, chkd.maps.clipboard.isPasting());
            }
            break;

        default:
            {
                if ( chkd.maps.clipboard.isPasting() == true )
                {
                    if ( GetKeyState(VK_SPACE) & 0x8000 )
                    {
                        RECT rcMap;
                        GetClientRect(hWnd, &rcMap);
    
                        if ( x == 0 || x == rcMap.right-2 || y == 0 || y == rcMap.bottom-2 )
                        {
                            if      ( x == 0 )
                                screenLeft -= 32;
                            else if ( x == rcMap.right-2 )
                                screenLeft += 32;
                            if      ( y == 0 )
                                screenTop -= 32;
                            else if ( y == rcMap.bottom-2 )
                                screenTop += 32;
    
                            Scroll(true, true, true);
                        }
                    }

                    if ( currLayer == Layer::Units )
                        snapUnitCoordinate(mapHoverX, mapHoverY);

                    selections.setEndDrag(mapHoverX, mapHoverY);
                    PaintMap(nullptr, chkd.maps.clipboard.isPasting());
                }
            }
            break;
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
            {
                if ( chkd.maps.clipboard.isPasting() == true )
                {
                    RECT rcMap;
                    GetClientRect(hWnd, &rcMap);
    
                    if      ( x == 0              )
                        screenLeft -= 8;
                    else if ( x >= rcMap.right-2  )
                        screenLeft += 8;
                    if      ( y == 0              )
                        screenTop -= 8;
                    else if ( y >= rcMap.bottom-2 )
                        screenTop += 8;
    
                    Scroll(true, true, true);
                    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE);

                    x = (s32(((double)x)/getZoom())) + screenLeft,
                    y = (s32(((double)y)/getZoom())) + screenTop;
                    selections.setEndDrag(x, y);
                    TrackMouse(100);
                }
            }
            break;
    }
}

void GuiMap::LButtonUp(HWND hWnd, int x, int y, WPARAM wParam)
{
    ReleaseCapture();
    if ( isDragging() )
    {
        setDragging(false);
        if ( x < 0 ) x = 0;
        if ( y < 0 ) y = 0;
        x = s16(x/getZoom() + screenLeft);
        y = s16(y/getZoom() + screenTop);

        if ( chkd.maps.clipboard.isPasting() )
            paste((s16)x, (s16)y);
    
        if ( currLayer == Layer::Terrain )
            TerrainLButtonUp(hWnd, x, y, wParam);
        else if ( currLayer == Layer::Locations )
            LocationLButtonUp(hWnd, x, y, wParam);
        else if ( currLayer == Layer::Units )
            UnitLButtonUp(hWnd, x, y, wParam);

        selections.setStartDrag(-1, -1);
        selections.setEndDrag(-1, -1);
        setDragging(false);
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
    }

    if ( !chkd.maps.clipboard.isPasting() )
        ClipCursor(NULL);
}

void GuiMap::TerrainLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    selections.setEndDrag((mapX+16)/32, (mapY+16)/32);
    selections.setStartDrag(selections.getStartDrag().x/32, selections.getStartDrag().y/32);
    u16 width = Scenario::getWidth();
                    
    if ( wParam == MK_CONTROL && selections.startEqualsEndDrag() ) // Add/remove single tile to/front existing selection
    {
        selections.setEndDrag(mapX/32, mapY/32);
                            
        u16 tileValue = 0;
        if ( getTile((u16)selections.getEndDrag().x, (u16)selections.getEndDrag().y, tileValue) )
            selections.addTile(tileValue, (u16)selections.getEndDrag().x, (u16)selections.getEndDrag().y);
    }
    else // Add/remove multiple tiles from selection
    {
        selections.sortDragPoints();
    
        if ( selections.getStartDrag().y < selections.getEndDrag().y &&
             selections.getStartDrag().x < selections.getEndDrag().x )
        {
            bool multiAdd = selections.getStartDrag().x + 1 < selections.getEndDrag().x ||
                            selections.getStartDrag().y + 1 < selections.getEndDrag().y;
    
            if ( selections.getEndDrag().x > Scenario::getWidth() )
                selections.setEndDrag(Scenario::getWidth(), selections.getEndDrag().y);
            if ( selections.getEndDrag().y > Scenario::getHeight() )
                selections.setEndDrag(selections.getEndDrag().x, Scenario::getHeight());
    
            for ( int yRow = selections.getStartDrag().y; yRow < selections.getEndDrag().y; yRow++ )
            {
                for ( int xRow = selections.getStartDrag().x; xRow < selections.getEndDrag().x; xRow++ )
                {
                    u16 tileValue = 0;
                    if ( getTile(xRow, yRow, tileValue) )
                        selections.addTile(tileValue, xRow, yRow);
                }
            }
        }
    }
}

void GuiMap::LocationLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    if ( selections.hasMoved() ) // attempt to move, resize, or create location
    {
        u32 startX = selections.getStartDrag().x,
            startY = selections.getStartDrag().y,
            endX = mapX,
            endY = mapY;
        s32 dragX = endX-startX;
        s32 dragY = endY-startY;

        if ( selections.getLocationFlags() == LocSelFlags::None ) // Create location
        {
            AscendingOrder(startX, endX);
            AscendingOrder(startY, endY);
            SnapLocationDimensions(startX, startY, endX, endY, LocSnapFlags::SnapAll);
                                    
            u16 locationIndex;
            if ( createLocation(startX, startY, endX, endY, locationIndex) )
            {
                undos.AddUndo(LocationCreateDel::Make(locationIndex));
                chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
                refreshScenario();
            }
        }
        else // Move or resize location
        {
            u16 selectedLocation = selections.getSelectedLocation();
            Chk::LocationPtr loc = selectedLocation != NO_LOCATION ? layers.getLocation(selectedLocation) : nullptr;
            if ( loc != nullptr )
            {
                LocSelFlags selFlags = selections.getLocationFlags();
                if ( selFlags == LocSelFlags::Middle ) // Move location
                {
                    bool xInverted = loc->right < loc->left,
                         yInverted = loc->bottom < loc->top;

                    loc->left += dragX;
                    loc->right += dragX;
                    loc->top += dragY;
                    loc->bottom += dragY;
                    s32 xc1Preserve = loc->left,
                        yc1Preserve = loc->top,
                        xc2Preserve = loc->right,
                        yc2Preserve = loc->bottom;
                                        
                    if ( xInverted )
                    {
                        if ( SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX2) )
                        {
                            loc->left += loc->right - xc2Preserve; // Maintain location width
                            dragX += loc->left - xc1Preserve;
                        }
                    }
                    else if ( SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX1) )
                    {
                        loc->right += loc->left - xc1Preserve; // Maintain location width
                        dragX += loc->right - xc2Preserve;
                    }

                    if ( yInverted )
                    {
                        if ( SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY2) )
                        {
                            loc->top += loc->bottom - yc2Preserve; // Maintain location height
                            dragY += loc->top - yc1Preserve;
                        }
                    }
                    else if ( SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY1) )
                    {
                        loc->bottom += loc->top - yc1Preserve; // Maintain location height
                        dragY += loc->bottom - yc2Preserve;
                    }
                                        
                    //undos().addUndoLocationMove(selectedLocation, dragX, dragY);
                    undos.AddUndo(LocationMove::Make(selectedLocation, -dragX, -dragY));
                }
                else // Resize location
                {
                    if ( ((u8)selFlags & (u8)LocSelFlags::North) == (u8)LocSelFlags::North )
                    {
                        if ( loc->top <= loc->bottom ) // Standard yc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Top, loc->top));
                            loc->top += dragY;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY1);
                        }
                        else // Inverted yc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Bottom, loc->bottom));
                            loc->bottom += dragY;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY2);
                        }
                                    
                    }
                    else if ( ((u8)selFlags & (u8)LocSelFlags::South) == (u8)LocSelFlags::South )
                    {
                        if ( loc->top <= loc->bottom ) // Standard yc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Bottom, loc->bottom));
                            loc->bottom += dragY;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY2);
                        }
                        else // Inverted yc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Top, loc->top));
                            loc->top += dragY;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapY1);
                        }
                    }
    
                    if ( ((u8)selFlags & (u8)LocSelFlags::West) == (u8)LocSelFlags::West )
                    {
                        if ( loc->left <= loc->right ) // Standard xc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Left, loc->left));
                            loc->left += dragX;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX1);
                        }
                        else // Inverted xc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Right, loc->right));
                            loc->right += dragX;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX2);
                        }
                    }
                    else if ( ((u8)selFlags & (u8)LocSelFlags::East) == (u8)LocSelFlags::East )
                    {
                        if ( loc->left <= loc->right ) // Standard xc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Right, loc->right));
                            loc->right += dragX;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX2);
                        }
                        else // Inverted xc
                        {
                            undos.AddUndo(LocationChange::Make(selectedLocation, Chk::Location::Field::Left, loc->left));
                            loc->left += dragX;
                            SnapLocationDimensions(loc->left, loc->top, loc->right, loc->bottom, LocSnapFlags::SnapX1);
                        }
                    }
                }
                //undos().submitUndo();
                Redraw(false);
                if ( chkd.locationWindow.getHandle() != NULL )
                    chkd.locationWindow.RefreshLocationInfo();
            }
        }
    }
    else // attempt to select location, if you aren't resizing
    {
        selections.selectLocation(selections.getStartDrag().x, selections.getStartDrag().y, !LockAnywhere());
        if ( chkd.locationWindow.getHandle() != NULL )
            chkd.locationWindow.RefreshLocationInfo();
    
        Redraw(false);
    }
    selections.setLocationFlags(LocSelFlags::None);
}

void GuiMap::UnitLButtonUp(HWND hWnd, int mapX, int mapY, WPARAM wParam)
{
    selections.setEndDrag(mapX, mapY);
    selections.sortDragPoints();
    if ( wParam != MK_CONTROL )
        // Remove selected units
    {
        if ( chkd.unitWindow.getHandle() != nullptr )
        {
            chkd.unitWindow.SetChangeHighlightOnly(true);
            auto &selUnits = selections.getUnits();
            for ( u16 unitIndex : selUnits )
                chkd.unitWindow.DeselectIndex(unitIndex);
            
            chkd.unitWindow.SetChangeHighlightOnly(false);
        }
        selections.removeUnits();
        chkd.unitWindow.UpdateEnabledState();
    }
        
    u16 numUnits = this->numUnits();
    for ( int i=0; i<numUnits; i++ )
    {
        int unitLeft = 0, unitRight  = 0,
            unitTop  = 0, unitBottom = 0;
    
        Chk::UnitPtr unit = layers.getUnit(i);
        
        if ( (u16)unit->type < (u16)Sc::Unit::TotalTypes )
        {
            unitLeft = unit->xc - chkd.scData.units.UnitDat((u16)unit->type)->UnitSizeLeft;
            unitRight = unit->xc + chkd.scData.units.UnitDat((u16)unit->type)->UnitSizeRight;
            unitTop = unit->yc - chkd.scData.units.UnitDat((u16)unit->type)->UnitSizeUp;
            unitBottom = unit->yc + chkd.scData.units.UnitDat((u16)unit->type)->UnitSizeDown;
        }
        else
        {
            unitLeft = unit->xc - chkd.scData.units.UnitDat(0)->UnitSizeLeft;
            unitRight = unit->xc + chkd.scData.units.UnitDat(0)->UnitSizeRight;
            unitTop = unit->yc - chkd.scData.units.UnitDat(0)->UnitSizeUp;
            unitBottom = unit->yc + chkd.scData.units.UnitDat(0)->UnitSizeDown;
        }
    
        if ( selections.getStartDrag().x <= unitRight  && selections.getEndDrag().x >= unitLeft
            && selections.getStartDrag().y <= unitBottom && selections.getEndDrag().y >= unitTop )
        {
            bool wasSelected = selections.unitIsSelected(i);
            if ( wasSelected )
                selections.removeUnit(i);
            else
                selections.addUnit(i);

            if ( chkd.unitWindow.getHandle() != nullptr )
            {
                chkd.unitWindow.SetChangeHighlightOnly(true);
                if ( wasSelected )
                    chkd.unitWindow.DeselectIndex(i);
                else
                    chkd.unitWindow.FocusAndSelectIndex(i);
                
                chkd.unitWindow.SetChangeHighlightOnly(false);
            }
            chkd.unitWindow.UpdateEnabledState();
        }
    }
    Redraw(true);
}

LRESULT GuiMap::ConfirmWindowClose(HWND hWnd)
{
    if ( CanExit() )
        return ClassWindow::WndProc(hWnd, WM_CLOSE, 0, 0);
    else
        return 0;
}

bool GuiMap::GetBackupPath(time_t currTime, std::string &outFilePath)
{
    std::string moduleDirectory;
    if ( GetModuleDirectory(moduleDirectory) )
    {
        tm* currTimes = localtime(&currTime);
        int year = currTimes->tm_year + 1900, month = currTimes->tm_mon + 1, day = currTimes->tm_mday,
            hour = currTimes->tm_hour, minute = currTimes->tm_min, seconds = currTimes->tm_sec;

        MakeDirectory(moduleDirectory + "\\chkd");
        MakeDirectory(moduleDirectory + "\\chkd\\Backups");

        outFilePath = moduleDirectory + std::string("\\chkd\\Backups\\") +
            std::to_string(year) + std::string(month <= 9 ? "-0" : "-") + std::to_string(month) +
            std::string(day <= 9 ? "-0" : "-") + std::to_string(day) + std::string(hour <= 9 ? " 0" : " ") +
            std::to_string(hour) + std::string(minute <= 9 ? "h0" : "h") + std::to_string(minute) +
            std::string(seconds <= 9 ? "m0" : "m") + std::to_string(seconds) + std::string("s ") + std::string(GetFileName());

        return true;
    }
    outFilePath.clear();
    return false;
}

bool GuiMap::TryBackup(bool &outCopyFailed)
{
    outCopyFailed = false;
    if ( doAutoBackups && MapFile::getFilePath().length() > 0 )
    {
        time_t currTime = time(0);
        // If there are no previous backups or enough time has elapsed since the last...
        if ( (lastBackupTime == -1 || difftime(lastBackupTime, currTime) >= minSecondsBetweenBackups) )
        {
            std::string backupPath;
            if ( GetBackupPath(currTime, backupPath) && MakeFileCopy(MapFile::getFilePath(), backupPath) )
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
