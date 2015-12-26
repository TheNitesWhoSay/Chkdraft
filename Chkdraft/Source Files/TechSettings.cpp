#include "TechSettings.h"
#include "Chkdraft.h"

#include <string>

enum ID {
	TREE_TECHS = ID_FIRST,
	CHECK_DEFAULTTECHCOSTS,
	BUTTON_RESETTECHDEFAULTS,
	GROUP_TECHCOSTS,
	TEXT_MINERALCOSTS,
	EDIT_MINERALCOSTS,
	TEXT_GASCOSTS,
	EDIT_GASCOSTS,
	TEXT_TIMECOSTS,
	EDIT_TIMECOSTS,
	TEXT_ENERGYCOSTS,
	EDIT_ENERGYCOSTS,
	GROUP_DEFAULTPLAYERSETTINGS,
	RADIO_DEFAULTDISABLED,
	RADIO_DEFAULTENABLED,
	RADIO_DEFAULTRESEARCHED,
	GROUP_TECHPLAYERSETTINGS,
	CHECK_P1TECHDEFAULT,
	CHECK_P12TECHDEFAULT = (CHECK_P1TECHDEFAULT+11),
	DROP_P1TECHSETTINGS,
	DROP_P12TECHSETTINGS = (DROP_P1TECHSETTINGS+11)
};

TechSettingsWindow::TechSettingsWindow() : selectedTech(-1), refreshing(false), isDisabled(true)
{

}

bool TechSettingsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TechSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TechSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

void TechSettingsWindow::RefreshWindow()
{
	refreshing = true;
	ScenarioPtr chk = chkd.maps.curr;
	if ( selectedTech >= 0 && selectedTech < 44 && chk != nullptr )
	{
		u8 tech = (u8)selectedTech;
		if ( selectedTech != -1 )
			chkd.mapSettingsWindow.SetTitle("Map Settings - [" + techNames.at(selectedTech) + "]");

		if ( isDisabled )
			EnableTechEditing();

		bool techUsesDefaultCosts = chk->techUsesDefaultCosts(tech);
		checkUseDefaultCosts.SetCheck(techUsesDefaultCosts);
		if ( techUsesDefaultCosts )
		{
			editMineralCosts.SetEditNum<u16>(chkd.scData.techs.TechDat(tech)->MineralCost);
			editGasCosts.SetEditNum<u16>(chkd.scData.techs.TechDat(tech)->VespeneCost);
			editTimeCosts.SetEditNum<u16>(chkd.scData.techs.TechDat(tech)->ResearchTime/15);
			editEnergyCosts.SetEditNum<u16>(chkd.scData.techs.TechDat(tech)->EnergyCost);
			DisableTechCosts();
		}
		else
		{
			u16 mineralCost, gasCost, timeCost, energyCost;
			EnableTechCosts();
			if ( chk->getTechMineralCost(tech, mineralCost) )
				editMineralCosts.SetEditNum<u16>(mineralCost);
			if ( chk->getTechGasCost(tech, gasCost) )
				editGasCosts.SetEditNum<u16>(gasCost);
			if ( chk->getTechTimeCost(tech, timeCost) )
				editTimeCosts.SetEditNum<u16>(timeCost/15);
			if ( chk->getTechEnergyCost(tech, energyCost) )
				editEnergyCosts.SetEditNum<u16>(energyCost);
		}

		bool enabledByDefault = chk->techAvailableByDefault(tech),
			 researchedByDefault = chk->techResearchedByDefault(tech);
		
		if ( enabledByDefault )
		{
			if ( researchedByDefault ) // Researched
			{
				radioResearchedByDefault.SetCheck(true);
				radioEnabledByDefault.SetCheck(false);
			}
			else // Enabled
			{
				radioEnabledByDefault.SetCheck(true);
				radioResearchedByDefault.SetCheck(false);
			}
			radioDisabledByDefault.SetCheck(false);
		}
		else // Disabled
		{
			radioDisabledByDefault.SetCheck(true);
			radioEnabledByDefault.SetCheck(false);
			radioResearchedByDefault.SetCheck(false);
		}

		for ( u8 player=0; player<12; player++ )
		{
			checkUsePlayerDefaults[player].SetCheck(chk->playerUsesDefaultTechSettings(tech, player));
			
			bool playerUsesDefaults = chk->playerUsesDefaultTechSettings(tech, player),
				 playerEnabledByDefault = chk->techAvailableForPlayer(tech, player),
				 playerResearchedByDefault = chk->techResearchedForPlayer(tech, player);

			if ( playerUsesDefaults ) // Player uses default settings for this tech
			{
				if ( enabledByDefault )
				{
					if ( researchedByDefault )
						dropPlayerTechSettings[player].SetSel(2); // Researched
					else
						dropPlayerTechSettings[player].SetSel(1); // Enabled
				}
				else
					dropPlayerTechSettings[player].SetSel(0); // Disabled

				dropPlayerTechSettings[player].DisableThis();
			}
			else // Player has his own settings for this tech
			{
				dropPlayerTechSettings[player].EnableThis();
				if ( playerEnabledByDefault )
				{
					if ( playerResearchedByDefault )
						dropPlayerTechSettings[player].SetSel(2); // Researched
					else
						dropPlayerTechSettings[player].SetSel(1); // Enabled
				}
				else
					dropPlayerTechSettings[player].SetSel(0); // Disabled
			}
		}
	}
	else
		DisableTechEditing();

	refreshing = false;
}

