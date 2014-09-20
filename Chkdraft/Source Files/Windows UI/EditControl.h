#ifndef EDITCONTROL_H
#define EDITCONTROL_H
#include "WindowControl.h"

class EditControl : public WindowControl
/**	A basic edit control
	This EditControl class maps Select All to Ctrl+A by default */
{
	public:
		EditControl();
		bool FindThis(HWND hParent, u32 controlId); // Attempts to encapsulate an edit control and enable Select All
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, u32 id); // Attempts to create an edit control
		bool SetText(const char* newText); // Sets new text content
		void MaximizeTextLimit(); // Sets text limit to 0x7FFFFFFF

	private:
		bool isMultiLine;
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Used to map Select All to Ctrl+A
};

#endif