#ifndef BRIEFINGTRIGGERS_H
#define BRIEFINGTRIGGERS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/PeerListBox.h"
#include "BriefingTrigModify.h"
#include <unordered_map>
#include <string>

class BriefingTriggersWindow : public WinLib::ClassWindow
{
    public:
        BriefingTrigModifyWindow briefingTrigModifyWindow;

        BriefingTriggersWindow();
        virtual ~BriefingTriggersWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow(bool focus);
        void DoSize();

        void DeleteSelection();
        void CopySelection();
        void MoveUp();
        void MoveDown();
        void MoveTrigTo();

        void ButtonNew();
        void ButtonModify();

        std::string GetActionString(u8 actionNum, const Chk::Trigger & briefingTrigger, BriefingTextTrigGenerator & tt);
        std::string GetBriefingTriggerString(u32 briefingTrigNum, const Chk::Trigger & briefingTrigger, BriefingTextTrigGenerator & tt);

    protected:
        bool SelectTrigListItem(int listIndex); // Attempts to select item at listIndex, updating currTrigger

        void CreateSubWindows(HWND hWnd);
        void RefreshGroupList();
        void RefreshTrigList();

        /** Attempts to delete the item at listIndex and decrement higher item's briefing trigger indexes
            If this method returns false, parts of the briefing trigger list may be invalid */
        bool DeleteTrigListItem(int listIndex);

        /** Attempts to copy the item at listIndex and increment higher item's briefing trigger indexes
            If this method returns false, parts of the briefing trigger list may be invalid */
        bool CopyTrigListItem(int listIndex);

        /** Attempts to swap the item at listIndex with the item at listIndex-1
            If this method returns false, parts of the briefing trigger list may be invalid */
        bool MoveUpTrigListItem(int listIndex, u32 prevBriefingTrigIndex);

        /** Attempt to swap the item at listIndex with the item at listIndex+1
            If this method returns false, parts of the briefing trigger list may be invalid */
        bool MoveDownTrigListItem(int listIndex, u32 nextBriefingTrigIndex);

        /** Attempt to move the item at currListIndex to the appropriate space considering
            targetBriefingTrigIndex, listIndexMovedTo is determined by the briefingTrigIndexes and does not
            need to be set prior to calling this function.
            returns true and sets listIndexMovedTo if successful */
        bool MoveTrigListItemTo(int currListIndex, u32 currBriefingTrigIndex, u32 targetBriefingTrigIndex, int & listIndexMovedTo);

        bool FindTargetListIndex(int currListIndex, u32 currBriefingTrigIndex, u32 targetBriefingTrigIndex, int & targetListIndex);

    protected:
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 currBriefingTrigger; // The index of the current briefing trigger
        bool groupSelected[Chk::Trigger::MaxOwners];
        bool displayAll;
        u32 numVisibleBriefingTrigs;
        bool changeGroupHighlightOnly;
        bool drawingAll;

        WinLib::ButtonControl buttonNew, buttonModify, buttonDelete, buttonCopy, buttonMoveUp, buttonMoveDown, buttonMoveTo;

        PeerListBox listGroups;
        WinLib::ListBoxControl listBriefingTriggers;
        BriefingTextTrigGenerator briefingTextTrigGenerator; // Text trig generator for assisting briefing trigger display
        std::optional<WinLib::DeviceContext> briefingTrigListDc; // Trig list HDC for speeding up briefing trigger measurement
        std::hash<std::string> strHash; // A hasher to help generate tables
        std::unordered_multimap<size_t, WinLib::LineSize> briefingTrigLineSizeTable;

        bool ShowTrigger(const Chk::Trigger & briefingTrigger); // Checks if briefing trigger should currently be shown
        void ClearGroups();
        bool GetTriggerDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, Scenario & chk, u32 briefingTriggerNum, const Chk::Trigger & briefingTrigger);
        void DrawGroup(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, u8 groupNum);
        void DrawTrigger(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, Scenario & chk, u32 briefingTriggerNum, const Chk::Trigger & briefingTrigger);
};

#endif