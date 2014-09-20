#ifndef MAPSETTINGS_H
#define MAPSETTINGS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "MapProperties.h"
#include "Forces.h"
#include "UnitSettings.h"
#include "UpgradeSettings.h"
#include "TechSettings.h"
#include "StringEditor.h"
#include "WavEditor.h"

class MapSettingsWindow : public ClassWindow
{
	public:
		MapSettingsWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void ChangeTab(u32 tabID);

	private:
		u32 currTab;
		HWND hTabs;
		MapPropertiesWindow mapPropertiesWindow;
		ForcesWindow forcesWindow;
		UnitSettingsWindow unitSettingsWindow;
		UpgradeSettingsWindow upgradeSettingsWindow;
		TechSettingsWindow techSettingsWindow;
		StringEditorWindow stringEditorWindow;
		WavEditorWindow wavEditorWindow;

		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif