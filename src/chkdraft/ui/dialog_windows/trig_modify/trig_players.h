#ifndef TRIGPLAYERS_H
#define TRIGPLAYERS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

class TrigPlayersWindow : public WinLib::ClassWindow
{
    public:
        TrigPlayersWindow();
        virtual ~TrigPlayersWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow(u32 trigIndex);
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);
        void CheckBoxUpdated(u16 checkId);
        void OnLeave();
        void ParseRawPlayers();
        void ToggleAdvancedMode();
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool refreshing;
        u32 trigIndex;

        WinLib::GroupBoxControl groupExecutingPlayers;
        WinLib::CheckBoxControl checkMainPlayers[8];
        WinLib::CheckBoxControl checkForces[4];
        WinLib::CheckBoxControl checkAllPlayers;
        WinLib::TextControl textPlayerStats;

        WinLib::GroupBoxControl groupNonExecutingPlayers;
        WinLib::CheckBoxControl checkNonExecutingPlayers[14]; // Starting at p9//ID:8, Skip ID:17-21, Ending at ID:26

        WinLib::GroupBoxControl groupRawEdit;
        WinLib::CheckBoxControl checkAllowRawEdit;
        WinLib::TextControl textExtendedData;
        WinLib::EditControl editRawPlayers; // 56 hex characters // 28 bytes
        WinLib::ButtonControl buttonAdvanced;
        bool advancedMode;
};

#endif