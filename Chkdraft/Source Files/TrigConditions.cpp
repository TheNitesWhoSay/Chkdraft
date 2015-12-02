#include "TrigConditions.h"
#include "Clipboard.h"
#include "Chkdraft.h"
#include <string>
#include <map>

#define TOP_CONDITION_PADDING 50
#define BOTTOM_CONDITION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum ID
{
	LIST_CONDITIONS = ID_FIRST,
	CHECK_ENABLED_1,
	CHECK_ENABLED_16 = (CHECK_ENABLED_1+15)
};

TrigConditionsWindow::TrigConditionsWindow() : hBlack(NULL), trigIndex(0), listConditions(*this),
	suggestions(listConditions.GetSuggestions())
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
	suggestions.Hide();
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

				listConditions.item(1, y).SetText(
					ttg.GetConditionName(condition.condition).c_str()
				);
				for ( u8 x=0; x<numArgs; x++ )
				{
					listConditions.item(x+2, y).SetText(
						ttg.GetConditionArgument(condition, x, conditionArgMaps[condition.condition]).c_str()
					);
				}
				for ( u8 x=numArgs; x<8; x++ )
					listConditions.item(x+2, y).SetText("");
				
				listConditions.SetEnabledCheck(y, !condition.isDisabled());
			}
			else if ( condition.condition == 0 )
			{
				for ( u8 x=0; x<10; x++ )
					listConditions.item(x, y).SetText("");

				listConditions.SetEnabledCheck(y, false);
			}
		}

		listConditions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH, DEFAULT_COLUMN_WIDTH*2);
	}
	listConditions.RedrawHeader();
}

void TrigConditionsWindow::DoSize()
{
	listConditions.SetPos(2, TOP_CONDITION_PADDING, cliWidth()-2, cliHeight()-TOP_CONDITION_PADDING-BOTTOM_CONDITION_PADDING);
}

void TrigConditionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_TAB )
		SendMessage(listConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
	else if ( wParam == VK_RETURN )
		SendMessage(listConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void TrigConditionsWindow::HideSuggestions()
{
	suggestions.Hide();	
}

void TrigConditionsWindow::ConditionEnableToggled(u8 conditionNum)
{
	Trigger* trig;
	if ( conditionNum >= 0 && conditionNum < 16 && chkd.maps.curr->getTrigger(trig, trigIndex) )
	{
		Condition &condition = trig->condition(conditionNum);
		if ( condition.condition != CID_NO_CONDITION )
		{
			condition.ToggleDisabled();

			chkd.maps.curr->notifyChange(false);
			RefreshWindow(trigIndex);
			chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);

			listConditions.SetEnabledCheck(conditionNum, !condition.isDisabled());
		}
	}
}

void TrigConditionsWindow::EscapeString(std::string &str)
{
	size_t length = str.length();
	for ( size_t i = 0; i < length; i++ )
	{
		if ( str[i] == '\t' )
		{
			str[i] = '\\';
			i++;
			str.insert(i, "t");
			length++;
		}
	}
}

bool TrigConditionsWindow::BuildSelectionString(std::string &str)
{
	int numRows = listConditions.NumRows();
	int numColumns = listConditions.NumColumns();
	std::stringstream ssSelection;
	for ( int y = 0; y < numRows; y++ )
	{
		bool rowUsed = false;
		bool hasPreviousThisRow = false;
		for ( int x = 0; x < numColumns; x++ )
		{
			auto currItem = listConditions.item(x, y);
			RawString currItemText;
			if ( listConditions.isFocused(x, y) || currItem.isSelected() )
			{
				if ( hasPreviousThisRow )
					ssSelection << '\t';
				else
					hasPreviousThisRow = true;

				if ( currItem.getTextLength() > 0 && currItem.getText(currItemText) )
				{
					ChkdString chkdString;
					if ( MakeChkdStr(currItemText, chkdString) )
						ssSelection << currItemText;
				}
			}
		}

		if ( hasPreviousThisRow )
			ssSelection << "\r\n";
	}
	str = ssSelection.str();
	return true;
}

void TrigConditionsWindow::CutSelection()
{
	if ( listConditions.isSelectionRectangular() )
	{
		std::string str;
		if ( BuildSelectionString(str) )
			StringToWindowsClipboard(str);
		else
			Error("Problem building clipboard string");

		listConditions.DeleteSelection();
	}
	else
		Error("Operation only supported on rectangular selections.");
}

void TrigConditionsWindow::CopySelection()
{
	if ( listConditions.isSelectionRectangular() )
	{
		std::string str;
		if ( BuildSelectionString(str) )
			StringToWindowsClipboard(str);
		else
			Error("Problem building clipboard string");
	}
	else
		Error("Operation only supported on rectangular selections.");
}

void TrigConditionsWindow::Paste()
{
	if ( listConditions.isSelectionRectangular() )
	{

	}
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
	u8 highestScore[] = { 0 };
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
	listConditions.CreateThis(hWnd, 2, 40, 100, 100, LIST_CONDITIONS);
	suggestions.CreateThis(hWnd, 0, 0, 200, 100);
	RefreshWindow(trigIndex);
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
	return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
	DrawSelectedCondition();
	return result;
}

void TrigConditionsWindow::ChangeConditionType(Condition &condition, u8 newId)
{
	if ( condition.condition != newId )
	{
		if ( newId == CID_COMMAND || newId == CID_BRING ||
			newId == CID_COMMAND_THE_MOST_AT || newId == CID_COMMAND_THE_LEAST_AT )
		{
			condition.locationNum = 64;
		}
		else
			condition.locationNum = 0;

		condition.players = 0;
		condition.amount = 0;
		condition.unitID = 0;

		if ( newId == CID_SWITCH )
			condition.comparison = 3;
		else
			condition.comparison = 0;

		condition.condition = newId;
		condition.typeIndex = 0;
		condition.flags = TextTrigCompiler::defaultFlags(newId);
		condition.internalData = 0;
	}
}

bool TrigConditionsWindow::TransformCondition(Condition &condition, u8 newId)
{
	if ( condition.condition != newId )
	{
		ChangeConditionType(condition, newId);
		chkd.maps.curr->notifyChange(false);
		RefreshWindow(trigIndex);
		chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
		return true;
	}
	return false;
}

void TrigConditionsWindow::UpdateConditionName(u8 conditionNum, std::string &newText)
{
	Trigger* trig;
	TextTrigCompiler ttc;
	u8 newId = CID_NO_CONDITION;
	if ( ttc.ParseConditionName(newText, newId) || ttc.ParseConditionName(suggestions.Take(), newId) )
	{
		if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
		{
			Condition &condition = trig->condition(conditionNum);
			TransformCondition(condition, newId);
		}
	}
	else if ( newText.length() == 0 )
	{
		if ( chkd.maps.curr->getTrigger(trig, trigIndex) &&
			 trig->conditions[conditionNum].condition != newId )
		{
			trig->deleteCondition((u8)conditionNum);
			chkd.maps.curr->notifyChange(false);
			RefreshWindow(trigIndex);
			chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
		}
	}
}

void TrigConditionsWindow::UpdateConditionArg(u8 conditionNum, u8 argNum, std::string &newText)
{
	RawString rawUpdateText, rawSuggestText;
	std::string suggestionString = suggestions.Take();
	Trigger* trig;
	TextTrigCompiler ttc;
	if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
	{
		if ( ( ParseChkdStr(ChkdString(newText), rawUpdateText) &&
			   ttc.ParseConditionArg(rawUpdateText, argNum, conditionArgMaps[trig->condition(conditionNum).condition],
				trig->condition(conditionNum), chkd.maps.curr) ) ||
			 ( ParseChkdStr(ChkdString(suggestionString), rawSuggestText) &&
			   ttc.ParseConditionArg(rawSuggestText, argNum, conditionArgMaps[trig->condition(conditionNum).condition],
				trig->condition(conditionNum), chkd.maps.curr) ) )
		{
			chkd.maps.curr->notifyChange(false);
			RefreshWindow(trigIndex);
			chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
		}
	}
}

BOOL TrigConditionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, std::string& str)
{
	if ( gridItemY >= 0 && gridItemY < NUM_TRIG_CONDITIONS )
	{
		u8 conditionNum = (u8)gridItemY;
		if ( gridItemX == 1 ) // Condition Name
		{
			UpdateConditionName(conditionNum, str);
		}
		else if ( gridItemX > 1 ) // Condition Arg
		{
			u8 argNum = (u8)gridItemX - 2;
			UpdateConditionArg(conditionNum, argNum, str);
		}
	}
	return FALSE;
}

BOOL TrigConditionsWindow::GridItemDeleting(u16 gridItemX, u16 gridItemY)
{
	Trigger* trig;
	if ( gridItemY >= 0 && gridItemY < NUM_TRIG_CONDITIONS )
	{
		u8 conditionNum = (u8)gridItemY;

		if ( gridItemX == 1 && // Condition Name
			 chkd.maps.curr->getTrigger(trig, trigIndex) &&
			 trig->conditions[conditionNum].condition != 0 )
		{
			ChangeConditionType(trig->conditions[conditionNum], 0);
		}
		else if ( gridItemX > 1 ) // Condition Arg
		{
			// Nothing planned yet
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
				std::string str;
				ttg.LoadScenario(chkd.maps.curr);
				str = chkd.trigEditorWindow.triggersWindow.GetConditionString(conditionNum, trig, ttg);
				ttg.ClearScenario();

				UINT width = 0, height = 0;
				GetStringDrawSize(hDC, width, height, str);
				HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_MENU));
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
		ReleaseDC(hDC);
	}
}

int TrigConditionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
	std::string text;
	if ( listConditions.item(gridItemX, gridItemY).getText(text) )
	{
		HDC hDC = GetDC(getHandle());
		UINT width = 0, height = 0;
		if ( GetStringDrawSize(hDC, width, height, text) )
			return width+2;
		ReleaseDC(hDC);
	}
	return 0;
}

void TrigConditionsWindow::PreDrawItems()
{
	HDC hDC = GetDC(getHandle());
	EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, NULL);
	ReleaseDC(hDC);
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
	if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < 16 )
		listConditions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

	int width = ListView_GetColumnWidth(listConditions.getHandle(), gridItemX);
	DrawItemBackground(hDC, gridItemX, gridItemY, rcItem, width, xStart);

	std::string text;
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

void TrigConditionsWindow::SuggestNothing()
{
	std::cout << "Suggest Nothing" << std::endl;
	suggestions.ClearStrings();
	suggestions.Hide();
	HDC hDC = GetDC(getHandle());
	HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_MENU));
	RECT rect;
	rect.left = dropUnits.Left();
	rect.right = dropUnits.Right();
	rect.top = dropUnits.Top();
	rect.bottom = dropUnits.Bottom();

	dropUnits.Hide();

	FillRect(hDC, &rect, hBackground);
	DeleteBrush(hBackground);
}

void TrigConditionsWindow::SuggestUnit()
{
	std::cout << "Suggest Unit" << std::endl;
	dropUnits.Show();
	if ( chkd.maps.curr != nullptr )
	{
		for ( u16 i = 0; i < NUM_UNIT_NAMES; i++ )
		{
			ChkdString str;
			chkd.maps.curr->getUnitName(str, i);
			suggestions.AddString(str);
			if ( str.compare(std::string(DefaultUnitDisplayName[i])) != 0 )
				suggestions.AddString(std::string(DefaultUnitDisplayName[i]));
		}
	}
	suggestions.Show();
}

void TrigConditionsWindow::SuggestLocation()
{
	std::cout << "Suggest Location" << std::endl;
	ScenarioPtr chk = chkd.maps.curr;
	ChkLocation* loc = nullptr;
	if ( chk != nullptr )
	{
		suggestions.AddString(std::string("No Location"));
		u16 locationCapacity = (u16)chk->locationCapacity();
		for ( u16 i = 0; i < locationCapacity; i++ )
		{
			if ( chk->locationIsUsed(i) )
			{
				ChkdString locationName;
				if ( chk->getLocation(loc, u8(i)) && loc->stringNum > 0 && chk->getLocationName((u16)i, locationName) )
					suggestions.AddString(locationName);
				else
				{
					std::stringstream ssLoc;
					ssLoc << "Location " << i;
					suggestions.AddString(ssLoc.str());
				}
			}
		}
	}
	suggestions.Show();
}

void TrigConditionsWindow::SuggestPlayer()
{
	std::cout << "Suggest Player" << std::endl;
	ScenarioPtr chk = chkd.maps.curr;
	if ( chk != nullptr )
	{
		for ( u8 i = 0; i < numTriggerPlayers; i++ )
			suggestions.AddString(std::string(triggerPlayers[i]));
	}
	suggestions.Show();
}

void TrigConditionsWindow::SuggestAmount()
{
	std::cout << "Suggest Amount" << std::endl;
	//suggestions.Show();
}

void TrigConditionsWindow::SuggestNumericComparison()
{
	std::cout << "Suggest Numeric Comparison" << std::endl;
	suggestions.AddString(std::string("At Least"));
	suggestions.AddString(std::string("At Most"));
	suggestions.AddString(std::string("Exactly"));
	suggestions.Show();
}

void TrigConditionsWindow::SuggestResourceType()
{
	std::cout << "Suggest Resource Type" << std::endl;
	suggestions.AddString(std::string("Ore"));
	suggestions.AddString(std::string("Ore and Gas"));
	suggestions.AddString(std::string("Gas"));
	suggestions.Show();
}

void TrigConditionsWindow::SuggestScoreType()
{
	std::cout << "Suggest Score Type" << std::endl;
	for ( u8 i = 0; i < numTriggerScores; i++ )
		suggestions.AddString(std::string(triggerScores[i]));
	suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitch()
{
	ScenarioPtr chk = chkd.maps.curr;
	if ( chk != nullptr )
	{
		for ( u16 i = 0; i < 256; i++ )
		{
			ChkdString str;
			if ( chk->getSwitchName(str, (u8)i) )
				suggestions.AddString(str);
			else
			{
				std::stringstream ss;
				ss << "Switch " << i + 1 << std::endl;
				suggestions.AddString(ss.str());
			}
		}
	}
	std::cout << "Suggest Switch" << std::endl;
	suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitchState()
{
	std::cout << "Suggest Switch State" << std::endl;
	suggestions.AddString(std::string("Cleared"));
	suggestions.AddString(std::string("Set"));
	suggestions.Show();
}

void TrigConditionsWindow::SuggestComparison()
{
	std::cout << "Suggest Comparison" << std::endl;
	suggestions.Show();
}

void TrigConditionsWindow::SuggestConditionType()
{
	std::cout << "Suggest Condition Type" << std::endl;
	suggestions.AddStrings(triggerConditions, numTriggerConditions);
	suggestions.Show();
}

void TrigConditionsWindow::SuggestTypeIndex()
{
	std::cout << "Suggest Type Index" << std::endl;
	suggestions.Show();
}

void TrigConditionsWindow::SuggestFlags()
{
	std::cout << "Suggest Flags" << std::endl;
	suggestions.Show();
}

void TrigConditionsWindow::SuggestInternalData()
{
	std::cout << "Suggest Internal Data" << std::endl;
	suggestions.Show();
}

void TrigConditionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
	Trigger* trig;
	if ( chkd.maps.curr->getTrigger(trig, trigIndex) )
	{
		Condition &condition = trig->condition((u8)gridItemY);
		ConditionArgType argType = ConditionArgType::CndNoType;
		if ( gridItemX == 1 ) // Condition Name
			argType = ConditionArgType::CndConditionType;
		else if ( gridItemX > 1 ) // Condition Arg
		{
			u8 conditionArgNum = (u8)gridItemX - 2;
			if ( condition.condition <= 23 && conditionArgMaps[condition.condition].size() > conditionArgNum )
			{
				u8 textTrigArgNum = conditionArgMaps[condition.condition][conditionArgNum];
				argType = condition.TextTrigArgType(textTrigArgNum);
			}
		}

		if ( argType != ConditionArgType::CndNoType )
		{
			POINT pt = listConditions.GetFocusedBottomRightScreenPt();
			if ( pt.x != -1 || pt.y != -1 )
				suggestions.MoveTo(pt.x, pt.y);
		}

		suggestions.ClearStrings();
		switch ( argType )
		{
			case CndNoType: SuggestNothing(); break;
			case CndUnit: SuggestUnit(); break;
			case CndLocation: SuggestLocation(); break;
			case CndPlayer: SuggestPlayer(); break;
			case CndAmount: SuggestAmount(); break;
			case CndNumericComparison: SuggestNumericComparison(); break;
			case CndResourceType: SuggestResourceType(); break;
			case CndScoreType: SuggestScoreType(); break;
			case CndSwitch: SuggestSwitch(); break;
			case CndSwitchState: SuggestSwitchState(); break;
			case CndComparison: SuggestComparison(); break;
			case CndConditionType: SuggestConditionType(); break;
			case CndTypeIndex: SuggestTypeIndex(); break;
			case CndFlags: SuggestFlags(); break;
			case CndInternalData: SuggestInternalData(); break;
		}
	}
}

void TrigConditionsWindow::NewSuggestion(std::string &str)
{
	listConditions.SetEditText(str);
}

LRESULT TrigConditionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( (BOOL)wParam == FALSE )
		suggestions.Hide();

	return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_MEASUREITEM: return MeasureItem(hWnd, msg, wParam, lParam); break;
		case WM_ERASEBKGND: return EraseBackground(hWnd, msg, wParam, lParam); break;
		case WM_SHOWWINDOW: return ShowWindow(hWnd, msg, wParam, lParam); break;
		case WM_NEWSELTEXT: NewSuggestion(*(std::string*)lParam); break;
		case WM_GETGRIDITEMWIDTH: return GetGridItemWidth(LOWORD(wParam), HIWORD(wParam)); break;
		case WM_PREDRAWITEMS: PreDrawItems(); break;
		case WM_DRAWGRIDVIEWITEM: DrawGridViewRow((UINT)wParam, (PDRAWITEMSTRUCT)lParam); break;
		case WM_DRAWTOUCHUPS: DrawTouchups((HDC)wParam); break;
		case WM_POSTDRAWITEMS: PostDrawItems(); break;
		case WM_GETGRIDITEMCARETPOS: return -1; break;
		case WM_GRIDITEMCHANGING: return GridItemChanging(LOWORD(wParam), HIWORD(wParam), *(std::string*)lParam); break;
		case WM_GRIDITEMDELETING: return GridItemDeleting(LOWORD(wParam), HIWORD(wParam)); break;
		case WM_GRIDDELETEFINISHED: RefreshWindow(trigIndex); break;
		case WM_GRIDEDITSTART: GridEditStart(LOWORD(wParam), HIWORD(wParam)); break;
		case WM_GRIDEDITEND: suggestions.Hide(); break;
		default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}
