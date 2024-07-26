#ifndef BRIEFINGEDITOR_H
#define BRIEFINGEDITOR_H
#include "../../../CommonFiles/CommonFiles.h"
#include "../../../../WindowsLib/WindowsUi.h"
#include "../../../../MappingCoreLib/MappingCore.h"
#include "BriefingTriggers.h"
#include "BriefingTemplates.h"

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