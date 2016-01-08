#ifndef TRIGGENERAL_H
#define TRIGGENERAL_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"

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

class TrigGeneralWindow : public ClassWindow
{
	public:
		TrigGeneralWindow();
		bool CreateThis(HWND hParent, u32 windowId);
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
		GroupBoxControl groupComment;
		EditControl editComment;
		CheckBoxControl checkExtendedCommentString;
		CheckBoxControl checkExtendedCommentAction;
		ButtonControl buttonDeleteComment;

		GroupBoxControl groupNotes;
		EditControl editNotes;
		CheckBoxControl checkExtendedNotesString;
		CheckBoxControl checkExtendedNotesAction;
		ButtonControl buttonDeleteNotes;

		GroupBoxControl groupExecutionFlags;

		CheckBoxControl checkPreservedFlag;
		CheckBoxControl checkDisabled;
		CheckBoxControl checkIgnoreConditionsOnce;

		CheckBoxControl checkIgnoreWaitSkipOnce;
		CheckBoxControl checkIgnoreManyActions;
		CheckBoxControl checkIgnoreDefeatDraw;
		CheckBoxControl checkFlagPaused;

		TextControl textRawFlags;
		EditControl editRawFlags;

		ButtonControl buttonAdvanced;
		bool advancedMode;
};

#endif