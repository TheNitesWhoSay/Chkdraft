#ifndef CHKDRAFT_H
#define CHKDRAFT_H
#include <windows/windows_ui.h>
#include <common_files/common_files.h>
#include <mapping_core/render/color_cycler.h>
#include <mapping_core/render/game_clock.h>
#include "cross_cut/logger.h"
#include "ui/dialog_windows/dialog_windows.h"
#include "mapping/chkd_plugins.h"
#include "mapping/chkd_profiles.h"
#include "ui/main_windows/main_toolbar.h"
#include "ui/main_windows/main_plot.h"
#include "ui/dialog_windows/new_map/new_map.h"
#include "ui/dialog_windows/profiles/edit_profiles.h"
#include "ui/main_windows/maps.h"
#include <optional>

struct GraphicsData;

class Chkdraft : public WinLib::ClassWindow
{
    public:
                    void OnLoadTest(); // Write testing code here
                    void PreLoadTest(); // Write testing code that runs before data & UI loads here

/*  Main Items  */  ChkdProfiles profiles {};
                    std::optional<Sc::Data> scData = std::make_optional<Sc::Data>(); // Data from StarCraft files
                    std::unique_ptr<GraphicsData> scrData; // Remastered graphics data from StarCraft files
                    ColorCycler colorCycler {}; // Graphics palette color cycler
                    Maps maps; // Main map container
                    GameClock gameClock {};

/* Dialog Boxes */  NewMap newMap; // New map DialogBox

/* Main Windows */  MainPlot mainPlot; // Holds left bar, assists sizing left bar and map MDI frame
                    WinLib::WindowMenu mainMenu; // Main window's menu
                    WinLib::StatusControl statusBar; // Main window's status bar
                    MainToolbar mainToolbar; // Main window's toolbar

/*   Modeless   */  std::optional<UnitPropertiesWindow> unitWindow; // Modeless unit properties
                    std::optional<SpritePropertiesWindow> spriteWindow; // Modeless sprite properties
                    std::optional<ActorPropertiesWindow> actorWindow; // Modeless actor properties
                    std::optional<LocationWindow> locationWindow; // Modeless location properties
                    std::optional<TerrainPaletteWindow> terrainPalWindow; // Modeless terrain palette
                    std::optional<TilePropWindow> tilePropWindow; // Modeless tile properties
                    std::optional<TextTrigWindow> textTrigWindow; // Modeless text triggers
                    std::optional<BriefingTextTrigWindow> briefingTextTrigWindow; // Modeless briefing text triggers
                    std::optional<MapSettingsWindow> mapSettingsWindow; // Modeless map settings
                    std::optional<TrigEditorWindow> trigEditorWindow; // Modeless trig editor
                    std::optional<BriefingTrigEditorWindow> briefingTrigEditorWindow; // Modeless briefing trig editor
                    std::optional<DimensionsWindow> dimensionsWindow; // Modeless dimensions window
                    std::optional<ChangePasswordDialog> changePasswordWindow; // Modeless password editor
                    std::optional<EnterPasswordDialog> enterPasswordWindow; // Modeless login window
                    std::optional<AboutWindow> aboutWindow; // Modeless about window
                    std::optional<EditProfilesWindow> editProfilesWindow; // Modeless edit profiles window

/* Constructors */  Chkdraft();
                    
/*  Destructor  */  virtual ~Chkdraft();

/*   Startup    */  int Run(LPSTR lpCmdLine, int nCmdShow);
                    void SetupLogging();

/* Quick Access */  bool EditFocused();
                    HWND CurrDialog();

/*   UI Accel   */  void SetCurrDialog(HWND hDialog);
                    void SetEditFocused(bool editFocused);
                    void SizeSubWindows();
                    void OpenLogFile();
                    void OpenLogFileDirectory();
                    void OpenBackupsDirectory();
                    void OpenFindProfileDialog();
                    void OpenEditProfilesDialog();
                    void UpdateLogLevelCheckmarks(LogLevel logLevel);
                    void SetLogLevel(LogLevel newLogLevel);
                    void OnProfileLoad();
                    void ProfilesReload(); // Reloads profile settings from disk & reloads the currently selected profile
                    void UpdateProfilesMenu(); // Doesn't remove existing menu items but adds new profiles and updates existing
                    void RemoveProfileFromMenu(ChkdProfile & profile);
                    void ProfileNameUpdated(const ChkdProfile & profile);

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
                    HMENU profilesMenu = NULL;
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