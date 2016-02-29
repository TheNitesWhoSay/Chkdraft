#include "NewMap.h"
#include "Chkdraft.h"

enum ID {
	COMBO_TRIGS = ID_FIRST
};

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
		TCHAR lpszWidth[11], lpszHeight[11];
		WORD dimLength;

		dimLength = (WORD)SendDlgItemMessage(hWnd, IDC_EDIT_WIDTH, EM_LINELENGTH, 0, 0);
		*((LPWORD)lpszWidth) = dimLength;
		SendDlgItemMessage(hWnd, IDC_EDIT_WIDTH, EM_GETLINE, 0, (LPARAM)lpszWidth);	lpszWidth[dimLength] = '\0';
		width = atoi(lpszWidth);

		dimLength = (WORD)SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_LINELENGTH, 0, 0);	*((LPWORD)lpszHeight) = dimLength;
		SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_GETLINE, 0, (LPARAM)lpszHeight);	lpszHeight[dimLength] = '\0';
		height = atoi(lpszHeight);

		tileset = SendDlgItemMessage(hWnd, IDC_LIST_TILESET, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		terrain = SendDlgItemMessage(hWnd, IDC_LIST_DEFAULTTERRAIN, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		triggers = SendDlgItemMessage(hWnd, COMBO_TRIGS, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

		if ( width>0 && height>0 )
		{
			if ( width > 65535 ) width %= 65536;
			if ( height > 65535 ) height %= 65536;
			if ( (u32)width*height < 429496729 )
			{
				if ( chkd.maps.NewMap(width, height, tileset, terrain, triggers) )
				{
                    CM->Scroll(true, true, false);

					// Tiling Code
					u16 tilenum = 0;
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
					}

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
			int ItemIndex = SendMessage((HWND)lParam, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			if ( LOWORD(wParam) == IDC_LIST_TILESET )
			{
				HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
				HWND hDefaultTrigs = GetDlgItem(hWnd, COMBO_TRIGS);
				SendMessage(hInitialTerrain, LB_RESETCONTENT, 0, 0);
				switch ( ItemIndex )
				{
				case TERRAIN_BADLANDS: // Badlands
					for (auto initTerrain : badlandsInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_SPACE: // Space Platform
					for (auto initTerrain : spaceInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_INSTALLATION: // Installation
					for (auto initTerrain : installInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					break;
				case TERRAIN_ASH: // Ash World
					for (auto initTerrain : ashInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_JUNGLE: // Jungle World
					for (auto initTerrain : jungInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_DESERT: // Desert World
					for (auto initTerrain : desertInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_ICE: // Ice World
					for (auto initTerrain : iceInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_TWILIGHT: // Twilight World
					for (auto initTerrain : twilightInitTerrain)
						SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)initTerrain.c_str());
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
				HWND hTrigStatic = GetDlgItem(hWnd, IDC_TRIGGERS_GROUP);
				RECT rcTrigStatic;
				GetWindowRect(hTrigStatic, &rcTrigStatic);
				POINT upperLeft;
				upperLeft.x = rcTrigStatic.left;
				upperLeft.y = rcTrigStatic.top;
				ScreenToClient(hWnd, &upperLeft);

				SendMessage(hWnd, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
				HWND hDefaultTrigs = CreateWindowEx(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_HASSTRINGS,
						upperLeft.x+10, upperLeft.y+20, 305, 200, hWnd, (HMENU)COMBO_TRIGS, (HINSTANCE)GetModuleHandle(NULL), NULL);
					// Add the trigger options
						const char* defaultTriggerTitles[] = { "No triggers", "Default melee triggers", "Melee with observers (2 players)",
							"Melee with observers (3 players)", "Melee with observers (4 players)", "Melee with observers (5 players)",
							"Melee with observers (6 players)", "Melee with observers (7 players)"};
						for ( int i=0; i<8; i++ )
							SendMessage(hDefaultTrigs, CB_ADDSTRING, 0, (LPARAM)defaultTriggerTitles[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					SendMessage(hDefaultTrigs, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hWidth = GetDlgItem(hWnd, IDC_EDIT_WIDTH);
				SendMessage(hWidth, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hWidth, "128");

				HWND hHeight = GetDlgItem(hWnd, IDC_EDIT_HEIGHT);
				SendMessage(hHeight, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hHeight, "128");

				HWND hTileset = GetDlgItem(hWnd, IDC_LIST_TILESET);
				for (auto tileset : tilesetNames)
					SendMessage(hTileset, LB_ADDSTRING, 0, (LPARAM)tileset.c_str());
				SendMessage(hTileset, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hTileset, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
				for (auto terrain : badlandsInitTerrain)
					SendMessage(hInitialTerrain, LB_ADDSTRING, 0, (LPARAM)terrain.c_str());
				SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hInitialTerrain, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
				return TRUE;
			}
			break;
	}
	return FALSE;
}
