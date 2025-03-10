#include "chkdraft.h"
#include <cross_cut/logger.h>
#include <cross_cut/test_commands.h>
#include "mapping/data_file_browsers.h"
#include "mapping/settings.h"
#include "mapping/scr_graphics.h"
#include <shellapi.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <regex>
#include "ui/chkd_controls/chkd_string_input.h"
#include <CommCtrl.h>

void Chkdraft::OnLoadTest()
{
    /*auto & map = []() -> GuiMap & {
        auto map = chkd.maps.NewMap();
        map->addUnit(Chk::Unit {map->getNextClassId(), 64, 64, Sc::Unit::Type::StartLocation, 0, 0, 0, Sc::Player::Id::Player1});
        map->addUnit(Chk::Unit {map->getNextClassId(), 192, 64, Sc::Unit::Type::StartLocation, 0, 0, 0, Sc::Player::Id::Player2});
        map->setForceFlags(Chk::Force::Force1, Chk::ForceFlags::All & Chk::ForceFlags::xRandomizeStartLocation);
        map->setForceFlags(Chk::Force::Force2, Chk::ForceFlags::All & Chk::ForceFlags::xRandomizeStartLocation);
        map->setPlayerForce(Sc::Player::Id::Player2, Chk::Force::Force2);
        map->setSlotType(1, Sc::Player::SlotType::Computer);
        _Pragma("warning(suppress: 26716)") return *map;
    }();*/
}

enum_t(Id, u32, {
    IDR_MAIN_TOOLBAR = ID_FIRST,
    IDR_MAIN_STATUS,
    IDR_MAIN_MDI,
    IDR_MAIN_PLOT,
    NextToLastId,
    ID_MDI_FIRSTCHILD = (NextToLastId+500) // Keep this higher than all other main window identifiers
});

#define ifmapopen(dothis) if ( CM != nullptr ) dothis;

Chkdraft::Chkdraft() : currDialog(NULL), editFocused(false), mainCommander(std::shared_ptr<Logger>(&logger, [](Logger*){})), logFile(nullptr, nullptr, logger.getLogLevel())
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

    InitCommonControls();
    UpdateLogLevelCheckmarks(logger.getLogLevel());
    ShowWindow(getHandle(), nCmdShow);
    logFile.setAggregator(nullptr); // Stop aggregating messages to std::out
#ifdef HAS_CONSOLE
    Console::setVisible(false); // Remove the console
#endif
    mainPlot.loggerWindow.Refresh();
    UpdateWindow();

    scData.load(Sc::DataFile::BrowserPtr(new ChkdDataFileBrowser()), ChkdDataFileBrowser::getDataFileDescriptors(), ChkdDataFileBrowser::getExpectedStarCraftDirectory());
    mainPlot.leftBar.mainTree.spriteTree.UpdateSpriteTree();
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
#ifndef _DEBUG
                try
                {
#endif
                    bool isDlgKey = DlgKeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                    if ( ::IsDialogMessage(currDialog, &msg) == FALSE )
                    {
                        ::TranslateMessage(&msg);
                        if ( !isDlgKey )
                            KeyListener(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                        ::DispatchMessage(&msg);
                    }
#ifndef _DEBUG
                }
                catch ( std::exception & e )
                {
                    logger.fatal() << "Unhandled exception: " << e.what() << std::endl;
                    throw;
                }
#endif
            }
        }


        if ( CM != nullptr && CM->GetSkin() == GuiMap::Skin::ClassicGDI && colorCycler.cycleColors(GetTickCount64(), CM->getTileset(), CM->getPalette()) )
            CM->Redraw(false);
        else if ( CM != nullptr && CM->GetSkin() != GuiMap::Skin::ClassicGDI && CM->UpdateGlGraphics() )
            CM->Redraw(false);

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Avoid consuming a core

    } while ( keepRunning );

    return (int)msg.wParam;
}

void Chkdraft::SetupLogging()
{
    if ( auto loggerPath = GetLoggerPath() )
    {
        logFilePath = *loggerPath + Logger::getTimestamp() + ".log";
        std::shared_ptr<Logger> stdOut = std::shared_ptr<Logger>(new Logger(logger.getLogLevel()));
        logger.setAggregator(stdOut);
        logger.setOutputStream(mainPlot.loggerWindow);
        logger.info("Setting log file to: " + logFilePath);
        logFile.setOutputStream(std::shared_ptr<std::ostream>(new std::ofstream(std::filesystem::path(asUtf8(logFilePath))), [](std::ostream* os) {
            if ( os != nullptr ) { // Close and delete when output stream goes out of scope
                ((std::ofstream*)os)->close();
                delete os;
            }
        }));
        logFile.setAggregator(stdOut);
        logger.setAggregator(logFile); // Forwards all logger messages to the log file, which will then save messages based on their importance
        logger.info() << "Chkdraft version: " << GetFullVersionString() << std::endl;
    }
    else
        WinLib::Message("Failed to get logger path, log file disabled!");
}

bool Chkdraft::CreateThis()
{
    if ( !ClassWindow::WindowClassIsRegistered("wcChkdraft") )
    {
        DWORD classStyle = 0;
        HICON hIcon = WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 32, 32);
        HICON hIconSmall = WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16);
        HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
        HBRUSH hBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);
        std::string wcName = "wcChkdraft";
        if ( !ClassWindow::RegisterWindowClass(classStyle, hIcon, hCursor, hBackground, IDR_MAIN_MENU, wcName, hIconSmall, false) )
            return false;
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

void Chkdraft::SizeSubWindows()
{
    mainToolbar.AutoSize();
    statusBar.AutoSize();

    // Get the size of the client area, toolbar, status bar, and left bar
    RECT rcMain, rcTool, rcStatus, rcLeftBar;
    GetClientRect(getHandle(), &rcMain);
    GetWindowRect(mainToolbar.getHandle(), &rcTool);
    GetWindowRect(statusBar.getHandle(), &rcStatus);
    GetWindowRect(mainPlot.leftBar.getHandle(), &rcLeftBar);

    int xBorder = GetSystemMetrics(SM_CXSIZEFRAME),
        yBorder = GetSystemMetrics(SM_CYSIZEFRAME);

    // Fit plot to the area between the toolbar and statusbar
    SetWindowPos(mainPlot.getHandle(), NULL, 0, rcTool.bottom - rcTool.top,
        rcMain.right - rcMain.left, rcMain.bottom - rcMain.top - (rcTool.bottom - rcTool.top) - (rcStatus.bottom - rcStatus.top),
        SWP_NOZORDER | SWP_NOACTIVATE);

    // Fit left bar to the area between the toolbar and statusbar without changing width
    SetWindowPos(mainPlot.leftBar.getHandle(), NULL, 1-xBorder, 1-yBorder,
        rcLeftBar.right - rcLeftBar.left, rcStatus.top - rcTool.bottom + (yBorder-1) * 2,
        SWP_NOZORDER | SWP_NOACTIVATE);

    // Fit logger to the area between the left bar and right edge without changing the height
    SetWindowPos(mainPlot.loggerWindow.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - 3*xBorder,
        rcMain.bottom-rcMain.top+2*yBorder-1-mainPlot.loggerWindow.Height()-(rcStatus.bottom-rcStatus.top)-(rcTool.bottom-rcTool.top),
        rcMain.right - rcMain.left - (rcLeftBar.right - rcLeftBar.left) + 4*xBorder+5, mainPlot.loggerWindow.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

    // Fit the map MDIClient to the area right of the left bar and between the toolbar and logger
    SetWindowPos(maps.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - xBorder + 1, rcTool.bottom - rcTool.top,
        rcMain.right - rcMain.left - rcLeftBar.right + rcLeftBar.left + xBorder - 1, mainPlot.loggerWindow.IsVisible() ? mainPlot.loggerWindow.Top() : rcStatus.top - rcTool.bottom,
        SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(maps.getHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

    RedrawWindow(statusBar.getHandle(), NULL, NULL, RDW_INVALIDATE);
}

void Chkdraft::OpenLogFile()
{
    int result = 0;
    WinLib::executeOpen(logFilePath, result);
}

void Chkdraft::OpenLogFileDirectory()
{
    int result = 0;
    WinLib::executeOpen(getSystemFileDirectory(logFilePath, false), result);
}

void Chkdraft::OpenBackupsDirectory()
{
    if (auto backupsFolderPath = GetBackupsPath()) {
        int result = 0;
        WinLib::executeOpen(*backupsFolderPath, result);
    }
}

void Chkdraft::UpdateLogLevelCheckmarks(LogLevel logLevel)
{
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_OFF, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_FATAL, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_ERROR, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_WARN, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_INFO, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_DEBUG, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_TRACE, false);
    chkd.mainMenu.SetCheck(ID_LOGLEVEL_ALL, false);

    switch ( logLevel )
    {
        case LogLevel::Off: chkd.mainMenu.SetCheck(ID_LOGLEVEL_OFF, true); break;
        case LogLevel::Fatal: chkd.mainMenu.SetCheck(ID_LOGLEVEL_FATAL, true); break;
        case LogLevel::Error: chkd.mainMenu.SetCheck(ID_LOGLEVEL_ERROR, true); break;
        case LogLevel::Warn: chkd.mainMenu.SetCheck(ID_LOGLEVEL_WARN, true); break;
        case LogLevel::Info: chkd.mainMenu.SetCheck(ID_LOGLEVEL_INFO, true); break;
        case LogLevel::Debug: chkd.mainMenu.SetCheck(ID_LOGLEVEL_DEBUG, true); break;
        case LogLevel::Trace: chkd.mainMenu.SetCheck(ID_LOGLEVEL_TRACE, true); break;
        case LogLevel::All: chkd.mainMenu.SetCheck(ID_LOGLEVEL_ALL, true); break;
    }
}

void Chkdraft::SetLogLevel(LogLevel newLogLevel)
{
    logger.setLogLevel(newLogLevel);
    Settings::logLevel = newLogLevel;
    Settings::updateSettingsFile();
    UpdateLogLevelCheckmarks(newLogLevel);
}

bool Chkdraft::DlgKeyListener(HWND hWnd, UINT & msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_KEYDOWN:
            {
                switch ( wParam )
                {
                    case VK_TAB:
                        if ( trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() != NULL &&
                            (GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle() ||
                             GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.getHandle()) )
                        {
                            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
                            trigEditorWindow.triggersWindow.trigModifyWindow.conditionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        else if ( trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() != NULL &&
                            (GetParent(GetParent(hWnd)) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle() ||
                            GetParent(hWnd) == trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.getHandle()) )
                        {
                            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
                            trigEditorWindow.triggersWindow.trigModifyWindow.actionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        else if ( briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.getHandle() != NULL &&
                            (GetParent(GetParent(hWnd)) == briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.getHandle() ||
                            GetParent(hWnd) == briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.getHandle()) )
                        {
                            msg = WM_NULL; // Dirty fix to prevent tabs from being focused
                            briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.ProcessKeyDown(wParam, lParam);
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
                        else if ( GetParent(GetParent(hWnd)) == briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.getHandle() ||
                            GetParent(hWnd) == briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.getHandle() )
                        {
                            briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.briefingActionsWindow.ProcessKeyDown(wParam, lParam);
                            return true;
                        }
                        if ( GetParent(hWnd) == unitWindow.getHandle() )
                        {
                            unitWindow.DestroyThis();
                            return true;
                        }
                        else if ( GetParent(hWnd) == spriteWindow.getHandle() )
                        {
                            spriteWindow.DestroyThis();
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
                        else if ( GetParent(hWnd) == spriteWindow.getHandle() )
                        {
                            SendMessage(spriteWindow.getHandle(), WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DELETE, 0), 0);
                            return true;
                        }
                        break;
                    case 'Z': case 'Y': case 'X': case 'C': case 'V':
                        if ( GetKeyState(VK_CONTROL) & 0x8000 && (
                            GetParent(hWnd) == unitWindow.getHandle() ||
                            GetParent(hWnd) == spriteWindow.getHandle() ||
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
                    case VK_SPACE: if ( CM != nullptr ) CM->LockCursor(); return; break;
                    case VK_DELETE: if ( CM != nullptr ) CM->deleteSelection(); return; break;
                    case VK_ESCAPE: if ( CM != nullptr ) { maps.endPaste(); CM->clearSelection(); } return; break;
                    case VK_RETURN: if ( CM != nullptr ) CM->ReturnKeyPress(); return; break;
                }

                if ( GetKeyState(VK_CONTROL) & 0x8000 ) // Control is down
                {
                    if ( GetKeyState(VK_SHIFT) & 0x8000 ) // Shift is down
                    {
                        switch ( wParam )
                        {
                            case 'S': if ( CM != nullptr ) maps.SaveCurr(true); return; break;
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
                                        case 'A': if ( CM != nullptr ) CM->selectAll(); return; break;
                                        case 'C': if ( CM != nullptr ) maps.copy(); return; break;
                                        case 'X': if ( CM != nullptr ) maps.cut(); return; break;
                                        case 'V': if ( CM != nullptr ) maps.startPaste(false); return; break;
                                    }
                                }
                                break;
                            case 'D': if ( CM != nullptr ) maps.ChangeLayer(Layer::Doodads); return; break;
                            case 'E': FindLeaks(); return; break;
                            case 'F': if ( CM != nullptr ) maps.ChangeLayer(Layer::FogEdit); return; break;
                            case 'L': if ( CM != nullptr ) maps.ChangeLayer(Layer::Locations);return; break;
                            case 'N': newMap.CreateThis(getHandle()); return; break;
                            case 'O': maps.OpenMap(); return; break;
                            case 'R': if ( CM != nullptr ) maps.ChangeLayer(Layer::Sprites); return; break;
                            case 'S': if ( CM != nullptr ) maps.SaveCurr(false); return; break;
                            case 'T': if ( CM != nullptr ) maps.ChangeLayer(Layer::Terrain); return; break;
                            case 'U': if ( CM != nullptr ) maps.ChangeLayer(Layer::Units); return; break;
                            case 'Y': if ( CM != nullptr ) CM->redo(); return; break;
                            case 'Z': if ( CM != nullptr ) CM->undo(); return; break;
                            case VK_OEM_PLUS: if ( CM != nullptr ) maps.ChangeZoom(true); return; break;
                            case VK_OEM_MINUS: if ( CM != nullptr ) maps.ChangeZoom(false); return; break;
                            case VK_F4: if ( CM != nullptr ) maps.CloseActive(); return; break;
                            case VK_F6: if ( CM != nullptr ) maps.nextMdi(); return; break;
                            case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
                                if ( GetParent(hWnd) == maps.getHandle() )
                                {
                                    CM->moveSelection([&]() -> Direction {
                                        switch ( wParam ) {
                                            case VK_UP: return Direction::Up;
                                            case VK_DOWN: return Direction::Down;
                                            case VK_LEFT: return Direction::Left;
                                            case VK_RIGHT: return Direction::Right;
                                            default: throw std::exception();
                                        }
                                    }(), true);
                                }
                                break;
                        }
                    }
                }
                else if ( GetKeyState(VK_SHIFT) & 0x8000 ) // Shift is down
                {
                    switch ( wParam )
                    {
                    case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
                        if ( GetParent(hWnd) == maps.getHandle() )
                        {
                            CM->moveSelection([&]() -> Direction {
                                switch ( wParam ) {
                                    case VK_UP: return Direction::Up;
                                    case VK_DOWN: return Direction::Down;
                                    case VK_LEFT: return Direction::Left;
                                    case VK_RIGHT: return Direction::Right;
                                    default: throw std::exception();
                                }
                            }(), false);
                        }
                        break;
                    }
                }
            }
            break;

        case WM_SYSKEYDOWN: // Sent insted of WM_KEYUP when alt is pressed
            {
                switch ( wParam )
                {
                    case '1': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::ClassicGDI); return; break;
                    case '2': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::ClassicGL); return; break;
                    case '3': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::ScrSD); return; break;
                    case '4': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::ScrHD2); return; break;
                    case '5': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::ScrHD); return; break;
                    case '6': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::CarbotHD2); return; break;
                    case '7': if ( CM != nullptr ) CM->SetSkin(GuiMap::Skin::CarbotHD); return; break;
                    case 'U': if ( CM != nullptr ) maps.SetGrid(8, 8); return; break;
                    case 'F': if ( CM != nullptr ) maps.SetGrid(16, 16); return; break;
                    case 'G': if ( CM != nullptr ) maps.SetGrid(32, 32); return; break;
                    case 'L': if ( CM != nullptr ) maps.SetGrid(64, 64); return; break;
                    case 'E': if ( CM != nullptr ) maps.SetGrid(128, 128); return; break;
                }
            }
            break;

        case WM_KEYUP:
            {
                switch ( wParam )
                {
                    case VK_SPACE:
                        if ( CM != nullptr && !maps.clipboard.isPasting() )
                            UnlockCursor();
                        return; break;
                }
            }
            break;
    }

    if ( CM != nullptr && editFocused == false && GetActiveWindow() == getHandle() )
    {
        Layer layer = CM->getLayer();
        if ( layer == Layer::Units || layer == Layer::FogEdit || layer == Layer::Sprites || layer == Layer::Doodads )
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
    size_t length = std::strlen(lpCmdLine);
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

void Chkdraft::toggleLogger()
{
#ifdef HAS_CONSOLE
    Console::toggleVisible();
#endif
}

LRESULT Chkdraft::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
        // File
    case ID_FILE_NEW1: newMap.CreateThis(hWnd); break;
    case ID_FILE_OPEN1: maps.OpenMap(); break;
    case ID_ADVANCED_OPENBACKUPDATABASE: OpenBackupsDirectory(); break;
    case ID_FILE_CLOSE1: maps.CloseActive(); break;
    case ID_FILE_SAVE1: maps.SaveCurr(false); break;
    case ID_FILE_SAVEAS1: maps.SaveCurr(true); break;
    case ID_FILE_QUIT1: PostQuitMessage(0); break;

        // Edit
    case ID_EDIT_UNDO1: CM->undo(); break;
    case ID_EDIT_REDO1: CM->redo(); break;
    case ID_EDIT_CUT1: maps.cut(); break;
    case ID_EDIT_COPY1: maps.copy(); break;
    case ID_EDIT_PASTE1: maps.startPaste(false); break;
    case ID_EDIT_SELECTALL: CM->selectAll(); break;
    case ID_EDIT_DELETE: CM->deleteSelection(); break;
    case ID_EDIT_CLEARSELECTIONS: CM->clearSelection(); break;
    case ID_EDIT_CONVERTTOTERRAIN: CM->convertSelectionToTerrain(); break;
    case ID_EDIT_STACKSELECTED: CM->stackSelected(); break;
    case ID_EDIT_CREATELOCATION: CM->createLocation(); break;
    case ID_EDIT_CREATEINVERTEDLOCATION: CM->createInvertedLocation(); break;
    case ID_EDIT_CREATEMOBILEINVERTEDLOCATION: CM->createMobileInvertedLocation(); break;
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
        // Skin
    case ID_SKIN_CLASSICGDI: CM->SetSkin(GuiMap::Skin::ClassicGDI); break;
    case ID_SKIN_CLASSICOPENGL: CM->SetSkin(GuiMap::Skin::ClassicGL); break;
    case ID_SKIN_REMASTEREDSD: CM->SetSkin(GuiMap::Skin::ScrSD); break;
    case ID_SKIN_REMASTEREDHD2: CM->SetSkin(GuiMap::Skin::ScrHD2); break;
    case ID_SKIN_REMASTEREDHD: CM->SetSkin(GuiMap::Skin::ScrHD); break;
    case ID_SKIN_CARBOTHD2: CM->SetSkin(GuiMap::Skin::CarbotHD2); break;
    case ID_SKIN_CARBOTHD: CM->SetSkin(GuiMap::Skin::CarbotHD); break;
        // Terrain
    case ID_TERRAIN_DISPLAYTILEBUILDABILITY: CM->ToggleDisplayBuildability(); break;
    case ID_TERRAIN_DISPLAYTILEELEVATIONS: CM->ToggleDisplayElevations(); break;
    case ID_TERRAIN_DISPLAYTILEVALUES: CM->ToggleTileNumSource(false); break;
    case ID_TERRAIN_DISPLAYTILEVALUESMTXM: CM->ToggleTileNumSource(true); break;
    case ID_TERRAIN_DISPLAYISOMVALUES: CM->ToggleDisplayIsomValues(); break;
        // View
    case ID_VIEW_DISPLAYFPS: CM->ToggleDisplayFps(); break;

        // Editor
        // Units
    case ID_UNITS_BUILDINGSSNAPTOTILE: CM->ToggleBuildingsSnapToTile(); break;
    case ID_UNITS_UNITSSNAPTOGRID: CM->ToggleUnitSnap(); break;
    case ID_UNITS_ALLOWSTACK: CM->ToggleUnitStack(); break;
    case ID_UNITS_ENABLEAIRSTACK: CM->ToggleEnableAirStack(); break;
    case ID_UNITS_PLACEUNITSANYWHERE: CM->TogglePlaceUnitsAnywhere(); break;
    case ID_UNITS_PLACEBUILDINGSANYWHERE: CM->TogglePlaceBuildingsAnywhere(); break;
    case ID_UNITS_ADDONAUTOPLAYERSWAP: CM->ToggleAddonAutoPlayerSwap(); break;
    case ID_UNITS_REQUIREMINERALDISTANCE: CM->ToggleRequireMineralDistance(); break;

        // Logger
    case ID_LOGGER_TOGGLELOGGER: mainPlot.loggerWindow.ToggleVisible(); break;
    case ID_LOGGER_TOGGLELINENUMBERS: mainPlot.loggerWindow.ToggleLineNumbers(); break;
    case ID_LOGGER_OPENLOGFILE: OpenLogFile(); break;
    case ID_LOGGER_OPENLOGFILEDIRECTORY: OpenLogFileDirectory(); break;
    case ID_LOGLEVEL_OFF: chkd.SetLogLevel(LogLevel::Off); break;
    case ID_LOGLEVEL_FATAL: chkd.SetLogLevel(LogLevel::Fatal); break;
    case ID_LOGLEVEL_ERROR: chkd.SetLogLevel(LogLevel::Error); break;
    case ID_LOGLEVEL_WARN: chkd.SetLogLevel(LogLevel::Warn); break;
    case ID_LOGLEVEL_INFO: chkd.SetLogLevel(LogLevel::Info); break;
    case ID_LOGLEVEL_DEBUG: chkd.SetLogLevel(LogLevel::Debug); break;
    case ID_LOGLEVEL_TRACE: chkd.SetLogLevel(LogLevel::Trace); break;
    case ID_LOGLEVEL_ALL: chkd.SetLogLevel(LogLevel::All); break;

        // Locations
    case ID_LOCATIONS_SNAPTOTILE: CM->SetLocationSnap(GuiMap::Snap::SnapToTile); break;
    case ID_LOCATIONS_SNAPTOACTIVEGRID: CM->SetLocationSnap(GuiMap::Snap::SnapToGrid); break;
    case ID_LOCATIONS_NOSNAP: CM->SetLocationSnap(GuiMap::Snap::NoSnap); break;
    case ID_LOCATIONS_LOCKANYWHERE: CM->ToggleLockAnywhere(); break;
    case ID_LOCATIONS_CLIPNAMES: CM->ToggleLocationNameClip(); break;

        // Doodads
    case ID_DOODADS_ALLOWILLEGALPLACEMENT: CM->ToggleAllowIllegalDoodadPlacement(); break;

        // Sprites
    case ID_SPRITES_SNAPTOGRID: CM->ToggleSpriteSnap(); break;

        // Cut Copy Paste
    case ID_CUTCOPYPASTE_SNAPSELECTIONTOTILES: CM->SetCutCopyPasteSnap(GuiMap::Snap::SnapToTile); break;
    case ID_CUTCOPYPASTE_SNAPSELECTIONTOGRID: CM->SetCutCopyPasteSnap(GuiMap::Snap::SnapToGrid); break;
    case ID_CUTCOPYPASTE_NOSNAP: CM->SetCutCopyPasteSnap(GuiMap::Snap::NoSnap); break;
    case ID_CUTCOPYPASTE_INCLUDEDOODADTILES: CM->ToggleIncludeDoodadTiles(); break;
    case ID_CUTCOPYPASTE_FILLSIMILARTILES: maps.clipboard.toggleFillSimilarTiles(); break;

        // Scenario
    case ID_TRIGGERS_CLASSICMAPTRIGGERS: trigEditorWindow.CreateThis(getHandle()); break;
    case ID_TRIGGERS_CLASSICMISSIONBRIEFING: briefingTrigEditorWindow.CreateThis(getHandle()); break;
    case ID_SCENARIO_MAPDIMENSIONS: dimensionsWindow.CreateThis(getHandle()); break;
    case ID_SCENARIO_DESCRIPTION: case ID_SCENARIO_FORCES: case ID_SCENARIO_UNITSETTINGS:
    case ID_SCENARIO_UPGRADESETTINGS: case ID_SCENARIO_TECHSETTINGS: case ID_SCENARIO_STRINGS:
    case ID_SCENARIO_SOUNDEDITOR: OpenMapSettings(LOWORD(wParam)); break;

        // Tools
    case ID_TRIGGERS_TRIGGEREDITOR: textTrigWindow.CreateThis(getHandle()); break;
    case ID_TRIGGERS_MISSIONBRIEFINGEDITOR: briefingTextTrigWindow.CreateThis(getHandle()); break;
    case ID_TOOLS_PASSWORD: ifmapopen(changePasswordWindow.CreateThis(getHandle())) break;
    case ID_TOOLS_MPQRECOMPILER: runMpqRecompiler(); break;
    case ID_SCRIPTS_REPAIRSOUNDS: repairSounds(); break;
    case ID_SCRIPTS_REPAIRSTRINGS_MIN: repairStrings(false); break;
    case ID_SCRIPTS_REPAIRSTRINGS_MAX: repairStrings(true); break;

        // Windows
    case ID_WINDOWS_CASCADE: maps.cascade(); break;
    case ID_WINDOWS_TILEHORIZONTALLY: maps.tileHorizontally(); break;
    case ID_WINDOWS_TILEVERTICALLY: maps.tileVertically(); break;
    case ID_WINDOW_CLOSE: maps.destroyActive(); break;

        // Help
    case ID_HELP_ABOUT: aboutWindow.CreateThis(getHandle()); break;
    case ID_HELP_STARCRAFT_WIKI: OpenWebPage("http://www.staredit.net/wiki/index.php?title=Main_Page"); break;
    case ID_HELP_SUPPORT_FORUM: OpenWebPage("http://www.staredit.net/forums/"); break;
    case ID_HELP_CHKDRAFTGITHUB: OpenWebPage("https://github.com/TheNitesWhoSay/Chkdraft/"); break;
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
            if ( !maps.CloseAll() )
                return 0; // Abort close

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

void Chkdraft::HandleDroppedFile(const std::string & dropFilePath)
{
    maps.OpenMap(dropFilePath);
}

void Chkdraft::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    if ( hWndFrom == mainToolbar.checkTerrain.getHandle() )
        mainToolbar.checkTerrain.SetCheck(maps.toggleCutCopyPasteTerrain());
    else if ( hWndFrom == mainToolbar.checkDoodads.getHandle() )
        mainToolbar.checkDoodads.SetCheck(maps.toggleCutCopyPasteDoodads());
    else if ( hWndFrom == mainToolbar.checkSprites.getHandle() )
        mainToolbar.checkSprites.SetCheck(maps.toggleCutCopyPasteSprites());
    else if ( hWndFrom == mainToolbar.checkUnits.getHandle() )
        mainToolbar.checkUnits.SetCheck(maps.toggleCutCopyPasteUnits());
    else if ( hWndFrom == mainToolbar.checkFog.getHandle() )
        mainToolbar.checkFog.SetCheck(maps.toggleCutCopyPasteFog());
}

bool Chkdraft::CreateSubWindows()
{
    HWND hWnd = getHandle();
    if ( hWnd != NULL )
    {
        DragAcceptFiles(hWnd, TRUE);
        int statusWidths[] = { 130, 205, 350, 450, 600, -1 };

        return mainMenu.FindThis(hWnd) &&
            mainToolbar.CreateThis(hWnd, Id::IDR_MAIN_TOOLBAR) &&
            statusBar.CreateThis(sizeof(statusWidths) / sizeof(int), statusWidths, 0,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, hWnd, (HMENU)Id::IDR_MAIN_STATUS) &&
            mainPlot.CreateThis(hWnd, Id::IDR_MAIN_PLOT) &&
            BecomeMDIFrame(maps, GetSubMenu(GetMenu(hWnd), 6), Id::ID_MDI_FIRSTCHILD,
                WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                0, 0, 0, 0, (HMENU)Id::IDR_MAIN_MDI);
    }
    else
        return false;
}

void Chkdraft::MinimizeDialogs()
{
    ShowWindow(unitWindow.getHandle(), SW_HIDE);
    ShowWindow(spriteWindow.getHandle(), SW_HIDE);
    ShowWindow(locationWindow.getHandle(), SW_HIDE);
    ShowWindow(terrainPalWindow.getHandle(), SW_HIDE);
    ShowWindow(mapSettingsWindow.getHandle(), SW_HIDE);
    ShowWindow(trigEditorWindow.getHandle(), SW_HIDE);
    ShowWindow(briefingTrigEditorWindow.getHandle(), SW_HIDE);
}

