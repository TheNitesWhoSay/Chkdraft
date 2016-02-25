#include "TrigActions.h"
#include "Chkdraft.h"
#include "ChkdStringInput.h"

#define TOP_ACTION_PADDING 50
#define BOTTOM_ACTION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum ID
{
	GRID_ACTIONS = ID_FIRST,
	BUTTON_EDITSTRING,
	BUTTON_EDITWAV
};

TrigActionsWindow::TrigActionsWindow() : hBlack(NULL), trigIndex(0), gridActions(*this, 64),
	suggestions(gridActions.GetSuggestions()), stringEditEnabled(false), wavEditEnabled(false)
{
	InitializeArgMaps();
}

bool TrigActionsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TrigActions", NULL, false) &&
		 ClassWindow::CreateClassWindow(0, "TrigActions", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigActionsWindow::DestroyThis()
{
	suggestions.Hide();
	return false;
}

void TrigActionsWindow::RefreshWindow(u32 trigIndex)
{
	gridActions.ClearItems();
	this->trigIndex = trigIndex;
	Trigger* trig;
	TextTrigGenerator ttg;
	if ( CM->getTrigger(trig, trigIndex) &&
		ttg.LoadScenario(CM) )
	{
		for ( u8 y = 0; y<NUM_TRIG_ACTIONS; y++ )
		{
			Action& action = trig->actions[y];
			if ( action.action > 0 && action.action <= 59 )
			{
				u8 numArgs = u8(actionArgMaps[action.action].size());
				if ( numArgs > 8 )
					numArgs = 8;

				gridActions.item(1, y).SetText(
					ttg.GetActionName(action.action).c_str()
					);
				for ( u8 x = 0; x<numArgs; x++ )
				{
					gridActions.item(x + 2, y).SetText(
						ttg.GetActionArgument(action, x, actionArgMaps[action.action]).c_str()
						);
				}
				for ( u8 x = numArgs; x<8; x++ )
					gridActions.item(x + 2, y).SetText("");

				gridActions.SetEnabledCheck(y, !action.isDisabled());
			}
			else if ( action.action == 0 )
			{
				for ( u8 x = 0; x<10; x++ )
					gridActions.item(x, y).SetText("");

				gridActions.SetEnabledCheck(y, false);
			}
		}

		gridActions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH, DEFAULT_COLUMN_WIDTH * 2);
	}
	gridActions.RedrawHeader();
}

void TrigActionsWindow::DoSize()
{
	if ( stringEditEnabled || wavEditEnabled )
	{
		gridActions.SetPos(2, TOP_ACTION_PADDING, cliWidth() - 2,
			cliHeight() - TOP_ACTION_PADDING - BOTTOM_ACTION_PADDING - buttonEditString.Height() - 5);
	}
	else
		gridActions.SetPos(2, TOP_ACTION_PADDING, cliWidth() - 2, cliHeight() - TOP_ACTION_PADDING - BOTTOM_ACTION_PADDING - 5);

	if ( stringEditEnabled )
	{
		buttonEditString.MoveTo(cliWidth() - 2 - buttonEditString.Width(),
			cliHeight() - buttonEditString.Height() - 3);
	}

	if ( wavEditEnabled )
	{
		if ( stringEditEnabled )
		{
			buttonEditWav.MoveTo(cliWidth() - 4 - buttonEditWav.Width() - buttonEditString.Width(),
				cliHeight() - buttonEditWav.Height() - 3);
		}
		else
		{
			buttonEditWav.MoveTo(cliWidth() - 2 - buttonEditString.Width(),
				cliHeight() - buttonEditString.Height() - 3);
		}
	}
}

void TrigActionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_TAB )
		SendMessage(gridActions.getHandle(), WM_KEYDOWN, wParam, lParam);
	else if ( wParam == VK_RETURN )
		SendMessage(gridActions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void TrigActionsWindow::HideSuggestions()
{
	suggestions.Hide();
}

void TrigActionsWindow::CndActEnableToggled(u8 actionNum)
{
	Trigger* trig;
	if ( actionNum >= 0 && actionNum < 64 && CM->getTrigger(trig, trigIndex) )
	{
		Action &action = trig->action(actionNum);
		if ( (ActionId)action.action != ActionId::NoAction )
		{
			action.ToggleDisabled();

			CM->notifyChange(false);
			RefreshWindow(trigIndex);
			chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);

			gridActions.SetEnabledCheck(actionNum, !action.isDisabled());
		}
	}
}

void TrigActionsWindow::CutSelection()
{
	if ( gridActions.isSelectionRectangular() )
	{
		std::string str;
		if ( gridActions.BuildSelectionString(str) )
			StringToWindowsClipboard(str);
		else
			Error("Problem building clipboard string");

		gridActions.DeleteSelection();
	}
	else
		Error("Operation only supported on rectangular selections.");
}

void TrigActionsWindow::CopySelection()
{
	if ( gridActions.isSelectionRectangular() )
	{
		std::string str;
		if ( gridActions.BuildSelectionString(str) )
			StringToWindowsClipboard(str);
		else
			Error("Problem building clipboard string");
	}
	else
		Error("Operation only supported on rectangular selections.");
}

void TrigActionsWindow::Paste()
{
	if ( gridActions.isSelectionRectangular() )
	{
		std::string pasteString;
		if ( WindowsClipboardToString(pasteString) )
		{
			int topLeftX = 0, topLeftY = 0;
			if ( gridActions.GetSelTopLeft(topLeftX, topLeftY) )
			{
				int xc = topLeftX, yc = topLeftY;

				ArgumentEnd argEndsBy;
				bool argEndsByLine = false;
				size_t argStart = 0, argEnd = 0, argEndSize = 0;
				size_t pasteStringLength = pasteString.length();
				while ( argStart < pasteStringLength )
				{
					argEnd = gridActions.FindArgEnd(pasteString, argStart, argEndsBy);

					if ( argEnd - argStart > 0 )
						GridItemChanging(xc, yc, pasteString.substr(argStart, argEnd - argStart));
					else
						GridItemChanging(xc, yc, std::string(""));

					if ( argEndsBy == ArgumentEnd::OneCharLineBreak || argEndsBy == ArgumentEnd::TwoCharLineBreak )
					{
						xc = topLeftX;
						yc = yc + 1;
					}
					else if ( argEndsBy == ArgumentEnd::Tab )
						xc = xc + 1;

					if ( argEndsBy == ArgumentEnd::TwoCharLineBreak )
						argStart = argEnd + 2;
					else
						argStart = argEnd + 1;
				}
			}
		}
		else
			Error("Failed to get clipboard data.");
	}
}

void TrigActionsWindow::RedrawThis()
{
	HDC hDC = GetDC(getHandle());
	EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, 0);
	ReleaseDC(hDC);
	ClassWindow::RedrawThis();
	gridActions.RedrawThis();
}

void TrigActionsWindow::InitializeArgMaps()
{
	#define ADD_ARRAY(anArray, vector)						\
		for ( u8 i=0; i<sizeof(anArray)/sizeof(u8); i++ )	\
			vector.push_back(anArray[i]);

	u8 centerView[] = { 0 };
	u8 comment[] = { 0 };
	u8 createUnits[] = { 2, 1, 3, 0 };
	u8 createUnitsWithProperties[] = { 2, 1, 3, 0, 4 };
	u8 displayTextMessage[] = { 1 };
	u8 giveUnitsToPlayer[] = { 3, 2, 0, 4, 1 };
	u8 killUnit[] = { 1, 0 };
	u8 killUnitsAtLocation[] = { 2, 1, 0, 3 };
	u8 leaderboardCtrlAtLoc[] = { 1, 2, 0 };
	u8 leaderboardCtrl[] = { 1, 0 };
	u8 leaderboardGreed[] = { 0 };
	u8 leaderboardKills[] = { 1, 0 };
	u8 leaderboardPoints[] = { 1, 0 };
	u8 leaderboardResources[] = { 1, 0 };
	u8 leaderboardGoalCtrlAtLoc[] = { 2, 1, 3, 0 };
	u8 leaderboardGoalCtrl[] = { 2, 1, 0 };
	u8 leaderboardGoalKills[] = { 2, 1, 0 };
	u8 leaderboardGoalPoints[] = { 2, 1, 0 };
	u8 leaderboardGoalResources[] = { 1, 2, 0 };
	u8 leaderboardCompPlayers[] = { 0 };
	u8 minimapPing[] = { 0 };
	u8 modifyUnitEnergy[] = { 3, 1, 0, 4, 2 };
	u8 modifyUnitHangerCount[] = { 2, 3, 1, 4, 0 };
	u8 modifyUnitHitPoints[] = { 3, 1, 0, 4, 2 };
	u8 modifyUnitResourceAmount[] = { 2, 0, 3, 1 };
	u8 modifyUnitShieldPoints[] = { 3, 1, 0, 4, 2 };
	u8 moveLocation[] = { 3, 1, 0, 2 };
	u8 moveUnits[] = { 2, 1, 0, 3, 4 };
	u8 order[] = { 1, 0, 2, 4, 3 };
	u8 playWav[] = { 0 };
	u8 removeUnit[] = { 1, 0 };
	u8 removeUnitsAtLocation[] = { 2, 1, 0, 3 };
	u8 runAiScript[] = { 0 };
	u8 runAiScriptAtLocation[] = { 0, 1 };
	u8 setAllianceStatus[] = { 0, 1 };
	u8 setCountdownTimer[] = { 0, 1 };
	u8 setDeaths[] = { 0, 2, 3, 1 };
	u8 setDoodadState[] = { 3, 1, 0, 2 };
	u8 setInvincibility[] = { 3, 1, 0, 2 };
	u8 setMissionObjectives[] = { 0 };
	u8 setNextScenario[] = { 0 };
	u8 setResources[] = { 0, 1, 2, 3 };
	u8 setScore[] = { 0, 1, 2, 3 };
	u8 setSwitch[] = { 1, 0 };
	u8 talkingPortrait[] = { 0, 1 };
	u8 transmission[] = { 2, 3, 6, 4, 5, 1 };
	u8 wait[] = { 0 };

	ADD_ARRAY(centerView, actionArgMaps[10]);
	ADD_ARRAY(comment, actionArgMaps[47]);
	ADD_ARRAY(createUnits, actionArgMaps[44]);
	ADD_ARRAY(createUnitsWithProperties, actionArgMaps[11]);
	ADD_ARRAY(displayTextMessage, actionArgMaps[9]);
	ADD_ARRAY(giveUnitsToPlayer, actionArgMaps[48]);
	ADD_ARRAY(killUnit, actionArgMaps[22]);
	ADD_ARRAY(killUnitsAtLocation, actionArgMaps[23]);
	ADD_ARRAY(leaderboardCtrlAtLoc, actionArgMaps[18]);
	ADD_ARRAY(leaderboardCtrl, actionArgMaps[17]);
	ADD_ARRAY(leaderboardGreed, actionArgMaps[40]);
	ADD_ARRAY(leaderboardKills, actionArgMaps[20]);
	ADD_ARRAY(leaderboardPoints, actionArgMaps[21]);
	ADD_ARRAY(leaderboardResources, actionArgMaps[19]);
	ADD_ARRAY(leaderboardGoalCtrlAtLoc, actionArgMaps[34]);
	ADD_ARRAY(leaderboardGoalCtrl, actionArgMaps[33]);
	ADD_ARRAY(leaderboardGoalKills, actionArgMaps[36]);
	ADD_ARRAY(leaderboardGoalPoints, actionArgMaps[37]);
	ADD_ARRAY(leaderboardGoalResources, actionArgMaps[35]);
	ADD_ARRAY(leaderboardCompPlayers, actionArgMaps[32]);
	ADD_ARRAY(minimapPing, actionArgMaps[28]);
	ADD_ARRAY(modifyUnitEnergy, actionArgMaps[50]);
	ADD_ARRAY(modifyUnitHangerCount, actionArgMaps[53]);
	ADD_ARRAY(modifyUnitHitPoints, actionArgMaps[49]);
	ADD_ARRAY(modifyUnitResourceAmount, actionArgMaps[52]);
	ADD_ARRAY(modifyUnitShieldPoints, actionArgMaps[51]);
	ADD_ARRAY(moveLocation, actionArgMaps[38]);
	ADD_ARRAY(moveUnits, actionArgMaps[39]);
	ADD_ARRAY(order, actionArgMaps[46]);
	ADD_ARRAY(playWav, actionArgMaps[8]);
	ADD_ARRAY(removeUnit, actionArgMaps[24]);
	ADD_ARRAY(removeUnitsAtLocation, actionArgMaps[25]);
	ADD_ARRAY(runAiScript, actionArgMaps[15]);
	ADD_ARRAY(runAiScriptAtLocation, actionArgMaps[16]);
	ADD_ARRAY(setAllianceStatus, actionArgMaps[57]);
	ADD_ARRAY(setCountdownTimer, actionArgMaps[14]);
	ADD_ARRAY(setDeaths, actionArgMaps[45]);
	ADD_ARRAY(setDoodadState, actionArgMaps[42]);
	ADD_ARRAY(setInvincibility, actionArgMaps[43]);
	ADD_ARRAY(setMissionObjectives, actionArgMaps[12]);
	ADD_ARRAY(setNextScenario, actionArgMaps[41]);
	ADD_ARRAY(setResources, actionArgMaps[26]);
	ADD_ARRAY(setScore, actionArgMaps[27]);
	ADD_ARRAY(setSwitch, actionArgMaps[13]);
	ADD_ARRAY(talkingPortrait, actionArgMaps[29]);
	ADD_ARRAY(transmission, actionArgMaps[7]);
	ADD_ARRAY(wait, actionArgMaps[4]);
}

void TrigActionsWindow::CreateSubWindows(HWND hWnd)
{
	gridActions.CreateThis(hWnd, 2, 40, 100, 100, GRID_ACTIONS);
	suggestions.CreateThis(hWnd, 0, 0, 200, 100);
	buttonEditString.CreateThis(hWnd, 0, 0, 100, 22, "Edit String", BUTTON_EDITSTRING);
	buttonEditString.Hide();
	buttonEditWav.CreateThis(hWnd, 0, 0, 100, 22, "Edit WAV", BUTTON_EDITWAV);
	buttonEditWav.Hide();
	RefreshWindow(trigIndex);
}

LRESULT TrigActionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( wParam == GRID_ACTIONS )
	{
		MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
		mis->itemWidth = DEFAULT_COLUMN_WIDTH;
		mis->itemHeight = 15;
		return TRUE;
	}
	return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigActionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
	DrawSelectedAction();
	return result;
}

void TrigActionsWindow::ChangeActionType(Action &action, ActionId newId)
{
	if ( (ActionId)action.action != newId )
	{
		action.location = 0;
		action.stringNum = 0;
		action.wavID = 0;
		action.time = 0;
		action.group = 0;
		action.number = 0;
		action.type = 0;
		action.action = (u8)newId;

		if ( newId == ActionId::SetSwitch || newId == ActionId::LeaderboardCompPlayers || newId == ActionId::SetDoodadState ||
			newId == ActionId::SetInvincibility )
		{
			action.type2 = 5;
		}
		else if ( newId == ActionId::SetCountdownTimer || newId == ActionId::SetDeaths || newId == ActionId::SetResources ||
			newId == ActionId::SetScore || newId == ActionId::Transmission )
		{
			action.type2 = 7;
		}
		else
			action.type2 = 0;

		action.flags = TextTrigCompiler::defaultActionFlags(newId);
		action.internalData[0] = 0;
		action.internalData[1] = 0;
		action.internalData[2] = 0;
	}
}

