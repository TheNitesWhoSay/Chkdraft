#include "Triggers.h"
#include "Chkdraft.h"

#define TRIGGER_TOP_PADDING 1
#define TRIGGER_LEFT_PADDING 1
#define TRIGGER_RIGHT_PADDING 1
#define TRIGGER_BOTTOM_PADDING 1

#define STRING_TOP_PADDING 0
#define STRING_LEFT_PADDING 2
#define STRING_RIGHT_PADDING 1
#define STRING_BOTTOM_PADDING 2

#define NO_TRIGGER (u32(-1))

#define BUTTON_NEW 41001
#define BUTTON_MODIFY 41002
#define BUTTON_DELETE 41003
#define BUTTON_COPY 41004
#define BUTTON_MOVEUP 41005
#define BUTTON_MOVEDOWN 41006
#define BUTTON_MOVETO 41007
#define LIST_GROUPS 41008
#define ID_LB_TRIGGERS 41009

TriggersWindow::TriggersWindow() : currTrigger(NO_TRIGGER), displayAll(true), numVisibleTrigs(0), changeGroupHighlightOnly(false)
{
	for ( u8 i=0; i<NUM_TRIG_PLAYERS; i++ )
		groupSelected[i] = false;
}

bool TriggersWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) == TRUE &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "Triggers", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Triggers", WS_VISIBLE|WS_CHILD,
			5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)ID_TRIGGERS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TriggersWindow::DestroyThis()
{
	return false;
}

void TriggersWindow::RefreshWindow()
{
	RefreshGroupList();
	RefreshTrigList();
	SetFocus(listTriggers.getHandle());
}

void TriggersWindow::DoSize()
{
	RECT rcCli;
	if ( GetClientRect(getHandle(), &rcCli) )
	{
		long left = rcCli.left,
			 top = rcCli.top,
			 right = rcCli.right,
			 bottom = rcCli.bottom,
			 width = right - left,
			 height = bottom - top,

			 leftPadding = 5,
			 topPadding = 5,
			 rightPadding = 5,
			 bottomPadding = 5,
			 buttonPadding = 2,
			 listPadding = 4,

			 buttonX = right-buttonNew.Width()-rightPadding,
			 leftWidth = buttonX-buttonPadding-leftPadding,
			 listGroupsBottom = topPadding+listGroups.Height(),

			 moveButtonsHeight = buttonMoveUp.Height()+buttonMoveDown.Height()+buttonMoveTo.Height()+buttonPadding*3+bottomPadding,
			 buttonsHeight = buttonNew.Height()+buttonModify.Height()+buttonDelete.Height()+buttonCopy.Height()+buttonPadding*4+
							 moveButtonsHeight,

			 triggerListHeight = height-listGroupsBottom-bottomPadding-listPadding;

		if ( buttonsHeight+listGroupsBottom > height )
		{
			listGroups.SetPos(leftPadding, topPadding, leftWidth, listGroups.Height());
			listTriggers.SetPos(leftPadding, 117, listGroups.Width(), triggerListHeight);
			buttonNew.MoveTo(buttonX, topPadding);
		}
		else
		{
			buttonNew.MoveTo(buttonX, listGroups.Bottom()+buttonPadding);
			listGroups.SetPos(leftPadding, topPadding, buttonNew.Right()-leftPadding, listGroups.Height());
			listTriggers.SetPos(leftPadding, 117, buttonNew.Left()-buttonPadding-leftPadding, triggerListHeight);
		}

		buttonModify.MoveTo(buttonX, buttonNew.Bottom()+buttonPadding );
		buttonDelete.MoveTo(buttonX, buttonModify.Bottom()+buttonPadding );
		buttonCopy.MoveTo(buttonX, buttonDelete.Bottom()+buttonPadding );
		
		long remainingButtonSpace = height-buttonCopy.Bottom();

		if ( moveButtonsHeight > remainingButtonSpace )
		{
			buttonMoveUp.MoveTo(buttonX, buttonCopy.Bottom()+buttonPadding);
			buttonMoveDown.MoveTo(buttonX, buttonMoveUp.Bottom()+buttonPadding);
			buttonMoveTo.MoveTo(buttonX, buttonMoveDown.Bottom()+buttonPadding);
		}
		else
		{
			buttonMoveTo.MoveTo(buttonX, bottom-bottomPadding-buttonMoveTo.Height());
			buttonMoveDown.MoveTo(buttonX, buttonMoveTo.Top()-buttonPadding-buttonMoveDown.Height());
			buttonMoveUp.MoveTo(buttonX, buttonMoveDown.Top()-buttonPadding-buttonMoveUp.Height());
		}
	}
}

void TriggersWindow::CreateSubWindows(HWND hWnd)
{
	buttonNew.CreateThis(hWnd, 0, 0, 75, 23, "New", BUTTON_NEW);
	buttonModify.CreateThis(hWnd, 0, 25, 75, 23, "Modify", BUTTON_MODIFY);
	buttonDelete.CreateThis(hWnd, 0, 50, 75, 23, "Delete", BUTTON_DELETE);
	buttonCopy.CreateThis(hWnd, 0, 75, 75, 23, "Copy", BUTTON_COPY);
	
	buttonMoveUp.CreateThis(hWnd, 0, 100, 75, 23, "Move Up", BUTTON_MOVEUP);
	buttonMoveDown.CreateThis(hWnd, 0, 125, 75, 23, "Move Down", BUTTON_MOVEDOWN);
	buttonMoveTo.CreateThis(hWnd, 0, 150, 75, 23, "Move To", BUTTON_MOVETO);

	listGroups.CreateThis(hWnd, 0, 0, 200, 116, true, true, LIST_GROUPS);
	listTriggers.CreateThis(hWnd, 0, 120, 200, 150, true, false, ID_LB_TRIGGERS);

	RefreshWindow();

	buttonNew.DisableThis();
	buttonModify.DisableThis();
	buttonDelete.DisableThis();
	buttonCopy.DisableThis();
	buttonMoveUp.DisableThis();
	buttonMoveDown.DisableThis();
	buttonMoveTo.DisableThis();
}

void TriggersWindow::RefreshGroupList()
{
	HWND hGroupList = GetDlgItem(getHandle(), LIST_GROUPS);
	Scenario* chk = chkd.maps.curr;
	Trigger* trigger;
	u8 firstNotFound = 0;
	bool addedPlayer[NUM_TRIG_PLAYERS];
	for ( u8 i=0; i<NUM_TRIG_PLAYERS; i++ )
		addedPlayer[i] = false;

	if ( hGroupList != NULL && chk != nullptr && chk->TRIG().exists() )
	{
		SendMessage(hGroupList, WM_SETREDRAW, FALSE, NULL);
		SendMessage(hGroupList, LB_RESETCONTENT, NULL, NULL);
		u32 numTriggers = chk->numTriggers();
		for ( u32 i=0; i<numTriggers; i++ )
		{
			if ( chk->getTrigger(trigger, i) )
			{
				for ( u8 player=firstNotFound; player<NUM_TRIG_PLAYERS; player++ )
				{
					if ( !addedPlayer[player] && trigger->players[player] != 0 )
					{
						addedPlayer[player] = true;
						if ( player == firstNotFound )
							firstNotFound ++;
					}
				}
			}
		}
		int selectAllIndex = -1;
		for ( u8 i=0; i<12; i++ ) // Players
		{
			if ( addedPlayer[i] )
				SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)i);
		}
		for ( u8 i=18; i<22; i++ ) // Forces
		{
			if ( addedPlayer[i] )
				SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)i);
		}
		if ( addedPlayer[17] ) // All Players
			SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)17);
		if ( numTriggers > 0 ) // Show All
		{
			LRESULT result = SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)(NUM_TRIG_PLAYERS));
			if ( result != LB_ERR && result != LB_ERRSPACE )
				selectAllIndex = int(result);
		}
		for ( u8 i=12; i<17; i++ ) // Lower unused groups
		{
			if ( addedPlayer[i] )
				SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)i);
		}
		for ( u8 i=22; i<NUM_TRIG_PLAYERS; i++ ) // Upper unused groups
		{
			if ( addedPlayer[i] )
				SendMessage(hGroupList, LB_ADDSTRING, NULL, (LPARAM)i);
		}
		changeGroupHighlightOnly = true;
		if ( displayAll )
			SendMessage(hGroupList, LB_SETSEL, TRUE, selectAllIndex);
		for ( u8 i=0; i<28; i++ )
		{
			if ( groupSelected[i] )
				SendMessage(hGroupList, LB_SETSEL, TRUE, i);
		}
		changeGroupHighlightOnly = false;
		SendMessage(hGroupList, WM_SETREDRAW, TRUE, NULL);
	}
}

void TriggersWindow::RefreshTrigList()
{
	HWND hTriggerList = GetDlgItem(getHandle(), ID_LB_TRIGGERS);
	Scenario* chk = chkd.maps.curr;
	Trigger* trigger;
	if ( hTriggerList != NULL && chk != nullptr && chk->TRIG().exists() )
	{
		numVisibleTrigs = 0;
		SendMessage(hTriggerList, WM_SETREDRAW, FALSE, NULL);
		SendMessage(hTriggerList, LB_RESETCONTENT, NULL, NULL);
		for ( u32 i=0; i<chk->numTriggers(); i++ )
		{
			if ( chk->getTrigger(trigger, i) && ShowTrigger(trigger) )
			{
				SendMessage(hTriggerList, LB_ADDSTRING, NULL, (LPARAM)i);
				numVisibleTrigs ++;
			}
		}
		SendMessage(hTriggerList, WM_SETREDRAW, TRUE, NULL);
	}
}

