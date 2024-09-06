#ifndef TRIGEDITOR_H
#define TRIGEDITOR_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "triggers.h"
#include "templates.h"
#include "counters.h"
#include "cuwps.h"
#include "switches.h"

class TrigEditorWindow : public WinLib::ClassDialog
{
    public:
        enum class Tab : u32 {
            Triggers,
            Templates,
            Counters,
            Cuwps,
            Switches
        };

        TriggersWindow triggersWindow;

        TrigEditorWindow();
        virtual ~TrigEditorWindow();
        bool CreateThis(HWND hParent);
        bool DestroyThis();
        void ChangeTab(Tab tab);
        void RefreshWindow();

    protected:
        void CreateSubWindows(HWND hWnd);
        void DoSize();
        BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
        BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Tab currTab;
        WinLib::TabControl tabs;

        TemplatesWindow templatesWindow;
        CountersWindow countersWindow;
        CUWPsWindow cuwpsWindow;
        SwitchesWindow switchesWindow;
};

#endif