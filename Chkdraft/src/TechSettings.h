#ifndef TECHSETTINGS_H
#define TECHSETTINGS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "TechTree.h"

/**
    TECS/TECx - tech uses default costs & all costs
    PTEC/PTEx - tech availability (global and player based)
*/

class TechSettingsWindow : public ClassWindow
{
    public:
        TechSettingsWindow();
        bool CreateThis(HWND hParent, u32 windowId);
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

        CheckBoxControl checkUseDefaultCosts;
        ButtonControl buttonResetTechDefaults;

        GroupBoxControl groupTechCosts;
        TextControl textMineralCosts;
        EditControl editMineralCosts;
        TextControl textGasCosts;
        EditControl editGasCosts;
        TextControl textTimeCosts;
        EditControl editTimeCosts;
        TextControl textEnergyCosts;
        EditControl editEnergyCosts;

        GroupBoxControl groupDefaultPlayerSettings;
        RadioButtonControl radioDisabledByDefault;
        RadioButtonControl radioEnabledByDefault;
        RadioButtonControl radioResearchedByDefault;

        GroupBoxControl groupPlayerSettings;
        CheckBoxControl checkUsePlayerDefaults[12];
        DropdownControl dropPlayerTechSettings[12];
};

#endif
