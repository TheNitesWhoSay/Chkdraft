#include "TechSettings.h"
#include "Chkdraft.h"

TechSettingsWindow::TechSettingsWindow() : selectedTech(-1), refreshing(false), isDisabled(true)
{

}

bool TechSettingsWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TechSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TechSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_TECHSETTINGS) )
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
	Scenario* chk = (Scenario*)chkd.maps.curr;
	if ( selectedTech >= 0 && selectedTech < 44 && chk != nullptr )
	{
		u8 tech = (u8)selectedTech;

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
	treeTechs.CreateThis(hWnd, 5, 5, 200, 489, false, ID_TREE_TECHS);
	checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 110, 20, false, "Use Default Costs", ID_CHECK_DEFAULTTECHCOSTS);
	buttonResetTechDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Techs To Default", ID_BUTTON_RESETTECHDEFAULTS);

	groupTechCosts.CreateThis(hWnd, 210, 25, 377, 65, "Tech Costs", ID_GROUP_TECHCOSTS);
	textMineralCosts.CreateThis(hWnd, 215, 45, 75, 20, "Mineral", ID_TEXT_MINERALCOSTS);
	editMineralCosts.CreateThis(hWnd, 304, 45, 84, 20, false, ID_EDIT_MINERALCOSTS);
	textGasCosts.CreateThis(hWnd, 408, 45, 75, 20, "Gas", ID_TEXT_GASCOSTS);
	editGasCosts.CreateThis(hWnd, 497, 45, 84, 20, false, ID_EDIT_GASCOSTS);
	textTimeCosts.CreateThis(hWnd, 215, 65, 75, 20, "Time", ID_TEXT_TIMECOSTS);
	editTimeCosts.CreateThis(hWnd, 304, 65, 84, 20, false, ID_EDIT_TIMECOSTS);
	textEnergyCosts.CreateThis(hWnd, 408, 65, 75, 20, "Energy", ID_TEXT_ENERGYCOSTS);
	editEnergyCosts.CreateThis(hWnd, 497, 65, 84, 20, false, ID_EDIT_ENERGYCOSTS);

	groupDefaultPlayerSettings.CreateThis(hWnd, 210, 105, 377, 80, "Default Player Setting", ID_GROUP_DEFAULTPLAYERSETTINGS);
	radioDisabledByDefault.CreateThis(hWnd, 215, 120, 110, 20, "Disabled by default", ID_RADIO_DEFAULTDISABLED);
	radioEnabledByDefault.CreateThis(hWnd, 215, 140, 110, 20, "Enabled by default", ID_RADIO_DEFAULTENABLED);
	radioResearchedByDefault.CreateThis(hWnd, 215, 160, 130, 20, "Researched by default", ID_RADIO_DEFAULTRESEARCHED);

	groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 265, "Player Settings", ID_GROUP_PLAYERSETTINGS);

	const char* playerTechSettings[] = { "Disabled", "Enabled", "Researched" };
	for ( int player=0; player<12; player++ )
	{
		stringstream ssPlayerTech;
		ssPlayerTech << "Use Default for Player ";
		if ( player < 9 )
			ssPlayerTech << "0" << player+1;
		else
			ssPlayerTech << player+1;

		checkUsePlayerDefaults[player].CreateThis(hWnd, 215, 210+20*player, 150, 20, false, ssPlayerTech.str().c_str(), ID_CHECK_P1TECHDEFAULT+player);
		dropPlayerTechSettings[player].CreateThis(hWnd, 460, 210+20*player, 120, 100, false, ID_DROP_P1TECHSETTINGS+player, 3, playerTechSettings, defaultFont);
	}

	buttonResetTechDefaults.DisableThis();
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
		Scenario* chk = chkd.maps.curr;
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
		Scenario* chk = chkd.maps.curr;

		chk->setTechMineralCost(tech, 0);
		chk->setTechGasCost(tech, 0);
		chk->setTechTimeCost(tech, 0);
		chk->setTechEnergyCost(tech, 0);
	}
}

LRESULT TechSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
			if ( refreshing == false )
			{
				switch ( LOWORD(wParam) )
				{
					case ID_CHECK_DEFAULTTECHCOSTS:
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
					case ID_EDIT_MINERALCOSTS:
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
					case ID_EDIT_GASCOSTS:
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
					case ID_EDIT_TIMECOSTS:
						if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
						{
							u16 newTimeCost;
							if ( editTimeCosts.GetEditNum<u16>(newTimeCost) )
							{
								if ( 15*(u32)newTimeCost > 65535 ) // Overflow
									chkd.maps.curr->setTechTimeCost((u8)selectedTech, 65535); // Set to max
								else // Normal
									chkd.maps.curr->setTechTimeCost((u8)selectedTech, newTimeCost*15);

								chkd.maps.curr->notifyChange(false);
							}
						}
						break;
					case ID_EDIT_ENERGYCOSTS:
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
					case ID_RADIO_DEFAULTDISABLED:
						if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
							 SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED &&
							 chkd.maps.curr->setTechDefaultAvailability((u8)selectedTech, false) )
						{
							chkd.maps.curr->setTechDefaultResearched((u8)selectedTech, false);
							chkd.maps.curr->notifyChange(false);
							RefreshWindow();
						}
						break;
					case ID_RADIO_DEFAULTENABLED:
						if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
							 SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL) == BST_CHECKED &&
							 chkd.maps.curr->setTechDefaultAvailability((u8)selectedTech, true) )
						{
							chkd.maps.curr->setTechDefaultResearched((u8)selectedTech, false);
							chkd.maps.curr->notifyChange(false);
							RefreshWindow();
						}
						break;
					case ID_RADIO_DEFAULTRESEARCHED:
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
							 LOWORD(wParam) >= ID_CHECK_P1TECHDEFAULT && LOWORD(wParam) <= ID_CHECK_P12TECHDEFAULT )
						{
							u8 player = (u8)(LOWORD(wParam)-ID_CHECK_P1TECHDEFAULT);
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
							 LOWORD(wParam) >= ID_DROP_P1TECHSETTINGS && LOWORD(wParam) <= ID_DROP_P12TECHSETTINGS )
						{
							u8 player = (u8)(LOWORD(wParam)-ID_DROP_P1TECHSETTINGS);
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
			else
				return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_NOTIFY:
			if ( refreshing == false && ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((LPNMTREEVIEW)lParam)->action != TVC_UNKNOWN )
			{
				LPARAM itemType = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_TYPE,
					   itemData = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_DATA;

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
			else
				return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}
