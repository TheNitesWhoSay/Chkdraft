#ifndef UPGRADESETTINGS_H
#define UPGRADESETTINGS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/UpgradeTree.h"

/*
    UPGS/UPGx - upgrade uses deafult costs & all costs
    UPGR/PUPx - upgrade max levels (global and player based)
*/

class UpgradeSettingsWindow : public WinLib::ClassWindow
{
    public:
        UpgradeSettingsWindow();
        virtual ~UpgradeSettingsWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hWnd);
        void DisableUpgradeEditing();
        void EnableUpgradeEditing();
        void DisableCostEditing();
        void EnableCostEditing();
        void DisablePlayerEditing(u8 player);
        void EnablePlayerEditing(u8 player);
        void SetDefaultUpgradeCosts();
        void ClearDefaultUpgradeCosts();
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        s32 selectedUpgrade;
        bool isDisabled;
        bool refreshing;

        UpgradeTree treeUpgrades;

        WinLib::CheckBoxControl checkUseDefaultCosts;
        WinLib::ButtonControl buttonResetUpgradeDefaults;

        WinLib::GroupBoxControl groupMineralCosts;
        WinLib::TextControl textMineralBaseCosts;
        WinLib::EditControl editMineralBaseCosts;
        WinLib::TextControl textMineralUpgradeFactor;
        WinLib::EditControl editMineralUpgradeFactor;

        WinLib::GroupBoxControl groupGasCosts;
        WinLib::TextControl textGasBaseCosts;
        WinLib::EditControl editGasBaseCosts;
        WinLib::TextControl textGasUpgradeFactor;
        WinLib::EditControl editGasUpgradeFactor;

        WinLib::GroupBoxControl groupTimeCosts;
        WinLib::TextControl textTimeBaseCosts;
        WinLib::EditControl editTimeBaseCosts;
        WinLib::TextControl textTimeUpgradeFactor;
        WinLib::EditControl editTimeUpgradeFactor;

        WinLib::GroupBoxControl groupPlayerSettings;
        WinLib::GroupBoxControl groupDefaultSettings;
        WinLib::TextControl textDefaultStartLevel;
        WinLib::EditControl editDefaultStartLevel;
        WinLib::TextControl textDefaultMaxLevel;
        WinLib::EditControl editDefaultMaxLevel;

        WinLib::TextControl textPlayer[12];
        WinLib::CheckBoxControl checkPlayerDefault[12];
        WinLib::TextControl textPlayerStartLevel[12];
        WinLib::EditControl editPlayerStartLevel[12];
        WinLib::TextControl textPlayerMaxLevel[12];
        WinLib::EditControl editPlayerMaxLevel[12];
};

#endif