bool TriggersWindow::ShowTrigger(Trigger* trigger)
{
	if ( displayAll )
		return true;

	for ( u8 i=0; i<NUM_TRIG_PLAYERS; i++ )
	{
		if ( groupSelected[i] && trigger->players[i] != 0 )
			return true;
	}

	return false;
}

void TriggersWindow::ClearGroups()
{
	for ( u8 i=0; i<28; i++ )
		groupSelected[i] = false;
}

string TriggersWindow::GetConditionString(u8 conditionNum, Trigger* trigger)
{
	stringstream ssCondition;
	Condition &condition = trigger->conditions[conditionNum];
	u8 conditionIndex = condition.condition;
	switch ( conditionIndex )
	{
		case CID_ACCUMULATE: // Players, Comparison, Amount, TypeIndex
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C accumulates \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
				<< "\x0C \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
			break;
		case CID_ALWAYS:
			ssCondition << "Always";
			break;
		case CID_BRING: // Players, Comparison, Amount, UnitID, Location
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C brings \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C to \'\x08"
				<< tt.GetTrigLocation(condition.locationNum) << "\x0C\'.";
			break;
		case CID_COMMAND: // Players, Comparison, Amount, UnitID
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C commands \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_COMMAND_THE_LEAST: // UnitID
			ssCondition << "Current player commands the least \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_COMMAND_THE_LEAST_AT: // UnitID, Location
			ssCondition << "Current player commands the least \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C at \x08"
				<< tt.GetTrigLocation(condition.locationNum) << "\x0C.";
			break;
		case CID_COMMAND_THE_MOST: // UnitID
			ssCondition << "Current player commands the most \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_COMMAND_THE_MOST_AT: // UnitID, Location
			ssCondition << "Current player commands the most \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C at \x08"
				<< tt.GetTrigLocation(condition.locationNum) << "\x0C.";
			break;
		case CID_COUNTDOWN_TIMER: // Comparison, Amount
			ssCondition << "Countdown timer is \x08" << tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C game seconds.";
			break;
		case CID_DEATHS: // Players, Comparison, Amount, UnitID
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C has suffered \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
				<< "\x0C deaths of \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_ELAPSED_TIME: // Comparison, Amount
			ssCondition << "Elapsed scenario time is \x08" << tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C game seconds.";
			break;
		case CID_HIGHEST_SCORE: // TypeIndex
			ssCondition << "Current player has highest score \x08" << tt.GetTrigScoreType(condition.typeIndex) << "\x0C.";
			break;
		case CID_KILL: // Players, Comparison, Amount, UnitID
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C kills \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_LEAST_KILLS: // UnitID
			ssCondition << "Current player has least kills of \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_LEAST_RESOURCES: // TypeIndex
			ssCondition << "Current player has least \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
			break;
		case CID_LOWEST_SCORE: // TypeIndex
			ssCondition << "Current player has lowest score \x08" << tt.GetTrigScoreType(condition.typeIndex) << "\x0C.";
			break;
		case CID_MOST_KILLS: // UnitID
			ssCondition << "Current player has the most kills of \x08" << tt.GetTrigUnit(condition.unitID) << "\x0C.";
			break;
		case CID_MOST_RESOURCES: // TypeIndex
			ssCondition << "Current player has most \x08" << tt.GetTrigResourceType(condition.typeIndex) << "\x0C.";
			break;
		case CID_NEVER:
			ssCondition << "Never";
			break;
		case CID_NEVER_ALT:
			ssCondition << "Never (alt)";
			break;
		case CID_NO_CONDITION:
			ssCondition << "(No condition)";
			break;
		case CID_OPPONENTS: // Players, Comparison, Amount
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C has \x08"
				<< tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08" << tt.GetTrigNumber(condition.amount)
				<< "\x0C opponents remaining in the game.";
			break;
		case CID_SCORE: // Players, TypeIndex, Comparison, Amount
			ssCondition << '\x08' << tt.GetTrigPlayer(condition.players) << "\x0C \x08" << tt.GetTrigScoreType(condition.typeIndex)
				<< "\x0C score is \x08" << tt.GetTrigNumericComparison(condition.comparison) << "\x0C \x08"
				<< tt.GetTrigNumber(condition.amount) << "\x0C.";
			break;
		case CID_SWITCH: // TypeIndex, Comparison
			ssCondition << "\'\x08" << tt.GetTrigSwitch(condition.typeIndex) << "\x0C\' is \x08"
				<< tt.GetTrigSwitchState(condition.comparison) << "\x0C.";
			break;
		default: // Location, Player, Amount, Unit, NumericComparison, Condition, TypeIndex, Flags, Internal
			ssCondition << "Condition: \x08" << condition.locationNum << "\x0C, \x08" << condition.players << "\x0C, \x08"
				<< condition.amount << "\x0C, \x08" << condition.unitID << "\x0C, \x08" << u16(condition.comparison)
				<< "\x0C, \x08" << u16(condition.condition) << "\x0C, \x08" << u16(condition.typeIndex) << "\x0C, \x08"
				<< u16(condition.flags) << "\x0C, \x08" << condition.internalData << '\x0C';
			break;
	}
	return ssCondition.str();
}

string TriggersWindow::GetActionString(u8 actionNum, Trigger* trigger)
{
	stringstream ssAction;
	Action &action = trigger->actions[actionNum];
	u8 actionIndex = action.action;
	switch ( actionIndex )
	{
		case AID_CENTER_VIEW: // Location
			ssAction << "Center view for current player at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_COMMENT: // String
			ssAction << "Comment:\x08" << tt.GetTrigString(action.stringNum) << "\x0C";
			break;
		case AID_CREATE_UNIT: // Type2, Type, Location, Players
			ssAction << "Create \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\' for \x08"
				<< tt.GetTrigPlayer(action.group) << "\x0C.";
			break;
		case AID_CREATE_UNIT_WITH_PROPERTIES: // Type2, Type, Location, Players, Number
			ssAction << "Create \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\' for \x08"
				<< tt.GetTrigPlayer(action.group) << "\x0C. Apply \x08properties\x0C.";
			break;
		case AID_DEFEAT:
			ssAction << "End scenario in defeat for current player.";
			break;
		case AID_DISPLAY_TEXT_MESSAGE: // String
			ssAction << "Display for current player:\x08" << tt.GetTrigString(action.stringNum) << '\x0C';
			break;
		case AID_DRAW:
			ssAction << "End the scenario in a draw for all players.";
			break;
		case AID_GIVE_UNITS_TO_PLAYER: // Type2, Type, Players, Location, Number
			ssAction << "Give \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\' to \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
			break;
		case AID_KILL_UNIT: // Type, Players
			ssAction << "Kill all \x08" << tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
			break;
		case AID_KILL_UNIT_AT_LOCATION: // Type2, Type, Players, Location
			ssAction << "Kill \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\'.";
			break;
		case AID_LEADERBOARD_CONTROL_AT_LOCATION: // Type, Location, String
			ssAction << "Show Leader Board for most control of \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\'. Display label: \'\x08" << tt.GetTrigString(action.stringNum)
				<< "\x0C\'";
			break;
		case AID_LEADERBOARD_CONTROL: // Type, String
			ssAction << "Show Leader Board for most control of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
				<< tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_GREED: // Number
			ssAction << "Show Greed Leader Board for player closest to accumulation of \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C ore and gas.";
			break;
		case AID_LEADERBOARD_KILLS: // Type, String
			ssAction << "Show Leader Board for most kills of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
				<< tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_POINTS: // Type, String
			ssAction << "Show Leader Board for most \x08" << tt.GetTrigScoreType(action.type) << "\x0C. Display label: \'\x08"
				<< tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_RESOURCES: // Type, String
			ssAction << "Show Leader board for accumulation of most \x08" << tt.GetTrigResourceType(action.type)
				<< "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_COMPUTER_PLAYERS: // Type2
			ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C use of computer players in leaderboard calculations.";
			break;
		case AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION: // Number, Type, Location, String
			ssAction << "Show Leader Board for player closest to control of \x08" << tt.GetTrigNumber(action.number) << "\x0C of \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\'. Display label: \'\x08" << tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_GOAL_CONTROL: // Number, Type, String
			ssAction << "Show Leader Board for player closest to control of \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C of \x08" << tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08"
				<< tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_GOAL_KILLS: // Number, Type, String
			ssAction << "Show Leader Board for player closest to \x08" << tt.GetTrigNumber(action.number) << "\x0C kills of \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_GOAL_POINTS: // Number, Type, String
			ssAction << "Show Leader Board for player closest to \x08" << tt.GetTrigNumber(action.number) << "\x0C \x08"
				<< tt.GetTrigScoreType(action.type) << "\x0C. Display label: \'\x08" << tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_LEADERBOARD_GOAL_RESOURCES: // Number, Type, String
			ssAction << "Show Leader Board for player closest to accumulation of \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C \x08" << tt.GetTrigResourceType(action.type) << "\x0C. Display label: \'\x08"
				<< tt.GetTrigString(action.stringNum) << "\x0C\'";
			break;
		case AID_MINIMAP_PING: // Location
			ssAction << "Show minimap ping for current player at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\'";
			break;
		case AID_MODIFY_UNIT_ENERGY: // Type2, Type, Players, Location, Number
			ssAction << "Set energy points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\' to \x08" << tt.GetTrigNumber(action.number) << "\x0C%.";
			break;
		case AID_MODIFY_UNIT_HANGER_COUNT: // Number, Type2, Type, Location, Players
			ssAction << "Add at most \x08" << tt.GetTrigNumber(action.number) << "\x0C to hanger for \x08"
				<< tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\' owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
			break;
		case AID_MODIFY_UNIT_HITPOINTS: // Type2, Type, Players, Location, Number
			ssAction << "Set hit points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group)
				<< "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\' to \x08"
				<< tt.GetTrigNumber(action.number) << "\x0C%";
			break;
		case AID_MODIFY_UNIT_RESOURCE_AMOUNT: // Type2, Players, Location, Number
			ssAction << "Set resource amount for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C resource sources owned by \x08"
				<< tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\' to \x08"
				<< tt.GetTrigNumber(action.number) << "\x0C%.";
			break;
		case AID_MODIFY_UNIT_SHIELD_POINTS: // Type2, Type, Players, Location, Number
			ssAction << "Set shield points for \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\' to \x08" << tt.GetTrigNumber(action.number) << "\x0C%.";
			break;
		case AID_MOVE_LOCATION: // Number, Type, Players, Location
			ssAction << "Center location labeled \'\x08" << tt.GetTrigLocation(action.number) << "\x0C\' on \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_MOVE_UNIT: // Type2, Type, Players, Location, Number
			ssAction << "Move \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\' to \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_MUTE_UNIT_SPEECH:
			ssAction << "Mute all non-trigger unit sounds for current player.";
			break;
		case AID_NO_ACTION:
			ssAction << "(No action)";
			break;
		case AID_ORDER: // Type, Players, Location, Type2, Number
			ssAction << "Issue order to all \x08" << tt.GetTrigUnit(action.type) << "\x0C owned by \x08"
				<< tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\':\x08"
				<< tt.GetTrigOrder(action.type2) << "\x0C to \'\x08" << tt.GetTrigLocation(action.number) << "\x0C\'.";
			break;
		case AID_PAUSE_GAME:
			ssAction << "Pause the game.";
			break;
		case AID_PAUSE_TIMER:
			ssAction << "Pause the countdown timer.";
			break;
		case AID_PLAY_WAV: // Wav, Duration
			ssAction << "Play \'\x08" << tt.GetTrigString(action.wavID) << "\x0C\'.";
			break;
		case AID_PRESERVE_TRIGGER:
			ssAction << "Preserve trigger.";
			break;
		case AID_REMOVE_UNIT: // Type, Players
			ssAction << "Remove all \x08" << tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C.";
			break;
		case AID_REMOVE_UNIT_AT_LOCATION: // Type2, Type, Players, Location
			ssAction << "Remove \x08" << tt.GetTrigNumUnits(action.type2) << "\x0C \x08" << tt.GetTrigUnit(action.type)
				<< "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08" << tt.GetTrigLocation(action.location)
				<< "\x0C\'.";
			break;
		case AID_RUN_AI_SCRIPT: // Number
			ssAction << "Execute AI script \'\x08" << tt.GetTrigScript(action.number) << "\x0C\'.";
			break;
		case AID_RUN_AI_SCRIPT_AT_LOCATION: // Number, Location
			ssAction << "Execute AI script \'\x08" << tt.GetTrigScript(action.number) << "\x0C\' at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_SET_ALLIANCE_STATUS: // Players, Type
			ssAction << "Set \x08" << tt.GetTrigPlayer(action.group) << "\x0C to \x08" << tt.GetTrigAllyState(action.type) << "\x0C.";
			break;
		case AID_SET_COUNTDOWN_TIMER: // Type2, Duration
			ssAction << "Modify Countdown Timer: \x08" << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08"
				<< tt.GetTrigNumber(action.time) << "\x0C seconds.";
			break;
		case AID_SET_DEATHS: // Players, Type2, Number, Type
			ssAction << "Modify death counts for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
				<< tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C for \x08" << tt.GetTrigUnit(action.type) << "\x0C.";
			break;
		case AID_SET_DOODAD_STATE: // Type2, Type, Players, Location
			ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C doodad state for \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C for \x08" << tt.GetTrigPlayer(action.group) << "\x0C at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_SET_INVINCIBILITY: // Type2, Type, Players, Location
			ssAction << '\x08' << tt.GetTrigStateModifier(action.type2) << "\x0C invincibility for \x08"
				<< tt.GetTrigUnit(action.type) << "\x0C owned by \x08" << tt.GetTrigPlayer(action.group)
				<< "\x0C at \'\x08" << tt.GetTrigLocation(action.location) << "\x0C\'.";
			break;
		case AID_SET_MISSION_OBJECTIVES: // String
			ssAction << "Set Mission Objectives to:\x08" << tt.GetTrigString(action.stringNum) << '\x0C';
			break;
		case AID_SET_NEXT_SCENARIO: // String
			ssAction << "Load \'\x08" << tt.GetTrigString(action.stringNum) << "\x0C\' after completion of current game.";
			break;
		case AID_SET_RESOURCES: // Players, Type2, Number, Type
			ssAction << "Modify resources for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
				<< tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C \x08" << tt.GetTrigResourceType(action.type) << "\x0C.";
			break;
		case AID_SET_SCORE: // Players, Type2, Number, Type
			ssAction << "Modify score for \x08" << tt.GetTrigPlayer(action.group) << "\x0C: \x08"
				<< tt.GetTrigNumericModifier(action.type2) << "\x0C \x08" << tt.GetTrigNumber(action.number)
				<< "\x0C \x08" << tt.GetTrigScoreType(action.type) << "\x0C.";
			break;
		case AID_SET_SWITCH: // Type2, Number
			ssAction << '\x08' << tt.GetTrigSwitchModifier(action.type2) << "\x0C \'\x08" << tt.GetTrigSwitch(action.number) << "\x0C\'.";
			break;
		case AID_TALKING_PORTRAIT: // Type, Duration
			ssAction << "Show \x08" << tt.GetTrigUnit(action.type) << "\x0C talking to current player for \x08"
				<< tt.GetTrigNumber(action.time) << "\x0C miliseconds.";
			break;
		case AID_TRANSMISSION: // Type, Location, Wav, Type2, Number, String
			ssAction << "Send transmission to current player from \x08" << tt.GetTrigUnit(action.type) << "\x0C at \'\x08"
				<< tt.GetTrigLocation(action.location) << "\x0C\'. Play '\x08" << tt.GetTrigString(action.wavID)
				<< "\x0C\'. Modify duration: \x08" << tt.GetTrigNumericModifier(action.type2) << "\x0C \x08"
				<< tt.GetTrigNumber(action.number) << "\x0C miliseconds. Display text:\x08"
				<< tt.GetTrigString(action.stringNum) << '\x0C';
			break;
		case AID_UNMUTE_UNIT_SPEECH:
			ssAction << "Unmute all non-trigger unit sounds for current player.";
			break;
		case AID_UNPAUSE_GAME:
			ssAction << "Unpause the game.";
			break;
		case AID_UNPAUSE_TIMER:
			ssAction << "Unpause the countdown timer.";
			break;
		case AID_VICTORY:
			ssAction << "End scenario in victory for current player.";
			break;
		case AID_WAIT: // Duration
			ssAction << "Wait for \x08" << tt.GetTrigNumber(action.time) << "\x0C miliseconds.";
			break;//*/
		default: // Location, String, Wav, Duration, Player, Number, Type, Action, Type2, Flags, Internal
			ssAction << "Action: \x08" << action.location << "\x0C, \x08" << action.stringNum << "\x0C, \x08" << action.wavID
				<< "\x0C, \x08" << action.time << "\x0C, \x08" << action.group << "\x0C, \x08" << action.number
				<< "\x0C, \x08" << action.type << "\x0C, \x08" << u16(action.type2) << "\x0C, \x08" << u16(action.flags)
				<< "\x0C, \x08" << TripletToInt(action.internalData) << "\x0C.";
			break;
	}
	return ssAction.str();
}

string TriggersWindow::GetTriggerString(u32 trigNum, Trigger* trigger)
{
	bool more = false;
	stringstream ssTrigger;
	u8 remainingLines = 13;
	u8 numConditions = trigger->numUsedConditions();
	u8 numActions = trigger->numUsedActions();
	if ( numConditions > 0 )
	{
		ssTrigger << "CONDITIONS:\r\n";
		u8 i=0;
		while ( numConditions > 0 && i < 16 )
		{
			if ( trigger->conditions[i].condition != CID_NO_CONDITION )
			{
				ssTrigger << ' ' << GetConditionString(i, trigger) << "\r\n";
				remainingLines --;
				if ( remainingLines == 0 )
				{
					if ( numConditions > 1 )
						more = true;
					break;
				}
				numConditions --;
			}
			i++;
		}
	}
	if ( numActions > 0 && remainingLines > 0 )
	{
		ssTrigger << "ACTIONS:\r\n" << endl;
		u8 i=0;
		while ( numActions > 0 && i < 64 )
		{
			if ( trigger->actions[i].action != AID_NO_ACTION )
			{
				ssTrigger << ' ' << GetActionString(i, trigger) << "\r\n";
				remainingLines --;
				if ( remainingLines == 0 )
				{
					if ( numActions > 1 )
						more = true;
					break;
				}
				numActions --;
			}
			i++;
		}
	}
	if ( more )
		ssTrigger << "\x13(MORE)" << endl;
	return ssTrigger.str();
}

bool TriggersWindow::GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, Scenario* chk, u32 triggerNum, Trigger* trigger)
{
	string str;
	if ( chk->getActiveComment(trigger, str) )
	{
		if ( GetStringDrawSize(hDC, width, height, str) )
		{
			width += TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
			height += TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;
		}
	}
	else
	{
		str = GetTriggerString(triggerNum, trigger);
		HGDIOBJ sel = SelectObject(hDC, defaultFont);
		if ( sel != NULL && sel != HGDI_ERROR )
		{
			width = 0;
			height = TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;

			UINT strWidth, strHeight;
			if ( GetStringDrawSize(hDC, strWidth, strHeight, str) )
			{
				UINT newWidth = strWidth+TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
				if ( newWidth > width )
					width = newWidth;

				height += strHeight;
				return true;
			}
		}
	}
	return false;
}

void TriggersWindow::DrawGroup(HDC hDC, RECT &rcItem, bool isSelected, u8 groupNum)
{
	HBRUSH hBackground = NULL;
	if ( isSelected )
		hBackground = CreateSolidBrush(RGB(51, 153, 255));
	else
		hBackground = CreateSolidBrush(RGB(255, 255, 255));

	if ( hBackground != NULL )
	{
		FillRect(hDC, &rcItem, hBackground);
		DeleteObject(hBackground);
	}

	SetBkMode(hDC, TRANSPARENT);
	if ( groupNum < 29 )
	{
		string groupNames[] = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6",
							    "Player 7", "Player 8", "Player 9 (unused)", "Player 10 (unused)",
								"Player 11 (unused)", "Player 12 (unused)",
								"unknown/unused", "Current Player (unused)", "Foes (unused)", "Allies (unused)", "Neutral Players (unused)",
								"All players", "Force 1", "Force 2", "Force 3", "Force 4",
								"unknown/unused", "unknown/unused", "unknown/unused", "unknown/unused",
								"Non AV Players (unused)", "unknown/unused", "Show All" };

		DrawString(hDC, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
			rcItem.right-rcItem.left, RGB(0, 0, 0), groupNames[groupNum]);
	}
	else
	{
		stringstream ssGroup;
		ssGroup << "Group: " << u16(groupNum);
		DrawString(hDC, rcItem.left+STRING_LEFT_PADDING, rcItem.top+STRING_TOP_PADDING,
			rcItem.right-rcItem.left, RGB(0, 0, 0), ssGroup.str());
	}
}

