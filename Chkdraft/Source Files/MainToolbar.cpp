#include "MainToolbar.h"

bool MainToolbar::CreateThis(HWND hParent)
{
	// Initilize primary toolbar
	if ( WindowControl::CreateControl( NULL, TOOLBARCLASSNAME, NULL, WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS,
									   0, 0, 0, 0, hParent, (HMENU)IDR_MAIN_TOOLBAR, false ) )
	{
		HWND hTool = getHandle();
		SendMessage(hTool, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
		SendMessage(hTool, TB_SETMAXTEXTROWS, 0, 0);

		// Set toolbar to default button set
		TBADDBITMAP tbab;
		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;
		SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		// Load Save-As image from resource and store the resulting index as STD_FILESAVEAS
		HIMAGELIST hImages = ImageList_Create(16, 16, NULL, 1, 1);
		HBITMAP hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SAVEAS), IMAGE_BITMAP, 16, 16, LR_DEFAULTCOLOR);
		ImageList_Add(hImages, hBmp, NULL);
		TBADDBITMAP tb; tb.hInst = NULL; tb.nID = (UINT_PTR)hBmp;
		int STD_FILESAVEAS = SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tb);

		// Add buttons to toolbar
		TBBUTTON tbb[13];
		ZeroMemory(tbb, sizeof(TBBUTTON));
		tbb[ 0].iBitmap = STD_FILENEW;	  tbb[ 0].fsState = TBSTATE_ENABLED; tbb[ 0].fsStyle = TBSTYLE_BUTTON; tbb[ 0].idCommand = ID_FILE_NEW1;	   tbb[ 0].iString = (INT_PTR)"New";
		tbb[ 1].iBitmap = STD_FILEOPEN;   tbb[ 1].fsState = TBSTATE_ENABLED; tbb[ 1].fsStyle = TBSTYLE_BUTTON; tbb[ 1].idCommand = ID_FILE_OPEN1;	   tbb[ 1].iString = (INT_PTR)"Open";
		tbb[ 2].iBitmap = STD_FILESAVE;   tbb[ 2].fsState = NULL;			 tbb[ 2].fsStyle = TBSTYLE_BUTTON; tbb[ 2].idCommand = ID_FILE_SAVE1;	   tbb[ 2].iString = (INT_PTR)"Save";
		tbb[ 3].iBitmap = STD_FILESAVEAS; tbb[ 3].fsState = NULL;			 tbb[ 3].fsStyle = TBSTYLE_BUTTON; tbb[ 3].idCommand = ID_FILE_SAVEAS1;	   tbb[ 3].iString = (INT_PTR)"Save As";
		tbb[ 4].iBitmap = 8;			  tbb[ 4].fsState = NULL;			 tbb[ 4].fsStyle = BTNS_SEP;
		tbb[ 5].iBitmap = STD_UNDO;		  tbb[ 5].fsState = NULL;			 tbb[ 5].fsStyle = TBSTYLE_BUTTON; tbb[ 5].idCommand = ID_EDIT_UNDO1;	   tbb[ 5].iString = (INT_PTR)"Undo";
		tbb[ 6].iBitmap = STD_REDOW;	  tbb[ 6].fsState = NULL;			 tbb[ 6].fsStyle = TBSTYLE_BUTTON; tbb[ 6].idCommand = ID_EDIT_REDO1;	   tbb[ 6].iString = (INT_PTR)"Redo";
		tbb[ 7].iBitmap = STD_PROPERTIES; tbb[ 7].fsState = NULL;			 tbb[ 7].fsStyle = TBSTYLE_BUTTON; tbb[ 7].idCommand = ID_EDIT_PROPERTIES; tbb[ 7].iString = (INT_PTR)"Properties";
		tbb[ 8].iBitmap = STD_DELETE;	  tbb[ 8].fsState = NULL;			 tbb[ 8].fsStyle = TBSTYLE_BUTTON; tbb[ 8].idCommand = ID_EDIT_DELETE;	   tbb[ 8].iString = (INT_PTR)"Delete";
		tbb[ 9].iBitmap = 8;			  tbb[ 9].fsState = NULL;			 tbb[ 9].fsStyle = BTNS_SEP;
		tbb[10].iBitmap = STD_COPY;		  tbb[10].fsState = NULL;			 tbb[10].fsStyle = TBSTYLE_BUTTON; tbb[10].idCommand = ID_EDIT_COPY1;	   tbb[10].iString = (INT_PTR)"Copy";
		tbb[11].iBitmap = STD_CUT;		  tbb[11].fsState = NULL;			 tbb[11].fsStyle = TBSTYLE_BUTTON; tbb[11].idCommand = ID_EDIT_CUT1;	   tbb[11].iString = (INT_PTR)"Cut";
		tbb[12].iBitmap = STD_PASTE;	  tbb[12].fsState = NULL;			 tbb[12].fsStyle = TBSTYLE_BUTTON; tbb[12].idCommand = ID_EDIT_PASTE1;	   tbb[12].iString = (INT_PTR)"Paste";
		SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

		// Add layer ComboBox to toolbar
		const char* layerStrings[] = { "Terrain", "Doodads", "Fog of War", "Locations", "Units", "Sprites", "Preview Fog",
									   "Copy\\Cut\\Paste\\Brush" };
		layerBox.CreateThis(hTool, 277, 2, 110, 200, false, ID_COMBOBOX_LAYER, 8, layerStrings, defaultFont);
		layerBox.SetSel(0);
		ShowWindow(layerBox.getHandle(), SW_HIDE); // Hide until a map is open

		// Add zoom ComboBox to toolbar
		const char* zoomStrings[] = { "400% Zoom", "300% Zoom", "200% Zoom", "150% Zoom", "100% Zoom", "66% Zoom",
									  "50% Zoom", "33% Zoom", "25% Zoom", "10% Zoom"};
		zoomBox.CreateThis(hTool, 395, 2, 80, 200, false, ID_COMBOBOX_ZOOM, 10, zoomStrings, defaultFont);
		zoomBox.SetSel(4);
		ShowWindow(zoomBox.getHandle(), SW_HIDE); // Hide until a map is open

		// Add player ComboBox to toolbar
		const char* playerStrings[] = { "Player 01", "Player 02", "Player 03", "Player 04", "Player 05", "Player 06",
										"Player 07", "Player 08", "Player 09", "Player 10", "Player 11", "Player 12 (Neutral)" };
		playerBox.CreateThis(hTool, 483, 2, 75, 200, true, ID_COMBOBOX_PLAYER, 12, playerStrings, defaultFont);
		playerBox.SetSel(0);
		playerBox.ClearEditSel();
		ShowWindow(playerBox.getHandle(), SW_HIDE); // Hide until a map is open

		// Add terrain ComboBox to toolbar			
		const char* terrPalette[] = { "Isometrical", "Rectangular", "Subtile", "Tileset Indexed", "Copy/Cut/Paste" };
		terrainBox.CreateThis(hTool, 483, 2, 100, 200, false, ID_COMBOBOX_TERRAIN, 5, terrPalette, defaultFont);
		terrainBox.SetSel(0);
		ShowWindow(terrainBox.getHandle(), SW_HIDE); // Hide until a map is open

		return true;
	}
	else
		return false;
}

void MainToolbar::AutoSize()
{
	SendMessage(getHandle(), TB_AUTOSIZE, NULL, NULL);
}
