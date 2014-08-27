#ifndef UPGRADESETTINGS_H
#define UPGRADESETTINGS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class UpgradeSettingsWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif