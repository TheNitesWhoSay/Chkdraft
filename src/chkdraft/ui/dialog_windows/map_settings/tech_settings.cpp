#include "tech_settings.h"
#include "chkdraft/chkdraft.h"
#include <string>

enum_t(Id, u32, {
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
});

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

bool TechSettingsWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    this->selectedTech = -1;
    this->refreshing = false;
    this->isDisabled = true;
    return true;
}

void TechSettingsWindow::RefreshWindow()
{
    refreshing = true;
    if ( selectedTech >= 0 && selectedTech < 44 && CM != nullptr )
    {
        Sc::Tech::Type tech = (Sc::Tech::Type)selectedTech;
        if ( selectedTech != -1 )
            chkd.mapSettingsWindow->SetWinText("Map Settings - [" + techNames.at(selectedTech) + "]");

        if ( isDisabled )
            EnableTechEditing();

        bool techUsesDefaultCosts = CM->techUsesDefaultSettings((Sc::Tech::Type)tech);
        checkUseDefaultCosts.SetCheck(techUsesDefaultCosts);
        if ( techUsesDefaultCosts )
        {
            editMineralCosts.SetEditNum<u16>(chkd.scData.techs.getTech(tech).mineralCost);
            editGasCosts.SetEditNum<u16>(chkd.scData.techs.getTech(tech).vespeneCost);
            editTimeCosts.SetEditNum<u16>(chkd.scData.techs.getTech(tech).researchTime/15);
            editEnergyCosts.SetEditNum<u16>(chkd.scData.techs.getTech(tech).energyCost);
            DisableTechCosts();
        }
        else
        {
            EnableTechCosts();
            editMineralCosts.SetEditNum<u16>(CM->getTechMineralCost((Sc::Tech::Type)tech));
            editGasCosts.SetEditNum<u16>(CM->getTechGasCost((Sc::Tech::Type)tech));
            editTimeCosts.SetEditNum<u16>(CM->getTechResearchTime((Sc::Tech::Type)tech)/15);
            editEnergyCosts.SetEditNum<u16>(CM->getTechEnergyCost((Sc::Tech::Type)tech));
        }

        bool enabledByDefault = CM->techDefaultAvailable((Sc::Tech::Type)tech),
             researchedByDefault = CM->techDefaultResearched((Sc::Tech::Type)tech);
        
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
            checkUsePlayerDefaults[player].SetCheck(CM->playerUsesDefaultTechSettings((Sc::Tech::Type)tech, player));
            
            bool playerUsesDefaults = CM->playerUsesDefaultTechSettings((Sc::Tech::Type)tech, player),
                playerEnabledByDefault = CM->techAvailable((Sc::Tech::Type)tech, player),
                playerResearchedByDefault = CM->techResearched((Sc::Tech::Type)tech, player);

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
    treeTechs.CreateThis(hWnd, 5, 5, 200, 489, false, Id::TREE_TECHS);
    checkUseDefaultCosts.CreateThis(hWnd, 210, 5, 110, 20, false, "Use Default Costs", Id::CHECK_DEFAULTTECHCOSTS);
    buttonResetTechDefaults.CreateThis(hWnd, 5, 494, 200, 25, "Reset All Techs To Default", Id::BUTTON_RESETTECHDEFAULTS);

    groupTechCosts.CreateThis(hWnd, 210, 25, 377, 65, "Tech Costs", Id::GROUP_TECHCOSTS);
    textMineralCosts.CreateThis(hWnd, 215, 45, 75, 20, "Mineral", Id::TEXT_MINERALCOSTS);
    editMineralCosts.CreateThis(hWnd, 304, 45, 84, 20, false, Id::EDIT_MINERALCOSTS);
    textGasCosts.CreateThis(hWnd, 408, 45, 75, 20, "Gas", Id::TEXT_GASCOSTS);
    editGasCosts.CreateThis(hWnd, 497, 45, 84, 20, false, Id::EDIT_GASCOSTS);
    textTimeCosts.CreateThis(hWnd, 215, 65, 75, 20, "Time", Id::TEXT_TIMECOSTS);
    editTimeCosts.CreateThis(hWnd, 304, 65, 84, 20, false, Id::EDIT_TIMECOSTS);
    textEnergyCosts.CreateThis(hWnd, 408, 65, 75, 20, "Energy", Id::TEXT_ENERGYCOSTS);
    editEnergyCosts.CreateThis(hWnd, 497, 65, 84, 20, false, Id::EDIT_ENERGYCOSTS);

    groupDefaultPlayerSettings.CreateThis(hWnd, 210, 105, 377, 80, "Default Player Setting", Id::GROUP_DEFAULTPLAYERSETTINGS);
    radioDisabledByDefault.CreateThis(hWnd, 215, 120, 110, 20, "Disabled by default", Id::RADIO_DEFAULTDISABLED);
    radioEnabledByDefault.CreateThis(hWnd, 215, 140, 110, 20, "Enabled by default", Id::RADIO_DEFAULTENABLED);
    radioResearchedByDefault.CreateThis(hWnd, 215, 160, 130, 20, "Researched by default", Id::RADIO_DEFAULTRESEARCHED);

    groupPlayerSettings.CreateThis(hWnd, 210, 190, 377, 265, "Player Settings", Id::GROUP_TECHPLAYERSETTINGS);

    const std::vector<std::string> playerTechSettings = { "Disabled", "Enabled", "Researched" };
    for ( int player=0; player<12; player++ )
    {
        std::stringstream ssPlayerTech;
        ssPlayerTech << "Use Default for Player ";
        if ( player < 9 )
            ssPlayerTech << "0" << player+1;
        else
            ssPlayerTech << player+1;

        checkUsePlayerDefaults[player].CreateThis(hWnd, 215, 210+20*player, 150, 20, false, ssPlayerTech.str(), Id::CHECK_P1TECHDEFAULT+player);
        dropPlayerTechSettings[player].CreateThis(hWnd, 460, 210+20*player, 120, 100, false, false, Id::DROP_P1TECHSETTINGS+player, playerTechSettings);
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
    chkd.mapSettingsWindow->SetWinText("Map Settings");
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

    if ( selectedTech >= 0 && selectedTech < Sc::Tech::TotalTypes && !CM->techUsesDefaultSettings((Sc::Tech::Type)selectedTech) )
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
        Sc::Tech::Type tech = (Sc::Tech::Type)selectedTech;
        const Sc::Tech::DatEntry & techDat = chkd.scData.techs.getTech(tech);

        CM->setTechMineralCost((Sc::Tech::Type)tech, techDat.mineralCost);
        CM->setTechGasCost((Sc::Tech::Type)tech, techDat.vespeneCost);
        CM->setTechResearchTime((Sc::Tech::Type)tech, techDat.researchTime);
        CM->setTechEnergyCost((Sc::Tech::Type)tech, techDat.energyCost);
    }
}

void TechSettingsWindow::ClearDefaultTechCosts()
{
    if ( selectedTech > 0 && selectedTech < 44 && CM != nullptr )
    {
        u8 tech = (u8)selectedTech;

        CM->setTechMineralCost((Sc::Tech::Type)tech, 0);
        CM->setTechGasCost((Sc::Tech::Type)tech, 0);
        CM->setTechResearchTime((Sc::Tech::Type)tech, 0);
        CM->setTechEnergyCost((Sc::Tech::Type)tech, 0);
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
        switch ( LOWORD(wParam) )
        {
        case Id::BUTTON_RESETTECHDEFAULTS:
            if ( HIWORD(wParam) == BN_CLICKED )
            {
                if ( WinLib::GetYesNo("Are you sure you want to reset all tech settings?", "Confirm") == WinLib::PromptResult::Yes )
                {
                    CM->setTechsToDefault();
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
                    CM->setTechUsesDefaultSettings((Sc::Tech::Type)selectedTech, true);
                }
                else
                {
                    SetDefaultTechCosts();
                    EnableTechCosts();
                    CM->setTechUsesDefaultSettings((Sc::Tech::Type)selectedTech, false);
                }

                RefreshWindow();
            }
            break;
        case Id::EDIT_MINERALCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newMineralCost;
                if ( editMineralCosts.GetEditNum<u16>(newMineralCost) )
                    CM->setTechMineralCost((Sc::Tech::Type)selectedTech, newMineralCost);
            }
            break;
        case Id::EDIT_GASCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newGasCost;
                if ( editGasCosts.GetEditNum<u16>(newGasCost) )
                    CM->setTechGasCost((Sc::Tech::Type)selectedTech, newGasCost);
            }
            break;
        case Id::EDIT_TIMECOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newTimeCost;
                if ( editTimeCosts.GetEditNum<u16>(newTimeCost) )
                {
                    if ( 15 * (u32)newTimeCost > 65535 ) // Overflow
                        CM->setTechResearchTime((Sc::Tech::Type)selectedTech, 65535); // Set to max
                    else // Normal
                        CM->setTechResearchTime((Sc::Tech::Type)selectedTech, newTimeCost * 15);
                }
            }
            break;
        case Id::EDIT_ENERGYCOSTS:
            if ( HIWORD(wParam) == EN_CHANGE && selectedTech != -1 )
            {
                u16 newEnergyCost;
                if ( editEnergyCosts.GetEditNum<u16>(newEnergyCost) )
                    CM->setTechEnergyCost((Sc::Tech::Type)selectedTech, newEnergyCost);
            }
            break;
        case Id::RADIO_DEFAULTDISABLED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
            {
                CM->setDefaultTechAvailable((Sc::Tech::Type)selectedTech, false);
                CM->setTechUsesDefaultSettings((Sc::Tech::Type)selectedTech, false);
                RefreshWindow();
            }
            break;
        case Id::RADIO_DEFAULTENABLED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
            {
                CM->setDefaultTechAvailable((Sc::Tech::Type)selectedTech, true);
                CM->setDefaultTechResearched((Sc::Tech::Type)selectedTech, false);
                RefreshWindow();
            }
            break;
        case Id::RADIO_DEFAULTRESEARCHED:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED )
            {
                CM->setDefaultTechResearched((Sc::Tech::Type)selectedTech, true);
                CM->setDefaultTechAvailable((Sc::Tech::Type)selectedTech, true);
                RefreshWindow();
            }
            break;
        default:
            if ( HIWORD(wParam) == BN_CLICKED && selectedTech != -1 &&
                LOWORD(wParam) >= Id::CHECK_P1TECHDEFAULT && LOWORD(wParam) <= Id::CHECK_P12TECHDEFAULT )
            {
                u8 player = u8(LOWORD(wParam) - Id::CHECK_P1TECHDEFAULT);
                bool useDefault = (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED);
                if ( useDefault )
                    dropPlayerTechSettings[player].DisableThis();
                else
                    dropPlayerTechSettings[player].EnableThis();
                CM->setPlayerUsesDefaultTechSettings((Sc::Tech::Type)selectedTech, player, useDefault);
                RefreshWindow();
            }
            if ( HIWORD(wParam) == CBN_SELCHANGE && selectedTech != -1 &&
                LOWORD(wParam) >= Id::DROP_P1TECHSETTINGS && LOWORD(wParam) <= Id::DROP_P12TECHSETTINGS )
            {
                auto edit = CM->operator()(ActionDescriptor::SetTechResearched);
                u8 player = u8(LOWORD(wParam) - Id::DROP_P1TECHSETTINGS);
                int selection = dropPlayerTechSettings[player].GetSel();
                if ( selection == 0 ) // Disabled
                {
                    CM->setTechAvailable((Sc::Tech::Type)selectedTech, player, false);
                    CM->setTechResearched((Sc::Tech::Type)selectedTech, player, false);
                }
                else if ( selection == 1 ) // Enabled
                {
                    CM->setTechAvailable((Sc::Tech::Type)selectedTech, player, true);
                    CM->setTechResearched((Sc::Tech::Type)selectedTech, player, false);
                }
                else if ( selection == 2 ) // Researched
                {
                    CM->setTechAvailable((Sc::Tech::Type)selectedTech, player, true);
                    CM->setTechResearched((Sc::Tech::Type)selectedTech, player, true);
                }
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
                    chkd.mapSettingsWindow->SetWinText("Map Settings - [" + techNames.at(selectedTech) + "]");
                else
                    chkd.mapSettingsWindow->SetWinText("Map Settings");
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
