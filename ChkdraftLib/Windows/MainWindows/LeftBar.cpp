#include "LeftBar.h"
#include "../../Chkdraft.h"

enum ID
{
    IDR_MAIN_TREE,
    IDR_LEFT_BAR,
    IDR_MINIMAP
};

LeftBar::~LeftBar()
{

}

bool LeftBar::CreateThis(HWND hParent)
{
    return RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), CreateSolidBrush(RGB(240, 240, 240)), NULL, "LeftBar", NULL, false) &&
           CreateClassWindow(WS_EX_CLIENTEDGE, "", WS_CHILD|WS_THICKFRAME, 0, 0, 162, 50, hParent, (HMENU)IDR_LEFT_BAR);
}

void LeftBar::NotifyTreeSelChanged(LPARAM newValue)
{
    if ( CM != nullptr )
    {
        LPARAM itemType = newValue&TreeTypePortion,
            itemData = newValue&TreeDataPortion;

        switch ( itemType )
        {
            //case TREE_TYPE_ROOT: // Same as category
        case TreeTypeCategory: if ( CM->getLayer() != (Layer)itemData ) chkd.maps.ChangeLayer((Layer)itemData); break; // The layer was AND'd with the category
        case TreeTypeIsom: if ( CM->getLayer() != Layer::Terrain ) chkd.maps.ChangeLayer(Layer::Terrain); break;
        case TreeTypeUnit: if ( CM->getLayer() != Layer::Units ) chkd.maps.ChangeLayer(Layer::Units); break;
        case TreeTypeLocation: if ( CM->getLayer() != Layer::Locations ) chkd.maps.ChangeLayer(Layer::Locations); break;
        case TreeTypeSprite: if ( CM->getLayer() != Layer::Sprites ) chkd.maps.ChangeLayer(Layer::Sprites); break;
        case TreeTypeDoodad: if ( CM->getLayer() != Layer::Doodads ) chkd.maps.ChangeLayer(Layer::Doodads); break;
        }

        switch ( itemType )
        {
        case TreeTypeUnit: // itemData == UnitID
        {
            CM->clearSelectedUnits();
            chkd.maps.endPaste();
            if ( CM->getLayer() != Layer::Units )
                chkd.maps.ChangeLayer(Layer::Units);

            Chk::UnitPtr unit = Chk::UnitPtr(new Chk::Unit);
            unit->energyPercent = 100;
            unit->hangerAmount = 0;
            unit->hitpointPercent = 100;
            unit->type = (Sc::Unit::Type)itemData;
            unit->relationClassId = 0;
            unit->relationFlags = 0;
            unit->owner = CM->getCurrPlayer();
            unit->resourceAmount = 0;
            unit->classId = 0;
            unit->shieldPercent = 100;
            unit->validStateFlags = 0;
            unit->stateFlags = 0;
            unit->unused = 0;
            unit->validFieldFlags = 0;
            unit->xc = 0;
            unit->yc = 0;
            chkd.maps.clipboard.addQuickUnit(unit);
            chkd.maps.startPaste(true);
            //SetFocus(CM->getHandle());
        }
        break;

        case TreeTypeLocation: // itemData = location index
            CM->GetSelections().selectLocation(u16(itemData));
            if ( chkd.locationWindow.getHandle() != nullptr )
                chkd.locationWindow.RefreshLocationInfo();
            CM->viewLocation(u16(itemData));
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
        case WM_SIZE:
            {
                HWND hTree = mainTree.getHandle();
                HWND hMiniMap = miniMap.getHandle();
                RECT rcMain, rcTool, rcStatus, rcLeftBar;

                //Get the size of the client area, toolbar, status bar, and left bar
                    GetClientRect(chkd.getHandle(), &rcMain);
                    GetWindowRect(chkd.mainToolbar.getHandle(), &rcTool);
                    GetWindowRect(chkd.statusBar.getHandle(), &rcStatus);
                    GetWindowRect(hWnd, &rcLeftBar);

                int border = GetSystemMetrics(SM_CXSIZEFRAME)-1;
                // Interrupt any attempt to vertically resize the left bar
                    SetWindowPos( hWnd, NULL,
                                  -border, -border,
                                  rcLeftBar.right-rcLeftBar.left,
                                  rcStatus.top-rcTool.bottom+border*2, 
                                  SWP_NOZORDER
                                );
                // Fit the map MDI window to the area right of the left bar and between the toolbar and statusbar
                    SetWindowPos( chkd.maps.getHandle(), NULL,
                                  rcLeftBar.right-rcLeftBar.left-border-2,
                                  rcTool.bottom-rcTool.top,
                                  rcMain.right-rcMain.left-rcLeftBar.right+rcLeftBar.left+border+2,
                                  rcStatus.top-rcTool.bottom+2,
                                  SWP_NOZORDER);
                // Fit the minimap to the center of the top part of the left bar
                    SetWindowPos(hMiniMap, NULL, (rcLeftBar.right-rcLeftBar.left-(132+4+border*2))/2, 3, 132, 132, SWP_NOZORDER);
                //Fit the tree to the bottom part of the left bar
                    GetClientRect(hWnd, &rcLeftBar);
                    SetWindowPos(hTree, NULL, -2, 145, rcLeftBar.right-rcLeftBar.left+2, rcLeftBar.bottom-rcLeftBar.top-146/*rcStatus.top-rcTool.bottom-146*/, SWP_NOZORDER);
            }
            break;

        //case WM_SIZING:
        //  SendMessage(hWnd, WM_SIZE, wParam, lParam);
        //  break;

        case WM_GETMINMAXINFO:
            {
                LPMINMAXINFO minmax = (LPMINMAXINFO)lParam;
                             minmax->ptMinTrackSize.x = 151;
            }
            break;

        case WM_CREATE:
            {
                miniMap.CreateThis(hWnd, IDR_MINIMAP);

                if ( mainTree.CreateThis(hWnd, -2, 14, 162, 150, true, IDR_MAIN_TREE) )
                {
                    SendMessage(mainTree.getHandle(), WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
                    mainTree.unitTree.UpdateUnitNames(Sc::Unit::defaultDisplayNames);
                    mainTree.BuildMainTree();
                }
            }
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
