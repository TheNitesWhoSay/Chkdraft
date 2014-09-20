#ifndef MAPPROPERTIES_H
#define MAPPROPERTIES_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class MapPropertiesWindow : public ClassWindow
{
	public:
		MapPropertiesWindow();
		bool CreateThis(HWND hParent);

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CheckReplaceMapTitle();
		void CheckReplaceMapDescription();

		bool possibleTitleUpdate;
		bool possibleDescriptionUpdate;

		ButtonControl buttonApply;
};

#endif