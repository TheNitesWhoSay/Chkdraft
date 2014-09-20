#ifndef TERRAINPALETTE_H
#define TERRAINPALETTE_H
#include "Common Files/CommonFiles.h"
#include "Windows UI/WindowsUI.h"

class TerrainPaletteWindow : public ClassWindow
{
	public:
		TerrainPaletteWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void SetTileDialog(HWND hWnd);

	protected:
		void DoScroll(HWND hWnd);
		BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		u32 tilesetIndexedYC;
};

#endif