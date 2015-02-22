#ifndef STRINGEDITOR_H
#define STRINGEDITOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "StringGuide.h"
#include "StringPreview.h"

class StringEditorWindow : public ClassWindow
{
	public:
		StringEditorWindow();
		bool CreateThis(HWND hParent, u32 windowId);
		void RefreshWindow();

	protected:
		void saveStrings();
		void addUseItem(HWND hStringUse, const char* title, u32 amount); // Adds items to StringUse list box
		bool updateString(u32 stringNum); // Checks if string matches edit box content, if not, replaces string; returns true iff string changes

	private:
		u32 currSelString;
		TextControl textAboutStrings;
		TextControl textStringUsage;
		EditControl editString;
		StringGuideWindow stringGuide;
		StringPreviewWindow stringPreviewWindow;
		ButtonControl buttonSaveString;
		ButtonControl buttonDeleteString;
		CheckBoxControl checkExtendedString;
		ListBoxControl listboxStrings;
		ListBoxControl listUsage;

		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK StringPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif