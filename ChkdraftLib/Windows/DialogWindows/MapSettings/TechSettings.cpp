#include "TechSettings.h"
#include "../../../Chkdraft.h"
#include <string>

enum class Id {
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

TechSettingsWindow::~TechSettingsWindow()
{

}

bool TechSettingsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TechSettings", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "TechSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
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
    if ( selectedTech >= 0 && selectedTech < 44 && CM != nullptr )
    {
        u8 tech = (u8)selectedTech;
        if ( selectedTech != -1 )
            chkd.mapSettingsWindow.SetWinText("Map Settings - [" + techNames.at(selectedTech) + "]");

        if ( isDisabled )
            EnableTechEditing();

        bool techUsesDefaultCosts = CM->techUsesDefaultCosts(tech);
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
            if ( CM->getTechMineralCost(tech, mineralCost) )
                editMineralCosts.SetEditNum<u16>(mineralCost);
            if ( CM->getTechGasCost(tech, gasCost) )
                editGasCosts.SetEditNum<u16>(gasCost);
            if ( CM->getTechTimeCost(tech, timeCost) )
                editTimeCosts.SetEditNum<u16>(timeCost/15);
            if ( CM->getTechEnergyCost(tech, energyCost) )
                editEnergyCosts.SetEditNum<u16>(energyCost);
        }

        bool enabledByDefault = CM->techAvailableByDefault(tech),
             researchedByDefault = CM->techResearchedByDefault(tech);
        
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
            checkUsePlayerDefaults[player].SetCheck(CM->playerUsesDefaultTechSettings(tech, player));
            
            bool playerUsesDefaults = CM->playerUsesDefaultTechSettings(tech, player),
                 playerEnabledByDefault = CM->techAvailableForPlayer(tech, player),
                 playerResearchedByDefault = CM->techResearchedForPlayer(tech, player);

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
    treeTechs.CreateThis(hWnd, 5, 5, 200, 489, false, (u64)Id::TREE_TECHS);
    checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 110, 20, false, "Use Default Costs", (u64)Id::CHECK_DEFAULTTECHCOSTS);
    buttonResetTechDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Techs To Default", (u64)Id::BUTTON_RESETTECHDEFAULTS);

    groupTechCosts.CreateThis(hWnd, 210, 25, 377, 65, "Tech Costs", (u64)Id::GROUP_TECHCOSTS);
    textMineralCosts.CreateThis(hWnd, 215, 45, 75, 20, "Mineral", (u64)Id::TEXT_MINERALCOSTS);
    editMineralCosts.CreateThis(hWnd, 304, 45, 84, 20, false, (u64)Id::EDIT_MINERALCOSTS);
    textGasCosts.CreateThis(hWnd, 408, 45, 75, 20, "Gas", (u64)Id::TEXT_GASCOSTS);
    editGasCosts.CreateThis(hWnd, 497, 45, 84, 20, false, (u64)Id::EDIT_GASCOSTS);
    textTimeCosts.CreateThis(hWnd, 215, 65, 75, 20, "Time", (u64)Id::TEXT_TIMECOSTS);
    editTimeCosts.CreateThis(hWnd, 304, 65, 84, 20, false, (u64)Id::EDIT_TIMECOSTS);
    textEnergyCosts.CreateThis(hWnd, 408, 65, 75, 20, "Energy", (u64)Id::TEXT_ENERGYCOSTS);
    editEnergyCosts.CreateThis(hWnd, 497, 65, 84, 20, false, (u64)Id::EDIT_ENERGYCOSTS);

    groupDefaultPlayerSettings.CreateThis(hWnd, 210, 105, 377, 80, "Default Player Setting", (u64)Id::GROUP_DEFAULTPLAYERSETTINGS);
    radioDisabledByDefault.CreateThis(hWnd, 215, 120, 110, 20, "Disabled by default", (u64)Id::RADIO_DEFAULTDISABLED);
    radioEnabledByDefault.CreateThis(hWnd, 215, 140, 110, 20, "Enabled by default", (u64)Id::RADIO_DEFAULTENABLED);
    radioResearchedByDefault.CreateThis(hWnd, 215, 160, 130, 20, "Researched by default", (u64)Id::RADIO_DEFAULTRESEARCHED);

    groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 265, "Player Settings", (u64)Id::GROUP_TECHPLAYERSETTINGS);

    const std::vector<std::string> playerTechSettings = { "Disabled", "Enabled", "Researched" };
    for ( int player=0; player<12; player++ )
    {
        std::stringstream ssPlayerTech;
        ssPlayerTech << "Use Default for Player ";
        if ( player < 9 )
            ssPlayerTech << "0" << player+1;
        else
            ssPlayerTech << player+1;

        checkUsePlayerDefaults[player].CreateThis(hWnd, 215, 210+20*player, 150, 20, false, ssPlayerTech.str(), (u64)Id::CHECK_P1TECHDEFAULT+player);
        dropPlayerTechSettings[player].CreateThis(hWnd, 460, 210+20*player, 120, 100, false, false, (u64)Id::DROP_P1TECHSETTINGS+player, playerTechSettings, defaultFont);
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
    chkd.mapSettingsWindow.SetWinText("Map Settings");
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

    if ( CM->techUsesDefaultCosts((u8)selectedTech) == false )
        EnableTechCosts();

    groupDefaultPlayerSettings.EnableThis();
    radioDisabledByDefault.EnableThis();
    radioEnabledByDefault.EnableThis();
    radioResearchedByDefault.EnableThis();

    groupPlayerSettings.EnableThis();
    for ( int i=0; i<12; i++ )
    {
        checkUsePlayerDefaults[i].EnableThis();
        if ( SendMessage(checkUsePlayerDefaults[i].getHandle(), BM_GETCHECK, 0, 0) == BST_UNCHECKED )
            dropPlayerTechSettings[i].EnableThis();
    }
    isDisabled = false;
}

