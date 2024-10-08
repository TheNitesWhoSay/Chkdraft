#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "ui/chkd_controls/unit_tree.h"

class UnitSettingsWindow : public WinLib::ClassWindow
{
    public:
        UnitSettingsWindow();
        virtual ~UnitSettingsWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hParent);
        void DisableUnitEditing();
        void EnableUnitEditing();
        void DisableUnitProperties();
        void EnableUnitProperties();
        void CheckReplaceUnitName();
        void SetDefaultUnitProperties();
        void ClearDefaultUnitProperties();
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Sc::Unit::Type selectedUnitType;
        bool possibleUnitNameUpdate;
        bool isDisabled;
        bool refreshing;

        UnitTree unitTree;

        WinLib::CheckBoxControl checkUseUnitDefaults;
        WinLib::CheckBoxControl checkEnabledByDefault;
        WinLib::ButtonControl buttonResetUnitDefaults;

        WinLib::GroupBoxControl groupUnitProperties;
        WinLib::TextControl textHitPoints;
        WinLib::EditControl editHitPoints;
        WinLib::EditControl editHitPointsByte;
        WinLib::TextControl textShieldPoints;
        WinLib::EditControl editShieldPoints;
        WinLib::TextControl textArmor;
        WinLib::EditControl editArmor;
        WinLib::TextControl textBuildTime;
        WinLib::EditControl editBuildTime;
        WinLib::TextControl textMineralCost;
        WinLib::EditControl editMineralCost;
        WinLib::TextControl textGasCost;
        WinLib::EditControl editGasCost;

        WinLib::GroupBoxControl groupGroundWeapon;
        WinLib::TextControl textGroundDamage;
        WinLib::EditControl editGroundDamage;
        WinLib::TextControl textGroundBonus;
        WinLib::EditControl editGroundBonus;

        WinLib::GroupBoxControl groupAirWeapon;
        WinLib::TextControl textAirDamage;
        WinLib::EditControl editAirDamage;
        WinLib::TextControl textAirBonus;
        WinLib::EditControl editAirBonus;

        WinLib::GroupBoxControl groupUnitName;
        WinLib::CheckBoxControl checkUseDefaultName;
        WinLib::EditControl editUnitName;

        WinLib::GroupBoxControl groupUnitAvailability;
        WinLib::TextControl textPlayerAvailability[12];
        WinLib::DropdownControl dropPlayerAvailability[12];
};

#endif