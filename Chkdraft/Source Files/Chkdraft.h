#ifndef CHKDRAFT_H
#define CHKDRAFT_H
#include "Windows UI/WindowsUI.h"
#include "Common Files/CommonFiles.h"
#include "ChkdPlugins.h"
#include "DialogWindows.h"
#include "MainToolbar.h"
#include "MainPlot.h"
#include "GuiAccel.h"
#include "NewMap.h"
#include "Maps.h"

class Chkdraft : public ClassWindow
{
	public:

/*  Main Items  */	DATA scData; // Main data
					MAPS maps; // Main map container

/* Dialog Boxes */	NewMap newMap; // New map DialogBox

/* Main Windows */	MainPlot mainPlot; // Holds left bar, assists sizing left bar and map MDI frame
					WindowMenu mainMenu; // Main window's menu
					StatusControl statusBar; // Main window's status bar
					MainToolbar mainToolbar; // Main window's toolbar

/*   Modeless	*/	UnitWindow unitWindow;	// Modeless unit properties
					LocationWindow locationWindow; // Modeless location properties
					TerrainPaletteWindow terrainPalWindow; // Modeless terrain palette
					TilePropWindow tilePropWindow; // Modeless tile properties
					TextTrigWindow textTrigWindow; // Modeless text triggers
					MapSettingsWindow mapSettingsWindow; // Modeless map settings

/* Constructors */	Chkdraft();

/*   Startup	*/	int Run(LPSTR lpCmdLine);

/* Quick Access */	bool ChangesLocked(u16 mapId);
					bool EditFocused();


	protected:

/*	  Setup		*/	bool CreateThis();
					void ParseCmdLine(LPSTR lpCmdLine);

/*  Preprocess	*/	void DlgKeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					void KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:

/*	   Data     */	bool dlgHotkey;
					bool editFocused;

/* Priv Methods */	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

/** By signaling that chkd is declared externally here,
	arbitrary .cpp files can include this header and
	access various parts of the program as needed */
extern Chkdraft chkd;

#endif