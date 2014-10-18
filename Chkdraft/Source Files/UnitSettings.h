#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "UnitTree.h"

class UnitSettingsWindow : public ClassWindow
{
	public:
		UnitSettingsWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void RefreshWindow();

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CreateSubWindows(HWND hParent);
		void DisableUnitEditing();
		void EnableUnitEditing();
		void DisableUnitProperties();
		void EnableUnitProperties();
		void CheckReplaceUnitName();
		void SetDefaultUnitProperties();
		void ClearDefaultUnitProperties();

		s32 selectedUnit;
		bool possibleUnitNameUpdate;
		bool isDisabled;
		bool refreshing;

		UnitTree unitTree;

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
		TextControl textPlayerAvailability[12];
		DropdownControl dropPlayerAvailability[12];
};

#endif