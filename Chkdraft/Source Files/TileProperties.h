#ifndef TILEPROPERTIES_H
#define TILEPROPERTIES_H
#include "Common Files/CommonFiles.h"
#include "Windows UI/WindowsUI.h"

class TilePropWindow : public ClassWindow
{
	public:
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void UpdateTile();

	protected:
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

};

#endif