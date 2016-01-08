#ifndef LIT_H
#define LIT_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include <string>

class LitWindow : public ClassDialog
{
	public:
		bool CreateThis(HWND hParent);
		void RefreshWindow();

	protected:
		bool GetLitPaths(std::string &litDirectory, std::string &litPath);
		bool GetInputPaths(std::string &luaDirectory, std::string &luaName);
		bool WriteLitBat(std::string &luaDirectory, std::string &luaName, std::string &litDirectory, std::string &litBatPath,
			std::string &textOutName, std::string &trigOutName);
		bool RunLit(ScenarioPtr map);
		void ButtonLit();
		void ButtonLitSave();
		virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		EditControl editPath;
};

#endif