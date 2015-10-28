#ifndef TRIGEDITOR_H
#define TRIGEDITOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "Triggers.h"
#include "Templates.h"
#include "Counters.h"
#include "CUWPs.h"
#include "Switches.h"

class TrigEditorWindow : public ClassDialog
{
	public:
		TriggersWindow triggersWindow;

		TrigEditorWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void ChangeTab(u32 tabId);
		void RefreshWindow();

	protected:
		void CreateSubWindows(HWND hWnd);
		void DoSize();
		BOOL DlgNotify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		u32 currTab;
		TabControl tabs;

		TemplatesWindow templatesWindow;
		CountersWindow countersWindow;
		CUWPsWindow cuwpsWindow;
		SwitchesWindow switchesWindow;
};

#endif