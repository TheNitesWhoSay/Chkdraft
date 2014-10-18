#include "TechSettings.h"
#include "Chkdraft.h"

TechSettingsWindow::TechSettingsWindow() : selectedTech(-1)
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
	if ( selectedTech >= 0 && selectedTech < 44 && chkd.maps.curr != nullptr )
	{

	}
	else
		DisableTechEditing();
}

LRESULT TechSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case BN_CLICKED:
					switch ( LOWORD(wParam) )
					{
						case ID_CHECK_DEFAULTTECHCOSTS:
							{
								LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, NULL, NULL);
								if ( selectedTech != -1 )
								{
									if ( state == BST_CHECKED )
									{
										DisableTechCosts();
										chkd.maps.curr->setTechUseDefaults((u8)selectedTech, true);
									}
									else
									{
										EnableTechCosts();
										chkd.maps.curr->setTechUseDefaults((u8)selectedTech, false);
									}

									chkd.maps.curr->notifyChange(false);
								}
							}
							break;
					}
					break;
				
			}
			break;

		case WM_NOTIFY:
			if ( ((NMHDR*)lParam)->code == TVN_SELCHANGED && ((LPNMTREEVIEW)lParam)->action != TVC_UNKNOWN )
			{
				LPARAM itemType = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_TYPE,
					   itemData = (((NMTREEVIEW*)lParam)->itemNew.lParam)&TREE_ITEM_DATA;

				u16 techId = (u16)itemData;
				if ( itemType == TREE_TYPE_TECH && techId < 44 )
				{
					cout << techId << endl;
				}
			}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void TechSettingsWindow::CreateSubWindows(HWND hWnd)
{
	treeTechs.CreateThis(hWnd, 5, 5, 200, 489, false, ID_TREE_TECHS);
	checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 110, 20, false, "Use Default Costs", ID_CHECK_DEFAULTTECHCOSTS);
	buttonResetTechDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Techs To Default", ID_BUTTON_RESETTECHDEFAULTS);

	groupTechCosts.CreateThis(hWnd, 210, 25, 377, 65, "Tech Costs", ID_GROUP_TECHCOSTS);
	textMineralCosts.CreateThis(hWnd, 215, 45, 75, 20, "Mineral", ID_TEXT_MINERALCOSTS);
	editMineralCosts.CreateThis(hWnd, 304, 45, 84, 20, false, ID_EDIT_MIENRALCOSTS);
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

		checkUsePlayerDefaults[player].CreateThis(hWnd, 215, 210+20*player, 150, 20, false, ssPlayerTech.str().c_str(), ID_CHECK_P1TECHDEFAULT);
		dropPlayerTechSettings[player].CreateThis(hWnd, 460, 210+20*player, 120, 100, false, ID_DROP_P1TECHSETTINGS, 3, playerTechSettings, defaultFont);
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

	if ( SendMessage((HWND)checkUseDefaultCosts.getHandle(), BM_GETCHECK, NULL, NULL) == BST_UNCHECKED )
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
}
