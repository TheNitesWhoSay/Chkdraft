#ifndef BRIEFINGEDITOR_H
#define BRIEFINGEDITOR_H
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <mapping_core/mapping_core.h>
#include "briefing_triggers.h"
#include "briefing_templates.h"

class BriefingTrigEditorWindow : public WinLib::ClassDialog
{
    public:
        enum class Tab : u32 {
            BriefingTriggers,
            BriefingTemplates,
        };

        BriefingTriggersWindow briefingTriggersWindow;

        BriefingTrigEditorWindow();
        virtual ~BriefingTrigEditorWindow();
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

        BriefingTemplatesWindow briefingTemplatesWindow;
};

#endif