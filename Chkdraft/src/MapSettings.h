#ifndef MAPSETTINGS_H
#define MAPSETTINGS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"
#include "WindowsUI/WindowsUI.h"
#include "MapProperties.h"
#include "Forces.h"
#include "UnitSettings.h"
#include "UpgradeSettings.h"
#include "TechSettings.h"
#include "StringEditor.h"
#include "WavEditor.h"

class MapSettingsWindow : public ClassDialog
{
	public:
		MapSettingsWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void ChangeTab(u32 tabID);
		void RefreshWindow();

	protected:
		BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
		BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
};

enum MapSettings {
	TAB_MAPPROPERTIES,
	TAB_FORCES,
	TAB_UNITSETTINGS,
	TAB_UPGRADESETTINGS,
	TAB_TECHSETTINGS,
	TAB_STRINGEDITOR,
	TAB_WAVEDITOR
};

#endif