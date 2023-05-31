#include "UpgradeSettings.h"
#include "../../../Chkdraft.h"
#include <sstream>
#include <string>

enum_t(Id, u32, {
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
});

UpgradeSettingsWindow::UpgradeSettingsWindow() : selectedUpgrade(-1), isDisabled(true), refreshing(false)
{

}

UpgradeSettingsWindow::~UpgradeSettingsWindow()
{

}

bool UpgradeSettingsWindow::CreateThis(HWND hParent, u64 windowId)
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
        Sc::Upgrade::Type upgrade = (Sc::Upgrade::Type)selectedUpgrade;
        chkd.mapSettingsWindow.SetWinText("Map Settings - [" + upgradeNames.at(selectedUpgrade) + "]");

        if ( isDisabled )
            EnableUpgradeEditing();

        bool useDefaultCosts = CM->upgradeUsesDefaultCosts((Sc::Upgrade::Type)upgrade);
        checkUseDefaultCosts.SetCheck(useDefaultCosts);
        if ( useDefaultCosts )
            DisableCostEditing();
        else
            EnableCostEditing();

        u16 mineralCost = 0, mineralFactor = 0, gasCost = 0, gasFactor = 0, timeCost = 0, timeFactor = 0;
        if ( useDefaultCosts )
        {
            const Sc::Upgrade::DatEntry & upgDat = chkd.scData.upgrades.getUpgrade(upgrade);
            editMineralBaseCosts.SetEditNum<u16>(upgDat.mineralCost);
            editMineralUpgradeFactor.SetEditNum<u16>(upgDat.mineralFactor);
            editGasBaseCosts.SetEditNum<u16>(upgDat.vespeneCost);
            editGasUpgradeFactor.SetEditNum<u16>(upgDat.vespeneFactor);
            editTimeBaseCosts.SetEditNum<u16>(upgDat.timeCost/15);
            editTimeUpgradeFactor.SetEditNum<u16>(upgDat.timeFactor/15);
        }
        else
        {
            editMineralBaseCosts.SetEditNum<u16>(CM->getUpgradeBaseMineralCost((Sc::Upgrade::Type)upgrade));
            editMineralUpgradeFactor.SetEditNum<u16>(CM->getUpgradeMineralCostFactor((Sc::Upgrade::Type)upgrade));
            editGasBaseCosts.SetEditNum<u16>(CM->getUpgradeBaseGasCost((Sc::Upgrade::Type)upgrade));
            editGasUpgradeFactor.SetEditNum<u16>(CM->getUpgradeGasCostFactor((Sc::Upgrade::Type)upgrade));
            editTimeBaseCosts.SetEditNum<u16>(CM->getUpgradeBaseResearchTime((Sc::Upgrade::Type)upgrade)/15);
            editTimeUpgradeFactor.SetEditNum<u16>(CM->getUpgradeResearchTimeFactor((Sc::Upgrade::Type)upgrade)/15);
        }

        size_t defaultStartLevel = CM->getDefaultStartUpgradeLevel((Sc::Upgrade::Type)upgrade),
            defaultMaxLevel = CM->getDefaultMaxUpgradeLevel((Sc::Upgrade::Type)upgrade);
        editDefaultStartLevel.SetEditNum<size_t>(defaultStartLevel);
        editDefaultMaxLevel.SetEditNum<size_t>(defaultMaxLevel);

        for ( int player=0; player<12; player++ )
        {
            bool playerUsesDefault = CM->playerUsesDefaultUpgradeLeveling((Sc::Upgrade::Type)upgrade, player);
            checkPlayerDefault[player].SetCheck(playerUsesDefault);
            if ( playerUsesDefault )
                DisablePlayerEditing(player);
            else
            {
                EnablePlayerEditing(player);
                editPlayerStartLevel[player].SetEditNum<size_t>(CM->getStartUpgradeLevel((Sc::Upgrade::Type)upgrade, player));
                editPlayerMaxLevel[player].SetEditNum<size_t>(CM->getMaxUpgradeLevel((Sc::Upgrade::Type)upgrade, player));
            }
        }
    }
    else
        DisableUpgradeEditing();

    refreshing = false;
};

