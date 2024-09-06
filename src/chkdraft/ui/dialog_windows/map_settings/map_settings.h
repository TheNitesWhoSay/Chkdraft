#ifndef MAPSETTINGS_H
#define MAPSETTINGS_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "map_properties.h"
#include "forces.h"
#include "unit_settings.h"
#include "upgrade_settings.h"
#include "tech_settings.h"
#include "string_editor/string_editor.h"
#include "sound_editor.h"

class MapSettingsWindow : public WinLib::ClassDialog
{
    public:
        enum class Tab : u32 {
            MapProperties,
            Forces,
            UnitSettings,
            UpgradeSettings,
            TechSettings,
            StringEditor,
            SoundEditor
        };

        MapSettingsWindow();
        virtual ~MapSettingsWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void ChangeTab(Tab tab);
        void RefreshWindow();

    protected:
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Tab currTab;
        WinLib::TabControl tabs;
        MapPropertiesWindow mapPropertiesWindow;
        ForcesWindow forcesWindow;
        UnitSettingsWindow unitSettingsWindow;
        UpgradeSettingsWindow upgradeSettingsWindow;
        TechSettingsWindow techSettingsWindow;
        StringEditorWindow stringEditorWindow;
        SoundEditorWindow soundEditorWindow;
};

enum class MapSettings {
    TAB_MAPPROPERTIES,
    TAB_FORCES,
    TAB_UNITSETTINGS,
    TAB_UPGRADESETTINGS,
    TAB_TECHSETTINGS,
    TAB_STRINGEDITOR,
    TAB_SOUNDEDITOR
};

#endif