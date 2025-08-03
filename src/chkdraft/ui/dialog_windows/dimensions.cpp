#include "dimensions.h"
#include "chkdraft/chkdraft.h"
#include <common_files/common_files.h>

DimensionsWindow::DimensionsWindow()
{

}

DimensionsWindow::~DimensionsWindow()
{

}

bool DimensionsWindow::CreateThis(HWND hParent)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_DIMENSIONS), hParent) )
    {
        ShowNormal();
        return true;
    }
    else if ( getHandle() != NULL )
        Show();

    return false;
}

bool DimensionsWindow::DestroyThis()
{
    return ClassDialog::DestroyDialog();
}

void DimensionsWindow::RefreshWindow()
{
    listTileset.SetCurSel(CM->getTileset());
    UpdateTerrainList();
    editWidth.SetEditNum<u16>(u16(CM->getTileWidth()));
    editHeight.SetEditNum<u16>(u16(CM->getTileHeight()));
}

void DimensionsWindow::UpdateTerrainList()
{
    listTerrain.ClearItems();
    int tilesetIndex = Sc::Terrain::Tileset::Badlands;
    if ( listTileset.GetCurSel(tilesetIndex) )
    {
        const auto & tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(tilesetIndex));
        for ( const auto & brushType : tileset.brushes )
        {
            LRESULT insertionIndex = SendMessage(listTerrain.getHandle(), LB_ADDSTRING, 0, (LPARAM)icux::toUistring(std::string(brushType.name)).c_str());
            if ( insertionIndex != LB_ERR && insertionIndex != LB_ERRSPACE )
                SendMessage(listTerrain.getHandle(), LB_SETITEMDATA, (WPARAM)insertionIndex, (LPARAM)brushType.index);
        }
        listTerrain.SetCurSel(tileset.defaultBrush.brushSortOrder);
    }
}

void DimensionsWindow::ResizeChangeTileset()
{
    auto newWidth = editWidth.GetEditNum<u16>();
    auto newHeight = editHeight.GetEditNum<u16>();
    int newTilesetIndex = CM->getTileset();
    listTileset.GetCurSel(newTilesetIndex);
    const auto & tileset = chkd.scData.terrain.get(Sc::Terrain::Tileset(newTilesetIndex));
    LPARAM newTerrainTypeIndex = 0;
    listTerrain.GetCurSelItem(newTerrainTypeIndex);
    auto newLeft = editLeft.GetEditNum<int>();
    auto newTop = editTop.GetEditNum<int>();
    bool attemptBorderSmoothing = checkSmoothBorder.isChecked();
    if ( !newWidth )
        Error("Invalid width");
    else if ( !newHeight )
        Error("Invalid height");
    else if ( !newLeft )
        Error("Invalid left");
    else if ( !newTop )
        Error("Invalid top");
    else
    {
        u16 width = *newWidth;
        u16 height = *newHeight;
        int leftOffset = *newLeft;
        int topOffset = *newTop;
        if ( width == CM->getTileWidth() && height == CM->getTileHeight() && leftOffset == 0 && topOffset == 0 &&
             Sc::Terrain::Tileset(newTilesetIndex) == CM->getTileset() )
        {
            DestroyThis(); // No change
        }
        else
        {
            if ( Sc::Terrain::Tileset(newTilesetIndex) != CM->getTileset() )
                CM->setTileset(Sc::Terrain::Tileset(newTilesetIndex));

            try {
                if ( width != CM->getTileWidth() || height != CM->getTileHeight() || leftOffset != 0 || topOffset != 0 )
                    CM->setDimensions(width, height, Scenario::SizeValidationFlag::Default, leftOffset, topOffset, size_t(newTerrainTypeIndex));

                CM->refreshScenario();
                DestroyThis();
            }  catch ( std::exception & e ) {
                CM->undo();
                CM->refreshScenario();
                DestroyThis();
                Error(e.what());
            }
        }
    }
}

void DimensionsWindow::CreateSubWindows(HWND hWnd)
{
    editWidth.FindThis(hWnd, IDC_EDIT_WIDTH);
    editHeight.FindThis(hWnd, IDC_EDIT_HEIGHT);
    editLeft.FindThis(hWnd, IDC_EDIT_LEFT);
    editTop.FindThis(hWnd, IDC_EDIT_TOP);
    listTileset.FindThis(hWnd, IDC_LIST_TILESET);
    listTerrain.FindThis(hWnd, IDC_LIST_TERRAIN);
    checkSmoothBorder.FindThis(hWnd, IDC_CHECK_SMOOTHBORDER);
    listTileset.AddStrings(tilesetNames);
    RefreshWindow();
    editLeft.SetEditNum<int>(0);
    editTop.SetEditNum<int>(0);
    checkSmoothBorder.DisableThis();
    //checkSmoothBorder.SetCheck(true);
}

BOOL DimensionsWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case BN_CLICKED:
        switch ( LOWORD(wParam) )
        {
        case IDCANCEL:
            DestroyThis();
            break;
        case ID_RESIZE_CHANGETILESET:
            ResizeChangeTileset();
            break;
        }
        break;
    case LBN_SELCHANGE:
        if ( LOWORD(wParam) == IDC_LIST_TILESET )
            UpdateTerrainList();
        break;
    }
    return TRUE;
}

BOOL DimensionsWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_INITDIALOG:
            {
                SetSmallIcon(WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16));
                CreateSubWindows(hWnd);
                defaultChildFonts();
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;

        case WM_CLOSE:
            DestroyThis();
            break;

        default:
            return FALSE;
            break;
    }
    return TRUE;
}
