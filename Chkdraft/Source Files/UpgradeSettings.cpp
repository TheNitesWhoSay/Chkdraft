#include "UpgradeSettings.h"
#include "Chkdraft.h"

UpgradeSettingsWindow::UpgradeSettingsWindow() : selectedUpgrade(-1)
{

}

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

void UpgradeSettingsWindow::RefreshWindow()
{
	if ( selectedUpgrade >= 0 && selectedUpgrade < 61 && chkd.maps.curr != nullptr )
	{

	}
	else
		DisableUpgradeEditing();
};

LRESULT UpgradeSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case BN_CLICKED:
					switch ( LOWORD(wParam) )
					{
						case ID_CHECK_USEDEFAULTCOSTS:
							{
								LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
								if ( selectedUpgrade != -1 )
								{
									if ( state == BST_CHECKED )
										DisableCostEditing();
									else
										EnableCostEditing();

									chkd.maps.curr->notifyChange(false);
								}
							}
							break;
						default:
							if ( selectedUpgrade != -1 && LOWORD(wParam) >= ID_CHECK_DEFAULTUPGRADEP1 && LOWORD(wParam) <= ID_CHECK_DEFAULTUPGRADEP12 )
							{
								LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
								int player = LOWORD(wParam)-ID_CHECK_DEFAULTUPGRADEP1;
								if ( state == BST_CHECKED )
								{
									textPlayerStartLevel[player].DisableThis();
									editPlayerStartLevel[player].DisableThis();
									textPlayerMaxLevel[player].DisableThis();
									editPlayerMaxLevel[player].DisableThis();
								}
								else
								{
									textPlayerStartLevel[player].EnableThis();
									editPlayerStartLevel[player].EnableThis();
									textPlayerMaxLevel[player].EnableThis();
									editPlayerMaxLevel[player].EnableThis();
								}
							}
							break;
					}
			}
			break;

		case WM_NOTIFY:
			if ( ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((LPNMTREEVIEW)lParam)->action != TVC_UNKNOWN )
			{
				LPARAM itemType = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_TYPE,
					   itemData = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_DATA;

				u16 upgradeId = (u16)itemData;
				if ( itemType == TREE_TYPE_UPGRADE && upgradeId < 61 )
				{
					cout << upgradeId << endl;
				}
			}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void UpgradeSettingsWindow::CreateSubWindows(HWND hWnd)
{
	treeUpgrades.CreateThis(hWnd, 5, 5, 200, 489, false, ID_TREE_UPGRADES);
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

	buttonResetUpgradeDefaults.DisableThis();
	DisableUpgradeEditing();
}

void UpgradeSettingsWindow::DisableUpgradeEditing()
{
	checkUseDefaultCosts.DisableThis();

	DisableCostEditing();

	groupPlayerSettings.DisableThis();
	groupDefaultSettings.DisableThis();
	textDefaultStartLevel.DisableThis();
	editDefaultStartLevel.DisableThis();
	textDefaultMaxLevel.DisableThis();
	editDefaultMaxLevel.DisableThis();

	for ( int i=0; i<12; i++ )
	{
		textPlayer[i].DisableThis();
		checkPlayerDefault[i].DisableThis();
		textPlayerStartLevel[i].DisableThis();
		editPlayerStartLevel[i].DisableThis();
		textPlayerMaxLevel[i].DisableThis();
		editPlayerMaxLevel[i].DisableThis();
	}
}

void UpgradeSettingsWindow::EnableUpgradeEditing()
{
	checkUseDefaultCosts.EnableThis();

	if ( SendMessage((HWND)checkUseDefaultCosts.getHandle(), BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
		EnableCostEditing();

	groupPlayerSettings.EnableThis();
	groupDefaultSettings.EnableThis();
	textDefaultStartLevel.EnableThis();
	editDefaultStartLevel.EnableThis();
	textDefaultMaxLevel.EnableThis();
	editDefaultMaxLevel.EnableThis();

	for ( int i=0; i<12; i++ )
	{
		textPlayer[i].EnableThis();
		checkPlayerDefault[i].EnableThis();
		if ( SendMessage((HWND)checkPlayerDefault[i].getHandle(), BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
		{
			textPlayerStartLevel[i].EnableThis();
			editPlayerStartLevel[i].EnableThis();
			textPlayerMaxLevel[i].EnableThis();
			editPlayerMaxLevel[i].EnableThis();
		}
	}
}

void UpgradeSettingsWindow::DisableCostEditing()
{
	groupMineralCosts.DisableThis();
	textMineralBaseCosts.DisableThis();
	editMineralBaseCosts.DisableThis();
	textMineralUpgradeFactor.DisableThis();
	editMineralUpgradeFactor.DisableThis();

	groupGasCosts.DisableThis();
	textGasBaseCosts.DisableThis();
	editGasBaseCosts.DisableThis();
	textGasUpgradeFactor.DisableThis();
	editGasUpgradeFactor.DisableThis();

	groupTimeCosts.DisableThis();
	textTimeBaseCosts.DisableThis();
	editTimeBaseCosts.DisableThis();
	textTimeUpgradeFactor.DisableThis();
	editTimeUpgradeFactor.DisableThis();
}

void UpgradeSettingsWindow::EnableCostEditing()
{
	groupMineralCosts.EnableThis();
	textMineralBaseCosts.EnableThis();
	editMineralBaseCosts.EnableThis();
	textMineralUpgradeFactor.EnableThis();
	editMineralUpgradeFactor.EnableThis();

	groupGasCosts.EnableThis();
	textGasBaseCosts.EnableThis();
	editGasBaseCosts.EnableThis();
	textGasUpgradeFactor.EnableThis();
	editGasUpgradeFactor.EnableThis();

	groupTimeCosts.EnableThis();
	textTimeBaseCosts.EnableThis();
	editTimeBaseCosts.EnableThis();
	textTimeUpgradeFactor.EnableThis();
	editTimeUpgradeFactor.EnableThis();
}