bool TrigActionsWindow::TransformAction(Action &action, ActionId newId)
{
	if ( (ActionId)action.action != newId )
	{
		ChangeActionType(action, newId);
		CM->notifyChange(false);
		RefreshWindow(trigIndex);
		chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
		return true;
	}
	return false;
}

void TrigActionsWindow::UpdateActionName(u8 actionNum, const std::string &newText)
{
	Trigger* trig;
	TextTrigCompiler ttc;
	ActionId newId = ActionId::NoAction;
	if ( ttc.ParseActionName(newText, newId) || ttc.ParseActionName(suggestions.Take(), newId) )
	{
		if ( CM->getTrigger(trig, trigIndex) )
		{
			Action &action = trig->action(actionNum);
			TransformAction(action, newId);
		}
	}
	else if ( newText.length() == 0 )
	{
		if ( CM->getTrigger(trig, trigIndex) &&
			(ActionId)trig->actions[actionNum].action != newId )
		{
			trig->deleteAction((u8)actionNum);
			CM->notifyChange(false);
			RefreshWindow(trigIndex);
			chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
		}
	}
}

void TrigActionsWindow::UpdateActionArg(u8 actionNum, u8 argNum, const std::string &newText)
{
	RawString rawUpdateText, rawSuggestText;
	std::string suggestionString = suggestions.Take();
	TextTrigCompiler ttc;
	Trigger* trig;
	if ( CM->getTrigger(trig, trigIndex) )
	{
		Action &action = trig->action(actionNum);
		if ( action.action < 64 && argNum < actionArgMaps[action.action].size() )
		{
			u8 textTrigArgNum = actionArgMaps[action.action][argNum];
			ActionArgType argType = action.TextTrigArgType(textTrigArgNum, (ActionId)action.action);
			ChkdString chkdSuggestText = ChkdString(suggestionString, true);
			ChkdString chkdNewText = ChkdString(newText, true);

			bool madeChange = false;
			if ( argType == ActionArgType::ActString || argType == ActionArgType::ActWav )
			{
				u32 newStringNum = 0;
				if ( CM->stringExists(chkdSuggestText, newStringNum) ||
					CM->stringExists(chkdNewText, newStringNum) )
				{
					if ( argType == ActionArgType::ActString )
						action.stringNum = newStringNum;
					else if ( argType == ActionArgType::ActWav )
						action.wavID = newStringNum;

					madeChange = true;
				}
			}
			else
			{
				std::vector<u8> &argMap = actionArgMaps[trig->action(actionNum).action];
				madeChange = (ParseChkdStr(chkdNewText, rawUpdateText) &&
					ttc.ParseActionArg(rawUpdateText, argNum, argMap, action, CM, chkd.scData)) ||
					(ParseChkdStr(chkdSuggestText, rawSuggestText) &&
						ttc.ParseActionArg(rawSuggestText, argNum, argMap, action, CM, chkd.scData));
			}

			if ( madeChange )
			{
				CM->notifyChange(false);
				RefreshWindow(trigIndex);
				chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
			}
		}
	}
}

BOOL TrigActionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string& str)
{
	if ( gridItemY >= 0 && gridItemY < NUM_TRIG_ACTIONS )
	{
		u8 actionNum = (u8)gridItemY;
		if ( gridItemX == 1 ) // Action Name
		{
			UpdateActionName(actionNum, str);
		}
		else if ( gridItemX > 1 ) // Action Arg
		{
			u8 argNum = (u8)gridItemX - 2;
			UpdateActionArg(actionNum, argNum, str);
		}
	}
	return FALSE;
}

BOOL TrigActionsWindow::GridItemDeleting(u16 gridItemX, u16 gridItemY)
{
	Trigger* trig;
	if ( gridItemY >= 0 && gridItemY < NUM_TRIG_ACTIONS )
	{
		u8 actionNum = (u8)gridItemY;

		if ( gridItemX == 1 && // Action Name
			CM->getTrigger(trig, trigIndex) &&
			trig->actions[actionNum].action != 0 )
		{
			ChangeActionType(trig->actions[actionNum], ActionId::NoAction);
		}
		else if ( gridItemX > 1 ) // Action Arg
		{
			// Nothing planned yet
		}
	}
	return FALSE;
}

void TrigActionsWindow::DrawSelectedAction()
{
	HDC hDC = GetDC(getHandle());
	if ( hDC != NULL )
	{
		Trigger* trig;
		if ( CM->getTrigger(trig, trigIndex) )
		{
			int focusedX = -1,
				focusedY = -1;

			if ( gridActions.GetFocusedItem(focusedX, focusedY) )
			{
				u8 actionNum = (u8)focusedY;
				TextTrigGenerator ttg;
				ttg.LoadScenario(CM);
				ChkdString str = chkd.trigEditorWindow.triggersWindow.GetActionString(actionNum, trig, ttg);
				ttg.ClearScenario();

				UINT width = 0, height = 0;
				GetStringDrawSize(hDC, width, height, str);
				HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_MENU));
				RECT rect;
				rect.left = gridActions.Left() + 5;
				rect.right = gridActions.Right() - 5;
				rect.top = gridActions.Top() - height - 1;
				rect.bottom = gridActions.Top() - 1;
				FillRect(hDC, &rect, hBackground);
				DeleteBrush(hBackground);

				SetBkMode(hDC, TRANSPARENT);
				DrawString(hDC, gridActions.Left() + 6, gridActions.Top() - height - 1, 500, RGB(0, 0, 0), str);
			}
		}
		ReleaseDC(hDC);
	}
}

int TrigActionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
	std::string text;
	if ( gridActions.item(gridItemX, gridItemY).getText(text) )
	{
		HDC hDC = GetDC(getHandle());
		UINT width = 0, height = 0;
		if ( GetStringDrawSize(hDC, width, height, text) )
			return width + 2;
		ReleaseDC(hDC);
	}
	return 0;
}

void TrigActionsWindow::PreDrawItems()
{
	HDC hDC = GetDC(getHandle());
	EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, 0);
	ReleaseDC(hDC);
	hBlack = CreateSolidBrush(RGB(0, 0, 0));
}

void TrigActionsWindow::SysColorRect(HDC hDC, RECT &rect, DWORD color)
{
	SetBkColor(hDC, GetSysColor(color));
	FillRect(hDC, &rect, GetSysColorBrush(color));
}

void TrigActionsWindow::DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart)
{
	RECT rcFill;
	rcFill.top = rcItem.top;
	rcFill.bottom = rcItem.bottom - 1;
	rcFill.left = xStart;
	rcFill.right = xStart + width - 1;

	if ( gridActions.isFocused(gridItemX, gridItemY) )
		SysColorRect(hDC, rcFill, COLOR_ACTIVEBORDER);
	else if ( gridActions.item(gridItemX, gridItemY).isSelected() )
		SysColorRect(hDC, rcFill, COLOR_HIGHLIGHT);
	else
		SysColorRect(hDC, rcFill, COLOR_WINDOW);
}

void TrigActionsWindow::DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart)
{
	RECT rcFill;
	rcFill.top = rcItem.top - 1;
	rcFill.bottom = rcItem.bottom;
	rcFill.left = xStart - 1;
	rcFill.right = xStart + width;

	::FrameRect(hDC, &rcFill, hBlack);
}

void TrigActionsWindow::DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart)
{
	if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < NUM_TRIG_ACTIONS )
		gridActions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

	int width = ListView_GetColumnWidth(gridActions.getHandle(), gridItemX);
	DrawItemBackground(hDC, gridItemX, gridItemY, rcItem, width, xStart);

	std::string text;
	if ( gridActions.item(gridItemX, gridItemY).getText(text) )
		DrawString(hDC, xStart + 1, rcItem.top, width - 2, RGB(0, 0, 0), text);

	DrawItemFrame(hDC, rcItem, width, xStart);
	xStart += width;
}

void TrigActionsWindow::DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis)
{
	if ( gridId == GRID_ACTIONS )
	{
		bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
			drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
			drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

		if ( pdis->itemID != -1 && (drawSelection || drawEntire) )
		{
			RECT &rcItem = pdis->rcItem;
			int itemStart = rcItem.left;

			int numColumns = gridActions.GetNumColumns();
			for ( int x = 0; x<numColumns; x++ )
				DrawGridViewItem(pdis->hDC, x, pdis->itemID, rcItem, itemStart);
		}
	}
}

void TrigActionsWindow::DrawTouchups(HDC hDC)
{
	RECT rect = {};
	if ( gridActions.GetEditItemRect(rect) )
	{
		rect.left -= 1;
		rect.top -= 1;
		HBRUSH hHighlight = CreateSolidBrush(RGB(0, 0, 200));
		::FrameRect(hDC, &rect, hHighlight);
		rect.left -= 1;
		rect.top -= 1;
		rect.right += 1;
		rect.bottom += 1;
		::FrameRect(hDC, &rect, hHighlight);
		DeleteBrush(hHighlight);
	}
	gridActions.RedrawHeader();
}

void TrigActionsWindow::PostDrawItems()
{
	DeleteBrush(hBlack);
	hBlack = NULL;
}

void TrigActionsWindow::SuggestNothing()
{
	suggestions.ClearStrings();
	suggestions.Hide();
}