void TechSettingsWindow::SetDefaultTechCosts()
{
    if ( selectedTech > 0 && selectedTech < 44 && CM != nullptr )
    {
        u8 tech = (u8)selectedTech;
        TECHDAT* techDat = chkd.scData.techs.TechDat(tech);

        CM->setTechMineralCost(tech, techDat->MineralCost);
        CM->setTechGasCost(tech, techDat->VespeneCost);
        CM->setTechTimeCost(tech, techDat->ResearchTime);
        CM->setTechEnergyCost(tech, techDat->EnergyCost);
    }
}

void TechSettingsWindow::ClearDefaultTechCosts()
{
    if ( selectedTech > 0 && selectedTech < 44 && CM != nullptr )
    {
        u8 tech = (u8)selectedTech;

        CM->setTechMineralCost(tech, 0);
        CM->setTechGasCost(tech, 0);
        CM->setTechTimeCost(tech, 0);
        CM->setTechEnergyCost(tech, 0);
    }
}

LRESULT TechSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( refreshing == false && nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeTypePortion,
            itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeDataPortion;

        u16 techId = (u16)itemData;
        if ( itemType == TreeTypeTech && techId < 44 )
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
        switch ( (Id)LOWORD(wParam) )
        {
        case Id::BUTTON_RESETTECHDEFAULTS:
            if ( HIWORD(wParam) == BN_CLICKED )
            {
                if ( WinLib::GetYesNo("Are you sure you want to reset all tech settings?", "Confirm") == WinLib::PromptResult::Yes )
                {
                    Section newTECS = TecsSection::GetDefault(), newTECx = TecxSection::GetDefault(), newPTEC = PtecSection::GetDefault(), newPTEx = PtexSection::GetDefault();
                    if ( newTECS != nullptr && newTECx != nullptr && newPTEC != nullptr && newPTEx != nullptr )
                    {
                        CM->AddOrReplaceSection(newTECS);
                        CM->AddOrReplaceSection(newTECx);
                        CM->AddOrReplaceSection(newPTEC);
                        CM->AddOrReplaceSection(newPTEx);
                    }

                    CM->notifyChange(false);
                    RefreshWindow();
                }
            }
            break;

        case Id::CHECK_DEFAULTTECHCOSTS:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 )
            {
                LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                if ( state == BST_CHECKED )
                {
                    ClearDefaultTechCosts();
                    DisableTechCosts();
                    CM->setTechUseDefaults((u8)selectedTech, true);
                }
                else
                {
                    SetDefaultTechCosts();
                    EnableTechCosts();
                    CM->setTechUseDefaults((u8)selectedTech, false);
                }

                CM->notifyChange(false);
                RefreshWindow();
            }
            break;
        case Id::EDIT_MINERALCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newMineralCost;
                if ( editMineralCosts.GetEditNum<u16>(newMineralCost) )
                {
                    CM->setTechMineralCost((u8)selectedTech, newMineralCost);
                    CM->notifyChange(false);
                }
            }
            break;
        case Id::EDIT_GASCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newGasCost;
                if ( editGasCosts.GetEditNum<u16>(newGasCost) )
                {
                    CM->setTechGasCost((u8)selectedTech, newGasCost);
                    CM->notifyChange(false);
                }
            }
            break;
        case Id::EDIT_TIMECOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newTimeCost;
                if ( editTimeCosts.GetEditNum<u16>(newTimeCost) )
                {
                    if ( 15 * (u32)newTimeCost > 65535 ) // Overflow
                        CM->setTechTimeCost((u8)selectedTech, 65535); // Set to max
                    else // Normal
                        CM->setTechTimeCost((u8)selectedTech, newTimeCost * 15);

                    CM->notifyChange(false);
                }
            }
            break;
        case Id::EDIT_ENERGYCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newEnergyCost;
                if ( editEnergyCosts.GetEditNum<u16>(newEnergyCost) )
                {
                    CM->setTechEnergyCost((u8)selectedTech, newEnergyCost);
                    CM->notifyChange(false);
                }
            }
            break;
        case Id::RADIO_DEFAULTDISABLED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED &&
                CM->setTechDefaultAvailability((u8)selectedTech, false) )
            {
                CM->setTechDefaultResearched((u8)selectedTech, false);
                CM->notifyChange(false);
                RefreshWindow();
            }
            break;
        case Id::RADIO_DEFAULTENABLED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED &&
                CM->setTechDefaultAvailability((u8)selectedTech, true) )
            {
                CM->setTechDefaultResearched((u8)selectedTech, false);
                CM->notifyChange(false);
                RefreshWindow();
            }
            break;
        case Id::RADIO_DEFAULTRESEARCHED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED &&
                CM->setTechDefaultResearched((u8)selectedTech, true) )
            {
                CM->setTechDefaultAvailability((u8)selectedTech, true);
                CM->notifyChange(false);
                RefreshWindow();
            }
            break;
        default:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                LOWORD(wParam) >= (WORD)Id::CHECK_P1TECHDEFAULT && LOWORD(wParam) <= (WORD)Id::CHECK_P12TECHDEFAULT )
            {
                u8 player = (u8)(LOWORD(wParam) - (WORD)Id::CHECK_P1TECHDEFAULT);
                bool useDefault = (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED);
                if ( useDefault )
                    dropPlayerTechSettings[player].DisableThis();
                else
                    dropPlayerTechSettings[player].EnableThis();
                CM->setPlayerUsesDefaultTechSettings((u8)selectedTech, player, useDefault);
                CM->notifyChange(false);
                RefreshWindow();
            }
            if ( HIWORD(wParam) == CBN_SELCHANGE && selectedTech != -1 &&
                LOWORD(wParam) >= (WORD)Id::DROP_P1TECHSETTINGS && LOWORD(wParam) <= (WORD)Id::DROP_P12TECHSETTINGS )
            {
                u8 player = (u8)(LOWORD(wParam) - (WORD)Id::DROP_P1TECHSETTINGS);
                int selection = dropPlayerTechSettings[player].GetSel();
                if ( selection == 0 ) // Disabled
                {
                    CM->setTechAvailableForPlayer((u8)selectedTech, player, false);
                    CM->setTechResearchedForPlayer((u8)selectedTech, player, false);
                }
                else if ( selection == 1 ) // Enabled
                {
                    CM->setTechAvailableForPlayer((u8)selectedTech, player, true);
                    CM->setTechResearchedForPlayer((u8)selectedTech, player, false);
                }
                else if ( selection == 2 ) // Researched
                {
                    CM->setTechAvailableForPlayer((u8)selectedTech, player, true);
                    CM->setTechResearchedForPlayer((u8)selectedTech, player, true);
                }

                CM->notifyChange(false);
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
                    chkd.mapSettingsWindow.SetWinText("Map Settings - [" + techNames.at(selectedTech) + "]");
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
