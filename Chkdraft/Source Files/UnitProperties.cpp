#include "UnitProperties.h"
#include "UnitMoveTo.h"
#include "Chkdraft.h"

UnitWindow::UnitWindow() : columnSortedBy(UNIT_INDEX_COLUMN), flipSort(false), initilizing(true), changeHighlightOnly(false)
{

}

bool UnitWindow::CreateThis(HWND hParent)
{
	return ClassWindow::CreateModelessDialog(MAKEINTRESOURCE(IDD_UNITPROPERTIES), hParent);
}

bool UnitWindow::DestroyThis()
{
	return ClassWindow::DestroyDialog();
}

void UnitWindow::SetChangeHighlightOnly(bool changeHighlightOnly)
{
	this->changeHighlightOnly = changeHighlightOnly;
}

void UnitWindow::ChangeCurrOwner(HWND hUnitProperties, u8 newPlayer)
{
	HWND hUnitList = GetDlgItem(hUnitProperties, IDC_UNITLIST);
	UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
	ChkUnit* unit;

	while ( currUnit != nullptr )
	{
		int index = currUnit->index;
		if ( chkd.maps.curr->getUnit(unit, index) )
		{
			ChangeOwner(hUnitList, index, newPlayer);
			unit->owner = newPlayer;
		}
		currUnit = currUnit->next;
	}
	chkd.maps.curr->Redraw(true);
}

void UnitWindow::ChangeOwner(HWND hUnitList, int index, u8 newPlayer)
{
	char owner[32], padding[2] = { };
	if ( newPlayer < 9 )
		padding[0] = '0';
	else
		padding[0] = '\0';
	sprintf_s(owner, "Player %s%i", padding, newPlayer+1);

	int row = GetItemRow(hUnitList, index);
	SetItem(hUnitList, row, UNIT_OWNER_COLUMN, owner);
}

void UnitWindow::EnableUnitEditing(HWND hWnd)
{
	HWND hItem = GetDlgItem(hWnd, IDC_COMBO_PLAYER);
	EnableWindow(hItem, TRUE);

	const int buttons[] = { IDC_BUTTON_MOVEUP, IDC_BUTTON_MOVETOP, IDC_BUTTON_MOVEDOWN,
							IDC_BUTTON_MOVEEND, IDC_BUTTON_DELETE, IDC_BUTTON_MOVE_TO };
	for ( int i=0; i<sizeof(buttons)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, buttons[i]);
		EnableWindow(hItem, TRUE);
	}

	const int editBoxes[] = { IDC_EDIT_HP, IDC_EDIT_MP, IDC_EDIT_SHIELD, IDC_EDIT_RESOURCES,
							  IDC_EDIT_HANGER, IDC_EDIT_ID, IDC_EDIT_XC, IDC_EDIT_YC };
	for ( int i=0; i<sizeof(editBoxes)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, editBoxes[i]);
		EnableWindow(hItem, TRUE);
	}

	const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
							  IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
	for ( int i=0; i<sizeof(buddyIds)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, buddyIds[i]);
		EnableWindow(hItem, TRUE);
	}

	const int checkBoxes[] = { IDC_CHECK_INVINCIBLE, IDC_CHECK_HALLUCINATED, IDC_CHECK_BURROWED,
							   IDC_CHECK_CLOAKED, IDC_CHECK_LIFTED };
	for ( int i=0; i<sizeof(checkBoxes)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, checkBoxes[i]);
		SendMessage(hItem, BM_SETCHECK, BST_UNCHECKED, NULL);
		EnableWindow(hItem, TRUE);
	}
}

void UnitWindow::DisableUnitEditing(HWND hWnd)
{
	SetWindowText(hWnd, "");

	HWND hItem = GetDlgItem(hWnd, IDC_COMBO_PLAYER);
	SendMessage(hItem, CB_SETCURSEL, -1, NULL);
	EnableWindow(hItem, FALSE);

	const int buttons[] = { IDC_BUTTON_MOVEUP, IDC_BUTTON_MOVETOP, IDC_BUTTON_MOVEDOWN,
							IDC_BUTTON_MOVEEND, IDC_BUTTON_DELETE, IDC_BUTTON_MOVE_TO };
	for ( int i=0; i<sizeof(buttons)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, buttons[i]);
		EnableWindow(hItem, FALSE);
	}

	const int editBoxes[] = { IDC_EDIT_HP, IDC_EDIT_MP, IDC_EDIT_SHIELD, IDC_EDIT_RESOURCES,
							  IDC_EDIT_HANGER, IDC_EDIT_ID, IDC_EDIT_XC, IDC_EDIT_YC };
	for ( int i=0; i<sizeof(editBoxes)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, editBoxes[i]);
		SetWindowText(hItem, "");
		EnableWindow(hItem, FALSE);
	}

	const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
							  IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
	for ( int i=0; i<sizeof(buddyIds)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, buddyIds[i]);
		EnableWindow(hItem, FALSE);
	}

	const int checkBoxes[] = { IDC_CHECK_INVINCIBLE, IDC_CHECK_HALLUCINATED, IDC_CHECK_BURROWED,
							   IDC_CHECK_CLOAKED, IDC_CHECK_LIFTED };
	for ( int i=0; i<sizeof(checkBoxes)/sizeof(const int); i++ )
	{
		hItem = GetDlgItem(hWnd, checkBoxes[i]);
		SendMessage(hItem, BM_SETCHECK, BST_UNCHECKED, NULL);
		EnableWindow(hItem, FALSE);
	}
}

