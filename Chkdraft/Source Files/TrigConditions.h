#ifndef TRIGCONDITIONS_H
#define TRIGCONDITIONS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"

class TrigConditionsWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent, u32 windowId);
		bool DestroyThis();
		void RefreshWindow();
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);
		void OnLeave();

	private:
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		GridViewControl listConditions;
};

#endif