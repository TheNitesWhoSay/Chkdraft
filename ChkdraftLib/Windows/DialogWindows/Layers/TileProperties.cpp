#include "TileProperties.h"
#include "../../../Chkdraft.h"

TilePropWindow::~TilePropWindow()
{

}

bool TilePropWindow::CreateThis(HWND hParent)
{
    return ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_DIALOG_SETTILE), hParent);
}

bool TilePropWindow::DestroyThis()
{
    return ClassDialog::DestroyDialog();
}

void TilePropWindow::UpdateTile()
{
    TileNode tile = CM->GetSelections().getFirstTile();
    SetWinText("Tile Properties (" + std::to_string(tile.xc) + ", " + std::to_string(tile.yc) +")");
    editTileValue.SetEditNum<u16>(tile.value);
}

BOOL TilePropWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case EN_UPDATE:
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
        break;
    }
    switch ( LOWORD(wParam) )
    {
    case IDOK:
    {
        u16 tile = 0;
        editTileValue.GetEditNum<u16>(tile);
        if ( tile > 65535 )
            tile %= 65536;

        TileNode tileNode = CM->GetSelections().getFirstTile();
        tileNode.value = tile;
        CM->SetTile(tileNode.xc, tileNode.yc, tile);
        EndDialog(hWnd, IDOK);
        break;
    }

    case IDCANCEL:
        EndDialog(hWnd, IDCANCEL);
        break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL TilePropWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            {
                editTileValue.FindThis(hWnd, IDC_EDIT_TILEVALUE);
                editTileValue.SetTextLimit(10);
                UpdateTile();
                PostMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)editTileValue.getHandle(), true);
                return true;
            }
            break;

        case WM_PAINT:
            {
                const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(CM->getTileset());
                if ( auto tile = editTileValue.GetEditNum<u16>() )
                {
                    BITMAPINFO bmi = GetBMI(32, 32);
                    WinLib::DeviceContext tileDc(WindowsItem::getHandle(), RECT{55, 50, 55+32, 50+32}); // Setup buffer to draw tile graphics

                    DrawTile(tileDc, CM->getPalette(), tiles, 0, 0, *tile, bmi, 0, 0, 0);
                    tileDc.flushBuffer();

                    tileDc.setBuffer(RECT{350, 50, 350+36, 50+36}); // Setup buffer to draw elevations
                    tileDc.fillRect(RECT{0, 0, 36, 36}, RGB(166, 156, 132));
                    BITMAPINFO bmiMini = GetBMI(8, 8);
                    for ( int yMiniTile=0; yMiniTile<4; yMiniTile++ )
                    {
                        for ( int xMiniTile=0; xMiniTile<4; xMiniTile++ )
                            DrawMiniTileElevation(tileDc, tiles, xMiniTile*9, yMiniTile*9, *tile, xMiniTile, yMiniTile, bmiMini);
                    }
                    tileDc.flushBuffer();
                    tileDc.release();
                }
            }
            break;

        case WM_DESTROY:
            EndDialog(hWnd, IDCANCEL);
            break;

        default:
            return false;
            break;
    }
    return 0;
}