void UnitWindow::SetUnitFieldText(HWND hWnd, ChkUnit* unit)
{
	initilizing = true;
	HWND hPlayer   = GetDlgItem(hWnd, IDC_COMBO_PLAYER		), hHp	   = GetDlgItem(hWnd, IDC_EDIT_HP		  ),
		 hMp	   = GetDlgItem(hWnd, IDC_EDIT_MP			), hShield = GetDlgItem(hWnd, IDC_EDIT_SHIELD	  ),
		 hResource = GetDlgItem(hWnd, IDC_EDIT_RESOURCES	), hHanger = GetDlgItem(hWnd, IDC_EDIT_HANGER	  ),
		 hId	   = GetDlgItem(hWnd, IDC_EDIT_ID			), hInvinc = GetDlgItem(hWnd, IDC_CHECK_INVINCIBLE),
		 hHallu	   = GetDlgItem(hWnd, IDC_CHECK_HALLUCINATED), hBurrow = GetDlgItem(hWnd, IDC_CHECK_BURROWED  ),
		 hCloak	   = GetDlgItem(hWnd, IDC_CHECK_CLOAKED		), hLifted = GetDlgItem(hWnd, IDC_CHECK_LIFTED	  ),
		 hXc	   = GetDlgItem(hWnd, IDC_EDIT_XC			), hYc	   = GetDlgItem(hWnd, IDC_EDIT_YC		  );

	char number[16];
	if ( unit->owner < 12 )
		SendMessage(hPlayer, CB_SETCURSEL, unit->owner, NULL);
	else
	{
		_itoa_s(unit->owner+1, number, 10);
		SetWindowText(hPlayer  , number);
	}
	_itoa_s(unit->hitpoints, number, 10); SetWindowText(hHp		, number);
	_itoa_s(unit->energy   , number, 10); SetWindowText(hMp		, number);
	_itoa_s(unit->shields  , number, 10); SetWindowText(hShield	, number);
	_itoa_s(unit->resources, number, 10); SetWindowText(hResource, number);
	_itoa_s(unit->hanger   , number, 10); SetWindowText(hHanger  , number);
	_itoa_s(unit->id	   , number, 10); SetWindowText(hId		, number);
	_itoa_s(unit->xc	   , number, 10); SetWindowText(hXc		, number);
	_itoa_s(unit->yc	   , number, 10); SetWindowText(hYc		, number);
	SendMessage(hInvinc, BM_SETCHECK, unit->stateFlags&UNIT_STATE_INVINCIBLE  , NULL);
	SendMessage(hHallu , BM_SETCHECK, unit->stateFlags&UNIT_STATE_HALLUCINATED, NULL);
	SendMessage(hBurrow, BM_SETCHECK, unit->stateFlags&UNIT_STATE_BURROWED	  , NULL);
	SendMessage(hCloak , BM_SETCHECK, unit->stateFlags&UNIT_STATE_CLOAKED	  , NULL);
	SendMessage(hLifted, BM_SETCHECK, unit->stateFlags&UNIT_STATE_LIFTED	  , NULL);
	initilizing = false;
}

void UnitWindow::SwapIndexes(HWND hListView, LPARAM index1, LPARAM index2)
{
	int row1 = GetItemRow(hListView, index1),
		row2 = GetItemRow(hListView, index2);

	LVITEM item = { };
	item.mask = LVIF_PARAM;
	item.iItem = row1;
	item.lParam = index2;

	ListView_SetItem(hListView, &item);
	SetItem(hListView, row1, UNIT_INDEX_COLUMN, index2);

	item.iItem = row2;
	item.lParam = index1;

	ListView_SetItem(hListView, &item);
	SetItem(hListView, row2, UNIT_INDEX_COLUMN, index1);
}

void UnitWindow::ChangeIndex(HWND hListView, LPARAM oldLParam, LPARAM newLParam)
{
	int row = GetItemRow(hListView, oldLParam);

	LVITEM item = { };
	item.mask = LVIF_PARAM;
	item.iItem = row;
	item.lParam = newLParam;

	ListView_SetItem(hListView, &item);

	SetItem(hListView, row, UNIT_INDEX_COLUMN, newLParam);
}

int UnitWindow::CompareLvItems(LPARAM index1, LPARAM index2)
{
	int sort = 0;

	if ( columnSortedBy < 4 )
	{
		ChkUnit* unit1,
			   * unit2;

		if (	chkd.maps.curr->getUnit(unit1, u16(index1))
			 && chkd.maps.curr->getUnit(unit2, u16(index2)) )
		{
			switch ( columnSortedBy )
			{
				case 0: // Sort by ID / type
					if ( unit1->id < unit2->id )
						sort = -1;
					else if ( unit1->id > unit2->id )
						sort = 1;
					break;
				case 1: // Sort by owner
					if ( unit1->owner < unit2->owner )
						sort = -1;
					else if ( unit1->owner > unit2->owner )
						sort = 1;
					break;
				case 2: // Sort by xc
					if ( unit1->xc < unit2->xc )
						sort = -1;
					else if ( unit1->xc > unit2->xc )
						sort = 1;
					else if ( unit1->yc < unit2->yc ) // Try to sort by yc if xc's are equal
						sort = -1;
					else if ( unit1->yc > unit2->yc )
						sort = 1;
					break;
				case 3: // Sort by yc
					if ( unit1->yc < unit2->yc )
						sort = -1;
					else if ( unit1->yc > unit2->yc )
						sort = 1;
					else if ( unit1->xc < unit2->xc ) // Try to sort by xc if yc's are equal
						sort = -1;
					else if ( unit1->xc > unit2->xc )
						sort = 1;
					break;
			}
		}
	}
	else if ( columnSortedBy == 4 )
	{
		if ( index1 < index2 )
			sort = -1;
		else if ( index1 > index2 )
			sort = 1;
	}

	if ( flipSort )
		return -sort;
	else
		return sort;
}

