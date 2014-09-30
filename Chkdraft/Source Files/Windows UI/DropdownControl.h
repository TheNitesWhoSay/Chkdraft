#ifndef DROPDOWNCONTROL_H
#define DROPDOWNCONTROL_H
#include "WindowControl.h"

class DropdownControl : public WindowControl
{
	public:
		bool CreateThis(HWND hParent, int x, int y, int width, int height, bool editable, u32 id, int numItems, const char** items, HFONT font);
		void SetSel(int index);
		void ClearEditSel();
		int GetSel();
		bool GetText(int index, char* dest, int destLength);
		bool GetEditText(std::string& dest);

		template <typename numType>
			bool GetEditNum(numType &dest);

	protected:
		bool GetEditText(char* &dest);
};

#endif