void TriggersWindow::DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, Scenario* chk, u32 triggerNum, Trigger* trigger)
{
	HBRUSH hBackground = NULL;
	if ( isSelected )
		hBackground = CreateSolidBrush(RGB(51, 153, 255));
	else
		hBackground = CreateSolidBrush(RGB(253, 246, 208));

	if ( hBackground != NULL )
	{
		RECT rcNote;
		rcNote.left = rcItem.left+TRIGGER_LEFT_PADDING;
		rcNote.top = rcItem.top+TRIGGER_TOP_PADDING;
		rcNote.right = rcItem.right-TRIGGER_RIGHT_PADDING;
		rcNote.bottom = rcItem.bottom-TRIGGER_BOTTOM_PADDING;

		FillRect(hDC, &rcNote, hBackground);
		if ( isSelected )
			DrawFocusRect(hDC, &rcNote);

		DeleteObject(hBackground);
	}

	string str;
	if ( chk->getActiveComment(trigger, str) )
	{
		SetBkMode(hDC, TRANSPARENT);
		DrawString(hDC, rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
			rcItem.right-rcItem.left, RGB(0, 0, 0), str);
	}
	else
	{
		str = GetTriggerString(triggerNum, trigger);
		SetBkMode(hDC, TRANSPARENT);
		DrawString(hDC, rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING,
			rcItem.right-rcItem.left, RGB(0, 0, 0), str);
	}
}

LRESULT TriggersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				RefreshWindow();
			else if ( wParam == FALSE )
			{
				// Perhaps update details of the current trigger
			}
			break;

		case WM_MOUSEWHEEL:
			{
				int distanceScrolled = int((s16(HIWORD(wParam)))/WHEEL_DELTA);
				HWND hStringSel = GetDlgItem(hWnd, ID_LB_STRINGS);
				if ( hStringSel != NULL )
					ListBox_SetTopIndex(hStringSel, ListBox_GetTopIndex(hStringSel)-distanceScrolled);
			}
			break;

		case WM_CTLCOLORLISTBOX:
			if ( listTriggers.getHandle() == (HWND)lParam )
			{
				HBRUSH hBrush = CreateSolidBrush(RGB(171, 171, 171));
				if ( hBrush != NULL )
					return (LPARAM)hBrush;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case LBN_SELCHANGE:
					if ( LOWORD(wParam) == ID_LB_TRIGGERS ) // Change selection, update info boxes and so fourth
					{
						
					}
					else if ( LOWORD(wParam) == LIST_GROUPS )
					{
						if ( changeGroupHighlightOnly )
							return DefWindowProc(hWnd, msg, wParam, lParam);

						int numSel = listGroups.GetNumSel();
						displayAll = false;
						ClearGroups();
						for ( int i=0; i<numSel; i++ )
						{
							int selItem;
							if ( listGroups.GetSelItem(i, selItem) )
							{
								if ( selItem == 28 )
								{
									ClearGroups();
									displayAll = true;
								}
								else
									groupSelected[selItem] = true;
							}
						}
						RefreshTrigList();
					}
				case LBN_KILLFOCUS: // List box item may have lost focus, check if anything should be updated
					if ( LOWORD(wParam) == ID_LB_TRIGGERS )
					{
					
					}
					else if ( LOWORD(wParam) == LIST_GROUPS )
					{

					}
				case BN_CLICKED:
					if ( LOWORD(wParam) == BUTTON_NEW )
						mb("new");
					else if ( LOWORD(wParam) == BUTTON_MODIFY )
						mb("modify");
					else if ( LOWORD(wParam) == BUTTON_DELETE )
						mb("delete");
					else if ( LOWORD(wParam) == BUTTON_COPY )
						mb("copy");
					else if ( LOWORD(wParam) == BUTTON_MOVEUP )
						mb("move up");
					else if ( LOWORD(wParam) == BUTTON_MOVEDOWN )
						mb("move down");
					else if ( LOWORD(wParam) == BUTTON_MOVETO )
						mb("move to");
					break;
			}
			break;

		case WM_MEASUREITEM:
			if ( wParam == ID_LB_TRIGGERS )
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				HWND hTriggerList = GetDlgItem(hWnd, ID_LB_TRIGGERS);
				Trigger* trigger;
				u32 listIndex = (u32)mis->itemData;
				u32 triggerNum = listIndex; // Trigger number needs be determined by listIndex and selected players

				if ( hTriggerList != NULL && chkd.maps.curr->getTrigger(trigger, triggerNum) )
				{
					HDC hDC = GetDC(hTriggerList);
					if ( hDC != NULL )
					{
						if ( triggerNum == 0 ) // Measuring is time sensative, load scenario only on first trigger
							tt.LoadScenario(chkd.maps.curr);

						if ( GetTriggerDrawSize(hDC, mis->itemWidth, mis->itemHeight, chkd.maps.curr, triggerNum, trigger) )
						{
							ReleaseDC(hTriggerList, hDC);
							return TRUE;
						}

						ReleaseDC(hTriggerList, hDC);
						if ( triggerNum == numVisibleTrigs-1 ) // Release scenario after last trigger
							tt.ClearScenario();
					}
				}
				return TRUE;
			}
			else if ( wParam == LIST_GROUPS )
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				u32 listIndex = (u32)mis->itemData;
				mis->itemWidth = 120;
				mis->itemHeight = 13;
			}
			break;

		case WM_DRAWITEM:
			if ( wParam == ID_LB_TRIGGERS )
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;

				if ( pdis->itemID != -1 && (pdis->itemAction == ODA_SELECT || pdis->itemAction == ODA_DRAWENTIRE) )
				{
					Trigger* trigger;
					u32 listIndex = (u32)pdis->itemData;
					u32 triggerNum = listIndex; // Trigger number needs be determined by listIndex and selected players
					
					if ( chkd.maps.curr != nullptr && chkd.maps.curr->getTrigger(trigger, triggerNum) )
					{
						tt.LoadScenario(chkd.maps.curr); // Drawing not very time sensative, load scenario for each trigger
						DrawTrigger(pdis->hDC, pdis->rcItem, (pdis->itemState&ODS_SELECTED), chkd.maps.curr, triggerNum, trigger);
						tt.ClearScenario(); // Clear scenario after each trigger
					}
				}
				return TRUE;
			}
			else if ( wParam == LIST_GROUPS )
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
				u32 listIndex = (u32)pdis->itemData;
				DrawGroup(pdis->hDC, pdis->rcItem, (pdis->itemState&ODS_SELECTED), u8(listIndex));
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
