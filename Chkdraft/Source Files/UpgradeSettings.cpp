#include "UpgradeSettings.h"
#include "GuiAccel.h"

bool UpgradeSettingsWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "UpgradeSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "UpgradeSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_UPGRADESETTINGS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

LRESULT UpgradeSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void UpgradeSettingsWindow::CreateSubWindows(HWND hWnd)
{
	checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 120, 20, false, "Use Default Costs", ID_CHECK_USEDEFAULTCOSTS);
	buttonResetUpgradeDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Upgrades To Default", ID_BUTTON_RESETUPGRADEDEFAULTS);

	groupMineralCosts.CreateThis(hWnd, 210, 25, 377, 45, "Mineral Costs", ID_GROUP_MINERALCOSTS);
	textMineralBaseCosts.CreateThis(hWnd, 215, 45, 75, 20, "Base Cost", ID_TEXT_MINERALBASECOST);
	editMineralBaseCosts.CreateThis(hWnd, 304, 45, 84, 20, false, ID_EDIT_MINERALBASECOST);
	textMineralUpgradeFactor.CreateThis(hWnd, 408, 45, 75, 20, "Upgrade Factor", ID_TEXT_MINERALUPGRADEFACTOR);
	editMineralUpgradeFactor.CreateThis(hWnd, 497, 45, 84, 20, false, ID_EDIT_MINERALUPGRADEFACTOR);

	groupGasCosts.CreateThis(hWnd, 210, 75, 377, 45, "Gas Costs", ID_GROUP_GASCOSTS);
	textGasBaseCosts.CreateThis(hWnd, 215, 95, 75, 20, "Base Cost", ID_TEXT_GASBASECOST);
	editGasBaseCosts.CreateThis(hWnd, 304, 95, 84, 20, false, ID_EDIT_GASBASECOST);
	textGasUpgradeFactor.CreateThis(hWnd, 408, 95, 75, 20, "Upgrade Factor", ID_TEXT_GASUPGRADEFACTOR);
	editGasUpgradeFactor.CreateThis(hWnd, 497, 95, 84, 20, false, ID_EDIT_GASUPGRADEFACTOR);

	groupTimeCosts.CreateThis(hWnd, 210, 125, 377, 45, "Time Costs", ID_GROUP_TIMECOSTS);
	textTimeBaseCosts.CreateThis(hWnd, 215, 145, 75, 20, "Base Cost", ID_TEXT_TIMEBASECOST);
	editTimeBaseCosts.CreateThis(hWnd, 304, 145, 84, 20, false, ID_EDIT_TIMEBASECOST);
	textTimeUpgradeFactor.CreateThis(hWnd, 408, 145, 75, 20, "Upgrade Factor", ID_TEXT_TIMEUPGRADEFACTOR);
	editTimeUpgradeFactor.CreateThis(hWnd, 497, 145, 84, 20, false, ID_EDIT_TIMEUPGRADEFACTOR);

	groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 329, "Player Settings", ID_GROUP_PLAYERSETTINGS);
	groupDefaultSettings.CreateThis(hWnd, 215, 205, 367, 45, "Default Settings", ID_GROUP_DEFAULTSETTINGS);
	textDefaultStartLevel.CreateThis(hWnd, 220, 225, 75, 20, "Start Level", ID_TEXT_DEFAULTSTARTLEVEL);
	editDefaultStartLevel.CreateThis(hWnd, 306, 225, 81, 20, false, ID_EDIT_DEFAULTSTARTLEVEL);
	textDefaultMaxLevel.CreateThis(hWnd, 407, 225, 75, 20, "Max Level", ID_TEXT_DEFAULTMAXLEVEL);
	editDefaultMaxLevel.CreateThis(hWnd, 493, 225, 81, 20, false, ID_EDIT_DEFAULTMAXLEVEL);

	for ( int player=0; player<12; player++ )
	{
		stringstream ssPlayer;
		if ( player < 9 )
			ssPlayer << "Player 0" << player+1;
		else
			ssPlayer << "Player " << player+1;

		textPlayer[player].CreateThis(hWnd, 215, 260+21*player, 50, 20, ssPlayer.str().c_str(), ID_TEXT_UPGRADEP1+player);
		checkPlayerDefault[player].CreateThis(hWnd, 276, 260+21*player, 52, 20, false, "Default", ID_CHECK_DEFAULTUPGRADEP1+player);
		textPlayerStartLevel[player].CreateThis(hWnd, 337, 260+21*player, 60, 20, "Start Level", ID_TEXT_P1STARTLEVEL+player);
		editPlayerStartLevel[player].CreateThis(hWnd, 398, 260+21*player, 56, 21, false, ID_EDIT_P1STARTLEVEL+player);
		textPlayerMaxLevel[player].CreateThis(hWnd, 464, 260+21*player, 60, 20, "Max Level", ID_TEXT_P1MAXLEVEL+player);
		editPlayerMaxLevel[player].CreateThis(hWnd, 525, 260+21*player, 56, 21, false, ID_EDIT_P1MAXLEVEL+player);
	}
}
