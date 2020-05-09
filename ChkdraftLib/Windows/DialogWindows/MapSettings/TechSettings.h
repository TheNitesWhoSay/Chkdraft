#ifndef TECHSETTINGS_H
#define TECHSETTINGS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/TechTree.h"

/**
    TECS/TECx - tech uses default costs & all costs
    PTEC/PTEx - tech availability (global and player based)
*/

class TechSettingsWindow : public WinLib::ClassWindow
{
    public:
        TechSettingsWindow();
        virtual ~TechSettingsWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hWnd);
        void DisableTechCosts();
        void EnableTechCosts();
        void DisableTechEditing();
        void EnableTechEditing();
        void SetDefaultTechCosts();
        void ClearDefaultTechCosts();
        LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        s32 selectedTech;
        bool refreshing;
        bool isDisabled;

        TechTree treeTechs;

        WinLib::CheckBoxControl checkUseDefaultCosts;
        WinLib::ButtonControl buttonResetTechDefaults;

        WinLib::GroupBoxControl groupTechCosts;
        WinLib::TextControl textMineralCosts;
        WinLib::EditControl editMineralCosts;
        WinLib::TextControl textGasCosts;
        WinLib::EditControl editGasCosts;
        WinLib::TextControl textTimeCosts;
        WinLib::EditControl editTimeCosts;
        WinLib::TextControl textEnergyCosts;
        WinLib::EditControl editEnergyCosts;

        WinLib::GroupBoxControl groupDefaultPlayerSettings;
        WinLib::RadioButtonControl radioDisabledByDefault;
        WinLib::RadioButtonControl radioEnabledByDefault;
        WinLib::RadioButtonControl radioResearchedByDefault;

        WinLib::GroupBoxControl groupPlayerSettings;
        WinLib::CheckBoxControl checkUsePlayerDefaults[12];
        WinLib::DropdownControl dropPlayerTechSettings[12];
};

#endif