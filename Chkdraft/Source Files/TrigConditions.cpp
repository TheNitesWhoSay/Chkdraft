#include "TrigConditions.h"
#include "Chkdraft.h"

#define TOP_CONDITION_PADDING 50
#define BOTTOM_CONDITION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum ID
{
	LIST_CONDITIONS = ID_FIRST
};

TrigConditionsWindow::TrigConditionsWindow() : hBlack(NULL), trigIndex(0)
{
	InitializeArgMaps();
}

bool TrigConditionsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigConditions", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigConditions", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigConditionsWindow::DestroyThis()
{
	return false;
}

void TrigConditionsWindow::RefreshWindow(u32 trigIndex)
{
	listConditions.ClearItems();
	this->trigIndex = trigIndex;
	Trigger* trig;
	TextTrigGenerator ttg;
	if ( chkd.maps.curr->getTrigger(trig, trigIndex) &&
		 ttg.LoadScenario(chkd.maps.curr) )
	{
		for ( u8 y=0; y<NUM_TRIG_CONDITIONS; y++ )
		{
			Condition& condition = trig->conditions[y];
			if ( condition.condition > 0 && condition.condition <= 23 )
			{
				u8 numArgs = u8(conditionArgMaps[condition.condition].size());
				if ( numArgs > 8 )
					numArgs = 8;

				listConditions.item(0, y).SetText(
					ttg.GetConditionName(condition.condition).c_str()
				);
				for ( u8 x=0; x<numArgs; x++ )
				{
					listConditions.item(x+1, y).SetText(
						ttg.GetConditionArgument(condition, x, conditionArgMaps[condition.condition]).c_str()
					);
				}
				for ( u8 x=numArgs; x<8; x++ )
					listConditions.item(x+1, y).SetText("");
			}
			else if ( condition.condition == 0 )
			{
				for ( u8 x=0; x<9; x++ )
					listConditions.item(x, y).SetText("");
			}
		}

		listConditions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH);
	}
}

void TrigConditionsWindow::DoSize()
{
	listConditions.SetPos(0, TOP_CONDITION_PADDING, cliWidth()-2, cliHeight()-TOP_CONDITION_PADDING-BOTTOM_CONDITION_PADDING);
}

void TrigConditionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_TAB )
		SendMessage(listConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
	else if ( wParam == VK_RETURN )
		SendMessage(listConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void TrigConditionsWindow::InitializeArgMaps()
{
	#define ADD_ARRAY(anArray, vector)					\
	for ( u8 i=0; i<sizeof(anArray)/sizeof(u8); i++ )	\
		vector.push_back(anArray[i]);

	u8 accumulate[] = { 0, 1, 2, 3 };
	u8 bring[] = { 0, 3, 4, 1, 2 };
	u8 command[] = { 0, 2, 3, 1 };
	u8 commandTheLeast[] = { 0 };
	u8 commandTheLeastAt[] = { 0, 1 };
	u8 commandTheMost[] = { 0 };
	u8 commandTheMostAt[] = { 0, 1 };
	u8 coundownTimer[] = { 0, 1 };
	u8 deaths[] = { 0, 2, 3, 1 };
	u8 elapsedTime[] = { 0, 1 };
	u8 highestScore[] = { 0, 1, 2, 3 };
	u8 kill[] = { 0, 2, 3, 1 };
	u8 leastKills[] = { 0 };
	u8 leastResources[] = { 0 };
	u8 lowestScore[] = { 0 };
	u8 mostKills[] = { 0 };
	u8 mostResources[] = { 0 };
	u8 opponents[] = { 0, 1, 2 };
	u8 score[] = { 0, 1, 2, 3 };
	u8 switchState[] = { 0, 1 };

	ADD_ARRAY(accumulate, conditionArgMaps[4]);
	ADD_ARRAY(bring, conditionArgMaps[3]);
	ADD_ARRAY(command, conditionArgMaps[2]);
	ADD_ARRAY(commandTheLeast, conditionArgMaps[16]);
	ADD_ARRAY(commandTheLeastAt, conditionArgMaps[17]);
	ADD_ARRAY(commandTheMost, conditionArgMaps[6]);
	ADD_ARRAY(commandTheMostAt, conditionArgMaps[7]);
	ADD_ARRAY(coundownTimer, conditionArgMaps[1]);
	ADD_ARRAY(deaths, conditionArgMaps[15]);
	ADD_ARRAY(elapsedTime, conditionArgMaps[12]);
	ADD_ARRAY(highestScore, conditionArgMaps[9]);
	ADD_ARRAY(kill, conditionArgMaps[5]);
	ADD_ARRAY(leastKills, conditionArgMaps[18]);
	ADD_ARRAY(leastResources, conditionArgMaps[20]);
	ADD_ARRAY(lowestScore, conditionArgMaps[19]);
	ADD_ARRAY(mostKills, conditionArgMaps[8]);
	ADD_ARRAY(mostResources, conditionArgMaps[10]);
	ADD_ARRAY(opponents, conditionArgMaps[14]);
	ADD_ARRAY(score, conditionArgMaps[21]);
	ADD_ARRAY(switchState, conditionArgMaps[11]);
}

void TrigConditionsWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	listConditions.CreateThis(hWnd, 0, 40, 100, 100, true, LIST_CONDITIONS);
	listConditions.SetFont(defaultFont, false);
	for ( int i=0; i<9; i++ )
		listConditions.AddColumn(0, " - ", 50, LVCFMT_CENTER);

	listConditions.Show();

	for ( int y=0; y<16; y++ )
		listConditions.AddRow(9, (y+1)*100);

	RefreshWindow(trigIndex);
}

LRESULT TrigConditionsWindow::Notify(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( wParam == LIST_CONDITIONS )
	{
		MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
		mis->itemWidth = DEFAULT_COLUMN_WIDTH;
		mis->itemHeight = 15;
		return TRUE;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = DefWindowProc(hWnd, msg, wParam, lParam);
	DrawSelectedCondition();
	return result;
}

BOOL TrigConditionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, string& str)
{
	Trigger* trig;
	TextTrigCompiler ttc;
	if ( gridItemX == 0 ) // Condition Name
	{
		cout << '(' << gridItemX << ", " << gridItemY << ") Condition Name: " << str << endl;
		u8 ID = CID_NO_CONDITION;
		if ( ttc.ParseConditionName(str, ID) )
		{
			if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
			{
				Condition &condition = trig->condition((u8)gridItemY);
				if ( condition.condition != ID )
				{
					if ( ID == CID_COMMAND || ID == CID_BRING || ID == CID_COMMAND_THE_MOST_AT || ID == CID_COMMAND_THE_LEAST_AT )
						condition.locationNum = 64;
					else
						condition.locationNum = 0;

					condition.players = 0;
					condition.amount = 0;
					condition.unitID = 0;
					condition.comparison = 0;
					condition.condition = ID;
					condition.typeIndex = 0;
					condition.flags = ttc.defaultFlags(ID);
					condition.internalData = 0;
					chkd.maps.curr->notifyChange(false);
					RefreshWindow(trigIndex);
					chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
					return FALSE;
				}
			}
		}
		else if ( str.length() == 0 )
		{
			Trigger* trig;
			if ( chkd.maps.curr->getTrigger(trig, trigIndex) &&
				 ((u8)gridItemY) <= NUM_TRIG_CONDITIONS &&
				 trig->conditions[(u8)gridItemY].condition != ID )
			{
				trig->deleteCondition((u8)gridItemY);
				chkd.maps.curr->notifyChange(false);
				RefreshWindow(trigIndex);
				chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
				return FALSE;
			}
		}
		else
		{
			cout << "Unrecognized Condition Name" << endl;
			return FALSE;
		}
	}
	else // Condition Arg
	{
		u8 argNum = (u8)gridItemX - 1;
		cout << '(' << gridItemX << ", " << gridItemY << ") Argument " << (int)argNum << ": " << str << endl;
		if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
		{
			if ( ttc.ParseConditionArg(str, argNum, conditionArgMaps[trig->condition((u8)gridItemY).condition],
				 trig->condition((u8)gridItemY), chkd.maps.curr) )
			{
				chkd.maps.curr->notifyChange(false);
				RefreshWindow(trigIndex);
				chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
			}
		}
	}
	return FALSE;
}

void TrigConditionsWindow::DrawSelectedCondition()
{
	HDC hDC = GetDC(getHandle());
	if ( hDC != NULL )
	{
		Trigger* trig;
		if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
		{
			int focusedX = -1,
				focusedY = -1;

			if ( listConditions.GetFocusedItem(focusedX, focusedY) )
			{
				u8 conditionNum = (u8)focusedY;
				TextTrigGenerator ttg;
				string str;
				ttg.LoadScenario(chkd.maps.curr);
				str = chkd.trigEditorWindow.triggersWindow.GetConditionString(conditionNum, trig, ttg);
				ttg.ClearScenario();

				UINT width = 0, height = 0;
				GetStringDrawSize(hDC, width, height, str);
				HBRUSH hBackground = CreateSolidBrush(GetBkColor(hDC));
				RECT rect;
				rect.left = listConditions.Left()+5;
				rect.right = listConditions.Right()-5;
				rect.top = listConditions.Top()-height-1;
				rect.bottom = listConditions.Top()-1;
				FillRect(hDC, &rect, hBackground);
				DeleteBrush(hBackground);

				SetBkMode(hDC, TRANSPARENT);
				DrawString(hDC, listConditions.Left()+6, listConditions.Top()-height-1, 500, RGB(0, 0, 0), str);
			}
		}
		ReleaseDC(getHandle(), hDC);
	}
}

int TrigConditionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
	string text;
	if ( listConditions.item(gridItemX, gridItemY).getText(text) )
	{
		HDC hDC = GetDC(getHandle());
		UINT width = 0, height = 0;
		if ( GetStringDrawSize(hDC, width, height, text) )
			return width+2;
		ReleaseDC(getHandle(), hDC);
	}
	return 0;
}

void TrigConditionsWindow::PreDrawItems()
{
	HDC hDC = GetDC(getHandle());
	EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, NULL);
	ReleaseDC(getHandle(), hDC);
	hBlack = CreateSolidBrush(RGB(0, 0, 0));
}

void TrigConditionsWindow::SysColorRect(HDC hDC, RECT &rect, DWORD color)
{
	SetBkColor(hDC, GetSysColor(color));
	FillRect(hDC, &rect, GetSysColorBrush(color));
}

void TrigConditionsWindow::DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart)
{
	RECT rcFill;
	rcFill.top = rcItem.top;
	rcFill.bottom = rcItem.bottom - 1;
	rcFill.left = xStart;
	rcFill.right = xStart + width - 1;

	if ( listConditions.isFocused(gridItemX, gridItemY) )
		SysColorRect(hDC, rcFill, COLOR_ACTIVEBORDER);
	else if ( listConditions.item(gridItemX, gridItemY).isSelected() )
		SysColorRect(hDC, rcFill, COLOR_HIGHLIGHT);
	else
		SysColorRect(hDC, rcFill, COLOR_WINDOW);
}

void TrigConditionsWindow::DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart)
{
	RECT rcFill;
	rcFill.top = rcItem.top - 1;
	rcFill.bottom = rcItem.bottom;
	rcFill.left = xStart - 1;
	rcFill.right = xStart + width;

	FrameRect(hDC, &rcFill, hBlack);
}

void TrigConditionsWindow::DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart)
{
	int width = ListView_GetColumnWidth(listConditions.getHandle(), gridItemX);
	DrawItemBackground(hDC, gridItemX, gridItemY, rcItem, width, xStart);

	string text;
	if ( listConditions.item(gridItemX, gridItemY).getText(text) )
		DrawString(hDC, xStart+1, rcItem.top, width-2, RGB(0, 0, 0), text);

	DrawItemFrame(hDC, rcItem, width, xStart);
	xStart += width;
}

void TrigConditionsWindow::DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis)
{
	if ( gridId == LIST_CONDITIONS )
	{
		bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
			 drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
			 drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

		if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
		{
			RECT &rcItem = pdis->rcItem;
			int itemStart = rcItem.left;

			int numColumns = listConditions.GetNumColumns();
			for ( int x=0; x<numColumns; x++ )
				DrawGridViewItem(pdis->hDC, x, pdis->itemID, rcItem, itemStart);
		}
	}
}

void TrigConditionsWindow::DrawTouchups(HDC hDC)
{
	RECT rect = { };
	if ( listConditions.GetEditItemRect(rect) )
	{
		rect.left -= 1;
		rect.top -= 1;
		HBRUSH hHighlight = CreateSolidBrush(RGB(0, 0, 200));
		FrameRect(hDC, &rect, hHighlight);
		rect.left -= 1;
		rect.top -= 1;
		rect.right += 1;
		rect.bottom += 1;
		FrameRect(hDC, &rect, hHighlight);
		DeleteBrush(hHighlight);
	}
}

void TrigConditionsWindow::PostDrawItems()
{
	DeleteBrush(hBlack);
	hBlack = NULL;
}

LRESULT TrigConditionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_NOTIFY: return Notify(hWnd, msg, wParam, lParam); break;
		case WM_MEASUREITEM: return MeasureItem(hWnd, msg, wParam, lParam); break;
		case WM_ERASEBKGND: return EraseBackground(hWnd, msg, wParam, lParam); break;
		case WM_GETGRIDITEMWIDTH: return GetGridItemWidth(LOWORD(wParam), HIWORD(wParam)); break;
		case WM_PREDRAWITEMS: PreDrawItems(); break;
		case WM_DRAWGRIDVIEWITEM: DrawGridViewRow((UINT)wParam, (PDRAWITEMSTRUCT)lParam); break;
		case WM_DRAWTOUCHUPS: DrawTouchups((HDC)wParam); break;
		case WM_POSTDRAWITEMS: PostDrawItems(); break;
		case WM_GETGRIDITEMCARETPOS: return -1; break;
		case WM_GRIDITEMCHANGING: return GridItemChanging(LOWORD(wParam), HIWORD(wParam), *(string*)lParam); break;
		default: return DefWindowProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}
