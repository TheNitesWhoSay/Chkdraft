#include "NewMap.h"
#include "../../../Chkdraft.h"
#include "../../../../MappingCoreLib/MappingCore.h"

enum class Id {
    COMBO_TRIGS = ID_FIRST
};

NewMap::~NewMap()
{

}

void NewMap::CreateThis(HWND hParent)
{
    ClassDialog::CreateDialogBox(MAKEINTRESOURCE(IDD_DIALOG_NEW), hParent);
    //SetFocus(chkdgetHandle());
}

BOOL NewMap::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case ID_CREATEMAP:
    {
        int width = 0, height = 0, tileset = 0, terrain = 0, triggers = 0;

        editWidth.GetEditNum<int>(width);
        editHeight.GetEditNum<int>(height);
        listInitialTileset.GetCurSel(tileset);
        listInitialTerrain.GetCurSel(tileset);
        triggers = dropDefaultTriggers.GetSel();

        if ( width>0 && height>0 )
        {
            if ( width > 65535 ) width %= 65536;
            if ( height > 65535 ) height %= 65536;
            if ( (u32)width*height < 429496729 )
            {
                if ( chkd.maps.NewMap(width, height, (Sc::Terrain::Tileset)tileset, terrain, triggers) )
                {
                    CM->Scroll(true, true, false);

                    // Tiling Code
                    /*u16 tilenum = 0;
                    u16 xSize = CM->XSize();
                    for ( u32 xStart = 0; xStart<CM->XSize(); xStart += 16 )
                    {
                        for ( u16 yc = 0; yc<CM->YSize(); yc++ )
                        {
                            for ( u16 xc = (u16)xStart; xc<xStart + 16; xc++ )
                            {
                                CM->SetTile(xc, yc, tilenum);
                                tilenum++;
                            }
                        }
                    }*/

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
                HWND hDefaultTrigs = GetDlgItem(hWnd, (int)Id::COMBO_TRIGS);
                SendMessage(hInitialTerrain, LB_RESETCONTENT, 0, 0);
                switch ( itemIndex )
                {
                case (u32)Sc::Terrain::Tileset::Badlands: // Badlands
                    for (auto initTerrain : badlandsInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::SpacePlatform: // Space Platform
                    for (auto initTerrain : spaceInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Installation: // Installation
                    for (auto initTerrain : installInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Ashworld: // Ash World
                    for (auto initTerrain : ashInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Jungle: // Jungle World
                    for (auto initTerrain : jungInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Desert: // Desert World
                    for (auto initTerrain : desertInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Arctic: // Ice World
                    for (auto initTerrain : iceInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                case (u32)Sc::Terrain::Tileset::Twilight: // Twilight World
                    for (auto initTerrain : twilightInitTerrain)
                        SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)icux::toUistring(initTerrain).c_str());
                    SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
                    break;
                }
                SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
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

                SetFont(defaultFont, false);
                const std::vector<std::string> defaultTriggerTitles = {
                    "No triggers", "Default melee triggers", "Melee with observers (2 players)",
                    "Melee with observers (3 players)", "Melee with observers (4 players)", "Melee with observers (5 players)",
                    "Melee with observers (6 players)", "Melee with observers (7 players)"
                };
                dropDefaultTriggers.CreateThis(hWnd, upperLeft.x+10, upperLeft.y+20, 305, 200, false, false, (u64)Id::COMBO_TRIGS, defaultTriggerTitles, defaultFont);
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
                listInitialTileset.SetFont(defaultFont, false);

                listInitialTerrain.FindThis(hWnd, IDC_LIST_DEFAULTTERRAIN);
                listInitialTerrain.AddStrings(badlandsInitTerrain);
                listInitialTerrain.SetCurSel(0);
                listInitialTerrain.SetFont(defaultFont, false);
                return TRUE;
            }
            break;
    }
    return FALSE;
}
