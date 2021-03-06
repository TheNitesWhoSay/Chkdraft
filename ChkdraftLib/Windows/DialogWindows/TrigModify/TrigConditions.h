#ifndef TRIGCONDITIONS_H
#define TRIGCONDITIONS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/CndActGrid.h"
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

    protected:
        void CreateSubWindows(HWND hWnd);
        void OnLeave();
        virtual LRESULT Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        CndActGrid gridConditions;
        HBRUSH hBlack;
        u32 trigIndex;
        bool isPasting;

        Suggestions & suggestions;

        LRESULT MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void ChangeConditionType(Chk::Condition & condition, Chk::Condition::Type newType);
        bool TransformCondition(Chk::Condition & condition, Chk::Condition::Type newType, bool refreshImmediately);
        void RefreshConditionAreas();
        void ClearArgument(Chk::Condition & condition, u8 argNum);
        void UpdateConditionName(u8 conditionNum, const std::string & newText, bool refreshImmediately);
        void UpdateConditionArg(u8 conditionNum, u8 argNum, const std::string & newText, bool refreshImmediately);
        BOOL GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str);
        BOOL GridItemDeleting(u16 gridItemX, u16 gridItemY);
        void DrawSelectedCondition();
        int GetGridItemWidth(int gridItemX, int gridItemY);
        void CheckEnabledClicked(int conditionNum);

        void PreDrawItems();
        void SysColorRect(HDC hDC, RECT & rect, DWORD color);
        void DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart);
        void DrawItemFrame(HDC hDC, RECT & rcItem, int width, int & xStart);
        void DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int & xStart);
        void DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis);
        void DrawTouchups(HDC hDC);
        void PostDrawItems();

        void SuggestNothing();
        void SuggestUnit();
        void SuggestLocation();
        void SuggestPlayer();
        void SuggestAmount();
        void SuggestNumericComparison();
        void SuggestResourceType();
        void SuggestScoreType();
        void SuggestSwitch();
        void SuggestSwitchState();
        void SuggestComparison();
        void SuggestConditionType();
        void SuggestTypeIndex();
        void SuggestFlags();
        void SuggestInternalData();

        void GridEditStart(u16 gridItemX, u16 gridItemY);
        void NewSuggestion(std::string & str);

        void Activate(WPARAM wParam, LPARAM lParam);
        LRESULT ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif