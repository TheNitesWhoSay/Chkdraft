#include "UpgradeSettings.h"
#include "Chkdraft.h"
#include <sstream>
#include <string>

enum ID {
	TREE_UPGRADES = ID_FIRST,
	CHECK_USEDEFAULTCOSTS,
	BUTTON_RESETUPGRADEDEFAULTS,
	GROUP_MINERALCOSTS,
	TEXT_MINERALBASECOST,
	EDIT_MINERALBASECOST,
	TEXT_MINERALUPGRADEFACTOR,
	EDIT_MINERALUPGRADEFACTOR,
	GROUP_GASCOSTS,
	TEXT_GASBASECOST,
	EDIT_GASBASECOST,
	TEXT_GASUPGRADEFACTOR,
	EDIT_GASUPGRADEFACTOR,
	GROUP_TIMECOSTS,
	TEXT_TIMEBASECOST,
	EDIT_TIMEBASECOST,
	TEXT_TIMEUPGRADEFACTOR,
	EDIT_TIMEUPGRADEFACTOR,
	GROUP_PLAYERSETTINGS,
	GROUP_DEFAULTSETTINGS,
	TEXT_DEFAULTSTARTLEVEL,
	EDIT_DEFAULTSTARTLEVEL,
	TEXT_DEFAULTMAXLEVEL,
	EDIT_DEFAULTMAXLEVEL,
	ID_TEXT_UPGRADEP1,
	ID_TEXT_UPGRADEP12 = (ID_TEXT_UPGRADEP1+11),
	ID_CHECK_DEFAULTUPGRADEP1,
	ID_CHECK_DEFAULTUPGRADEP12 = (ID_CHECK_DEFAULTUPGRADEP1+11),
	ID_TEXT_P1STARTLEVEL,
	ID_TEXT_P12STARTLEVEL = (ID_TEXT_P1STARTLEVEL+11),
	ID_EDIT_P1STARTLEVEL,
	ID_EDIT_P12STARTLEVEL = (ID_EDIT_P1STARTLEVEL+11),
	ID_TEXT_P1MAXLEVEL,
	ID_TEXT_P12MAXLEVEL = (ID_TEXT_P1MAXLEVEL+11),
	ID_EDIT_P1MAXLEVEL,
	ID_EDIT_P12MAXLEVEL = (ID_EDIT_P1MAXLEVEL+11)
};

UpgradeSettingsWindow::UpgradeSettingsWindow() : selectedUpgrade(-1), isDisabled(true), refreshing(false)
{

}

bool UpgradeSettingsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "UpgradeSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(0, "UpgradeSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

void UpgradeSettingsWindow::RefreshWindow()
{
	refreshing = true;
	if ( selectedUpgrade >= 0 && selectedUpgrade < 61 && CM != nullptr )
	{
		u8 upgrade = (u8)selectedUpgrade;
		chkd.mapSettingsWindow.SetTitle("Map Settings - [" + upgradeNames.at(selectedUpgrade) + "]");

		if ( isDisabled )
			EnableUpgradeEditing();

		bool useDefaultCosts = CM->upgradeUsesDefaultCosts(upgrade);
		checkUseDefaultCosts.SetCheck(useDefaultCosts);
		if ( useDefaultCosts )
			DisableCostEditing();
		else
			EnableCostEditing();

		u16 mineralCost = 0, mineralFactor = 0, gasCost = 0, gasFactor = 0, timeCost = 0, timeFactor = 0;
		if ( useDefaultCosts )
		{
			UPGRADEDAT* upgDat = chkd.scData.upgrades.UpgradeDat(upgrade);
			if ( upgDat != nullptr )
			{
				editMineralBaseCosts.SetEditNum<u16>(upgDat->MineralCost);
				editMineralUpgradeFactor.SetEditNum<u16>(upgDat->MineralFactor);
				editGasBaseCosts.SetEditNum<u16>(upgDat->VespeneCost);
				editGasUpgradeFactor.SetEditNum<u16>(upgDat->VespeneFactor);
				editTimeBaseCosts.SetEditNum<u16>(upgDat->TimeCost/15);
				editTimeUpgradeFactor.SetEditNum<u16>(upgDat->TimeFactor/15);
			}
		}
		else
		{
			if ( CM->getUpgradeMineralCost(upgrade, mineralCost) )
				editMineralBaseCosts.SetEditNum<u16>(mineralCost);
			if ( CM->getUpgradeMineralFactor(upgrade, mineralFactor) )
				editMineralUpgradeFactor.SetEditNum<u16>(mineralFactor);
			if ( CM->getUpgradeGasCost(upgrade, gasCost) )
				editGasBaseCosts.SetEditNum<u16>(gasCost);
			if ( CM->getUpgradeGasFactor(upgrade, gasFactor) )
				editGasUpgradeFactor.SetEditNum<u16>(gasFactor);
			if ( CM->getUpgradeTimeCost(upgrade, timeCost) )
				editTimeBaseCosts.SetEditNum<u16>(timeCost/15);
			if ( CM->getUpgradeTimeFactor(upgrade, timeFactor) )
				editTimeUpgradeFactor.SetEditNum<u16>(timeFactor/15);
		}

		u8 defaultStartLevel, defaultMaxLevel;
		if ( CM->getUpgradeDefaultStartLevel(upgrade, defaultStartLevel) )
			editDefaultStartLevel.SetEditNum<u8>(defaultStartLevel);
		if ( CM->getUpgradeDefaultMaxLevel(upgrade, defaultMaxLevel) )
			editDefaultMaxLevel.SetEditNum<u8>(defaultMaxLevel);

		for ( int player=0; player<12; player++ )
		{
			bool playerUsesDefault = CM->playerUsesDefaultUpgradeLevels(upgrade, player);
			checkPlayerDefault[player].SetCheck(playerUsesDefault);
			if ( playerUsesDefault )
				DisablePlayerEditing(player);
			else
			{
				EnablePlayerEditing(player);
				u8 startLevel, maxLevel;
				if ( CM->getUpgradePlayerStartLevel(upgrade, player, startLevel) )
					editPlayerStartLevel[player].SetEditNum<u8>(startLevel);
				if ( CM->getUpgradePlayerMaxLevel(upgrade, player, maxLevel) )
					editPlayerMaxLevel[player].SetEditNum<u8>(maxLevel);
			}
		}
	}
	else
		DisableUpgradeEditing();

	refreshing = false;
};

void UpgradeSettingsWindow::CreateSubWindows(HWND hWnd)
{
	treeUpgrades.CreateThis(hWnd, 5, 5, 200, 489, false, TREE_UPGRADES);
	checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 120, 20, false, "Use Default Costs", CHECK_USEDEFAULTCOSTS);
	buttonResetUpgradeDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Upgrades To Default", BUTTON_RESETUPGRADEDEFAULTS);

	groupMineralCosts.CreateThis(hWnd, 210, 25, 377, 45, "Mineral Costs", GROUP_MINERALCOSTS);
	textMineralBaseCosts.CreateThis(hWnd, 215, 45, 75, 20, "Base Cost", TEXT_MINERALBASECOST);
	editMineralBaseCosts.CreateThis(hWnd, 304, 45, 84, 20, false, EDIT_MINERALBASECOST);
	textMineralUpgradeFactor.CreateThis(hWnd, 408, 45, 75, 20, "Upgrade Factor", TEXT_MINERALUPGRADEFACTOR);
	editMineralUpgradeFactor.CreateThis(hWnd, 497, 45, 84, 20, false, EDIT_MINERALUPGRADEFACTOR);

	groupGasCosts.CreateThis(hWnd, 210, 75, 377, 45, "Gas Costs", GROUP_GASCOSTS);
	textGasBaseCosts.CreateThis(hWnd, 215, 95, 75, 20, "Base Cost", TEXT_GASBASECOST);
	editGasBaseCosts.CreateThis(hWnd, 304, 95, 84, 20, false, EDIT_GASBASECOST);
	textGasUpgradeFactor.CreateThis(hWnd, 408, 95, 75, 20, "Upgrade Factor", TEXT_GASUPGRADEFACTOR);
	editGasUpgradeFactor.CreateThis(hWnd, 497, 95, 84, 20, false, EDIT_GASUPGRADEFACTOR);

	groupTimeCosts.CreateThis(hWnd, 210, 125, 377, 45, "Time Costs", GROUP_TIMECOSTS);
	textTimeBaseCosts.CreateThis(hWnd, 215, 145, 75, 20, "Base Cost", TEXT_TIMEBASECOST);
	editTimeBaseCosts.CreateThis(hWnd, 304, 145, 84, 20, false, EDIT_TIMEBASECOST);
	textTimeUpgradeFactor.CreateThis(hWnd, 408, 145, 75, 20, "Upgrade Factor", TEXT_TIMEUPGRADEFACTOR);
	editTimeUpgradeFactor.CreateThis(hWnd, 497, 145, 84, 20, false, EDIT_TIMEUPGRADEFACTOR);

	groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 329, "Player Settings", GROUP_PLAYERSETTINGS);
	groupDefaultSettings.CreateThis(hWnd, 215, 205, 367, 45, "Default Settings", GROUP_DEFAULTSETTINGS);
	textDefaultStartLevel.CreateThis(hWnd, 220, 225, 75, 20, "Start Level", TEXT_DEFAULTSTARTLEVEL);
	editDefaultStartLevel.CreateThis(hWnd, 306, 225, 81, 20, false, EDIT_DEFAULTSTARTLEVEL);
	textDefaultMaxLevel.CreateThis(hWnd, 407, 225, 75, 20, "Max Level", TEXT_DEFAULTMAXLEVEL);
	editDefaultMaxLevel.CreateThis(hWnd, 493, 225, 81, 20, false, EDIT_DEFAULTMAXLEVEL);

	for ( int player=0; player<12; player++ )
	{
		std::stringstream ssPlayer;
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

	DisableUpgradeEditing();
}

void UpgradeSettingsWindow::DisableUpgradeEditing()
{
	isDisabled = true;
	checkUseDefaultCosts.DisableThis();
	chkd.mapSettingsWindow.SetTitle("Map Settings");

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

	if ( !CM->upgradeUsesDefaultCosts((u8)selectedUpgrade) )
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
	}

	isDisabled = false;
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

void UpgradeSettingsWindow::DisablePlayerEditing(u8 player)
{
	textPlayerStartLevel[player].DisableThis();
	editPlayerStartLevel[player].SetText("");
	editPlayerStartLevel[player].DisableThis();
	textPlayerMaxLevel[player].DisableThis();
	editPlayerMaxLevel[player].SetText("");
	editPlayerMaxLevel[player].DisableThis();
}

void UpgradeSettingsWindow::EnablePlayerEditing(u8 player)
{
	textPlayerStartLevel[player].EnableThis();
	editPlayerStartLevel[player].EnableThis();
	textPlayerMaxLevel[player].EnableThis();
	editPlayerMaxLevel[player].EnableThis();
}

void UpgradeSettingsWindow::SetDefaultUpgradeCosts()
{
	if ( selectedUpgrade > 0 && selectedUpgrade < 61 && CM != nullptr )
	{
		u8 upgrade = (u8)selectedUpgrade;
		UPGRADEDAT* upgDat = chkd.scData.upgrades.UpgradeDat(upgrade);

		CM->setUpgradeMineralCost(upgrade, upgDat->MineralCost);
		CM->setUpgradeMineralFactor(upgrade, upgDat->MineralFactor);
		CM->setUpgradeGasCost(upgrade, upgDat->VespeneCost);
		CM->setUpgradeGasFactor(upgrade, upgDat->VespeneFactor);
		CM->setUpgradeTimeCost(upgrade, upgDat->TimeCost);
		CM->setUpgradeTimeFactor(upgrade, upgDat->TimeFactor);
	}
}

void UpgradeSettingsWindow::ClearDefaultUpgradeCosts()
{
	if ( selectedUpgrade != -1 && selectedUpgrade < 61 && CM != nullptr )
	{
		u8 upgrade = (u8)selectedUpgrade;

		CM->setUpgradeMineralCost(upgrade, 0);
		CM->setUpgradeMineralFactor(upgrade, 0);
		CM->setUpgradeGasCost(upgrade, 0);
		CM->setUpgradeGasFactor(upgrade, 0);
		CM->setUpgradeTimeCost(upgrade, 0);
		CM->setUpgradeTimeFactor(upgrade, 0);
	}
}

LRESULT UpgradeSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	if ( idFrom == TREE_UPGRADES &&
		refreshing == false &&
		nmhdr->code == TVN_SELCHANGED &&
		((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
	{
		LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TREE_ITEM_TYPE,
			itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TREE_ITEM_DATA;

		u8 upgradeId = (u8)itemData;
		if ( itemType == TREE_TYPE_UPGRADE && upgradeId < 61 )
		{
			selectedUpgrade = upgradeId;
			RefreshWindow();
		}
		else
		{
			selectedUpgrade = -1;
			DisableUpgradeEditing();
		}
	}
	return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT UpgradeSettingsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ( refreshing == false )
	{
		switch ( LOWORD(wParam) )
		{
		case EDIT_MINERALBASECOST:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newMineralCost;
				if ( editMineralBaseCosts.GetEditNum<u16>(newMineralCost) )
				{
					CM->setUpgradeMineralCost((u8)selectedUpgrade, newMineralCost);
					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_MINERALUPGRADEFACTOR:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newMineralFactor;
				if ( editMineralUpgradeFactor.GetEditNum<u16>(newMineralFactor) )
				{
					CM->setUpgradeMineralFactor((u8)selectedUpgrade, newMineralFactor);
					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_GASBASECOST:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newGasCost;
				if ( editGasBaseCosts.GetEditNum<u16>(newGasCost) )
				{
					CM->setUpgradeGasCost((u8)selectedUpgrade, newGasCost);
					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_GASUPGRADEFACTOR:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newGasFactor;
				if ( editGasUpgradeFactor.GetEditNum<u16>(newGasFactor) )
				{
					CM->setUpgradeGasFactor((u8)selectedUpgrade, newGasFactor);
					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_TIMEBASECOST:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newTimeCost;
				if ( editTimeBaseCosts.GetEditNum<u16>(newTimeCost) )
				{
					if ( newTimeCost * 15 < newTimeCost ) // Value overflow
						CM->setUpgradeTimeCost((u8)selectedUpgrade, 65535); // Set to max
					else // Normal
						CM->setUpgradeTimeCost((u8)selectedUpgrade, newTimeCost * 15);

					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_TIMEUPGRADEFACTOR:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u16 newTimeFactor;
				if ( editTimeUpgradeFactor.GetEditNum<u16>(newTimeFactor) )
				{
					if ( newTimeFactor * 15 < newTimeFactor ) // Value overflow
						CM->setUpgradeTimeFactor((u8)selectedUpgrade, 65535); // Set to max
					else // Normal
						CM->setUpgradeTimeFactor((u8)selectedUpgrade, newTimeFactor * 15);

					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_DEFAULTSTARTLEVEL:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u8 newStartLevel;
				if ( editDefaultStartLevel.GetEditNum<u8>(newStartLevel) )
				{
					CM->setUpgradeDefaultStartLevel((u8)selectedUpgrade, newStartLevel);
					CM->notifyChange(false);
				}
			}
			break;

		case EDIT_DEFAULTMAXLEVEL:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				u8 newMaxLevel;
				if ( editDefaultMaxLevel.GetEditNum<u8>(newMaxLevel) )
				{
					CM->setUpgradeDefaultMaxLevel((u8)selectedUpgrade, newMaxLevel);
					CM->notifyChange(false);
				}
			}
			break;

		case CHECK_USEDEFAULTCOSTS:
			if ( HIWORD(wParam) == BN_CLICKED )
			{
				LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
				if ( selectedUpgrade != -1 )
				{
					if ( state == BST_CHECKED )
					{
						CM->setUpgradeUseDefaults((u8)selectedUpgrade, true);
						ClearDefaultUpgradeCosts();
						DisableCostEditing();
					}
					else
					{
						CM->setUpgradeUseDefaults((u8)selectedUpgrade, false);
						SetDefaultUpgradeCosts();
						EnableCostEditing();
					}

					RefreshWindow();
					CM->notifyChange(false);
				}
			}
			break;
		case BUTTON_RESETUPGRADEDEFAULTS:
			if ( HIWORD(wParam) == BN_CLICKED )
			{
				if ( MessageBox(hWnd, "Are you sure you want to reset all ugprade settings?", "Confirm", MB_YESNO) == IDYES )
				{
					buffer newUPGS("UPGS"), newUPGx("UPGx"), newUPGR("UPGR"), newPUPx("PUPx");
					if ( Get_UPGS(newUPGS) )
					{
						newUPGS.del(0, 8);
						CM->UPGS().takeAllData(newUPGS);
					}
					if ( Get_UPGx(newUPGx) )
					{
						newUPGx.del(0, 8);
						CM->UPGx().takeAllData(newUPGx);
					}
					if ( Get_UPGR(newUPGR) )
					{
						newUPGR.del(0, 8);
						CM->UPGR().takeAllData(newUPGR);
					}
					if ( Get_PUPx(newPUPx) )
					{
						newPUPx.del(0, 8);
						CM->PUPx().takeAllData(newPUPx);
					}

					CM->notifyChange(false);
					DisableCostEditing();
					RefreshWindow();
				}
			}
			break;
		default:
			if ( HIWORD(wParam) == BN_CLICKED && selectedUpgrade != -1 &&
				LOWORD(wParam) >= ID_CHECK_DEFAULTUPGRADEP1 && LOWORD(wParam) <= ID_CHECK_DEFAULTUPGRADEP12 )
			{
				LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
				int player = LOWORD(wParam) - ID_CHECK_DEFAULTUPGRADEP1;
				bool useDefault = (state == BST_CHECKED);
				CM->setUpgradePlayerDefaults((u8)selectedUpgrade, player, useDefault);
				if ( state != BST_CHECKED )
				{
					u8 defaultStartLevel, defaultMaxLevel;
					if ( CM->getUpgradeDefaultStartLevel((u8)selectedUpgrade, defaultStartLevel) )
						CM->setUpgradePlayerStartLevel((u8)selectedUpgrade, player, defaultStartLevel);
					if ( CM->getUpgradeDefaultMaxLevel((u8)selectedUpgrade, defaultMaxLevel) )
						CM->setUpgradePlayerMaxLevel((u8)selectedUpgrade, player, defaultMaxLevel);
				}
				CM->notifyChange(false);
				RefreshWindow();
			}
			else if ( HIWORD(wParam) == EN_CHANGE && selectedUpgrade != -1 &&
				LOWORD(wParam) >= ID_EDIT_P1STARTLEVEL && LOWORD(wParam) <= ID_EDIT_P12STARTLEVEL )
			{
				u8 player = (u8)(LOWORD(wParam) - ID_EDIT_P1STARTLEVEL),
					newStartLevel;
				if ( editPlayerStartLevel[player].GetEditNum<u8>(newStartLevel) )
				{
					CM->setUpgradePlayerStartLevel((u8)selectedUpgrade, player, newStartLevel);
					CM->notifyChange(false);
				}
			}
			else if ( HIWORD(wParam) == EN_CHANGE && selectedUpgrade != -1 &&
				LOWORD(wParam) >= ID_EDIT_P1MAXLEVEL && LOWORD(wParam) <= ID_EDIT_P12MAXLEVEL )
			{
				u8 player = (u8)(LOWORD(wParam) - ID_EDIT_P1MAXLEVEL),
					newMaxLevel;
				if ( editPlayerMaxLevel[player].GetEditNum<u8>(newMaxLevel) )
				{
					CM->setUpgradePlayerMaxLevel((u8)selectedUpgrade, player, newMaxLevel);
					CM->notifyChange(false);
				}
			}
			break;
		}
	}
	return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT UpgradeSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
			{
				RefreshWindow();
				if ( selectedUpgrade != -1 )
					chkd.mapSettingsWindow.SetTitle("Map Settings - [" + upgradeNames.at(selectedUpgrade) + "]");
				else
					chkd.mapSettingsWindow.SetTitle("Map Settings");
			}
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;

		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
