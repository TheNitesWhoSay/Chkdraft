#ifndef CHKDRAFT_H
#define CHKDRAFT_H
#include "../WindowsLib/WindowsUi.h"
#include "CommonFiles/CommonFiles.h"
#include "Windows/DialogWindows/DialogWindows.h"
#include "Mapping/ChkdPlugins.h"
#include "Windows/MainWindows/MainToolbar.h"
#include "Windows/MainWindows/MainPlot.h"
#include "Windows/DialogWindows/NewMap/NewMap.h"
#include "Windows/MainWindows/Maps.h"
#include "../CrossCutLib/Commander.h"

class Chkdraft : public WinLib::ClassWindow
{
    public:
                    void OnLoadTest(); // Write testing code here

/*  Main Items  */  Sc::Data scData; // Data from StarCraft files
                    Maps maps; // Main map container
                    Commander mainCommander; // Main commander used for mapping-data and mapping-data-related UI changes

/* Dialog Boxes */  NewMap newMap; // New map DialogBox

/* Main Windows */  MainPlot mainPlot; // Holds left bar, assists sizing left bar and map MDI frame
                    WinLib::WindowMenu mainMenu; // Main window's menu
                    WinLib::StatusControl statusBar; // Main window's status bar
                    MainToolbar mainToolbar; // Main window's toolbar

/*   Modeless   */  UnitPropertiesWindow unitWindow; // Modeless unit properties
                    LocationWindow locationWindow; // Modeless location properties
                    TerrainPaletteWindow terrainPalWindow; // Modeless terrain palette
                    TilePropWindow tilePropWindow; // Modeless tile properties
                    TextTrigWindow textTrigWindow; // Modeless text triggers
                    BriefingTextTrigWindow briefingTextTrigWindow; // Modeless briefing text triggers
                    LitWindow litWindow; // Modeless lit window
                    MapSettingsWindow mapSettingsWindow; // Modeless map settings
                    TrigEditorWindow trigEditorWindow; // Modeless trig editor
                    BriefingTrigEditorWindow briefingTrigEditorWindow; // Modeless briefing trig editor
                    ChangePasswordDialog changePasswordWindow; // Modeless password editor
                    EnterPasswordDialog enterPasswordWindow; // Modeless login window

/* Constructors */  Chkdraft();
                    
/*  Destructor  */  virtual ~Chkdraft();

/*   Startup    */  int Run(LPSTR lpCmdLine, int nCmdShow);
                    void SetupLogging();

/* Quick Access */  bool ChangesLocked(u16 mapId);
                    bool EditFocused();
                    HWND CurrDialog();

/*   UI Accel   */  void SetCurrDialog(HWND hDialog);
                    void SetEditFocused(bool editFocused);
                    void SizeSubWindows();
                    void OpenLogFile();
                    void OpenLogFileDirectory();
                    void OpenBackupsDirectory();
                    void UpdateLogLevelCheckmarks(LogLevel logLevel);
                    void SetLogLevel(LogLevel newLogLevel);

    protected:

/*    Setup     */  bool CreateThis();
                    void ParseCmdLine(LPSTR lpCmdLine);
                    void toggleLogger();

/*  Preprocess  */  bool DlgKeyListener(HWND hWnd, UINT & msg, WPARAM wParam, LPARAM lParam);
                    void KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*    Procs     */  LRESULT Command(HWND hWnd, WPARAM wParam, LPARAM lParam);
                    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

                    void HandleDroppedFile(const std::string & dropFilePath) override;
                    void NotifyButtonClicked(int idFrom, HWND hWndFrom) override;

    private:

/*     Data     */  HWND currDialog;
                    bool editFocused;
                    Logger logFile;
                    std::string logFilePath;

/* Priv Methods */  bool CreateSubWindows();
                    void MinimizeDialogs();
                    void RestoreDialogs();
                    void UseDragFile(const std::string & fileName);
                    void OpenMapSettings(u16 menuId);
                    void OpenWebPage(const std::string & address);
                    void ComboEditChanged(HWND hCombo, u16 comboId);
                    void ComboSelChanged(HWND hCombo, u16 comboId);
};

extern Logger logger;

/** The main instance of Chkdraft.
    By signaling that chkd is declared externally here,
    arbitrary .cpp files can include this header and
    access various parts of the program as needed */
extern Chkdraft chkd; // The main instance of Chkdraft

/** Current Map (CM); this define retrieves a
    pointer to the map which is currently activated
    (only one is activated at once) */
#define CM chkd.maps.currentlyActiveMap

#endif