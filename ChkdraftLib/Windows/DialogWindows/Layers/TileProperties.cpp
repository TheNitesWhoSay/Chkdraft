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
                RECT rect;
                GetClientRect(hWnd, &rect);
                int width = 32,
                    height = 32;
                PAINTSTRUCT ps;
                HDC hDC = BeginPaint(hWnd, &ps),
                    MemhDC = CreateCompatibleDC(hDC);
                HBITMAP Membitmap = CreateCompatibleBitmap(hDC, width, height);
                SelectObject(MemhDC, Membitmap);

                u16 tile = 0;
                editTileValue.GetEditNum<u16>(tile);

                const Sc::Terrain::Tiles & tiles = chkd.scData.terrain.get(CM->layers.getTileset());

                HBRUSH brush = CreateSolidBrush(RGB(166, 156, 132));
                FillRect(MemhDC, &rect, brush);
                DeleteObject(brush);

                BITMAPINFO bmi = GetBMI(32, 32);
                DrawTile(MemhDC, CM->getPalette(), tiles, 0, 0, tile, bmi, 0, 0, 0);
                BitBlt(hDC, 55, 50, width, height, MemhDC, 0, 0, SRCCOPY);

                BITMAPINFO bmiMini = GetBMI(8, 8);
                for ( int yMiniTile=0; yMiniTile<4; yMiniTile++ )
                {
                    for ( int xMiniTile=0; xMiniTile<4; xMiniTile++ )
                        DrawMiniTileElevation(hDC, tiles, 350+xMiniTile*9, 50+yMiniTile*9, tile, xMiniTile, yMiniTile, bmiMini);
                }
                
                DeleteObject(Membitmap);
                DeleteDC    (MemhDC);
                DeleteDC    (hDC);
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
