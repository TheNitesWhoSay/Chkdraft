#ifndef TRIGMODIFY_H
#define TRIGMODIFY_H
#include "Windows UI/WindowsUI.h"

class TrigModifyWindow : public ClassWindow
{
	public:
		TrigModifyWindow();
		bool CreateThis(HWND hParent, u32 trigIndex);
		bool DestroyThis();
		void RefreshWindow(u32 trigIndex);
		void DoSize();

	protected:
		void CreateSubWindows(HWND hWnd);

	private:
		u32 trigIndex;

		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif