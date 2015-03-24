#ifndef TRIGGENERAL_H
#define TRIGGENERAL_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

/**
	Data storage drafting
	- Executing Player bytes 22 and 23: Comment String
	- Executing Player bytes 24 and 25: Notes String
	- Executing Player byte 27: Text Flags
		- Bit 0: Extended Comment String
		- Bit 1: Extended Comment Action
		- Bit 2: Extended Notes String
		- Bit 3: Extended Notes Action
		- Bit 4-7: Unused
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

		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif