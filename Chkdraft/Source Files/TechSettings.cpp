#include "TechSettings.h"
#include "GuiAccel.h"

bool TechSettingsWindow::CreateThis(HWND hParent)
{
	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TechSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TechSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_TECHSETTINGS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

LRESULT TechSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

void TechSettingsWindow::CreateSubWindows(HWND hWnd)
{
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
}
