#ifndef TRIGACTIONS_H
#define TRIGACTIONS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "ui/chkd_controls/cnd_act_grid.h"
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
        bool IsSuggestionsWindow(HWND hWnd);
        void FocusGrid();

    protected:
        void InitializeScriptTable();
        void CreateSubWindows(HWND hWnd);
        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        virtual LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        CndActGrid gridActions;
        WinLib::EditControl editListItem;
        WinLib::ButtonControl buttonEditSound;
        WinLib::ButtonControl buttonEditString;
        WinLib::ButtonControl buttonUnitProperties;
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
        void UpdateActionName(u8 actionNum, const std::string & newText, bool refreshImmediately);
        void UpdateActionArg(u8 actionNum, u8 argNum, const std::string & newText, bool refreshImmediately);
        BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str);
        BOOL GridItemDeleting(u16 gridItemX, u16 gridItemY);
        void DrawSelectedAction();
        int GetGridItemWidth(int gridItemX, int gridItemY);

        void PreDrawItems();
        void SysColorRect(const WinLib::DeviceContext & dc, RECT & rect, DWORD color);
        void DrawItemBackground(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart);
        void DrawItemFrame(const WinLib::DeviceContext & dc, RECT & rcItem, int width, int & xStart);
        void DrawGridViewItem(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int & xStart);
        void DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis);
        void DrawTouchups(HDC hDC);
        void PostDrawItems();

        void SuggestNothing();
        void SuggestLocation(u32 currLocationId);
        void SuggestString(u32 currStringId);
        void SuggestPlayer(u32 currPlayer);
        void SuggestUnit(u16 currUnit);
        void SuggestNumUnits(u8 currNumUnits);
        void SuggestCUWP(u32 currCuwp);
        void SuggestTextFlags(u8 currTextFlags);
        void SuggestAmount(u32 currAmount);
        void SuggestScoreType(u16 currType);
        void SuggestResourceType(u16 currType);
        void SuggestStateMod(u8 currStateMod);
        void SuggestPercent(u32 currPercent);
        void SuggestOrder(u8 currOrder);
        void SuggestSound(u32 currSound);
        void SuggestDuration(u32 currDuration);
        void SuggestScript(u32 currScript);
        void SuggestAllyState(u16 currAllyState);
        void SuggestNumericMod(u8 currNumericMod);
        void SuggestSwitch(u32 currSwitch);
        void SuggestSwitchMod(u8 currSwitchMod);
        void SuggestActionType(Chk::Action::Type currActionType);
        void SuggestFlags(u8 currFlags);
        void SuggestNumber(u32 currNumber); // Amount, Group2, LocDest, UnitPropNum, ScriptNum
        void SuggestTypeIndex(u16 currTypeIndex); // Unit, ScoreType, ResourceType, AllianceStatus
        void SuggestSecondaryTypeIndex(u8 currSecondaryTypeIndex); // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
        void SuggestPadding(u8 currPadding);
        void SuggestMaskFlag(Chk::Action::MaskFlag maskFlag);

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
        void SelConfirmed(WPARAM wParam);
        void NewSelection(u16 gridItemX, u16 gridItemY);
        void NewSuggestion(std::string & str);

        void GetCurrentActionString(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString);
        void GetCurrentActionSound(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString);

        LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif