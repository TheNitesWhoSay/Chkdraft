#ifndef STRINGGUIDE_H
#define STRINGGUIDE_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class StringGuideWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);

	protected:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		TextControl colorPrefix[NUM_STRING_COLORS];
		TextControl color[NUM_STRING_COLORS];
};

#endif