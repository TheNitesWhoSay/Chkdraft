#ifndef TILEPROPERTIES_H
#define TILEPROPERTIES_H
#include "CommonFiles/CommonFiles.h"
#include "WindowsUI/WindowsUI.h"

class TilePropWindow : public ClassDialog
{
	public:
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void UpdateTile();

	protected:
		BOOL DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

};

#endif