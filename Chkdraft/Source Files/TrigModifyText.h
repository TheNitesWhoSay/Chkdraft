#ifndef TRIG_MODIFY_TEXT
#define TRIG_MODIFY_TEXT
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class TrigModifyTextWindow : public ClassWindow
{
	public:
		TrigModifyTextWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow(u32 trigIndex);
		void DoSize();
		void ParentHidden();

	protected:
		void CreateSubWindows(HWND hWnd);
		bool unsavedChanges();
		void Compile(bool silent, bool saveAfter);
		bool CompileEditText(std::string &newText);
		void OnLeave();
		
	private:
		EditControl editText;
		CheckBoxControl checkAutoCompile;
		ButtonControl buttonCompile, buttonCompileAndSave, buttonReload;
		u32 trigIndex;
		std::string trigText;
		bool autoCompile;

		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif