#ifndef LIT_H
#define LIT_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class LitWindow : public ClassDialog
{
	public:
		bool CreateThis(HWND hParent);
		void RefreshWindow();

	protected:
		bool GetLitPaths(string &litDirectory, string &litPath);
		bool GetInputPaths(string &luaDirectory, string &luaName);
		bool WriteLitBat(string &luaDirectory, string &luaName, string &litDirectory, string &litBatPath,
			string &textOutName, string &trigOutName);
		bool RunLit(ScenarioPtr map);
		void ButtonLit();
		void ButtonLitSave();
		virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		EditControl editPath;
};

#endif