void TechSettingsWindow::CreateSubWindows(HWND hWnd)
{
	treeTechs.CreateThis(hWnd, 5, 5, 200, 489, false, TREE_TECHS);
	checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 110, 20, false, "Use Default Costs", CHECK_DEFAULTTECHCOSTS);
	buttonResetTechDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Techs To Default", BUTTON_RESETTECHDEFAULTS);

	groupTechCosts.CreateThis(hWnd, 210, 25, 377, 65, "Tech Costs", GROUP_TECHCOSTS);
	textMineralCosts.CreateThis(hWnd, 215, 45, 75, 20, "Mineral", TEXT_MINERALCOSTS);
	editMineralCosts.CreateThis(hWnd, 304, 45, 84, 20, false, EDIT_MINERALCOSTS);
	textGasCosts.CreateThis(hWnd, 408, 45, 75, 20, "Gas", TEXT_GASCOSTS);
	editGasCosts.CreateThis(hWnd, 497, 45, 84, 20, false, EDIT_GASCOSTS);
	textTimeCosts.CreateThis(hWnd, 215, 65, 75, 20, "Time", TEXT_TIMECOSTS);
	editTimeCosts.CreateThis(hWnd, 304, 65, 84, 20, false, EDIT_TIMECOSTS);
	textEnergyCosts.CreateThis(hWnd, 408, 65, 75, 20, "Energy", TEXT_ENERGYCOSTS);
	editEnergyCosts.CreateThis(hWnd, 497, 65, 84, 20, false, EDIT_ENERGYCOSTS);

	groupDefaultPlayerSettings.CreateThis(hWnd, 210, 105, 377, 80, "Default Player Setting", GROUP_DEFAULTPLAYERSETTINGS);
	radioDisabledByDefault.CreateThis(hWnd, 215, 120, 110, 20, "Disabled by default", RADIO_DEFAULTDISABLED);
	radioEnabledByDefault.CreateThis(hWnd, 215, 140, 110, 20, "Enabled by default", RADIO_DEFAULTENABLED);
	radioResearchedByDefault.CreateThis(hWnd, 215, 160, 130, 20, "Researched by default", RADIO_DEFAULTRESEARCHED);

	groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 265, "Player Settings", GROUP_TECHPLAYERSETTINGS);

	const char* playerTechSettings[] = { "Disabled", "Enabled", "Researched" };
	for ( int player=0; player<12; player++ )
	{
		std::stringstream ssPlayerTech;
		ssPlayerTech << "Use Default for Player ";
		if ( player < 9 )
			ssPlayerTech << "0" << player+1;
		else
			ssPlayerTech << player+1;

		checkUsePlayerDefaults[player].CreateThis(hWnd, 215, 210+20*player, 150, 20, false, ssPlayerTech.str().c_str(), CHECK_P1TECHDEFAULT+player);
		dropPlayerTechSettings[player].CreateThis(hWnd, 460, 210+20*player, 120, 100, false, false, DROP_P1TECHSETTINGS+player, 3, playerTechSettings, defaultFont);
	}

	DisableTechEditing();
}

void TechSettingsWindow::DisableTechCosts()
{
	groupTechCosts.DisableThis();
	textMineralCosts.DisableThis();
	editMineralCosts.DisableThis();
	textGasCosts.DisableThis();
	editGasCosts.DisableThis();
	textTimeCosts.DisableThis();
	editTimeCosts.DisableThis();
	textEnergyCosts.DisableThis();
	editEnergyCosts.DisableThis();
}

void TechSettingsWindow::EnableTechCosts()
{
	groupTechCosts.EnableThis();
	textMineralCosts.EnableThis();
	editMineralCosts.EnableThis();
	textGasCosts.EnableThis();
	editGasCosts.EnableThis();
	textTimeCosts.EnableThis();
	editTimeCosts.EnableThis();
	textEnergyCosts.EnableThis();
	editEnergyCosts.EnableThis();
}