void UpgradeSettingsWindow::CreateSubWindows(HWND hWnd)
{
    treeUpgrades.CreateThis(hWnd, 5, 5, 200, 489, false, Id::TREE_UPGRADES);
    checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 120, 20, false, "Use Default Costs", Id::CHECK_USEDEFAULTCOSTS);
    buttonResetUpgradeDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Upgrades To Default", Id::BUTTON_RESETUPGRADEDEFAULTS);

    groupMineralCosts.CreateThis(hWnd, 210, 25, 377, 45, "Mineral Costs", Id::GROUP_MINERALCOSTS);
    textMineralBaseCosts.CreateThis(hWnd, 215, 45, 75, 20, "Base Cost", Id::TEXT_MINERALBASECOST);
    editMineralBaseCosts.CreateThis(hWnd, 304, 45, 84, 20, false, Id::EDIT_MINERALBASECOST);
    textMineralUpgradeFactor.CreateThis(hWnd, 408, 45, 75, 20, "Upgrade Factor", Id::TEXT_MINERALUPGRADEFACTOR);
    editMineralUpgradeFactor.CreateThis(hWnd, 497, 45, 84, 20, false, Id::EDIT_MINERALUPGRADEFACTOR);

    groupGasCosts.CreateThis(hWnd, 210, 75, 377, 45, "Gas Costs", Id::GROUP_GASCOSTS);
    textGasBaseCosts.CreateThis(hWnd, 215, 95, 75, 20, "Base Cost", Id::TEXT_GASBASECOST);
    editGasBaseCosts.CreateThis(hWnd, 304, 95, 84, 20, false, Id::EDIT_GASBASECOST);
    textGasUpgradeFactor.CreateThis(hWnd, 408, 95, 75, 20, "Upgrade Factor", Id::TEXT_GASUPGRADEFACTOR);
    editGasUpgradeFactor.CreateThis(hWnd, 497, 95, 84, 20, false, Id::EDIT_GASUPGRADEFACTOR);

    groupTimeCosts.CreateThis(hWnd, 210, 125, 377, 45, "Time Costs", Id::GROUP_TIMECOSTS);
    textTimeBaseCosts.CreateThis(hWnd, 215, 145, 75, 20, "Base Cost", Id::TEXT_TIMEBASECOST);
    editTimeBaseCosts.CreateThis(hWnd, 304, 145, 84, 20, false, Id::EDIT_TIMEBASECOST);
    textTimeUpgradeFactor.CreateThis(hWnd, 408, 145, 75, 20, "Upgrade Factor", Id::TEXT_TIMEUPGRADEFACTOR);
    editTimeUpgradeFactor.CreateThis(hWnd, 497, 145, 84, 20, false, Id::EDIT_TIMEUPGRADEFACTOR);

    groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 329, "Player Settings", Id::GROUP_PLAYERSETTINGS);
    groupDefaultSettings.CreateThis(hWnd, 215, 205, 367, 45, "Default Settings", Id::GROUP_DEFAULTSETTINGS);
    textDefaultStartLevel.CreateThis(hWnd, 220, 225, 75, 20, "Start Level", Id::TEXT_DEFAULTSTARTLEVEL);
    editDefaultStartLevel.CreateThis(hWnd, 306, 225, 81, 20, false, Id::EDIT_DEFAULTSTARTLEVEL);
    textDefaultMaxLevel.CreateThis(hWnd, 407, 225, 75, 20, "Max Level", Id::TEXT_DEFAULTMAXLEVEL);
    editDefaultMaxLevel.CreateThis(hWnd, 493, 225, 81, 20, false, Id::EDIT_DEFAULTMAXLEVEL);

    for ( int player=0; player<12; player++ )
    {
        std::stringstream ssPlayer;
        if ( player < 9 )
            ssPlayer << "Player 0" << player+1;
        else
            ssPlayer << "Player " << player+1;

        textPlayer[player].CreateThis(hWnd, 215, 260+21*player, 50, 20, ssPlayer.str(), Id::ID_TEXT_UPGRADEP1+player);
        checkPlayerDefault[player].CreateThis(hWnd, 276, 260+21*player, 52, 20, false, "Default", Id::ID_CHECK_DEFAULTUPGRADEP1+player);
        textPlayerStartLevel[player].CreateThis(hWnd, 337, 260+21*player, 60, 20, "Start Level", Id::ID_TEXT_P1STARTLEVEL+player);
        editPlayerStartLevel[player].CreateThis(hWnd, 398, 260+21*player, 56, 21, false, Id::ID_EDIT_P1STARTLEVEL+player);
        textPlayerMaxLevel[player].CreateThis(hWnd, 464, 260+21*player, 60, 20, "Max Level", Id::ID_TEXT_P1MAXLEVEL+player);
        editPlayerMaxLevel[player].CreateThis(hWnd, 525, 260+21*player, 56, 21, false, Id::ID_EDIT_P1MAXLEVEL+player);
    }

    DisableUpgradeEditing();
}