void Chkdraft::RestoreDialogs()
{
    ShowWindow(unitWindow.getHandle(), SW_SHOW);
    ShowWindow(spriteWindow.getHandle(), SW_SHOW);
    ShowWindow(locationWindow.getHandle(), SW_SHOW);
    ShowWindow(terrainPalWindow.getHandle(), SW_SHOW);
    ShowWindow(mapSettingsWindow.getHandle(), SW_SHOW);
    ShowWindow(trigEditorWindow.getHandle(), SW_SHOW);
    ShowWindow(briefingTrigEditorWindow.getHandle(), SW_SHOW);
}

void Chkdraft::OpenMapSettings(u16 menuId)
{
    if ( mapSettingsWindow.getHandle() == NULL )
        mapSettingsWindow.CreateThis(getHandle());
                                    
    if ( mapSettingsWindow.getHandle() != NULL )
    {
        switch ( menuId )
        {
            case ID_SCENARIO_DESCRIPTION: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::MapProperties); break;
            case ID_SCENARIO_FORCES: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::Forces); break;
            case ID_SCENARIO_UNITSETTINGS: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::UnitSettings); break;
            case ID_SCENARIO_UPGRADESETTINGS: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::UpgradeSettings); break;
            case ID_SCENARIO_TECHSETTINGS: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::TechSettings); break;
            case ID_SCENARIO_STRINGS: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::StringEditor); break;
            case ID_SCENARIO_SOUNDEDITOR: mapSettingsWindow.ChangeTab(MapSettingsWindow::Tab::SoundEditor); break;
        }
        ShowWindow(mapSettingsWindow.getHandle(), SW_SHOW);
    }
}

void Chkdraft::OpenWebPage(const std::string & address)
{
    int resultCode = 0;
    WinLib::executeOpen(address, resultCode);
}

void Chkdraft::ComboEditChanged(HWND hCombo, u16 comboId)
{
    if ( hCombo == mainToolbar.playerBox.getHandle() )
    {
        u8 newPlayer;
        if ( mainToolbar.playerBox.GetPlayerNum(newPlayer) )
            maps.ChangePlayer(newPlayer);
    }
}

void Chkdraft::ComboSelChanged(HWND hCombo, u16 comboId)
{
    int itemIndex = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
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
        if ( itemIndex == 0 ) // Isometrical
            maps.ChangeSubLayer(TerrainSubLayer::Isom);
        else if ( itemIndex == 1 ) // Rectangular
            maps.ChangeSubLayer(TerrainSubLayer::Rectangular);
        else if ( itemIndex == 2 ) // Subtile
            maps.ChangeSubLayer(TerrainSubLayer::Rectangular);
        else if ( itemIndex == 3 ) // Tileset indexed
        {
            terrainPalWindow.CreateThis(getHandle());
            ShowWindow(terrainPalWindow.getHandle(), SW_SHOW);
            maps.ChangeSubLayer(TerrainSubLayer::Rectangular);
        }
        else if ( itemIndex == 4 ) // Cut/Copy/Paste
            maps.ChangeSubLayer(TerrainSubLayer::Rectangular);
    }
    else if ( hCombo == mainToolbar.brushWidth.getHandle() )
        maps.clipboard.setFogBrushSize(itemIndex+1, mainToolbar.brushHeight.GetSel()+1);
    else if ( hCombo == mainToolbar.brushHeight.getHandle() )
        maps.clipboard.setFogBrushSize(mainToolbar.brushWidth.GetSel()+1, itemIndex+1);
}