void TechSettingsWindow::DisableTechEditing()
{
	isDisabled = true;
	chkd.mapSettingsWindow.SetTitle("Map Settings");
	checkUseDefaultCosts.DisableThis();
	DisableTechCosts();

	groupDefaultPlayerSettings.DisableThis();
	radioDisabledByDefault.DisableThis();
	radioEnabledByDefault.DisableThis();
	radioResearchedByDefault.DisableThis();

	groupPlayerSettings.DisableThis();
	for ( int i=0; i<12; i++ )
	{
		checkUsePlayerDefaults[i].DisableThis();
		dropPlayerTechSettings[i].DisableThis();
	}
}

void TechSettingsWindow::EnableTechEditing()
{
	checkUseDefaultCosts.EnableThis();

	if ( chkd.maps.curr->techUsesDefaultCosts((u8)selectedTech) == false )
		EnableTechCosts();

	groupDefaultPlayerSettings.EnableThis();
	radioDisabledByDefault.EnableThis();
	radioEnabledByDefault.EnableThis();
	radioResearchedByDefault.EnableThis();

	groupPlayerSettings.EnableThis();
	for ( int i=0; i<12; i++ )
	{
		checkUsePlayerDefaults[i].EnableThis();
		if ( SendMessage(checkUsePlayerDefaults[i].getHandle(), BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
			dropPlayerTechSettings[i].EnableThis();
	}
	isDisabled = false;
}

void TechSettingsWindow::SetDefaultTechCosts()
{
	if ( selectedTech > 0 && selectedTech < 44 && chkd.maps.curr != nullptr )
	{
		u8 tech = (u8)selectedTech;
		ScenarioPtr chk = chkd.maps.curr;
		TECHDAT* techDat = chkd.scData.techs.TechDat(tech);

		chk->setTechMineralCost(tech, techDat->MineralCost);
		chk->setTechGasCost(tech, techDat->VespeneCost);
		chk->setTechTimeCost(tech, techDat->ResearchTime);
		chk->setTechEnergyCost(tech, techDat->EnergyCost);
	}
}

void TechSettingsWindow::ClearDefaultTechCosts()
{
	if ( selectedTech > 0 && selectedTech < 44 && chkd.maps.curr != nullptr )
	{
		u8 tech = (u8)selectedTech;
		ScenarioPtr chk = chkd.maps.curr;

		chk->setTechMineralCost(tech, 0);
		chk->setTechGasCost(tech, 0);
		chk->setTechTimeCost(tech, 0);
		chk->setTechEnergyCost(tech, 0);
	}
}

LRESULT TechSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	if ( refreshing == false && nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
	{
		LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TREE_ITEM_TYPE,
			itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TREE_ITEM_DATA;

		u16 techId = (u16)itemData;
		if ( itemType == TREE_TYPE_TECH && techId < 44 )
		{
			EnableTechEditing();
			selectedTech = (u32)techId;
		}
		else
		{
			selectedTech = -1;
			DisableTechEditing();
		}

		RefreshWindow();
	}
	return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT TechSettingsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ( refreshing == false )
	{
		switch ( LOWORD(wParam) )
		{
		case BUTTON_RESETTECHDEFAULTS:
			if ( HIWORD(wParam) == BN_CLICKED )
			{
				if ( MessageBox(hWnd, "Are you sure you want to reset all tech settings?", "Confirm", MB_YESNO) == IDYES )
				{
					buffer newTECS("TECS"), newTECx("TECx"), newPTEC("PTEC"), newPTEx("PTEx");
					if ( Get_TECS(newTECS) )
					{
						newTECS.del(0, 8);
						chkd.maps.curr->TECS().takeAllData(newTECS);
					}
					if ( Get_TECx(newTECx) )
					{
						newTECx.del(0, 8);
						chkd.maps.curr->TECx().takeAllData(newTECx);
					}
					if ( Get_PTEC(newPTEC) )
					{
						newPTEC.del(0, 8);
						chkd.maps.curr->PTEC().takeAllData(newPTEC);
					}
					if ( Get_PTEx(newPTEx) )
					{
						newPTEx.del(0, 8);
						chkd.maps.curr->PTEx().takeAllData(newPTEx);
					}

					chkd.maps.curr->notifyChange(false);
					RefreshWindow();
				}
			}
			break;

		case CHECK_DEFAULTTECHCOSTS:
			if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 )
			{
				LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
				if ( state == BST_CHECKED )
				{
					ClearDefaultTechCosts();
					DisableTechCosts();
					chkd.maps.curr->setTechUseDefaults((u8)selectedTech, true);
				}
				else
				{
					SetDefaultTechCosts();
					EnableTechCosts();
					chkd.maps.curr->setTechUseDefaults((u8)selectedTech, false);
				}

				chkd.maps.curr->notifyChange(false);
				RefreshWindow();
			}
			break;
		case EDIT_MINERALCOSTS:
			if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
			{
				u16 newMineralCost;
				if ( editMineralCosts.GetEditNum<u16>(newMineralCost) )
				{
					chkd.maps.curr->setTechMineralCost((u8)selectedTech, newMineralCost);
					chkd.maps.curr->notifyChange(false);
				}
			}
			break;
		case EDIT_GASCOSTS:
			if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
			{
				u16 newGasCost;
				if ( editGasCosts.GetEditNum<u16>(newGasCost) )
				{
					chkd.maps.curr->setTechGasCost((u8)selectedTech, newGasCost);
					chkd.maps.curr->notifyChange(false);
				}
			}
			break;
		case EDIT_TIMECOSTS:
			if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
			{
				u16 newTimeCost;
				if ( editTimeCosts.GetEditNum<u16>(newTimeCost) )
				{
					if ( 15 * (u32)newTimeCost > 65535 ) // Overflow
						chkd.maps.curr->setTechTimeCost((u8)selectedTech, 65535); // Set to max
					else // Normal
						chkd.maps.curr->setTechTimeCost((u8)selectedTech, newTimeCost * 15);

					chkd.maps.curr->notifyChange(false);
				}
			}
			break;
		case EDIT_ENERGYCOSTS:
			if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
			{
				u16 newEnergyCost;
				if ( editEnergyCosts.GetEditNum<u16>(newEnergyCost) )
				{
					chkd.maps.curr->setTechEnergyCost((u8)selectedTech, newEnergyCost);
					chkd.maps.curr->notifyChange(false);
				}
			}
			break;
		case RADIO_DEFAULTDISABLED:
			if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
				SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED &&
				chkd.maps.curr->setTechDefaultAvailability((u8)selectedTech, false) )
			{
				chkd.maps.curr->setTechDefaultResearched((u8)selectedTech, false);
				chkd.maps.curr->notifyChange(false);
				RefreshWindow();
			}
			break;
		case RADIO_DEFAULTENABLED:
			if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
				SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED &&
				chkd.maps.curr->setTechDefaultAvailability((u8)selectedTech, true) )
			{
				chkd.maps.curr->setTechDefaultResearched((u8)selectedTech, false);
				chkd.maps.curr->notifyChange(false);
				RefreshWindow();
			}
			break;
		case RADIO_DEFAULTRESEARCHED:
			if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
				SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED &&
				chkd.maps.curr->setTechDefaultResearched((u8)selectedTech, true) )
			{
				chkd.maps.curr->setTechDefaultAvailability((u8)selectedTech, true);
				chkd.maps.curr->notifyChange(false);
				RefreshWindow();
			}
			break;
		default:
			if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
				LOWORD(wParam) >= CHECK_P1TECHDEFAULT && LOWORD(wParam) <= CHECK_P12TECHDEFAULT )
			{
				u8 player = (u8)(LOWORD(wParam) - CHECK_P1TECHDEFAULT);
				bool useDefault = (SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED);
				if ( useDefault )
					dropPlayerTechSettings[player].DisableThis();
				else
					dropPlayerTechSettings[player].EnableThis();
				chkd.maps.curr->setPlayerUsesDefaultTechSettings((u8)selectedTech, player, useDefault);
				chkd.maps.curr->notifyChange(false);
				RefreshWindow();
			}
			if ( HIWORD(wParam) == CBN_SELCHANGE && selectedTech != -1 &&
				LOWORD(wParam) >= DROP_P1TECHSETTINGS && LOWORD(wParam) <= DROP_P12TECHSETTINGS )
			{
				u8 player = (u8)(LOWORD(wParam) - DROP_P1TECHSETTINGS);
				int selection = dropPlayerTechSettings[player].GetSel();
				if ( selection == 0 ) // Disabled
				{
					chkd.maps.curr->setTechAvailableForPlayer((u8)selectedTech, player, false);
					chkd.maps.curr->setTechResearchedForPlayer((u8)selectedTech, player, false);
				}
				else if ( selection == 1 ) // Enabled
				{
					chkd.maps.curr->setTechAvailableForPlayer((u8)selectedTech, player, true);
					chkd.maps.curr->setTechResearchedForPlayer((u8)selectedTech, player, false);
				}
				else if ( selection == 2 ) // Researched
				{
					chkd.maps.curr->setTechAvailableForPlayer((u8)selectedTech, player, true);
					chkd.maps.curr->setTechResearchedForPlayer((u8)selectedTech, player, true);
				}

				chkd.maps.curr->notifyChange(false);
			}
		}
	}
	return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT TechSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
			{
				RefreshWindow();
				if ( selectedTech != -1 )
					chkd.mapSettingsWindow.SetTitle("Map Settings - [" + techNames.at(selectedTech) + "]");
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