void UpgradeSettingsWindow::DisableUpgradeEditing()
{
    isDisabled = true;
    checkUseDefaultCosts.DisableThis();
    chkd.mapSettingsWindow.SetWinText("Map Settings");

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

    if ( !CM->upgradeUsesDefaultCosts((Sc::Upgrade::Type)selectedUpgrade) )
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
        Sc::Upgrade::Type upgrade = (Sc::Upgrade::Type)selectedUpgrade;
        const Sc::Upgrade::DatEntry & upgDat = chkd.scData.upgrades.getUpgrade(upgrade);
        
        CM->setUpgradeBaseMineralCost((Sc::Upgrade::Type)upgrade, upgDat.mineralCost);
        CM->setUpgradeMineralCostFactor((Sc::Upgrade::Type)upgrade, upgDat.mineralFactor);
        CM->setUpgradeBaseGasCost((Sc::Upgrade::Type)upgrade, upgDat.vespeneCost);
        CM->setUpgradeGasCostFactor((Sc::Upgrade::Type)upgrade, upgDat.vespeneFactor);
        CM->setUpgradeBaseResearchTime((Sc::Upgrade::Type)upgrade, upgDat.timeCost);
        CM->setUpgradeResearchTimeFactor((Sc::Upgrade::Type)upgrade, upgDat.timeFactor);
    }
}

void UpgradeSettingsWindow::ClearDefaultUpgradeCosts()
{
    if ( selectedUpgrade != -1 && selectedUpgrade < 61 && CM != nullptr )
    {
        u8 upgrade = (u8)selectedUpgrade;

        CM->setUpgradeBaseMineralCost((Sc::Upgrade::Type)upgrade, 0);
        CM->setUpgradeMineralCostFactor((Sc::Upgrade::Type)upgrade, 0);
        CM->setUpgradeBaseGasCost((Sc::Upgrade::Type)upgrade, 0);
        CM->setUpgradeGasCostFactor((Sc::Upgrade::Type)upgrade, 0);
        CM->setUpgradeBaseResearchTime((Sc::Upgrade::Type)upgrade, 0);
        CM->setUpgradeResearchTimeFactor((Sc::Upgrade::Type)upgrade, 0);
    }
}

