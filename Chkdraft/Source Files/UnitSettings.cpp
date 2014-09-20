#include "UnitSettings.h"

bool UnitSettingsWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "UnitSettings", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "UnitSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_UNITSETTINGS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool UnitSettingsWindow::DestroyThis()
{
	return true;
}

LRESULT UnitSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

void UnitSettingsWindow::CreateSubWindows(HWND hParent)
{
	//CreateStaticText(hParent, 5, 5, 100, 20, "Unit Settings...");
	CreateUnitTree(hParent);
	buttonResetUnitDefaults.CreateThis(hParent, 5, 494, 200, 25, "Reset All Units To Default", ID_BUTTON_RESETALLUNITDEFAULTS);
	checkUseUnitDefaults.CreateThis(hParent, 210, 5, 100, 20, false, "Use Unit Defaults", ID_CHECK_USEUNITDEFAULTS);
	checkEnabledByDefault.CreateThis(hParent, 403, 5, 120, 20, false, "Enabled by Default", ID_CHECK_ENABLEDBYDEFAULT);

	groupUnitProperties.CreateThis(hParent, 210, 25, 377, 292, "Unit Properties", ID_GROUP_UNITPROPERTIES);
	textHitPoints.CreateThis(hParent, 215, 40, 75, 20, "Hit Points", ID_TEXT_UNITHITPOINTS);
	editHitPoints.CreateThis(hParent, 303, 40, 63, 20, false, ID_EDIT_UNITHITPOINTS);
	editHitPointsByte.CreateThis(hParent, 371, 40, 15, 20, false, ID_EDIT_UNITHITPOINTSBYTE);
	textShieldPoints.CreateThis(hParent, 215, 65, 83, 20, "Shield Points", ID_TEXT_UNITSHIELDPOINTS);
	editShieldPoints.CreateThis(hParent, 303, 65, 83, 20, false, ID_EDIT_UNITSHIELDPOINTS);
	textArmor.CreateThis(hParent, 215, 90, 75, 20, "Armor", ID_TEXT_UNITARMOR);
	editArmor.CreateThis(hParent, 303, 90, 83, 20, false, ID_EDIT_UNITARMOR);
	textBuildTime.CreateThis(hParent, 411, 40, 75, 20, "Build Time", ID_TEXT_UNITBUILDTIME);
	editBuildTime.CreateThis(hParent, 499, 40, 83, 20, false, ID_EDIT_UNITBUILDTIME);
	textMineralCost.CreateThis(hParent, 411, 65, 75, 20, "Mineral Cost", ID_TEXT_UNITMINERALCOST);
	editMineralCost.CreateThis(hParent, 499, 65, 83, 20, false, ID_EDIT_UNITMINERALCOST);
	textGasCost.CreateThis(hParent, 411, 90, 75, 20, "Gas Cost", ID_TEXT_UNITGASCOST);
	editGasCost.CreateThis(hParent, 499, 90, 83, 20, false, ID_EDIT_UNITGASCOST);

	groupGroundWeapon.CreateThis(hParent, 215, 115, 367, 62, "Ground Weapon [NAME]", ID_GROUP_GROUNDWEAPON);
	textGroundDamage.CreateThis(hParent, 220, 135, 75, 20, "Damage", ID_TEXT_UNITGROUNDDAMAGE);
	editGroundDamage.CreateThis(hParent, 308, 135, 83, 20, false, ID_EDIT_UNITGROUNDDAMAGE);
	textGroundBonus.CreateThis(hParent, 401, 135, 75, 20, "Bonus", ID_TEXT_UNITGROUNDBONUS);
	editGroundBonus.CreateThis(hParent, 489, 135, 83, 20, false, ID_EDIT_UNITGROUNDBONUS);

	groupAirWeapon.CreateThis(hParent, 215, 182, 367, 62, "Air Weapon [NAME]", ID_GROUP_AIRWEAPON);
	textAirDamage.CreateThis(hParent, 220, 202, 75, 20, "Damage", ID_TEXT_UNITAIRDAMAGE);
	editAirDamage.CreateThis(hParent, 308, 202, 83, 20, false, ID_EDIT_UNITAIRDAMAGE);
	textAirBonus.CreateThis(hParent, 401, 202, 75, 20, "Bonus", ID_TEXT_UNITAIRBONUS);
	editAirBonus.CreateThis(hParent, 489, 202, 83, 20, false, ID_EDIT_UNITAIRBONUS);

	groupUnitName.CreateThis(hParent, 215, 249, 367, 62, "Unit Name", ID_GROUP_UNITNAME);
	checkUseDefaultName.CreateThis(hParent, 220, 269, 75, 20, false, "Use Default", ID_CHECK_USEDEFAULTUNITNAME);
	editUnitName.CreateThis(hParent, 342, 269, 234, 20, false, ID_EDIT_UNITNAME);

	// 215, 403
	//   +27
	const char* items[] = { "Default", "Yes", "No" };
	int numItems = sizeof(items)/sizeof(const char*);
	textP1Availability.CreateThis(hParent, 215, 336, 75, 20, "Player 01", ID_TEXT_P1UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 336, 75, 20, "Player 02", ID_TEXT_P2UNITAVAILABILITY);
	textP3Availability.CreateThis(hParent, 215, 363, 75, 20, "Player 03", ID_TEXT_P3UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 363, 75, 20, "Player 04", ID_TEXT_P4UNITAVAILABILITY);
	textP3Availability.CreateThis(hParent, 215, 390, 75, 20, "Player 05", ID_TEXT_P5UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 390, 75, 20, "Player 06", ID_TEXT_P6UNITAVAILABILITY);
	textP3Availability.CreateThis(hParent, 215, 417, 75, 20, "Player 07", ID_TEXT_P7UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 417, 75, 20, "Player 08", ID_TEXT_P8UNITAVAILABILITY);
	textP3Availability.CreateThis(hParent, 215, 444, 75, 20, "Player 09", ID_TEXT_P9UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 444, 75, 20, "Player 10", ID_TEXT_P10UNITAVAILABILITY);
	textP3Availability.CreateThis(hParent, 215, 471, 75, 20, "Player 11", ID_TEXT_P11UNITAVAILABILITY);
	textP2Availability.CreateThis(hParent, 403, 471, 75, 20, "Player 12", ID_TEXT_P12UNITAVAILABILITY);

	dropP1Availability.CreateThis(hParent, 304, 336, 84, 100, false, ID_DROP_P1UNITAVAILABILITY, numItems, items, defaultFont);
	dropP2Availability.CreateThis(hParent, 492, 336, 84, 100, false, ID_DROP_P2UNITAVAILABILITY, numItems, items, defaultFont);
	dropP3Availability.CreateThis(hParent, 304, 363, 84, 100, false, ID_DROP_P3UNITAVAILABILITY, numItems, items, defaultFont);
	dropP4Availability.CreateThis(hParent, 492, 363, 84, 100, false, ID_DROP_P4UNITAVAILABILITY, numItems, items, defaultFont);
	dropP5Availability.CreateThis(hParent, 304, 390, 84, 100, false, ID_DROP_P5UNITAVAILABILITY, numItems, items, defaultFont);
	dropP6Availability.CreateThis(hParent, 492, 390, 84, 100, false, ID_DROP_P6UNITAVAILABILITY, numItems, items, defaultFont);
	dropP7Availability.CreateThis(hParent, 304, 417, 84, 100, false, ID_DROP_P7UNITAVAILABILITY, numItems, items, defaultFont);
	dropP8Availability.CreateThis(hParent, 492, 417, 84, 100, false, ID_DROP_P8UNITAVAILABILITY, numItems, items, defaultFont);
	dropP9Availability.CreateThis(hParent, 304, 444, 84, 100, false, ID_DROP_P9UNITAVAILABILITY, numItems, items, defaultFont);
	dropP10Availability.CreateThis(hParent, 492, 444, 84, 100, false, ID_DROP_P10UNITAVAILABILITY, numItems, items, defaultFont);
	dropP11Availability.CreateThis(hParent, 304, 471, 84, 100, false, ID_DROP_P11UNITAVAILABILITY, numItems, items, defaultFont);
	dropP12Availability.CreateThis(hParent, 492, 471, 84, 100, false, ID_DROP_P12UNITAVAILABILITY, numItems, items, defaultFont);

	groupUnitAvailability.CreateThis(hParent, 210, 321, 372, 198, "Unit Availability", ID_GROUP_UNITAVAILABILITY);
}

void UnitSettingsWindow::CreateUnitTree(HWND hParent)
{

}