BOOL UnitWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		case WM_ACTIVATE:
			if ( LOWORD(wParam) != WA_INACTIVE )
				chkd.SetCurrDialog(hWnd);
			return FALSE; // Necessary for proper minimize/restore
			break;

		case SET_LIST_REDRAW:
			{
				HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
				SendMessage(hUnitList, WM_SETREDRAW, wParam, NULL);
				if ( wParam == TRUE )
				{
					ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
					UpdateWindow(hUnitList);
				}
			}
			break;

		case ADD_UNIT:
			{
				ChkUnit* unit = (ChkUnit*)lParam;
				u16 index = (u16)wParam;

				HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
				char owner[32], padding[2] = { }, xc[32], yc[32], cIndex[32];;

				u16 id = unit->id;

				if ( unit->owner+1 < 10 )
					padding[0] = '0';
				else
					padding[0] = '\0';
				sprintf_s(owner, "Player %s%i", padding, unit->owner+1);
						
				_itoa_s(unit->xc, xc, 10);
				_itoa_s(unit->yc, yc, 10);
				_itoa_s(index, cIndex, 10);

				string unitName;
				chkd.maps.curr->getUnitName(unitName, id);

				AddRow(hUnitList, 4, index);
				SetItem(hUnitList, index, UNIT_NAME_COLUMN, unitName.c_str());
				SetItem(hUnitList, index, UNIT_OWNER_COLUMN, owner);
				SetItem(hUnitList, index, UNIT_XC_COLUMN, xc);
				SetItem(hUnitList, index, UNIT_YC_COLUMN, yc);
				SetItem(hUnitList, index, UNIT_INDEX_COLUMN, index);
			}
			break;

		case WM_INITDIALOG:
			{
				initilizing = true;
				HWND hPlayer = GetDlgItem(hWnd, IDC_COMBO_PLAYER),
					 hHP = GetDlgItem(hWnd, IDC_EDIT_HP),
					 hMP = GetDlgItem(hWnd, IDC_EDIT_MP),
					 hShield = GetDlgItem(hWnd, IDC_EDIT_SHIELD),
					 hResources = GetDlgItem(hWnd, IDC_EDIT_RESOURCES),
					 hHanger = GetDlgItem(hWnd, IDC_EDIT_HANGER),
					 hID = GetDlgItem(hWnd, IDC_EDIT_ID);

				char* playerStrings[] = { "Player 01", "Player 02", "Player 03", "Player 04", "Player 05", "Player 06",
										  "Player 07", "Player 08", "Player 09", "Player 10", "Player 11", "Player 12 (Neutral)" };
				for ( int i=0; i<12; i++ )
					SendMessage(hPlayer, CB_ADDSTRING, 0, (LPARAM)playerStrings[i]);
				SendMessage(hPlayer, CB_LIMITTEXT, 0, NULL);

				HWND hBuddy[6];
				const int buddyIds[6] = { IDC_HP_BUDDY, IDC_MP_BUDDY, IDC_SHIELD_BUDDY,
										  IDC_RESOURCES_BUDDY, IDC_HANGER_BUDDY, IDC_ID_BUDDY };
				for ( int i=0; i<6; i++ )
				{
					hBuddy[i] = CreateWindowExA(NULL, UPDOWN_CLASS, NULL,
												WS_CHILDWINDOW|WS_VISIBLE|WS_DISABLED|
												UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS|UDS_HOTTRACK,
												0, 0, 0, 0,
												hWnd, (HMENU)buddyIds[i], NULL, NULL);
				}
				SendMessage(hBuddy[0], UDM_SETBUDDY, (WPARAM)hHP, NULL);
				SendMessage(hBuddy[0], UDM_SETRANGE32, 0, 100);
				SendMessage(hBuddy[1], UDM_SETBUDDY, (WPARAM)hMP, NULL);
				SendMessage(hBuddy[1], UDM_SETRANGE32, 0, 100);
				SendMessage(hBuddy[2], UDM_SETBUDDY, (WPARAM)hShield, NULL);
				SendMessage(hBuddy[2], UDM_SETRANGE32, 0, 100);
				SendMessage(hBuddy[3], UDM_SETBUDDY, (WPARAM)hResources, NULL);
				SendMessage(hBuddy[3], UDM_SETRANGE32, 0, 50000);
				SendMessage(hBuddy[4], UDM_SETBUDDY, (WPARAM)hHanger, NULL);
				SendMessage(hBuddy[4], UDM_SETRANGE32, 0, 8);
				SendMessage(hBuddy[5], UDM_SETBUDDY, (WPARAM)hID, NULL);
				SendMessage(hBuddy[5], UDM_SETRANGE32, 0, 65535);

				HWND hUnitList = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEW, "List View",
												 WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS,
												 9, 10, 549, 449,
												 hWnd, (HMENU)IDC_UNITLIST, GetModuleHandle(NULL), NULL);

				// Select all of the row when selecting a unit
				SendMessage(hUnitList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

				HFONT hListFont = CreateFontA(13, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");
				SendMessage(hUnitList, WM_SETFONT, (WPARAM)hListFont, 0);

				LV_COLUMN lvCol = { };
				AddColumn(hUnitList, 0, "Unit type", 200, LVCFMT_LEFT);
				AddColumn(hUnitList, 1, "Owner", 100, LVCFMT_LEFT);
				AddColumn(hUnitList, 2, "X", 75, LVCFMT_RIGHT);
				AddColumn(hUnitList, 3, "Y", 75, LVCFMT_RIGHT);
				AddColumn(hUnitList, 4, "Index", 75, LVCFMT_RIGHT);

				SendMessage(hWnd, REPOPULATE_LIST, NULL, NULL);

				ShowWindow(hUnitList, SW_SHOW);
				initilizing = false;
			}
			break;

		case REPOPULATE_LIST:
			{
				initilizing = true;
				HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
				SendMessage(hWnd, SET_LIST_REDRAW, FALSE, NULL);

				ListView_DeleteAllItems(hUnitList);

				if ( chkd.maps.curr )
				{
					ChkUnit* unit = nullptr;
					buffer& units = chkd.maps.curr->UNIT();

					int unitTableSize = units.size(),
						numUnits = unitTableSize/UNIT_STRUCT_SIZE;

					for ( int i=0; i<numUnits; i++ )
						SendMessage(hWnd, ADD_UNIT, i, (LPARAM)units.getPtr(i*UNIT_STRUCT_SIZE));

					bool unitsSelected = chkd.maps.curr->selections().hasUnits();
					if ( unitsSelected )
					{
						UnitNode* curr = chkd.maps.curr->selections().getFirstUnit();
						int index = curr->index;
						ListView_SetItemState(hUnitList, curr->index, LVIS_FOCUSED, LVIS_FOCUSED);
						while ( curr != nullptr )
						{
							ListView_SetItemState(hUnitList, curr->index, LVIS_SELECTED, LVIS_SELECTED);
							curr = curr->next;
						}
						EnableUnitEditing(hWnd);
						ChkUnit* unit;
						if ( chkd.maps.curr->getUnit(unit, index) )
						{
							SetUnitFieldText(hWnd, unit);

							string unitName;
							chkd.maps.curr->getUnitName(unitName, unit->id);
							SetWindowText(hWnd, unitName.c_str());

							int row = GetItemRow(hUnitList, chkd.maps.curr->selections().getLastUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
							row = GetItemRow(hUnitList, chkd.maps.curr->selections().getFirstUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
						}
					}
				}

				SendMessage(hWnd, SET_LIST_REDRAW, TRUE, NULL);
				initilizing = false;
			}
			break;

		case UPDATE_ENABLED_STATE:
			{
				if ( chkd.maps.curr->selections().hasUnits() )
				{
					EnableUnitEditing(hWnd);
					ChkUnit* unit;
					if ( chkd.maps.curr->getUnit(unit, chkd.maps.curr->selections().getFirstUnit()->index) )
						SetUnitFieldText(hWnd, unit);
				}
				else
					DisableUnitEditing(hWnd);
			}
			break;

		case WM_SHOWWINDOW:
			{
				LRESULT result = DefWindowProc(hWnd, msg, wParam, lParam);
				if ( wParam == TRUE )
					SetFocus( GetDlgItem(hWnd, IDC_UNITLIST) );

				return result;
			}
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case IDC_COMBO_PLAYER:
						switch ( HIWORD(wParam) )
						{
							case CBN_EDITCHANGE:
								{
									u8 newPlayer;
									if ( GetPlayerNum((HWND)lParam, newPlayer) )
										ChangeCurrOwner(hWnd, newPlayer);
								}
								break;
							case CBN_SELCHANGE:
								{
									u8 newPlayer = (u8)SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
									if ( newPlayer != CB_ERR )
										ChangeCurrOwner(hWnd, newPlayer);
								}
								break;
						}
						break;
					case IDCLOSE:
						EndDialog(hWnd, IDCLOSE);
						break;

					case IDC_BUTTON_MOVETOP:
						{
							GuiMap* map = chkd.maps.curr;
							buffer& UNIT = map->UNIT();
							SELECTIONS&  selections = map->selections();
							HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
							UNDOS& undos = map->undos();
							undos.startNext(UNDO_UNIT_MOVE);

							u16 unitStackTopIndex = selections.getFirstUnit()->index;
							selections.sortUnits(true); // sort with lowest indexes first
							UnitNode* track = selections.getFirstUnit();

							SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);

							ChkUnit* unit;
							ChkUnit preserve;
							for ( int i=0; track != nullptr; i++ )
							{
								if ( track->index != 0 && map->getUnit(unit, track->index) ) // If unit is not at the destination index and unitptr can be retrieved
								{
									preserve = *unit; // Preserve the unit info
									if ( UNIT.del<ChkUnit>(track->index*UNIT_STRUCT_SIZE) ) // Delete the unit from the section
									{
										if ( UNIT.insert<ChkUnit&>(i*UNIT_STRUCT_SIZE, preserve) ) // Insert the unit at the destination index
										{
											undos.addUndoUnitMove(track->index, i);
											if ( track->index == unitStackTopIndex )
												unitStackTopIndex = i;
											track->index = i; // Modify the index that denotes unit selection
										}
										else // Insertion failed
										{
											selections.removeUnit(track->index);
											break; // Can't advance to next, exit loop
										}
									}
								}
								track = track->next;
							}
							undos.startNext(0);
							selections.ensureFirst(unitStackTopIndex);
							SendMessage(hWnd, REPOPULATE_LIST, NULL, NULL);
						}
						break;

					case IDC_BUTTON_MOVEEND:
						{
							GuiMap* map = chkd.maps.curr;
							buffer& UNIT = map->UNIT();
							SELECTIONS& selections = map->selections();
							HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
							UNDOS& undos = map->undos();
							undos.startNext(UNDO_UNIT_MOVE);

							u16 unitStackTopIndex = selections.getFirstUnit()->index;
							selections.sortUnits(false); // Highest First
							UnitNode* track = selections.getFirstUnit();

							SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);
							u16 lastIndex = map->numUnits();
							u16 numUnitsSelected = selections.numUnits();

							ChkUnit* unit;
							ChkUnit preserve;
							for ( int i=1; track != nullptr; i++ )
							{
								if ( track->index != lastIndex && map->getUnit(unit, track->index) )
								{
									preserve = *unit;
									if ( UNIT.del<ChkUnit>(track->index*UNIT_STRUCT_SIZE) )
									{
										if ( UNIT.insert<ChkUnit&>((lastIndex-i)*UNIT_STRUCT_SIZE, preserve) )
										{
											undos.addUndoUnitMove(track->index, lastIndex-i);

											if ( track->index == unitStackTopIndex )
												unitStackTopIndex = lastIndex - i;

											track->index = lastIndex - i;
										}
										else
										{
											selections.removeUnit(track->index);
											break;
										}
									}
								}
								track = track->next;
							}
							undos.startNext(0);
							selections.ensureFirst(unitStackTopIndex);
							SendMessage(hWnd, REPOPULATE_LIST, NULL, NULL);
						}
						break;

					case IDC_BUTTON_MOVEUP:
						{
							GuiMap* map = chkd.maps.curr;
							buffer& UNIT = map->UNIT();
							SELECTIONS& selections = map->selections();
							HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
							
							selections.sortUnits(true);
							UnitNode* track = selections.getFirstUnit();
							SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);
							while ( track != nullptr )
							{
								if ( track->index > 0 && !selections.unitIsSelected(track->index-1) )
								{
									if ( UNIT.swap<ChkUnit>((u32)track->index*UNIT_STRUCT_SIZE, ((u32)track->index-1)*UNIT_STRUCT_SIZE) )
									{
										map->undos().addUndoUnitSwap(track->index, track->index-1);
										SwapIndexes(hUnitList, track->index, track->index-1);
										track->index --;
									}
								}
								
								track = track->next;
							}
							ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
							int row = GetItemRow(hUnitList, chkd.maps.curr->selections().getLastUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
							row = GetItemRow(hUnitList, chkd.maps.curr->selections().getFirstUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
							map->undos().startNext(0);
							SendMessage(hUnitList, WM_SETREDRAW, TRUE, NULL);
						}
						break;
					case IDC_BUTTON_MOVEDOWN:
						{
							GuiMap* map = chkd.maps.curr;
							buffer& UNIT = map->UNIT();
							SELECTIONS& selections = map->selections();
							HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
							
							selections.sortUnits(false);
							UnitNode* track = selections.getFirstUnit();
							SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);
							while ( track != nullptr )
							{
								if ( track->index < map->numUnits() && !selections.unitIsSelected(track->index+1) )
								{
									if ( UNIT.swap<ChkUnit>((u32)track->index*UNIT_STRUCT_SIZE, ((u32)track->index+1)*UNIT_STRUCT_SIZE) )
									{
										map->undos().addUndoUnitSwap(track->index, track->index+1);
										SwapIndexes(hUnitList, track->index, track->index+1);
										track->index ++;
									}
								}
								track = track->next;
							}
							ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
							int row = GetItemRow(hUnitList, chkd.maps.curr->selections().getLastUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
							row = GetItemRow(hUnitList, chkd.maps.curr->selections().getFirstUnit()->index);
							ListView_EnsureVisible(hUnitList, row, FALSE);
							map->undos().startNext(0);
							SendMessage(hUnitList, WM_SETREDRAW, TRUE, NULL);
						}
						break;
					case IDC_BUTTON_MOVE_TO:
						{
							UnitMoveTo moveToWindow;
							int unitMoveTo = (int)moveToWindow.GetMoveToIndex(hWnd);
							if ( unitMoveTo == 0 )
								return SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MOVETOP, NULL), NULL);
							else if ( unitMoveTo > 0 )
							{
								GuiMap* map = chkd.maps.curr;
								SELECTIONS& selections = map->selections();
								u16 numUnitsSelected = selections.numUnits();
								u16 limit = map->numUnits()-1;

								if ( unitMoveTo+numUnitsSelected > limit )
									return SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MOVEEND, NULL), NULL);
								else
								{
									buffer& UNIT = map->UNIT();
									HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
									UNDOS& undos = map->undos();
									undos.startNext(UNDO_UNIT_MOVETO);

									u16 unitStackTopIndex = selections.getFirstUnit()->index;
									u16 numUnits = selections.numUnits(),
										   shift = numUnits-1;
									selections.sortUnits(false); // Highest First
									UnitNode* track = selections.getFirstUnit();

									SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);

									ChkUnit* selectedUnits;
									try {
										selectedUnits = new ChkUnit[numUnits];
									} catch ( std::bad_alloc ) {
										Error("'Move To' failed.\n\nCould not allocate temporary storage, you may have run out of memory");
										return 0;
									}

									u16 numDeletes = 0,
										numCreates = 0;

									for ( int i=0; track != nullptr; i++ )
									{ // Remove each selected unit from the map, store in selectedUnits
										u32 loc = track->index*UNIT_STRUCT_SIZE;

										if ( UNIT.get<ChkUnit>(selectedUnits[shift-i], loc) &&
											 UNIT.del<ChkUnit>(loc) )
										{
											if ( undos.addUndoUnitDel(track->index, &selectedUnits[shift-i]) )
												numDeletes ++;

											track->index = unitMoveTo+shift-i;
										}

										track = track->next;
									}

									for ( int i=0; i<numUnits; i++ )
									{
										if ( UNIT.insert<ChkUnit&>((unitMoveTo+i)*UNIT_STRUCT_SIZE, selectedUnits[i]) )
										{
											if ( undos.addUndoUnitCreate(unitMoveTo+i) )
												numCreates ++;
										}
									}
									
									selections.finishMove();
									selections.ensureFirst(unitStackTopIndex);
									undos.addUndoUnitMoveToHeader(numDeletes, numCreates);
									undos.startNext(0);
									SendMessage(hWnd, REPOPULATE_LIST, NULL, NULL);
								}
							}
						}
						break;
					case IDC_BUTTON_DELETE:
						{
							GuiMap* map = chkd.maps.curr;
							SELECTIONS& selections = map->selections();
							HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
							SendMessage(hUnitList, WM_SETREDRAW, FALSE, NULL);
							while ( selections.hasUnits() )
							{
								u16 index = selections.getHighestIndex();
								selections.removeUnit(index);
								RemoveRow(hUnitList, index);

								HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
								int row = GetItemRow(hUnitList, index);

								ChkUnit* delUnit;
								if ( map->getUnit(delUnit, index) )
									map->undos().addUndoUnitDel(index, delUnit);

								map->UNIT().del(index*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);

								for ( int i=index+1; i<=map->numUnits(); i++ )
									ChangeIndex(hUnitList, i, i-1);
							}
							map->undos().startNext(0);
							map->Redraw(true);
							SendMessage(hUnitList, WM_SETREDRAW, TRUE, NULL);
						}
						break;
					default:
						switch ( HIWORD(wParam) )
						{
							case BN_CLICKED:
								switch ( LOWORD(wParam) )
								{
									case IDC_CHECK_INVINCIBLE:
										{
											UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
											ChkUnit* unit;
											while ( currUnit != nullptr )
											{
												int index = currUnit->index;
												if ( chkd.maps.curr->getUnit(unit, index) )
												{
													chkd.maps.curr->undos().addUndoUnitChange(index, UNIT_FIELD_STATEFLAGS, unit->stateFlags);
													if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED )
														unit->stateFlags |= UNIT_STATE_INVINCIBLE;
													else if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
														unit->stateFlags ^= UNIT_STATE_INVINCIBLE;
												}
												currUnit = currUnit->next;
											}
											chkd.maps.curr->undos().startNext(0);
										}
										break;
									case IDC_CHECK_HALLUCINATED:
										{
											UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
											ChkUnit* unit;
											while ( currUnit != nullptr )
											{
												int index = currUnit->index;
												if ( chkd.maps.curr->getUnit(unit, index) )
												{
													chkd.maps.curr->undos().addUndoUnitChange(index, UNIT_FIELD_STATEFLAGS, unit->stateFlags);
													if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED )
														unit->stateFlags |= UNIT_STATE_HALLUCINATED;
													else if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
														unit->stateFlags ^= UNIT_STATE_HALLUCINATED;
												}
												currUnit = currUnit->next;
											}
											chkd.maps.curr->undos().startNext(0);
										}
										break;
									case IDC_CHECK_BURROWED:
										{
											UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
											ChkUnit* unit;
											while ( currUnit != nullptr )
											{
												int index = currUnit->index;
												if ( chkd.maps.curr->getUnit(unit, index) )
												{
													chkd.maps.curr->undos().addUndoUnitChange(index, UNIT_FIELD_STATEFLAGS, unit->stateFlags);
													if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED )
														unit->stateFlags |= UNIT_STATE_BURROWED;
													else if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
														unit->stateFlags ^= UNIT_STATE_BURROWED;
												}
												currUnit = currUnit->next;
											}
											chkd.maps.curr->undos().startNext(0);
										}
										break;
									case IDC_CHECK_CLOAKED:
										{
											UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
											ChkUnit* unit;
											while ( currUnit != nullptr )
											{
												int index = currUnit->index;
												if ( chkd.maps.curr->getUnit(unit, index) )
												{
													chkd.maps.curr->undos().addUndoUnitChange(index, UNIT_FIELD_STATEFLAGS, unit->stateFlags);
													if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED )
														unit->stateFlags |= UNIT_STATE_CLOAKED;
													else if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
														unit->stateFlags ^= UNIT_STATE_CLOAKED;
												}
												currUnit = currUnit->next;
											}
											chkd.maps.curr->undos().startNext(0);
										}
										break;
									case IDC_CHECK_LIFTED:
										{
											UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
											ChkUnit* unit;
											while ( currUnit != nullptr )
											{
												int index = currUnit->index;
												if ( chkd.maps.curr->getUnit(unit, index) )
												{
													chkd.maps.curr->undos().addUndoUnitChange(index, UNIT_FIELD_STATEFLAGS, unit->stateFlags);
													if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED )
														unit->stateFlags |= UNIT_STATE_LIFTED; // Check lifted state
													else if ( SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
														unit->stateFlags ^= UNIT_STATE_LIFTED; // Uncheck lifted state
												}
												currUnit = currUnit->next;
											}
											chkd.maps.curr->undos().startNext(0);
										}
										break;
								}
								break;
							case EN_SETFOCUS:
								switch ( LOWORD(wParam) )
								{
									case IDC_EDIT_HP	   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_HITPOINTS); break;
									case IDC_EDIT_MP	   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_ENERGY	 ); break;
									case IDC_EDIT_SHIELD   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_SHIELDS	 ); break;
									case IDC_EDIT_RESOURCES: preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_RESOURCES); break;
									case IDC_EDIT_HANGER   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_HANGER	 ); break;
									case IDC_EDIT_ID	   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_ID		 ); break;
									case IDC_EDIT_XC	   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_XC		 ); break;
									case IDC_EDIT_YC	   : preservedStats.addStats(chkd.maps.curr->selections(), UNIT_FIELD_YC		 ); break;
								}
								break;
							case EN_KILLFOCUS:
								switch ( LOWORD(wParam) )
								{
									case IDC_EDIT_HP: case IDC_EDIT_MP: case IDC_EDIT_SHIELD: case IDC_EDIT_RESOURCES:
									case IDC_EDIT_HANGER: case IDC_EDIT_ID: case IDC_EDIT_XC: case IDC_EDIT_YC:
										{
											preservedStats.convertToUndo();
										}
										break;
								}
								break;
							case EN_UPDATE:
								if ( !initilizing )
								{
									switch ( LOWORD(wParam) )
									{
										case IDC_EDIT_HP:
											{
												u8 hpPercent;
												if ( GetEditNum<u8>(hWnd, IDC_EDIT_HP, hpPercent) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
															unit->hitpoints = hpPercent;
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(false);
												}
											}
											break;
										case IDC_EDIT_MP:
											{
												u8 mpPercent;
												if ( GetEditNum<u8>(hWnd, IDC_EDIT_MP, mpPercent) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
															unit->energy = mpPercent;
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(false);
												}
											}
											break;
										case IDC_EDIT_SHIELD:
											{
												u8 shieldPercent;
												if ( GetEditNum<u8>(hWnd, IDC_EDIT_SHIELD, shieldPercent) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
															unit->shields = shieldPercent;
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(false);
												}
											}
											break;
										case IDC_EDIT_RESOURCES:
											{
												u32 resources;
												if ( GetEditNum<u32>(hWnd, IDC_EDIT_RESOURCES, resources) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
															unit->resources = resources;
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(false);
												}
											}
											break;
										case IDC_EDIT_HANGER:
											{
												u16 hanger;
												if ( GetEditNum<u16>(hWnd, IDC_EDIT_HANGER, hanger) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
															unit->hanger = hanger;
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(true);
												}
											}
											break;
										case IDC_EDIT_ID:
											{
												u16 unitID;
												if ( GetEditNum<u16>(hWnd, IDC_EDIT_ID, unitID) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
														{
															unit->id = unitID;
															int row = GetItemRow(hUnitList, index);

															string unitName;
															chkd.maps.curr->getUnitName(unitName, unitID);
															SetItem(hUnitList, row, UNIT_NAME_COLUMN, unitName.c_str());
														
															if ( currUnit == chkd.maps.curr->selections().getFirstUnit() )
																SetWindowText(hWnd, unitName.c_str());
														}
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(true);
													ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
												}
											}
											break;
										case IDC_EDIT_XC:
											{
												u16 unitXC;
												if ( GetEditNum<u16>(hWnd, IDC_EDIT_XC, unitXC) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
														{
															unit->xc = unitXC;
															int row = GetItemRow(hUnitList, index);
															SetItem(hUnitList, row, UNIT_XC_COLUMN, unitXC);
														}
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(true);
													ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
												}
											}
											break;
										case IDC_EDIT_YC:
											{
												u16 unitYC;
												if ( GetEditNum<u16>(hWnd, IDC_EDIT_YC, unitYC) )
												{
													UnitNode* currUnit = chkd.maps.curr->selections().getFirstUnit();
													HWND hUnitList = GetDlgItem(hWnd, IDC_UNITLIST);
													ChkUnit* unit;
													while ( currUnit != nullptr )
													{
														int index = currUnit->index;
														if ( chkd.maps.curr->getUnit(unit, index) )
														{
															unit->yc = unitYC;
															int row = GetItemRow(hUnitList, index);
															SetItem(hUnitList, row, UNIT_YC_COLUMN, unitYC);
														}
														currUnit = currUnit->next;
													}
													chkd.maps.curr->Redraw(true);
													ListView_SortItems(hUnitList, ForwardCompareLvItems, this);
												}
											}
											break;
										default:
											return DefDlgProc(hWnd, msg, wParam, lParam);
											break;
									}
								}
								break;
						}
				}
			}
			break;

		case WM_NOTIFY:
			{
				switch ( LPNMHDR(lParam)->code )
				{
					case LVN_COLUMNCLICK:
						{
							int newColumn = LPNMLISTVIEW(lParam)->iSubItem;
							if ( newColumn == columnSortedBy )
								flipSort = !flipSort;
							else
								flipSort = false;

							columnSortedBy = newColumn;
							ListView_SortItems(LPNMHDR(lParam)->hwndFrom, ForwardCompareLvItems, this);
						}
						break;
					case LVN_ITEMCHANGED:
						{
							preservedStats.convertToUndo();
							if ( changeHighlightOnly == false )
							{
								NMLISTVIEW* itemInfo = (NMLISTVIEW*)lParam;
								u16 index = (u16)itemInfo->lParam;

								if ( itemInfo->uNewState & LVIS_SELECTED && initilizing == false ) // Selected
									// Add item to selection
								{
									bool firstSelected = !(chkd.maps.curr->selections().hasUnits());
									chkd.maps.curr->selections().addUnit(index);

									if ( firstSelected )
										EnableUnitEditing(hWnd);

									ChkUnit* unit;
									if ( chkd.maps.curr->getUnit(unit, index) )
									{
										string unitName;
										chkd.maps.curr->getUnitName(unitName, unit->id);
										SetWindowText(hWnd, unitName.c_str());
										SetUnitFieldText(hWnd, unit);
									}

									chkd.maps.curr->Redraw(false);
								}
								else if ( itemInfo->uOldState & LVIS_SELECTED ) // From selected to not selected
									// Remove item from selection
								{
									chkd.maps.curr->selections().removeUnit(index);

									if ( !( chkd.maps.curr->selections().hasUnits() )
										 && !(	  GetKeyState(VK_DOWN)	 &0x8000
											   || GetKeyState(VK_UP)	 &0x8000
											   || GetKeyState(VK_LEFT)	 &0x8000
											   || GetKeyState(VK_RIGHT)	 &0x8000
											   || GetKeyState(VK_LBUTTON)&0x8000
											   || GetKeyState(VK_RBUTTON)&0x8000 ) )
										DisableUnitEditing(hWnd);

									chkd.maps.curr->Redraw(false);
								}
							}
							return DefWindowProc(hWnd, msg, wParam, lParam);
						}
						break;
				}
			}
			break;

		case WM_KEYDOWN:
			switch ( wParam )
			{
				case VK_RETURN:
					{
						HWND hClose = GetDlgItem(hWnd, IDCLOSE);
						BOOL closeSelected = SendMessage(hClose, BM_GETSTATE, NULL, NULL) & BST_PUSHED;
					}
					break;
			}
			break;

		case IDCLOSE:
			columnSortedBy = UNIT_INDEX_COLUMN; // Reset to sort by index
			DestroyThis();
			break;

		case WM_CLOSE:
			columnSortedBy = UNIT_INDEX_COLUMN; // Reset to sort by index
			DestroyThis();
			break;

		default:
			return FALSE;
			break;
	}
	return TRUE;
}
