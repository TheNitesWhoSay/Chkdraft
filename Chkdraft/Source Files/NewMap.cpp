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
		SendDlgItemMessage(hWnd, IDC_EDIT_WIDTH, EM_GETLINE, 0, (LPARAM)lpszWidth);	lpszWidth[dimLength] = NULL;
		width = atoi(lpszWidth);

		dimLength = (WORD)SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_LINELENGTH, 0, 0);	*((LPWORD)lpszHeight) = dimLength;
		SendDlgItemMessage(hWnd, IDC_EDIT_HEIGHT, EM_GETLINE, 0, (LPARAM)lpszHeight);	lpszHeight[dimLength] = NULL;
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
					chkd.maps.curr->Scroll(SCROLL_X | SCROLL_Y);

					// Tiling Code
					u16 tilenum = 0;
					u16 xSize = chkd.maps.curr->XSize();
					for ( u32 xStart = 0; xStart<chkd.maps.curr->XSize(); xStart += 16 )
					{
						for ( u16 yc = 0; yc<chkd.maps.curr->YSize(); yc++ )
						{
							for ( u16 xc = (u16)xStart; xc<xStart + 16; xc++ )
							{
								u32 location = 2 * xSize*yc + 2 * xc; // Down y rows, over x columns
								chkd.maps.curr->TILE().replace<u16>(location, tilenum);
								chkd.maps.curr->MTXM().replace<u16>(location, tilenum);
								tilenum++;
							}
						}
					}

					chkd.maps.curr->Redraw(true);
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
					for ( int i = 0; i<numBadlandsInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)badlandsInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_SPACE: // Space Platform
					for ( int i = 0; i<numSpaceInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)spaceInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_INSTALLATION: // Installation
					for ( int i = 0; i<numInstallationInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)installInitTerrain[i]);
					SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					break;
				case TERRAIN_ASH: // Ash World
					for ( int i = 0; i<numAshInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)ashInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_JUNGLE: // Jungle World
					for ( int i = 0; i<numJungInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)jungInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_DESERT: // Desert World
					for ( int i = 0; i<numDesertInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)desertInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_ICE: // Ice World
					for ( int i = 0; i<numIceInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)iceInitTerrain[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					break;
				case TERRAIN_TWILIGHT: // Twilight World
					for ( int i = 0; i<numTwilightInitTerrain; i++ )
						SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)twilightInitTerrain[i]);
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
							SendMessage(hDefaultTrigs, CB_ADDSTRING, NULL, (LPARAM)defaultTriggerTitles[i]);
					SendMessage(hDefaultTrigs, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					SendMessage(hDefaultTrigs, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hWidth = GetDlgItem(hWnd, IDC_EDIT_WIDTH);
				SendMessage(hWidth, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hWidth, "128");

				HWND hHeight = GetDlgItem(hWnd, IDC_EDIT_HEIGHT);
				SendMessage(hHeight, EM_SETLIMITTEXT, 10, 0);
				SetWindowText(hHeight, "128");

				HWND hTileset = GetDlgItem(hWnd, IDC_LIST_TILESET);
				for ( int i=0; i<8; i++ )
					SendMessage(hTileset, LB_ADDSTRING, NULL, (LPARAM)tilesetNames[i]);
				SendMessage(hTileset, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hTileset, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

				HWND hInitialTerrain = GetDlgItem(hWnd, IDC_LIST_DEFAULTTERRAIN);
				for ( int i=0; i<numBadlandsInitTerrain; i++ )
					SendMessage(hInitialTerrain, LB_ADDSTRING, NULL, (LPARAM)badlandsInitTerrain[i]);
				SendMessage(hInitialTerrain, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hInitialTerrain, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
				return TRUE;
			}
			break;
	}
	return FALSE;
}
