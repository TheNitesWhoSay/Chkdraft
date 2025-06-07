#ifndef TRIGCONDITIONS_H
#define TRIGCONDITIONS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "ui/chkd_controls/cnd_act_grid.h"
#include <string>
#include <vector>

class TrigConditionsWindow : public WinLib::ClassWindow, public ICndActGridUser
{
    public:
        TrigConditionsWindow();
        virtual ~TrigConditionsWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow(u32 trigIndex);
        void DoSize();
        void ProcessKeyDown(WPARAM wParam, LPARAM lParam);
        void HideSuggestions();
        void CndActEnableToggled(u8 conditionNum);
        virtual void CutSelection();
        virtual void CopySelection();
        virtual void Paste();
        virtual void RedrawThis();
        bool IsSuggestionsWindow(HWND hWnd);
        void FocusGrid();

    protected:
        void CreateSubWindows(HWND hWnd);
        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        CndActGrid gridConditions;
        u32 trigIndex;
        bool isPasting;

        Suggestions & suggestions;

        LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void ChangeConditionType(std::size_t triggerIndex, std::size_t conditionIndex, Chk::Condition::Type newType);
        bool TransformCondition(std::size_t triggerIndex, std::size_t conditionIndex, Chk::Condition::Type newType, bool refreshImmediately);
        void RefreshConditionAreas();
        void UpdateConditionName(u8 conditionNum, const std::string & newText, bool refreshImmediately);
        void UpdateConditionArg(u8 conditionNum, u8 argNum, const std::string & newText, bool refreshImmediately);
        BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str);
        BOOL GridItemDeleting(u16 gridItemX, u16 gridItemY);
        void DrawSelectedCondition();
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
        void SuggestUnit(u16 currUnit);
        void SuggestLocation(u32 currLocationId);
        void SuggestPlayer(u32 currPlayer);
        void SuggestAmount(u32 currAmount);
        void SuggestNumericComparison(Chk::Condition::Comparison currNumericComparison);
        void SuggestResourceType(u8 currType);
        void SuggestScoreType(u8 currType);
        void SuggestSwitch(u8 currSwitch);
        void SuggestSwitchState(Chk::Condition::Comparison currSwitchState);
        void SuggestComparison(Chk::Condition::Comparison currComparison);
        void SuggestConditionType(Chk::Condition::Type currConditionType);
        void SuggestTypeIndex(u8 currTypeIndex);
        void SuggestFlags(u8 currFlags);
        void SuggestMaskFlag(Chk::Condition::MaskFlag maskFlag);

        void GridEditStart(u16 gridItemX, u16 gridItemY);
        void SelConfirmed(WPARAM wParam);
        void NewSelection(u16 gridItemX, u16 gridItemY);
        void NewSuggestion(std::string & str);

        LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif