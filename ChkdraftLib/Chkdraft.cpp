#include "Chkdraft.h"
#include "../CommanderLib/Logger.h"
#include "../CommanderLib/TestCommands.h"
#include "Mapping/DatFileBrowsers.h"
#include "Mapping/Settings.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <regex>
#include <winres.h>
#include <shellapi.h>

//extern Logger logger;

enum MainWindow {
    IDR_MAIN_TOOLBAR = ID_FIRST,
    IDR_MAIN_STATUS,
    IDR_MAIN_MDI,
    IDR_MAIN_PLOT,
    NextToLastId,
    ID_MDI_FIRSTCHILD = (NextToLastId+500) // Keep this higher than all other main window identifiers
};

#define ifmapopen(dothis) if ( CM != nullptr ) dothis;

void Chkdraft::OnLoadTest()
{
    /*if ( maps.OpenMap("C:\\Users\\Justin\\Desktop\\StarCraft 1.16.1\\Maps\\BroodWar\\Helms Deep AnnaModz 8.4.scx") )*/
    //maps.NewMap(128, 128, Tileset::TERRAIN_INSTALLATION, 0, 0);
    /*{
        //ShowWindow(getHandle(), SW_MAXIMIZE); // If a maximized window is desirable for testing

        trigEditorWindow.CreateThis(getHandle());
        trigEditorWindow.ChangeTab(3);

        OpenMapSettings(LOWORD(ID_SCENARIO_SOUNDEDITOR));
    }*/
}

Chkdraft::Chkdraft() : currDialog(NULL), editFocused(false), mainCommander(std::shared_ptr<Logger>(&logger, [](Logger*){})), logFile(nullptr, nullptr)
{

}

Chkdraft::~Chkdraft()
{

}

int Chkdraft::Run(LPSTR lpCmdLine, int nCmdShow)
{
    SetupLogging();
    if ( !CreateThis() )
        return 1;

    scData.Load(DatFileBrowserPtr(new ChkdDatFileBrowser()), ChkdDatFileBrowser::getDatFileDescriptors(), ChkdDatFileBrowser::getExpectedStarCraftDirectory());
    InitCommonControls();
    ShowWindow(getHandle(), nCmdShow);
    UpdateWindow();
    ParseCmdLine(lpCmdLine);
    GuiMap::SetAutoBackup(true);
    this->OnLoadTest();

    MSG msg = {};
    bool keepRunning = true;
    do
    {
        while ( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0 )
        {
            if ( msg.message == WM_QUIT )
                keepRunning = false;
            else
            {
                bool isDlgKey = DlgKeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                if ( ::IsDialogMessage(currDialog, &msg) == FALSE )
                {
                    ::TranslateMessage(&msg);
                    if ( !isDlgKey )
                        KeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    ::DispatchMessage(&msg);
                }
            }
        }

        if ( CM != nullptr && ColorCycler::CycleColors(CM->getTileset()) )
            CM->Redraw(false);

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid consuming a core

    } while ( keepRunning );

    return msg.wParam;
}

void Chkdraft::SetupLogging()
{
    std::string loggerPath;
    if ( GetLoggerPath(loggerPath) )
    {
        std::string logFilePath = loggerPath + Logger::getTimestamp();
        logger.info("Setting log file to: " + logFilePath);
        logFile.setOutputStream(std::shared_ptr<std::ostream>(new std::ofstream(logFilePath), [](std::ostream* os) {
            if ( os != nullptr ) { // Close and delete when output stream goes out of scope
                ((std::ofstream*)os)->close();
                delete os;
            }
        }));
        logger.setAggregator(logFile); // Forwards all logger messages to the log file, which will then save messages based on their importance
    }
    else
        WinLib::Message("Failed to get logger path, log file disabled!");
}

bool Chkdraft::CreateThis()
{
    if ( !ClassWindow::WindowClassIsRegistered("wcChkdraft") )
    {
        DWORD classStyle = 0;
        HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 32, 32, 0);
        HICON hIconSmall = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);
        std::string wcName = "wcChkdraft";
        if ( !ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, IDR_MAIN_MENU, wcName, hIconSmall, false) )
        {
            DestroyIcon(hIcon);
            DestroyCursor(hCursor);
            DestroyIcon(hIconSmall);
        }
    }

    std::string windowName = "Chkdraft " + GetFullVersionString();

    DWORD exStyle = 0;
    DWORD style = WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN;
    int windowX = CW_USEDEFAULT,
        windowY = CW_USEDEFAULT,
        windowWidth = CW_USEDEFAULT,
        windowHeight = CW_USEDEFAULT;
    HMENU id = NULL;

    return ClassWindow::CreateClassWindow(exStyle, windowName, style, windowX, windowY, windowWidth, windowHeight, NULL, id) && Chkdraft::CreateSubWindows();
}

bool Chkdraft::ChangesLocked(u16 mapId)
{
    GuiMapPtr map = maps.GetMap(mapId);
    return map != nullptr && map->changesLocked();
}

bool Chkdraft::EditFocused()
{
    return editFocused;
}

HWND Chkdraft::CurrDialog()
{
    return currDialog;
}

void Chkdraft::SetCurrDialog(HWND hDialog)
{
    if ( currDialog != hDialog )
        currDialog = hDialog;
}

void Chkdraft::SetEditFocused(bool editFocused)
{
    this->editFocused = editFocused;
}

bool Chkdraft::DlgKeyListener(HWND hWnd, UINT &msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_KEYDOWN:
            {
                switch ( wParam )
                {
                    case VK_TAB:
                        if ( GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() ||
                             GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() )
                        {
                            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
                            trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        else if ( GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() ||
                            GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() )
                        {
                            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
                            trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        break;
                    case VK_RETURN:
                        if ( GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() ||
                             GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() )
                        {
                            trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        else if ( GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() ||
                            GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() )
                        {
                            trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        if ( GetParent(hWnd) == unitWindow.getHandle() )
                        {
                            unitWindow.DestroyThis();
                            return true;
                        }
                        else if ( GetParent(hWnd) == locationWindow.getHandle() )
                        {
                            locationWindow.DestroyThis();
                            return true;
                        }
                        else if ( GetParent(hWnd) == enterPasswordWindow.getHandle())
                        {
                            enterPasswordWindow.ButtonLogin();
                            return true;
                        }
                        break;
                    case VK_DELETE:
                        if ( GetParent(hWnd) == unitWindow.getHandle() )
                        {
                            SendMessage(unitWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, 0), 0);
                            return true;
                        }
                        break;
                    case 'Z': case 'Y': case 'X': case 'C': case 'V':
                        if ( GetKeyState(VK_CONTROL) & 0x8000 && (
                            GetParent(hWnd) == unitWindow.getHandle() ||
                            GetParent(hWnd) == locationWindow.getHandle() ||
                            GetParent(hWnd) == terrainPalWindow.getHandle() ||
                            GetParent(hWnd) == tilePropWindow.getHandle() ) )
                        {
                            KeyListener(hWnd, msg, wParam, lParam);
                            return true;
                        }
                        break;
                }
            }
            break;
        case WM_KEYUP:
            if ( wParam == VK_SPACE && CM != nullptr && maps.clipboard.isPasting() )
            {
                UnlockCursor();
                return true;
            }
            break;
    }
    return false;
}

void Chkdraft::KeyListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_KEYDOWN:
            {
                switch ( wParam )
                {
                    case VK_SPACE: CM->LockCursor(); return; break;
                    case VK_DELETE: CM->deleteSelection(); return; break;
                    case VK_ESCAPE: maps.endPaste(); return; break;
                    case VK_RETURN: CM->ReturnKeyPress(); return; break;
                }

                if ( GetKeyState(VK_CONTROL) & 0x8000 ) // Control is down
                {
                    if ( GetKeyState(VK_SHIFT) & 0x8000 ) // Shift is down
                    {
                        switch ( wParam )
                        {
                            case 'S': maps.SaveCurr(true); return; break;
                        }
                    }
                    else // Only control
                    {
                        switch ( wParam )
                        {
                            case 'A': case 'C': case 'X': case 'V':
                                {
                                    switch ( wParam )
                                    {
                                        case 'A': CM->selectAll(); return; break;
                                        case 'C': maps.copy(); return; break;
                                        case 'X': maps.cut(); return; break;
                                        case 'V': maps.startPaste(false); return; break;
                                    }
                                }
                                break;
                            case 'D': maps.ChangeLayer(Layer::Doodads); return; break;
                            case 'E': FindLeaks(); return; break;
                            case 'F': maps.ChangeLayer(Layer::FogEdit); return; break;
                            case 'L': maps.ChangeLayer(Layer::Locations);return; break;
                            case 'N': newMap.CreateThis(getHandle()); return; break;
                            case 'O': maps.OpenMap(); return; break;
                            case 'R': maps.ChangeLayer(Layer::Sprites); return; break;
                            case 'S': maps.SaveCurr(false); return; break;
                            case 'T': maps.ChangeLayer(Layer::Terrain); return; break;
                            case 'U': maps.ChangeLayer(Layer::Units); return; break;
                            case 'Y': CM->redo(); return; break;
                            case 'Z': CM->undo(); return; break;
                            case VK_OEM_PLUS: maps.ChangeZoom(true); return; break;
                            case VK_OEM_MINUS: maps.ChangeZoom(false); return; break;
                            case VK_F4: maps.CloseActive(); return; break;
                            case VK_F6: maps.nextMdi(); return; break;
                        }
                    }
                }
            }
            break;

        case WM_SYSKEYDOWN: // Sent insted of WM_KEYUP when alt is pressed
            {
                switch ( wParam )
                {
                    case 'U': maps.SetGrid(8, 8); return; break;
                    case 'F': maps.SetGrid(16, 16); return; break;
                    case 'G': maps.SetGrid(32, 32); return; break;
                    case 'L': maps.SetGrid(64, 64); return; break;
                    case 'E': maps.SetGrid(128, 128); return; break;
                }
            }
            break;

        case WM_KEYUP:
            {
                switch ( wParam )
                {
                    case VK_SPACE:
                        if ( !maps.clipboard.isPasting() )
                            UnlockCursor();
                        return; break;
                }
            }
            break;
    }

    if ( CM && editFocused == false && GetActiveWindow() == getHandle() )
    {
        Layer layer = CM->getLayer();
        if ( layer == Layer::Units || layer == Layer::FogEdit || layer == Layer::Sprites )
        {
            u8 newPlayer;
            switch ( wParam )
            {
                case '1': newPlayer = 0 ; break;
                case '2': newPlayer = 1 ; break;
                case '3': newPlayer = 2 ; break;
                case '4': newPlayer = 3 ; break;
                case '5': newPlayer = 4 ; break;
                case '6': newPlayer = 5 ; break;
                case '7': newPlayer = 6 ; break;
                case '8': newPlayer = 7 ; break;
                case '9': newPlayer = 8 ; break;
                case '0': newPlayer = 9 ; break;
                case '-': newPlayer = 10; break;
                case '=': newPlayer = 11; break;
                default: return; break;
            }

            mainToolbar.playerBox.SetSel(newPlayer);
            maps.ChangePlayer(newPlayer);
        }
    }
}

void Chkdraft::ParseCmdLine(LPSTR lpCmdLine)
{
    int length = std::strlen(lpCmdLine);
    if ( length > 1 ) // Minimum length 2
    {
        if ( lpCmdLine[0] == '\"' )
        {
            lpCmdLine[length-1] = '\0';
            maps.OpenMap(&lpCmdLine[1]);
        }
        else
            maps.OpenMap(&lpCmdLine[0]);
    }
}

LRESULT Chkdraft::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
        // File
    case ID_FILE_NEW1: newMap.CreateThis(hWnd); break;
    case ID_FILE_OPEN1: maps.OpenMap(); break;
    case ID_FILE_CLOSE1: maps.CloseActive(); break;
    case ID_FILE_SAVE1: maps.SaveCurr(false); break;
    case ID_FILE_SAVEAS1: maps.SaveCurr(true); break;
    case ID_FILE_QUIT1: PostQuitMessage(0); break;

        // Edit
    case ID_EDIT_UNDO1: CM->undo(); break;
    case ID_EDIT_REDO1: CM->redo(); break;
    case ID_EDIT_CUT1: maps.cut(); break;
    case ID_EDIT_COPY1: maps.copy(); break;
    case ID_EDIT_PASTE1: maps.startPaste(true); break;
    case ID_EDIT_SELECTALL: CM->selectAll(); break;
    case ID_EDIT_DELETE: CM->deleteSelection(); break;
    case ID_EDIT_PROPERTIES: maps.properties(); break;

        // View
        // Grid
    case ID_GRID_ULTRAFINE: maps.SetGrid(8, 8); break;
    case ID_GRID_FINE: maps.SetGrid(16, 16); break;
    case ID_GRID_NORMAL: maps.SetGrid(32, 32); break;
    case ID_GRID_LARGE: maps.SetGrid(64, 64); break;
    case ID_GRID_EXTRALARGE: maps.SetGrid(128, 128); break;
    case ID_GRID_DISABLED: maps.SetGrid(0, 0); break;
        // Color
    case ID_COLOR_BLACK: maps.SetGridColor(0, 0, 0); break;
    case ID_COLOR_GREY: maps.SetGridColor(72, 72, 88); break;
    case ID_COLOR_WHITE: maps.SetGridColor(255, 255, 255); break;
    case ID_COLOR_GREEN: maps.SetGridColor(16, 252, 24); break;
    case ID_COLOR_RED: maps.SetGridColor(244, 4, 4); break;
    case ID_COLOR_BLUE: maps.SetGridColor(36, 36, 252); break;
    case ID_COLOR_OTHER: break;
        // Zoom
    case ID_ZOOM_400: CM->setZoom(defaultZooms[0]); break;
    case ID_ZOOM_300: CM->setZoom(defaultZooms[1]); break;
    case ID_ZOOM_200: CM->setZoom(defaultZooms[2]); break;
    case ID_ZOOM_150: CM->setZoom(defaultZooms[3]); break;
    case ID_ZOOM_100: CM->setZoom(defaultZooms[4]); break;
    case ID_ZOOM_66:  CM->setZoom(defaultZooms[5]); break;
    case ID_ZOOM_50:  CM->setZoom(defaultZooms[6]); break;
    case ID_ZOOM_33:  CM->setZoom(defaultZooms[7]); break;
    case ID_ZOOM_25:  CM->setZoom(defaultZooms[8]); break;
    case ID_ZOOM_10:  CM->setZoom(defaultZooms[9]); break;
        // Terrain
    case ID_TERRAIN_DISPLAYTILEELEVATIONS: CM->ToggleDisplayElevations(); break;
    case ID_TERRAIN_DISPLAYTILEVALUES: CM->ToggleTileNumSource(false); break;
    case ID_TERRAIN_DISPLAYTILEVALUESMTXM: CM->ToggleTileNumSource(true); break;

        // Editor
        // Units
    case ID_UNITS_UNITSSNAPTOGRID: CM->ToggleUnitSnap(); break;
    case ID_UNITS_ALLOWSTACK: CM->ToggleUnitStack(); break;

        // Locations
    case ID_LOCATIONS_SNAPTOTILE: CM->SetLocationSnap(GuiMap::LocationSnap::SnapToTile); break;
    case ID_LOCATIONS_SNAPTOACTIVEGRID: CM->SetLocationSnap(GuiMap::LocationSnap::SnapToGrid); break;
    case ID_LOCATIONS_NOSNAP: CM->SetLocationSnap(GuiMap::LocationSnap::NoSnap); break;
    case ID_LOCATIONS_LOCKANYWHERE: CM->ToggleLockAnywhere(); break;
    case ID_LOCATIONS_CLIPNAMES: CM->ToggleLocationNameClip(); break;

        // Cut Copy Paste
    case ID_CUTCOPYPASTE_FILLSIMILARTILES: maps.clipboard.toggleFillSimilarTiles(); break;

        // Scenario
    case ID_TRIGGERS_CLASSICMAPTRIGGERS: trigEditorWindow.CreateThis(getHandle()); break;
    case ID_SCENARIO_DESCRIPTION: case ID_SCENARIO_FORCES: case ID_SCENARIO_UNITSETTINGS:
    case ID_SCENARIO_UPGRADESETTINGS: case ID_SCENARIO_TECHSETTINGS: case ID_SCENARIO_STRINGS:
    case ID_SCENARIO_SOUNDEDITOR: OpenMapSettings(LOWORD(wParam)); break;

        // Tools
    case ID_TRIGGERS_TRIGGEREDITOR: textTrigWindow.CreateThis(getHandle()); break;
    case ID_TOOLS_LITTRIGGERS: ifmapopen(litWindow.CreateThis(getHandle())); break;
    case ID_TOOLS_PASSWORD: ifmapopen(changePasswordWindow.CreateThis(getHandle())) break;

        // Windows
    case ID_WINDOWS_CASCADE: maps.cascade(); break;
    case ID_WINDOWS_TILEHORIZONTALLY: maps.tileHorizontally(); break;
    case ID_WINDOWS_TILEVERTICALLY: maps.tileVertically(); break;
    case ID_WINDOW_CLOSE: maps.destroyActive(); break;

        // Help
    case ID_HELP_STARCRAFT_WIKI: OpenWebPage("http://www.staredit.net/wiki/index.php?title=Main_Page"); break;
    case ID_HELP_SUPPORT_FORUM: OpenWebPage("http://www.staredit.net/forums/"); break;
    case ID_HELP_CHKDRAFTGITHUB: OpenWebPage("https://github.com/jjf28/Chkdraft/"); break;
    case ID_HELP_CHKDRAFTTHREAD: OpenWebPage("http://www.staredit.net/topic/15514/"); break;

    default:
        switch ( HIWORD(wParam) )
        {
        case CBN_SETFOCUS: editFocused = true; break;
        case CBN_KILLFOCUS: editFocused = false; break;
        case CBN_EDITCHANGE: ComboEditChanged((HWND)lParam, LOWORD(wParam)); SetFocus(getHandle()); break;
        case CBN_SELCHANGE: ComboSelChanged((HWND)lParam, LOWORD(wParam)); SetFocus(getHandle()); break;
        default: return ClassWindow::Command(hWnd, wParam, lParam); break;
        }
        break;
    }
    return 0;
}

LRESULT Chkdraft::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_MENUCHAR:
            return MAKELPARAM(0, MNC_CLOSE);
            break;

        case WM_SIZE:
        case WM_SIZING:
            SizeSubWindows();
            break;

        case WM_SHOWWINDOW:
            {
                LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
                if ( wParam == TRUE )
                    ShowWindow(currDialog, SW_SHOW);

                return result;
            }
            break;

        case WM_SYSCOMMAND:
            if ( wParam == SC_RESTORE )
            {
                LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
                RestoreDialogs();
                return result;
            }
            else if ( wParam == SC_MINIMIZE )
                MinimizeDialogs();
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WM_CLOSE:
            while ( CM != nullptr )
            {
                if ( CM->CanExit() )
                    maps.destroyActive();
                else
                    return 0; // Abort close
            }
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            if ( msg == WM_COPYDATA || (msg >= PLUGIN_MSG_START && msg <= PLUGIN_MSG_END) )
                return CallWindowProc(PluginProc, hWnd, msg, wParam, lParam);
            else
                return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void Chkdraft::HandleDroppedFile(const std::string &dropFilePath)
{
    maps.OpenMap(dropFilePath);
}

bool Chkdraft::CreateSubWindows()
{
    HWND hWnd = getHandle();
    if ( hWnd != NULL )
    {
        DragAcceptFiles(hWnd, TRUE);
        int statusWidths[] = { 130, 205, 350, 450, 600, -1 };

        return mainMenu.FindThis(hWnd) &&
            mainToolbar.CreateThis(hWnd, IDR_MAIN_TOOLBAR) &&
            statusBar.CreateThis(sizeof(statusWidths) / sizeof(int), statusWidths, 0,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, hWnd, (HMENU)IDR_MAIN_STATUS) &&
            mainPlot.CreateThis(hWnd, IDR_MAIN_PLOT) &&
            BecomeMDIFrame(maps, GetSubMenu(GetMenu(hWnd), 6), ID_MDI_FIRSTCHILD,
                WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                0, 0, 0, 0, (HMENU)IDR_MAIN_MDI);
    }
    else
        return false;
}

void Chkdraft::MinimizeDialogs()
{
    ShowWindow(unitWindow.getHandle(), SW_HIDE);
    ShowWindow(locationWindow.getHandle(), SW_HIDE);
    ShowWindow(terrainPalWindow.getHandle(), SW_HIDE);
    ShowWindow(mapSettingsWindow.getHandle(), SW_HIDE);
    ShowWindow(trigEditorWindow.getHandle(), SW_HIDE);
}

void Chkdraft::RestoreDialogs()
{
    ShowWindow(unitWindow.getHandle(), SW_SHOW);
    ShowWindow(locationWindow.getHandle(), SW_SHOW);
    ShowWindow(terrainPalWindow.getHandle(), SW_SHOW);
    ShowWindow(mapSettingsWindow.getHandle(), SW_SHOW);
    ShowWindow(trigEditorWindow.getHandle(), SW_SHOW);
}

void Chkdraft::SizeSubWindows()
{
        RECT rcMain, rcTool, rcStatus, rcLeftBar;

        mainToolbar.AutoSize();
        statusBar.AutoSize();

        // Get the size of the client area, toolbar, status bar, and left bar
        GetClientRect(getHandle(), &rcMain);
        GetWindowRect(mainToolbar.getHandle(), &rcTool);
        GetWindowRect(statusBar.getHandle(), &rcStatus);
        GetWindowRect(mainPlot.leftBar.getHandle(), &rcLeftBar);

        int xBorder = GetSystemMetrics(SM_CXSIZEFRAME) - 1,
            yBorder = GetSystemMetrics(SM_CYSIZEFRAME) - 1;

        // Fit plot to the area between the toolbar and statusbar
        SetWindowPos(mainPlot.getHandle(), NULL, 0, rcTool.bottom - rcTool.top,
            rcMain.right - rcMain.left, rcMain.bottom - rcMain.top - (rcTool.bottom - rcTool.top) - (rcStatus.bottom - rcStatus.top),
            SWP_NOZORDER | SWP_NOACTIVATE);

        // Fit left bar to the area between the toolbar and statusbar without changing width
        SetWindowPos(mainPlot.leftBar.getHandle(), NULL, -xBorder, -yBorder,
            rcLeftBar.right - rcLeftBar.left, rcStatus.top - rcTool.bottom + yBorder * 2,
            SWP_NOZORDER | SWP_NOACTIVATE);

        // Fit the map MDIClient to the area right of the left bar and between the toolbar and statusbar
        SetWindowPos(maps.getHandle(), HWND_TOP, rcLeftBar.right - rcLeftBar.left - xBorder - 2, rcTool.bottom - rcTool.top,
            rcMain.right - rcMain.left - rcLeftBar.right + rcLeftBar.left + xBorder + 2, rcStatus.top - rcTool.bottom,
            SWP_NOACTIVATE);
    //}

    RedrawWindow(statusBar.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void Chkdraft::OpenMapSettings(u16 menuId)
{
    if ( mapSettingsWindow.getHandle() == NULL )
        mapSettingsWindow.CreateThis(getHandle());
                                    
    if ( mapSettingsWindow.getHandle() != NULL )
    {
        switch ( menuId )
        {
            case ID_SCENARIO_DESCRIPTION: mapSettingsWindow.ChangeTab(TAB_MAPPROPERTIES); break;
            case ID_SCENARIO_FORCES: mapSettingsWindow.ChangeTab(TAB_FORCES); break;
            case ID_SCENARIO_UNITSETTINGS: mapSettingsWindow.ChangeTab(TAB_UNITSETTINGS); break;
            case ID_SCENARIO_UPGRADESETTINGS: mapSettingsWindow.ChangeTab(TAB_UPGRADESETTINGS); break;
            case ID_SCENARIO_TECHSETTINGS: mapSettingsWindow.ChangeTab(TAB_TECHSETTINGS); break;
            case ID_SCENARIO_STRINGS: mapSettingsWindow.ChangeTab(TAB_STRINGEDITOR); break;
            case ID_SCENARIO_SOUNDEDITOR: mapSettingsWindow.ChangeTab(TAB_WAVEDITOR); break;
        }
        ShowWindow(mapSettingsWindow.getHandle(), SW_SHOW);
    }
}

void Chkdraft::OpenWebPage(const std::string &address)
{
    int resultCode = 0;
    WinLib::executeOpen(address, resultCode);
}

void Chkdraft::ComboEditChanged(HWND hCombo, u16 comboId)
{
    if ( hCombo = mainToolbar.playerBox.getHandle() )
    {
        u8 newPlayer;
        if ( mainToolbar.playerBox.GetPlayerNum(newPlayer) )
            maps.ChangePlayer(newPlayer);
    }
}

void Chkdraft::ComboSelChanged(HWND hCombo, u16 comboId)
{
    int itemIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if ( hCombo == mainToolbar.layerBox.getHandle() )
        maps.ChangeLayer((Layer)itemIndex);
    else if ( hCombo == mainToolbar.playerBox.getHandle() )
        maps.ChangePlayer(itemIndex);
    else if ( hCombo == mainToolbar.zoomBox.getHandle() )
    {
        if ( itemIndex >= 0 && itemIndex < 10 )
            CM->setZoom(defaultZooms[itemIndex]);
    }
    else if ( hCombo == mainToolbar.terrainBox.getHandle() )
    {
        if ( itemIndex == 3 )
        {
            terrainPalWindow.CreateThis(getHandle());
            ShowWindow(terrainPalWindow.getHandle(), SW_SHOW);
        }
    }
}
