#include "new_map.h"
#include "chkdraft/chkdraft.h"
#include <mapping_core/mapping_core.h>

enum_t(Id, u32, {
    COMBO_TRIGS = ID_FIRST
});

NewMap::~NewMap()
{

}

void NewMap::CreateThis(HWND hParent)
{
    ClassDialog::CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_NEW), hParent);
}

BOOL NewMap::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case ID_CREATEMAP:
    {
        LPARAM terrainTypeIndex = 0;
        int width = 0, height = 0, tileset = 0, triggers = 0;

        editWidth.GetEditNum<int>(width);
        editHeight.GetEditNum<int>(height);
        listInitialTileset.GetCurSel(tileset);
        listInitialTerrain.GetCurSelItem(terrainTypeIndex);
        triggers = dropDefaultTriggers.GetSel();

        if ( width>0 && height>0 )
        {
            if ( width > 65535 ) width %= 65536;
            if ( height > 65535 ) height %= 65536;
            if ( (u32)width*height < 429496729 )
            {
                if ( chkd.maps.NewMap((Sc::Terrain::Tileset)tileset, width, height, size_t(terrainTypeIndex), (Chk::DefaultTriggers)triggers) != nullptr )
                {
                    CM->Scroll(true, true, false);
                    CM->Redraw(true);
                }
                EndDialog(hWnd, ID_CREATEMAP);
            }
            else
            {
                Error("Maps cannot be over 2gb");
                EndDialog(hWnd, IDCANCEL);
            }
        }
        else
            EndDialog(hWnd, IDCANCEL);
    }
    break;

    case IDCANCEL:
        EndDialog(hWnd, IDCANCEL);
        break;

    default:
        if ( HIWORD(wParam) == LBN_SELCHANGE )
        {
            int itemIndex = (int)SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            if ( LOWORD(wParam) == IDC_LIST_TILESET )
            {
                HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
                HWND hDefaultTrigs = GetDlgItem(hWnd, Id::COMBO_TRIGS);
                SendMessage(hInitialTerrain, LB_RESETCONTENT, 0, 0);
                const auto & tileset = chkd.scData->terrain.get(Sc::Terrain::Tileset(itemIndex));
                for ( const auto & brushType : tileset.brushes )
                {
                    LRESULT insertionIndex = SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(std::string(brushType.name)).c_str());
                    if ( insertionIndex != LB_ERR && insertionIndex != LB_ERRSPACE )
                        SendMessage(hInitialTerrain, LB_SETITEMDATA, (WPARAM)insertionIndex, (LPARAM)brushType.index);
                }
                listInitialTerrain.SetCurSel(tileset.defaultBrush.brushSortOrder);
            }
        }
        break;
    }
    return TRUE;
}

BOOL NewMap::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_INITDIALOG:
            {
                WinLib::GroupBoxControl triggersGroup;
                triggersGroup.FindThis(hWnd, IDC_TRIGGERS_GROUP);
                RECT rcTriggersGroup;
                triggersGroup.getWindowRect(rcTriggersGroup);
                POINT upperLeft;
                upperLeft.x = rcTriggersGroup.left;
                upperLeft.y = rcTriggersGroup.top;
                ScreenToClient(hWnd, &upperLeft);

                WindowsItem::setDefaultFont(false);
                const std::vector<std::string> defaultTriggerTitles = {
                    "No triggers", "Default melee triggers", "Melee with observers (2 players)",
                    "Melee with observers (3 players)", "Melee with observers (4 players)", "Melee with observers (5 players)",
                    "Melee with observers (6 players)", "Melee with observers (7 players)"
                };
                dropDefaultTriggers.CreateThis(hWnd, upperLeft.x+10, upperLeft.y+20, 305, 200, false, false, Id::COMBO_TRIGS, defaultTriggerTitles);
                dropDefaultTriggers.SetSel(0);

                editWidth.FindThis(hWnd, IDC_EDIT_WIDTH);
                editWidth.SetTextLimit(10);
                editWidth.SetText("128");

                editHeight.FindThis(hWnd, IDC_EDIT_HEIGHT);
                editHeight.SetTextLimit(10);
                editHeight.SetText("128");

                listInitialTileset.FindThis(hWnd, IDC_LIST_TILESET);
                listInitialTileset.AddStrings(tilesetNames);
                listInitialTileset.SetCurSel(0);
                listInitialTileset.setDefaultFont(false);

                listInitialTerrain.FindThis(hWnd, IDC_LIST_DEFAULTTERRAIN);

                std::vector<std::string> badlandsTerrainTypes {};
                const auto & tileset = chkd.scData->terrain.get(Sc::Terrain::Tileset::Badlands);
                HWND hInitialTerrain = listInitialTerrain.getHandle();
                for ( const auto & brushType : tileset.brushes )
                {
                    LRESULT insertionIndex = SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(std::string(brushType.name)).c_str());
                    if ( insertionIndex != LB_ERR && insertionIndex != LB_ERRSPACE )
                        SendMessage(hInitialTerrain, LB_SETITEMDATA, (WPARAM)insertionIndex, (LPARAM)brushType.index);
                }

                listInitialTerrain.SetCurSel(tileset.defaultBrush.brushSortOrder);
                listInitialTerrain.setDefaultFont(false);
                return TRUE;
            }
            break;
    }
    return FALSE;
}
