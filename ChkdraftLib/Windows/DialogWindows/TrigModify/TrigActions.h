#ifndef TRIGACTIONS_H
#define TRIGACTIONS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/CndActGrid.h"
#include <string>
#include <vector>

class TrigActionsWindow : public WinLib::ClassWindow, public ICndActGridUser
{
    public:
        TrigActionsWindow();
        virtual ~TrigActionsWindow();
        bool CreateThis(HWND hParent, u64 windowId);
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
        void InitializeScriptTable();
        void CreateSubWindows(HWND hWnd);
        void OnLeave();
        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        CndActGrid gridActions;
        WinLib::EditControl editListItem;
        WinLib::ButtonControl buttonEditSound;
        WinLib::ButtonControl buttonEditString;
        WinLib::ButtonControl buttonUnitProperties;
        HBRUSH hBlack;
        u32 trigIndex;
        bool stringEditEnabled;
        bool soundEditEnabled;
        bool unitPropertiesEditEnabled;
        bool isPasting;

        std::hash<std::string> strHash; // A hasher to help generate tables
        std::unordered_multimap<size_t/*stringHash*/, std::pair<u32, std::string/*scriptId*/>> scriptTable; // Scripts in format: description (id)

        Suggestions & suggestions;

        LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void ChangeActionType(Chk::Action & action, Chk::Action::Type newType);
        bool TransformAction(Chk::Action & action, Chk::Action::Type newType, bool refreshImmediately);
        void RefreshActionAreas();
        void ClearArgument(Chk::Action & action, u8 argNum, bool refreshImmediately);
        void UpdateActionName(u8 actionNum, const std::string & newText, bool refreshImmediately);
        void UpdateActionArg(u8 actionNum, u8 argNum, const std::string & newText, bool refreshImmediately);
        BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str);
        BOOL GridItemDeleting(u16 gridItemX, u16 gridItemY);
        void DrawSelectedAction();
        int GetGridItemWidth(int gridItemX, int gridItemY);
        void CheckEnabledClicked(int actionNum);

        void PreDrawItems();
        void SysColorRect(HDC hDC, RECT & rect, DWORD color);
        void DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart);
        void DrawItemFrame(HDC hDC, RECT & rcItem, int width, int & xStart);
        void DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int & xStart);
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
        void SuggestSound();
        void SuggestDuration();
        void SuggestScript();
        void SuggestAllyState();
        void SuggestNumericMod();
        void SuggestSwitch();
        void SuggestSwitchMod();
        void SuggestActionType();
        void SuggestFlags();
        void SuggestNumber(); // Amount, Group2, LocDest, UnitPropNum, ScriptNum
        void SuggestTypeIndex(); // Unit, ScoreType, ResourceType, AllianceStatus
        void SuggestSecondaryTypeIndex(); // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
        void SuggestInternalData();

        void EnableStringEdit();
        void DisableStringEdit();
        void ButtonEditString();
        void EnableSoundEdit();
        void DisableSoundEdit();
        void ButtonEditSound();
        void EnableUnitPropertiesEdit();
        void DisableUnitPropertiesEdit();
        void ButtonEditUnitProperties();

        void GridEditStart(u16 gridItemX, u16 gridItemY);
        void NewSelection(u16 gridItemX, u16 gridItemY);
        void NewSuggestion(std::string & str);

        void GetCurrentActionString(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString);
        void GetCurrentActionSound(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString);

        void Activate(WPARAM wParam, LPARAM lParam);
        LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif