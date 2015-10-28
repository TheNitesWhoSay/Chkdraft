#ifndef STRINGPREVIEW_H
#define STRINGPREVIEW_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class StringPreviewWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent, u32 windowId);

	protected:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif