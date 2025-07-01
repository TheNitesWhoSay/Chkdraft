#include "left_bar.h"
#include "chkdraft/chkdraft.h"
#include <CommCtrl.h>

enum_t(Id, u32, {
    IDR_MAIN_TREE,
    IDR_LEFT_BAR,
    IDR_MINIMAP,
    IDR_HISTORY_TREE
});

LeftBar::~LeftBar()
{

}

bool LeftBar::CreateThis(HWND hParent)
{
    return RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), WinLib::ResourceManager::getSolidBrush(RGB(240, 240, 240)), NULL, "LeftBar", NULL, false) &&
           CreateClassWindow(WS_EX_CLIENTEDGE, "", WS_CHILD|WS_THICKFRAME, 0, 0, 162, 50, hParent, (HMENU)Id::IDR_LEFT_BAR);
}

void LeftBar::NotifyTreeItemSelected(LPARAM newValue)
{
    if ( CM != nullptr && !blockSelections )
    {
        LPARAM itemType = newValue&TreeTypePortion,
            itemData = newValue&TreeDataPortion;

        switch ( itemType )
        {
        case TreeTypeRoot: // Same as category
        case TreeTypeCategory: if ( CM->getLayer() != (Layer)itemData ) chkd.maps.ChangeLayer((Layer)itemData); break; // The layer was AND'd with the category
        case TreeTypeIsom: chkd.maps.ChangeSubLayer(TerrainSubLayer::Isom); break;
        case TreeTypeUnit: if ( CM->getLayer() != Layer::Units ) chkd.maps.ChangeLayer(Layer::Units); break;
        case TreeTypeLocation: if ( CM->getLayer() != Layer::Locations ) chkd.maps.ChangeLayer(Layer::Locations); break;
        case TreeTypeSprite: if ( CM->getLayer() != Layer::Sprites ) chkd.maps.ChangeLayer(Layer::Sprites); break;
        case TreeTypeSpriteUnit: if ( CM->getLayer() != Layer::Sprites ) chkd.maps.ChangeLayer(Layer::Sprites); break;
        case TreeTypeDoodad: if ( CM->getLayer() != Layer::Doodads ) chkd.maps.ChangeLayer(Layer::Doodads); break;
        }

        switch ( itemType )
        {
        case TreeTypeIsom: // itemData == terrainTypeIndex
            chkd.maps.clipboard.setQuickIsom(itemData);
            break;

        case TreeTypeDoodad:
            chkd.maps.clipboard.setQuickDoodad(u16(itemData));
            CM->Redraw(false);
            break;

        case TreeTypeUnit: // itemData == UnitID
        {
            CM->clearSelectedUnits();
            chkd.maps.endPaste();
            if ( CM->getLayer() != Layer::Units )
                chkd.maps.ChangeLayer(Layer::Units);

            Chk::Unit unit {};
            unit.validStateFlags = Chk::Unit::State::Invincible;
            const auto & unitDat = chkd.scData.units.getUnit(Sc::Unit::Type(itemData));

            if ( (unitDat.flags & Sc::Unit::Flags::FlyingBuilding) == Sc::Unit::Flags::FlyingBuilding )
                unit.validStateFlags |= Chk::Unit::State::InTransit;

            if ( (unitDat.flags & Sc::Unit::Flags::Burrowable) == Sc::Unit::Flags::Burrowable )
                unit.validStateFlags |= Chk::Unit::State::Burrow;

            if ( (unitDat.flags & Sc::Unit::Flags::Cloakable) == Sc::Unit::Flags::Cloakable )
                unit.validStateFlags |= Chk::Unit::State::Cloak;

            if ( (unitDat.flags & Sc::Unit::Flags::Invincible) == Sc::Unit::Flags::Invincible )
                unit.validStateFlags &= Chk::Unit::State::xInvincible;

            if ( (unitDat.flags & Sc::Unit::Flags::Invincible) == 0 && (unitDat.flags & Sc::Unit::Flags::Building) == 0 )
                unit.validStateFlags |= Chk::Unit::State::Hallucinated;

            unit.energyPercent = 100;
            unit.hangarAmount = 0;
            unit.hitpointPercent = 100;
            unit.type = (Sc::Unit::Type)itemData;
            unit.relationClassId = 0;
            unit.relationFlags = 0;
            unit.owner = CM->getCurrPlayer();
            unit.resourceAmount = 0;
            unit.classId = 0;
            unit.shieldPercent = 100;
            unit.stateFlags = 0;
            unit.unused = 0;

            unit.validFieldFlags = Chk::Unit::ValidField::Owner;

            if ( (unitDat.flags & Sc::Unit::Flags::Spellcaster) == Sc::Unit::Flags::Spellcaster )
                unit.validFieldFlags |= Chk::Unit::ValidField::Energy;

            if ( unitDat.shieldEnable != 0 )
                unit.validFieldFlags |= Chk::Unit::ValidField::Shields;

            if ( (unitDat.flags & Sc::Unit::Flags::ResourceContainer) == Sc::Unit::Flags::ResourceContainer )
            {
                unit.validFieldFlags |= Chk::Unit::ValidField::Resources;
                switch ( unit.type )
                {
                case Sc::Unit::Type::VespeneGeyser: unit.resourceAmount = 5000; break;
                case Sc::Unit::Type::TerranRefinery: unit.resourceAmount = 5000; break;
                case Sc::Unit::Type::ZergExtractor: unit.resourceAmount = 5000; break;
                case Sc::Unit::Type::ProtossAssimilator: unit.resourceAmount = 5000; break;
                default: unit.resourceAmount = 1500; break;
                }
            }

            if ( unit.type == Sc::Unit::Type::ProtossCarrier || unit.type == Sc::Unit::Type::Gantrithor_Carrier ||
                 unit.type == Sc::Unit::Type::ProtossReaver || unit.type == Sc::Unit::Type::Warbringer_Reaver )
            {
                unit.validFieldFlags |= Chk::Unit::ValidField::Hangar;
            }

            unit.xc = 0;
            unit.yc = 0;
            chkd.maps.clipboard.addQuickUnit(unit);
            chkd.maps.startPaste(true);
            //SetFocus(CM->getHandle());
        }
        break;

        case TreeTypeLocation: // itemData = location index
            if ( CM->GetSelectedLocation() != u16(itemData) )
            {
                CM->selections.selectLocation(u16(itemData));
                if ( chkd.locationWindow.getHandle() != nullptr )
                    chkd.locationWindow.RefreshLocationInfo();
                CM->viewLocation(u16(itemData));
            }
            break;

        case TreeTypeSprite: // itemData = sprite index
            {
                CM->clearSelectedSprites();
                chkd.maps.endPaste();
                if ( CM->getLayer() != Layer::Sprites )
                    chkd.maps.ChangeLayer(Layer::Sprites);

                Chk::Sprite sprite {};
                sprite.type = (Sc::Sprite::Type)itemData;
                sprite.owner = CM->getCurrPlayer();
                sprite.flags = Chk::Sprite::toPureSpriteFlags(chkd.scData.terrain.doodadSpriteFlags[itemData]);

                chkd.maps.clipboard.addQuickSprite(sprite);
                chkd.maps.startPaste(true);
            }
            break;

        case TreeTypeSpriteUnit: // itemData = sprite index
            {
                CM->clearSelectedSprites();
                chkd.maps.endPaste();
                if ( CM->getLayer() != Layer::Sprites )
                    chkd.maps.ChangeLayer(Layer::Sprites);

                Chk::Sprite sprite {};
                sprite.type = (Sc::Sprite::Type)itemData;
                sprite.owner = CM->getCurrPlayer();
                sprite.flags = Chk::Sprite::toSpriteUnitFlags(chkd.scData.terrain.doodadSpriteFlags[itemData]);

                chkd.maps.clipboard.addQuickSprite(sprite);
                chkd.maps.startPaste(true);
            }
            break;
        }
    }
}

LRESULT LeftBar::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        
    }
    else if ( nmhdr->code == NM_DBLCLK )
    {
        TVITEM item = {};
        item.mask = TVIF_PARAM;
        item.hItem = TreeView_GetSelection(nmhdr->hwndFrom);
        if ( item.hItem != NULL && TreeView_GetItem(nmhdr->hwndFrom, &item) == TRUE )
        {
            LPARAM itemType = item.lParam&TreeTypePortion;
            if ( itemType == TreeTypeLocation )
            {
                if ( chkd.locationWindow.getHandle() == NULL )
                {
                    if ( chkd.locationWindow.CreateThis(chkd.getHandle()) )
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
    else if ( nmhdr->code == NM_RETURN )
        return 1;
    
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT LeftBar::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_NCHITTEST:
        {
            auto result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            if ( result == HTLEFT || result == HTTOPLEFT || result == HTTOP || result == HTTOPRIGHT ||
                 result == HTBOTTOMRIGHT || result == HTBOTTOM || result == HTBOTTOMLEFT )
                return HTCLIENT; // Prevent use of any sizing border except right as a sizing border
            else
                return result;
        }
        break;

        case WM_SIZE:
            {
                // Get the size of the client area, toolbar, status bar, and left bar
                RECT rcMain, rcTool, rcStatus, rcLeftBar;
                GetClientRect(chkd.getHandle(), &rcMain);
                GetWindowRect(chkd.mainToolbar.getHandle(), &rcTool);
                GetWindowRect(chkd.statusBar.getHandle(), &rcStatus);
                GetWindowRect(hWnd, &rcLeftBar);

                int xBorder = GetSystemMetrics(SM_CXSIZEFRAME),
                    yBorder = GetSystemMetrics(SM_CYSIZEFRAME);

                // Interrupt any attempt to vertically resize the left bar
                SetWindowPos(hWnd, NULL, 1-xBorder, 1-yBorder, rcLeftBar.right-rcLeftBar.left, rcStatus.top-rcTool.bottom + 2*(yBorder-1), SWP_NOZORDER);

                // Fit logger to the area between the left bar and right edge without changing the height
                SetWindowPos(chkd.mainPlot.loggerWindow.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - 3*xBorder,
                    rcMain.bottom-rcMain.top+2*yBorder-1-chkd.mainPlot.loggerWindow.Height()-(rcStatus.bottom-rcStatus.top)-(rcTool.bottom-rcTool.top),
                    rcMain.right - rcMain.left - (rcLeftBar.right - rcLeftBar.left) + 4*xBorder+5, chkd.mainPlot.loggerWindow.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

                // Fit the map MDI window to the area right of the left bar and between the toolbar and statusbar
                SetWindowPos(chkd.maps.getHandle(), NULL, rcLeftBar.right-rcLeftBar.left-xBorder+1, rcTool.bottom-rcTool.top,
                    rcMain.right-rcMain.left-rcLeftBar.right+rcLeftBar.left+xBorder-1, chkd.mainPlot.loggerWindow.IsVisible() ? chkd.mainPlot.loggerWindow.Top() : rcStatus.top - rcTool.bottom, SWP_NOZORDER);

                // Fit the minimap to the center of the top part of the left bar
                SetWindowPos(miniMap.getHandle(), NULL, (rcLeftBar.right-rcLeftBar.left-(132+2*(xBorder+1)))/2-3, 5, 132, 132, SWP_NOZORDER);

                // Fit the history tree to the bottom part of the left bar
                int totalTreeHeight = rcLeftBar.bottom-rcLeftBar.top-146;
                int historyTreeHeight = int((rcLeftBar.bottom-rcLeftBar.top-146)*historyTreeSize);
                int mainTreeHeight = totalTreeHeight-historyTreeHeight;
                GetClientRect(hWnd, &rcLeftBar);
                SetWindowPos(historyTree.getHandle(), NULL, -2, 145+mainTreeHeight, rcLeftBar.right-rcLeftBar.left+2, historyTreeHeight, SWP_NOZORDER);

                // Fit the main tree to the middle part of the left bar
                GetClientRect(hWnd, &rcLeftBar);
                SetWindowPos(mainTree.getHandle(), NULL, -2, 145, rcLeftBar.right-rcLeftBar.left+2, mainTreeHeight, SWP_NOZORDER);
            }
            break;

        case WM_GETMINMAXINFO:
            {
                LPMINMAXINFO minmax = (LPMINMAXINFO)lParam;
                             minmax->ptMinTrackSize.x = 151;
            }
            break;

        case WM_CREATE:
            {
                miniMap.CreateThis(hWnd, Id::IDR_MINIMAP);

                if ( mainTree.CreateThis(hWnd, -2, 14, 162, 36, true, Id::IDR_MAIN_TREE) )
                {
                    mainTree.setDefaultFont();
                    mainTree.unitTree.UpdateUnitNames(Sc::Unit::defaultDisplayNames);
                    this->blockSelections = true;
                    mainTree.BuildMainTree();
                    this->blockSelections = false;
                }

                if ( historyTree.CreateThis(hWnd, -2, 50, 162, 114, true, Id::IDR_HISTORY_TREE) )
                {
                    historyTree.setDefaultFont();
                    historyTree.createRoot();
                }
            }
            break;

        case WM_MOUSEWHEEL:
            if ( CM != nullptr && chkd.maps.clipboard.isPasting() )
                SendMessage(CM->getHandle(), WM_MOUSEWHEEL, wParam, lParam);
            else
                return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