LRESULT UpgradeSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( idFrom == Id::TREE_UPGRADES &&
        refreshing == false &&
        nmhdr->code == TVN_SELCHANGED &&
        ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeTypePortion,
            itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeDataPortion;

        u8 upgradeId = (u8)itemData;
        if ( itemType == TreeTypeUpgrade && upgradeId < 61 )
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
        case Id::EDIT_MINERALBASECOST:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newMineralCost;
                if ( editMineralBaseCosts.GetEditNum<u16>(newMineralCost) )
                {
                    CM->setUpgradeBaseMineralCost((Sc::Upgrade::Type)selectedUpgrade, newMineralCost);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_MINERALUPGRADEFACTOR:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newMineralFactor;
                if ( editMineralUpgradeFactor.GetEditNum<u16>(newMineralFactor) )
                {
                    CM->setUpgradeMineralCostFactor((Sc::Upgrade::Type)selectedUpgrade, newMineralFactor);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_GASBASECOST:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newGasCost;
                if ( editGasBaseCosts.GetEditNum<u16>(newGasCost) )
                {
                    CM->setUpgradeBaseGasCost((Sc::Upgrade::Type)selectedUpgrade, newGasCost);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_GASUPGRADEFACTOR:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newGasFactor;
                if ( editGasUpgradeFactor.GetEditNum<u16>(newGasFactor) )
                {
                    CM->setUpgradeGasCostFactor((Sc::Upgrade::Type)selectedUpgrade, newGasFactor);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_TIMEBASECOST:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newTimeCost;
                if ( editTimeBaseCosts.GetEditNum<u16>(newTimeCost) )
                {
                    if ( newTimeCost * 15 < newTimeCost ) // Value overflow
                        CM->setUpgradeBaseResearchTime((Sc::Upgrade::Type)selectedUpgrade, 65535); // Set to max
                    else // Normal
                        CM->setUpgradeBaseResearchTime((Sc::Upgrade::Type)selectedUpgrade, newTimeCost * 15);

                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_TIMEUPGRADEFACTOR:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u16 newTimeFactor;
                if ( editTimeUpgradeFactor.GetEditNum<u16>(newTimeFactor) )
                {
                    if ( newTimeFactor * 15 < newTimeFactor ) // Value overflow
                        CM->setUpgradeResearchTimeFactor((Sc::Upgrade::Type)selectedUpgrade, 65535); // Set to max
                    else // Normal
                        CM->setUpgradeResearchTimeFactor((Sc::Upgrade::Type)selectedUpgrade, newTimeFactor * 15); // Set to max

                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_DEFAULTSTARTLEVEL:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u8 newStartLevel;
                if ( editDefaultStartLevel.GetEditNum<u8>(newStartLevel) )
                {
                    CM->setDefaultStartUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, newStartLevel);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::EDIT_DEFAULTMAXLEVEL:
            if ( HIWORD(wParam) == EN_CHANGE )
            {
                u8 newMaxLevel;
                if ( editDefaultMaxLevel.GetEditNum<u8>(newMaxLevel) )
                {
                    CM->setDefaultMaxUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, newMaxLevel);
                    CM->notifyChange(false);
                }
            }
            break;

        case Id::CHECK_USEDEFAULTCOSTS:
            if ( HIWORD(wParam) == BN_CLICKED )
            {
                LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                if ( selectedUpgrade != -1 )
                {
                    if ( state == BST_CHECKED )
                    {
                        CM->setUpgradeUsesDefaultCosts((Sc::Upgrade::Type)selectedUpgrade, true);
                        ClearDefaultUpgradeCosts();
                        DisableCostEditing();
                    }
                    else
                    {
                        CM->setUpgradeUsesDefaultCosts((Sc::Upgrade::Type)selectedUpgrade, false);
                        SetDefaultUpgradeCosts();
                        EnableCostEditing();
                    }

                    RefreshWindow();
                    CM->notifyChange(false);
                }
            }
            break;
        case Id::BUTTON_RESETUPGRADEDEFAULTS:
            if ( HIWORD(wParam) == BN_CLICKED )
            {
                if ( WinLib::GetYesNo("Are you sure you want to reset all ugprade settings?", "Confirm") == WinLib::PromptResult::Yes )
                {
                    CM->setUpgradesToDefault();
                    CM->notifyChange(false);
                    DisableCostEditing();
                    RefreshWindow();
                }
            }
            break;
        default:
            if ( HIWORD(wParam) == BN_CLICKED && selectedUpgrade != -1 &&
                LOWORD(wParam) >= Id::ID_CHECK_DEFAULTUPGRADEP1 && LOWORD(wParam) <= Id::ID_CHECK_DEFAULTUPGRADEP12 )
            {
                LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                int player = LOWORD(wParam) - Id::ID_CHECK_DEFAULTUPGRADEP1;
                bool useDefault = (state == BST_CHECKED);
                CM->setPlayerUsesDefaultUpgradeLeveling((Sc::Upgrade::Type)selectedUpgrade, player, useDefault);
                if ( state != BST_CHECKED )
                {
                    size_t defaultStartLevel = CM->getDefaultStartUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade),
                        defaultMaxLevel = CM->getDefaultStartUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade);
                    
                    CM->setStartUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, player, defaultStartLevel);
                    CM->setMaxUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, player, defaultMaxLevel);
                }
                CM->notifyChange(false);
                RefreshWindow();
            }
            else if ( HIWORD(wParam) == EN_CHANGE && selectedUpgrade != -1 &&
                      LOWORD(wParam) >= Id::ID_EDIT_P1STARTLEVEL && LOWORD(wParam) <= Id::ID_EDIT_P12STARTLEVEL )
            {
                u8 player = u8(LOWORD(wParam) - Id::ID_EDIT_P1STARTLEVEL),
                    newStartLevel;
                if ( editPlayerStartLevel[player].GetEditNum<u8>(newStartLevel) )
                {
                    CM->setStartUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, player, newStartLevel);
                    CM->notifyChange(false);
                }
            }
            else if ( HIWORD(wParam) == EN_CHANGE && selectedUpgrade != -1 &&
                LOWORD(wParam) >= Id::ID_EDIT_P1MAXLEVEL && LOWORD(wParam) <= Id::ID_EDIT_P12MAXLEVEL )
            {
                u8 player = u8(LOWORD(wParam) - Id::ID_EDIT_P1MAXLEVEL),
                    newMaxLevel;
                if ( editPlayerMaxLevel[player].GetEditNum<u8>(newMaxLevel) )
                {
                    CM->setMaxUpgradeLevel((Sc::Upgrade::Type)selectedUpgrade, player, newMaxLevel);
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
                    chkd.mapSettingsWindow.SetWinText("Map Settings - [" + upgradeNames.at(selectedUpgrade) + "]");
                else
                    chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
