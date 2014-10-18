#ifndef CHKDRAFT_H
#define CHKDRAFT_H
#include "Windows UI/WindowsUI.h"
#include "Common Files/CommonFiles.h"
#include "DialogWindows.h"
#include "ChkdPlugins.h"
#include "MainToolbar.h"
#include "MainPlot.h"
#include "NewMap.h"
#include "Maps.h"

class Chkdraft : public ClassWindow
{
	public:
					void OnLoadTest(); // Write testing code here

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

/*   Startup	*/	int Run(LPSTR lpCmdLine, int nCmdShow);

/* Quick Access */	bool ChangesLocked(u16 mapId);
					bool EditFocused();

/*	 UI Accel	*/	void SetCurrDialog(HWND hDialog);
					void SetEditFocused(bool editFocused);

	protected:

/*	  Setup		*/	bool CreateThis();
					void ParseCmdLine(LPSTR lpCmdLine);

/*  Preprocess	*/	bool DlgKeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					void KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:

/*	   Data     */	HWND currDialog;
					bool editFocused;

/* Priv Methods */	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
					bool CreateSubWindows();
					void MinimizeDialogs();
					void RestoreDialogs();
					void SizeSubWindows();
					void UseDragFile(const char* fileName);
					void OpenMapSettings(u16 menuId);
					void OpenWebPage(const char* address);
					void ComboEditChanged(HWND hCombo, u16 comboId);
					void ComboSelChanged(HWND hCombo, u16 comboId);
};

/** The main instance of Chkdraft.
	By signaling that chkd is declared externally here,
	arbitrary .cpp files can include this header and
	access various parts of the program as needed */
extern Chkdraft chkd; // The main instance of Chkdraft

#endif