#ifndef TRIGGENERAL_H
#define TRIGGENERAL_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"

/**
    Data storage drafting
    - Executing Player bytes 22, 23, 24, 25 make up a 32-bit extended trigger struct index
        - The following 16 indexes are never used: 0, 1, 256, 257, 65536, 65537, 65792, 65793,
        - In other words, if ((index&0xFEFEFEFE)==0), the bytes are not an extended trigger struct index
        - groupedWithPlayerId22 = (extendedData&0x10000000)
        - groupedWithPlayerId23 = (extendedData&0x20000000)
        - groupedWithPlayerId24 = (extendedData&0x40000000)
        - groupedWithPlayerId25 = (extendedData&0x80000000)
*/

class TrigGeneralWindow : public WinLib::ClassWindow
{
    public:
        TrigGeneralWindow();
        virtual ~TrigGeneralWindow();
        bool CreateThis(HWND hParent, u64 windowId);
        bool DestroyThis();
        void RefreshWindow(u32 trigIndex);
        void DoSize();

    protected:
        void CreateSubWindows(HWND hWnd);
        void OnLeave();
        void SetPreserveTrigger(bool preserve);
        void SetDisabledTrigger(bool disabled);
        void SetIgnoreConditionsOnce(bool ignoreConditionsOnce);
        void SetIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce);
        void SetIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce);
        void SetIgnoreDefeatDraw(bool ignoreDefeatDraw);
        void SetPausedTrigger(bool paused);
        void ParseRawFlagsText();
        void ToggleAdvancedMode();
        LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool refreshing;
        u32 trigIndex;
        WinLib::GroupBoxControl groupComment;
        WinLib::EditControl editComment;
        WinLib::CheckBoxControl checkExtendedCommentString;
        WinLib::CheckBoxControl checkExtendedCommentAction;
        WinLib::ButtonControl buttonDeleteComment;

        WinLib::GroupBoxControl groupNotes;
        WinLib::EditControl editNotes;
        WinLib::CheckBoxControl checkExtendedNotesString;
        WinLib::CheckBoxControl checkExtendedNotesAction;
        WinLib::ButtonControl buttonDeleteNotes;

        WinLib::GroupBoxControl groupExecutionFlags;

        WinLib::CheckBoxControl checkPreservedFlag;
        WinLib::CheckBoxControl checkDisabled;
        WinLib::CheckBoxControl checkIgnoreConditionsOnce;

        WinLib::CheckBoxControl checkIgnoreWaitSkipOnce;
        WinLib::CheckBoxControl checkIgnoreManyActions;
        WinLib::CheckBoxControl checkIgnoreDefeatDraw;
        WinLib::CheckBoxControl checkFlagPaused;

        WinLib::TextControl textRawFlags;
        WinLib::EditControl editRawFlags;

        WinLib::ButtonControl buttonAdvanced;
        bool advancedMode;
};

#endif