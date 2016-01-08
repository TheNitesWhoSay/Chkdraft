#ifndef TEXTTRIG_H
#define TEXTTRIG_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "TextTrigGenerator.h"
#include "TextTrigCompiler.h"

class TextTrigWindow : public ClassDialog
{
	public:
		bool CreateThis(HWND hParent);
		void RefreshWindow();

	protected:
		bool CompileEditText(ScenarioPtr map);
		virtual BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		virtual BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		EditControl editControl;
};

#endif