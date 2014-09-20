#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class UnitSettingsWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CreateSubWindows(HWND hParent);
		void CreateUnitTree(HWND hParent);

		CheckBoxControl checkUseUnitDefaults;
		CheckBoxControl checkEnabledByDefault;
		ButtonControl buttonResetUnitDefaults;

		GroupBoxControl groupUnitProperties;
		TextControl textHitPoints;
		EditControl editHitPoints;
		EditControl editHitPointsByte;
		TextControl textShieldPoints;
		EditControl editShieldPoints;
		TextControl textArmor;
		EditControl editArmor;
		TextControl textBuildTime;
		EditControl editBuildTime;
		TextControl textMineralCost;
		EditControl editMineralCost;
		TextControl textGasCost;
		EditControl editGasCost;

		GroupBoxControl groupGroundWeapon;
		TextControl textGroundDamage;
		EditControl editGroundDamage;
		TextControl textGroundBonus;
		EditControl editGroundBonus;

		GroupBoxControl groupAirWeapon;
		TextControl textAirDamage;
		EditControl editAirDamage;
		TextControl textAirBonus;
		EditControl editAirBonus;

		GroupBoxControl groupUnitName;
		CheckBoxControl checkUseDefaultName;
		EditControl editUnitName;

		GroupBoxControl groupUnitAvailability;
		TextControl textP1Availability;
		DropdownControl dropP1Availability;
		TextControl textP2Availability;
		DropdownControl dropP2Availability;
		TextControl textP3Availability;
		DropdownControl dropP3Availability;
		TextControl textP4Availability;
		DropdownControl dropP4Availability;
		TextControl textP5Availability;
		DropdownControl dropP5Availability;
		TextControl textP6Availability;
		DropdownControl dropP6Availability;
		TextControl textP7Availability;
		DropdownControl dropP7Availability;
		TextControl textP8Availability;
		DropdownControl dropP8Availability;
		TextControl textP9Availability;
		DropdownControl dropP9Availability;
		TextControl textP10Availability;
		DropdownControl dropP10Availability;
		TextControl textP11Availability;
		DropdownControl dropP11Availability;
		TextControl textP12Availability;
		DropdownControl dropP12Availability;
};

#endif