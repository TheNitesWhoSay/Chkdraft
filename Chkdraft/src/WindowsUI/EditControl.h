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
		bool CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool multiLine, bool clientEdge, u32 id); // Attempts to create an edit control
		void SetForwardArrowKeys(bool forwardArrowKeyToParent); // Sets whether this control forwards arrow-keypresses to the parent
		void SetStopForwardOnClick(bool stopFowardingOnClick); // Sets whether this stops forwarding arrow-keys when clicked
		bool SetText(const char* newText); // Sets new text content

		template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
			bool SetEditNum(numType num);

		template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
			bool SetEditBinaryNum(numType num);

		void SetTextLimit(u32 newLimit);
		void MaximizeTextLimit(); // Sets text limit to 0x7FFFFFFF
		void ExpandToText(); // Expands the edit control to show all the current text

		int GetTextLength();
		bool GetEditText(std::string& dest);
		bool GetEditBinaryNum(u16 &dest);
		bool GetEditBinaryNum(u32 &dest);

		bool GetHexByteString(u8* dest, u32 destLength);
		bool SetHexByteString(u8* bytes, u32 numBytes);

		template <typename numType> // Allowed types: u8, s8, u16, s16, u32, s32/int
			bool GetEditNum(numType &dest);


	protected:
		bool GetEditText(char* &dest); // This version isn't public, pass an std::string
		LRESULT ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Used to map Select All to Ctrl+A

	private:
		bool isMultiLine;
		bool forwardArrowKeys;
		bool stopFowardingOnClick;
		bool autoExpand;
};

#endif