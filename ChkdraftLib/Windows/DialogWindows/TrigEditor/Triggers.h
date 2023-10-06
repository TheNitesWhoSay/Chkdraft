#ifndef TRIGGERS_H
#define TRIGGERS_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../ChkdControls/PeerListBox.h"
#include "../TrigModify/TrigModify.h"
#include <unordered_map>
#include <string>

class TriggersWindow : public WinLib::ClassWindow
{
    public:
        TrigModifyWindow trigModifyWindow;

        TriggersWindow();
        virtual ~TriggersWindow();
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

        std::string GetConditionString(u8 conditionNum, const Chk::Trigger & trigger, TextTrigGenerator & tt);
        std::string GetActionString(u8 actionNum, const Chk::Trigger & trigger, TextTrigGenerator & tt);
        std::string GetTriggerString(u32 trigNum, const Chk::Trigger & trigger, TextTrigGenerator & tt);

    protected:
        bool SelectTrigListItem(int listIndex); // Attempts to select item at listIndex, updating currTrigger

        void CreateSubWindows(HWND hWnd);
        void RefreshGroupList();
        void RefreshTrigList();

        /** Attempts to delete the item at listIndex and decrement higher item's trigger indexes
            If this method returns false, parts of the trigger list may be invalid */
        bool DeleteTrigListItem(int listIndex);

        /** Attempts to copy the item at listIndex and increment higher item's trigger indexes
            If this method returns false, parts of the trigger list may be invalid */
        bool CopyTrigListItem(int listIndex);

        /** Attempts to swap the item at listIndex with the item at listIndex-1
            If this method returns false, parts of the trigger list may be invalid */
        bool MoveUpTrigListItem(int listIndex, u32 prevTrigIndex);

        /** Attempt to swap the item at listIndex with the item at listIndex+1
            If this method returns false, parts of the trigger list may be invalid */
        bool MoveDownTrigListItem(int listIndex, u32 nextTrigIndex);

        /** Attempt to move the item at currListIndex to the appropriate space considering
            targetTrigIndex, listIndexMovedTo is determined by the trigIndexes and does not
            need to be set prior to calling this function.
            returns true and sets listIndexMovedTo if successful */
        bool MoveTrigListItemTo(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int & listIndexMovedTo);

        bool FindTargetListIndex(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int & targetListIndex);

    protected:
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        u32 currTrigger; // The trig index of the current trigger
        bool groupSelected[Chk::Trigger::MaxOwners];
        bool displayAll;
        u32 numVisibleTrigs;
        bool changeGroupHighlightOnly;
        bool drawingAll;

        WinLib::ButtonControl buttonNew, buttonModify, buttonDelete, buttonCopy, buttonMoveUp, buttonMoveDown, buttonMoveTo;

        PeerListBox listGroups;
        WinLib::ListBoxControl listTriggers;
        TextTrigGenerator textTrigGenerator; // Text trig generator for assisting trigger display
        std::optional<WinLib::DeviceContext> trigListDc; // Trig list HDC for speeding up trigger measurement
        std::hash<std::string> strHash; // A hasher to help generate tables
        struct CommentSize {
            std::string str;
            UINT width;
            UINT height;
        };
        std::unordered_multimap<size_t, CommentSize> commentSizeTable;
        std::unordered_multimap<size_t, WinLib::LineSize> trigLineSizeTable;

        bool ShowTrigger(const Chk::Trigger & trigger); // Checks if trigger should currently be shown
        void ClearGroups();
        bool GetTriggerDrawSize(const WinLib::DeviceContext & dc, UINT & width, UINT & height, Scenario & chk, u32 triggerNum, const Chk::Trigger & trigger);
        void DrawGroup(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, u8 groupNum);
        void DrawTrigger(const WinLib::DeviceContext & dc, RECT & rcItem, bool isSelected, Scenario & chk, u32 triggerNum, const Chk::Trigger & trigger);
};

#endif