void TrigActionsWindow::SuggestLocation()
{
	ChkLocation* loc = nullptr;
	if ( CM != nullptr )
	{
		suggestions.AddString(std::string("No Location"));
		u16 locationCapacity = (u16)CM->locationCapacity();
		for ( u16 i = 0; i < locationCapacity; i++ )
		{
			if ( CM->locationIsUsed(i) )
			{
				ChkdString locationName(true);
				if ( CM->getLocation(loc, u8(i)) && loc->stringNum > 0 && CM->getLocationName((u16)i, locationName) )
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

void TrigActionsWindow::SuggestString()
{
	ChkdString str(true);
	if ( CM != nullptr )
	{
		suggestions.AddString(std::string("No String"));
		u32 numRegularStrings = (u32)CM->strSectionCapacity();
		u32 numExtendedStrings = CM->kstrSectionCapacity();
		for ( u32 i = 1; i <= numRegularStrings; i++ )
		{
			if ( CM->GetString(str, i) && str.size() > 0 )
				suggestions.AddString(str);
		}
		for ( u32 i = 65535; i > (65535 - numExtendedStrings); i-- )
		{
			if ( CM->GetString(str, i) && str.size() > 0 )
				suggestions.AddString(str);
		}
	}
	suggestions.Show();
}

void TrigActionsWindow::SuggestPlayer()
{
	suggestions.AddStrings(triggerPlayers);
	suggestions.Show();
}

void TrigActionsWindow::SuggestUnit()
{
	if ( CM != nullptr )
	{
		for ( u16 i = 0; i < NumUnitNames; i++ )
		{
			ChkdString str(true);
			CM->getUnitName(str, i);
			suggestions.AddString(str);
			if ( str.compare(std::string(DefaultUnitDisplayName[i])) != 0 )
				suggestions.AddString(std::string(DefaultUnitDisplayName[i]));
		}
	}
	suggestions.Show();
}

void TrigActionsWindow::SuggestNumUnits()
{
	suggestions.AddString(std::string("All"));
	suggestions.Show();
}

void TrigActionsWindow::SuggestCUWP()
{
	bool hasCuwps = false;
	for ( u8 i = 0; i < 64; i++ )
	{
		if ( CM->IsCuwpUsed(i) )
		{
			hasCuwps = true;
			suggestions.AddString(std::to_string((int)i));
		}
	}

	if ( hasCuwps )
		suggestions.Show();
}

void TrigActionsWindow::SuggestTextFlags()
{

}

void TrigActionsWindow::SuggestAmount()
{

}

void TrigActionsWindow::SuggestScoreType()
{
	suggestions.AddStrings(triggerScores);
	suggestions.Show();
}

void TrigActionsWindow::SuggestResourceType()
{
	suggestions.AddString(std::string("Ore"));
	suggestions.AddString(std::string("Ore and Gas"));
	suggestions.AddString(std::string("Gas"));
	suggestions.Show();
}

void TrigActionsWindow::SuggestStateMod()
{
	const char* stateMods[] = { "Disable", "Enable", "Toggle" };
	suggestions.AddStrings(stateMods, sizeof(stateMods)/sizeof(const char*));
	suggestions.Show();
}

void TrigActionsWindow::SuggestPercent()
{
	for ( int i = 0; i <= 100; i++ )
		suggestions.AddString(std::to_string(i));

	suggestions.Show();
}

void TrigActionsWindow::SuggestOrder()
{
	const char* orders[] = { "Attack", "Move", "Patrol" };
	suggestions.AddStrings(orders, sizeof(orders)/sizeof(const char*));
	suggestions.Show();
}

void TrigActionsWindow::SuggestWav()
{
	for ( u16 i = 0; i < 512; i++ )
	{
		if ( CM->IsWavUsed(i) )
		{
			ChkdString wavStr;
			u32 stringIndex = 0;
			if ( CM->GetWav(i, stringIndex) && stringIndex > 0 && CM->GetString(wavStr, stringIndex) )
				suggestions.AddString(wavStr);
			else
				suggestions.AddString(std::to_string(stringIndex));
		}
	}
}

void TrigActionsWindow::SuggestDuration()
{

}

void TrigActionsWindow::SuggestScript()
{
	suggestions.AddString(std::string("No Script"));
	int numScripts = chkd.scData.aiScripts.GetNumAiScripts();
	for ( int i = 0; i < numScripts; i++ )
	{
		std::string scriptName;
		if ( chkd.scData.aiScripts.GetAiName(i, scriptName) )
			suggestions.AddString(scriptName);
	}
	suggestions.Show();
}

void TrigActionsWindow::SuggestAllyState()
{
	const char* allyStates[] = { "Allied Victory", "Ally", "Enemy" };
	suggestions.AddStrings(allyStates, sizeof(allyStates) / sizeof(const char*));
	suggestions.Show();
}

void TrigActionsWindow::SuggestNumericMod()
{
	const char* numericMod[] = { "Add", "Set to", "Subtract" };
	suggestions.AddStrings(numericMod, sizeof(numericMod) / sizeof(const char*));
	suggestions.Show();
}

void TrigActionsWindow::SuggestSwitch()
{
	if ( CM != nullptr )
	{
		for ( u16 i = 0; i < 256; i++ )
		{
			ChkdString str(true);
			if ( CM->getSwitchName(str, (u8)i) )
				suggestions.AddString(str);
			else
			{
				std::stringstream ss;
				ss << "Switch " << i + 1 << std::endl;
				suggestions.AddString(ss.str());
			}
		}
	}
	suggestions.Show();
}

void TrigActionsWindow::SuggestSwitchMod()
{
	const char* switchMod[] = { "Clear", "Randomize", "Set", "Toggle" };
	suggestions.AddStrings(switchMod, sizeof(switchMod) / sizeof(const char*));
	suggestions.Show();
}

void TrigActionsWindow::SuggestType()
{

}

void TrigActionsWindow::SuggestActionType()
{
	suggestions.AddStrings(triggerActions);
	suggestions.Show();
}

void TrigActionsWindow::SuggestSecondaryType()
{

}

void TrigActionsWindow::SuggestFlags()
{

}

void TrigActionsWindow::SuggestNumber() // Amount, Group2, LocDest, UnitPropNum, ScriptNum
{

}

void TrigActionsWindow::SuggestTypeIndex() // Unit, ScoreType, ResourceType, AllianceStatus
{

}

void TrigActionsWindow::SuggestSecondaryTypeIndex() // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
{

}

void TrigActionsWindow::SuggestInternalData()
{

}

void TrigActionsWindow::EnableStringEdit()
{
	stringEditEnabled = true;
	buttonEditString.Show();
}

void TrigActionsWindow::DisableStringEdit()
{
	buttonEditString.Hide();
	stringEditEnabled = false;
}

void TrigActionsWindow::ButtonEditString()
{
	ChkdString str;
	if ( ChkdStringInputDialog::GetChkdString(str, GetCurrentActionsString(), getHandle()) )
	{
		int focusedX, focusedY;
		Trigger* trig;
		if ( CM->getTrigger(trig, trigIndex) && gridActions.GetFocusedItem(focusedX, focusedY) )
		{
			Action &action = trig->action((u8)focusedY);
			u8 actionId = action.action;

			std::vector<u8> &argMap = actionArgMaps[actionId];
			u8 numArgs = (u8)actionArgMaps[actionId].size();
			for ( u8 i = 0; i < numArgs; i++ )
			{
				ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionId);
				u32 stringNum = 0;
				if ( argType == ActionArgType::ActString && CM->addString(str, stringNum, false) )
				{
					action.stringNum = stringNum;
					chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
					break;
				}
			}
		}
	}
}

void TrigActionsWindow::EnableWavEdit()
{
	wavEditEnabled = true;
	buttonEditWav.Show();
}

void TrigActionsWindow::DisableWavEdit()
{
	buttonEditWav.Hide();
	wavEditEnabled = false;
}

void TrigActionsWindow::ButtonEditWav()
{
	ChkdString wavStr;
	if ( ChkdStringInputDialog::GetChkdString(wavStr, GetCurrentActionsWav(), getHandle()) )
	{
		int focusedX = 0, focusedY = 0;
		Trigger* trig = nullptr;
		if ( CM->getTrigger(trig, trigIndex) && gridActions.GetFocusedItem(focusedX, focusedY) )
		{
			Action &action = trig->action((u8)focusedY);
			u8 actionId = action.action;

			std::vector<u8> &argMap = actionArgMaps[actionId];
			u8 numArgs = (u8)actionArgMaps[actionId].size();
			for ( u8 i = 0; i < numArgs; i++ )
			{
				ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionId);
				u32 stringNum = 0;

				if ( argType == ActionArgType::ActWav && CM->addString(wavStr, stringNum, false) )
				{
					CM->AddWav(stringNum);
					action.wavID = stringNum;
					chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
					break;
				}
			}
		}
	}
}

void TrigActionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
	Trigger* trig;
	if ( CM->getTrigger(trig, trigIndex) )
	{
		Action &action = trig->action((u8)gridItemY);
		ActionArgType argType = ActionArgType::ActNoType;
		if ( gridItemX == 1 ) // Action Name
			argType = ActionArgType::ActActionType;
		else if ( gridItemX > 1 ) // Action Arg
		{
			u8 actionArgNum = (u8)gridItemX - 2;
			if ( action.action <= 59 && actionArgMaps[action.action].size() > actionArgNum )
			{
				u8 textTrigArgNum = actionArgMaps[action.action][actionArgNum];
				argType = action.TextTrigArgType(textTrigArgNum, (ActionId)action.action);
			}
		}

		if ( argType != ActionArgType::ActNoType )
		{
			POINT pt = gridActions.GetFocusedBottomRightScreenPt();
			if ( pt.x != -1 || pt.y != -1 )
				suggestions.MoveTo(pt.x, pt.y);
		}

		suggestions.ClearStrings();
		switch ( argType )
		{
            case ActionArgType::ActNoType: SuggestNothing(); break;
			case ActionArgType::ActLocation: SuggestLocation(); break;
			case ActionArgType::ActString: SuggestString(); break;
			case ActionArgType::ActPlayer: SuggestPlayer(); break;
			case ActionArgType::ActUnit: SuggestUnit(); break;
			case ActionArgType::ActNumUnits: SuggestNumUnits(); break;
			case ActionArgType::ActCUWP: SuggestCUWP(); break;
			case ActionArgType::ActTextFlags: SuggestTextFlags(); break;
			case ActionArgType::ActAmount: SuggestAmount(); break;
			case ActionArgType::ActScoreType: SuggestScoreType(); break;
			case ActionArgType::ActResourceType: SuggestResourceType(); break;
			case ActionArgType::ActStateMod: SuggestStateMod(); break;
			case ActionArgType::ActPercent: SuggestPercent(); break;
			case ActionArgType::ActOrder: SuggestOrder(); break;
			case ActionArgType::ActWav: SuggestWav(); break;
			case ActionArgType::ActDuration: SuggestDuration(); break;
			case ActionArgType::ActScript: SuggestScript(); break;
			case ActionArgType::ActAllyState: SuggestAllyState(); break;
			case ActionArgType::ActNumericMod: SuggestNumericMod(); break;
			case ActionArgType::ActSwitch: SuggestSwitch(); break;
			case ActionArgType::ActSwitchMod: SuggestSwitchMod(); break;
			case ActionArgType::ActType: SuggestType(); break;
			case ActionArgType::ActActionType: SuggestActionType(); break;
			case ActionArgType::ActSecondaryType: SuggestSecondaryType(); break;
			case ActionArgType::ActFlags: SuggestFlags(); break;
			case ActionArgType::ActNumber: SuggestNumber(); break; // Amount, Group2, LocDest, UnitPropNum, ScriptNum
			case ActionArgType::ActTypeIndex: SuggestTypeIndex(); break; // Unit, ScoreType, ResourceType, AllianceStatus
			case ActionArgType::ActSecondaryTypeIndex: SuggestSecondaryTypeIndex(); break; // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
			case ActionArgType::ActInternalData: SuggestInternalData(); break;
		}
	}
}

void TrigActionsWindow::NewSelection(u16 gridItemX, u16 gridItemY)
{
	Trigger* trig;
	if ( CM->getTrigger(trig, trigIndex) )
	{
		Action &action = trig->action((u8)gridItemY);
		u8 actionNum = action.action;
		if ( actionNum >= 0 && actionNum < 64 )
		{
			bool includesString = false, includesWav = false;
			std::vector<u8> &argMap = actionArgMaps[actionNum];
			u8 numArgs = (u8)actionArgMaps[actionNum].size();
			for ( u8 i = 0; i < numArgs; i++ )
			{
				ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionNum);
				if ( argType == ActionArgType::ActString )
					includesString = true;
			}

			for ( u8 i = 0; i < numArgs; i++ )
			{
				ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionNum);
				if ( argType == ActionArgType::ActWav )
					includesWav = true;
			}

			if ( includesString )
				EnableStringEdit();
			else
				DisableStringEdit();

			if ( includesWav )
				EnableWavEdit();
			else
				DisableWavEdit();
		}

		if ( gridItemX == 1 ) // Action Name
		{

		}
		else if ( gridItemX > 1 ) // Action Arg
		{

		}
	}
	DoSize();
	chkd.trigEditorWindow.triggersWindow.trigModifyWindow.RedrawThis();
}

void TrigActionsWindow::NewSuggestion(std::string &str)
{
	gridActions.SetEditText(str);
}

ChkdString TrigActionsWindow::GetCurrentActionsString()
{
	int focusedX, focusedY;
	Trigger* trig;
	if ( CM->getTrigger(trig, trigIndex) && gridActions.GetFocusedItem(focusedX, focusedY) )
	{
		Action &action = trig->action((u8)focusedY);
		u8 actionNum = action.action;

		std::vector<u8> &argMap = actionArgMaps[actionNum];
		u8 numArgs = (u8)actionArgMaps[actionNum].size();
		for ( u8 i = 0; i < numArgs; i++ )
		{
			ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionNum);
			if ( argType == ActionArgType::ActString )
			{
				ChkdString dest(false);
				if ( CM->GetString(dest, action.stringNum) )
					return dest;
			}
		}
	}
	return "";
}

ChkdString TrigActionsWindow::GetCurrentActionsWav()
{
	int focusedX, focusedY;
	Trigger* trig;
	if ( CM->getTrigger(trig, trigIndex) && gridActions.GetFocusedItem(focusedX, focusedY) )
	{
		Action &action = trig->action((u8)focusedY);
		u8 actionNum = action.action;

		std::vector<u8> &argMap = actionArgMaps[actionNum];
		u8 numArgs = (u8)actionArgMaps[actionNum].size();
		for ( u8 i = 0; i < numArgs; i++ )
		{
			ActionArgType argType = action.TextTrigArgType(argMap[i], (ActionId)actionNum);
			if ( argType == ActionArgType::ActWav )
			{
				ChkdString dest(false);
				if ( CM->GetString(dest, action.wavID) )
					return dest;
			}
		}
	}
	return "";
}

LRESULT TrigActionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( (BOOL)wParam == FALSE )
		suggestions.Hide();

	return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigActionsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	switch ( nmhdr->code )
	{
		default: return ClassWindow::Notify(hWnd, idFrom, nmhdr); break;
	}
	return 0;
}

LRESULT TrigActionsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( HIWORD(wParam) )
	{
		case BN_CLICKED:
			switch ( LOWORD(wParam) )
			{
				case BUTTON_EDITSTRING: ButtonEditString(); break;
				case BUTTON_EDITWAV: ButtonEditWav(); break;
				default: return ClassWindow::Command(hWnd, wParam, lParam); break;
			}
			break;
		default: return ClassWindow::Command(hWnd, wParam, lParam); break;
	}
	return 0;
}

LRESULT TrigActionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_MEASUREITEM: return MeasureItem(hWnd, msg, wParam, lParam); break;
		case WM_ERASEBKGND: return EraseBackground(hWnd, msg, wParam, lParam); break;
		case WM_SHOWWINDOW: return ShowWindow(hWnd, msg, wParam, lParam); break;
		case WM_GRIDSELCHANGED: NewSelection(LOWORD(wParam), HIWORD(wParam)); break;
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
