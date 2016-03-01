#ifndef TRIGACTIONS_H
#define TRIGACTIONS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "CndActGrid.h"
#include <string>
#include <vector>

class TrigActionsWindow : public ClassWindow, public ICndActGridUser
{
    public:
        TrigActionsWindow();
        bool CreateThis(HWND hParent, u32 windowId);
        bool DestroyThis();
        void RefreshWindow(u32 trigIndex);
        void DoSize();
        void ProcessKeyDown(WPARAM wParam, LPARAM lParam);
        void HideSuggestions();
        void CndActEnableToggled(u8 actionNum);
        virtual void CutSelection();
        virtual void CopySelection();
        virtual void Paste();
        virtual void RedrawThis();

    protected:
        void InitializeArgMaps(); // Gives default values to all the argMaps
        void CreateSubWindows(HWND hWnd);
        void OnLeave();
        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        CndActGrid gridActions;
        EditControl editListItem;
        ButtonControl buttonEditWav;
        ButtonControl buttonEditString;
        HBRUSH hBlack;
        u32 trigIndex;
        bool stringEditEnabled;
        bool wavEditEnabled;

        std::vector<u8> actionArgMaps[64];

        Suggestions &suggestions;

        LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void ChangeActionType(Action &action, ActionId newId);
        bool TransformAction(Action &action, ActionId newId);
        void ClearArgument(Action &action, u8 argNum);
        void UpdateActionName(u8 actionNum, const std::string &newText);
        void UpdateActionArg(u8 actionNum, u8 argNum, const std::string &newText);
        BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string& str);
        BOOL GridItemDeleting(u16 gridItemX, u16 gridItemY);
        void DrawSelectedAction();
        int GetGridItemWidth(int gridItemX, int gridItemY);
        void CheckEnabledClicked(int actionNum);

        void PreDrawItems();
        void SysColorRect(HDC hDC, RECT &rect, DWORD color);
        void DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart);
        void DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart);
        void DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart);
        void DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis);
        void DrawTouchups(HDC hDC);
        void PostDrawItems();

        void SuggestNothing();
        void SuggestLocation();
        void SuggestString();
        void SuggestPlayer();
        void SuggestUnit();
        void SuggestNumUnits();
        void SuggestCUWP();
        void SuggestTextFlags();
        void SuggestAmount();
        void SuggestScoreType();
        void SuggestResourceType();
        void SuggestStateMod();
        void SuggestPercent();
        void SuggestOrder();
        void SuggestWav();
        void SuggestDuration();
        void SuggestScript();
        void SuggestAllyState();
        void SuggestNumericMod();
        void SuggestSwitch();
        void SuggestSwitchMod();
        void SuggestType();
        void SuggestActionType();
        void SuggestSecondaryType();
        void SuggestFlags();
        void SuggestNumber(); // Amount, Group2, LocDest, UnitPropNum, ScriptNum
        void SuggestTypeIndex(); // Unit, ScoreType, ResourceType, AllianceStatus
        void SuggestSecondaryTypeIndex(); // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
        void SuggestInternalData();

        void EnableStringEdit();
        void DisableStringEdit();
        void ButtonEditString();
        void EnableWavEdit();
        void DisableWavEdit();
        void ButtonEditWav();

        void GridEditStart(u16 gridItemX, u16 gridItemY);
        void NewSelection(u16 gridItemX, u16 gridItemY);
        void NewSuggestion(std::string &str);

        ChkdString GetCurrentActionsString();
        ChkdString GetCurrentActionsWav();

        void Activate(WPARAM wParam, LPARAM lParam);
        LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif