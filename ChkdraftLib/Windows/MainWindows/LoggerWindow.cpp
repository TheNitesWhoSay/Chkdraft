#include "LoggerWindow.h"
#include "../../Chkdraft.h"

enum_t(Id, u32, {
    LineNumbers = ID_FIRST,
    RichText
});

LoggerWindow::LoggerWindow() : std::ostream((std::streambuf*)this), lineNumber(1), showLineNumbers(true), isVisible(true)
{

}

LoggerWindow::~LoggerWindow()
{

}

bool LoggerWindow::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, u64 id)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), CreateSolidBrush(RGB(240, 240, 240)), NULL, "NumberedEdit", NULL, false) &&
        ClassWindow::CreateClassWindow(0, "", WS_CHILD|WS_THICKFRAME, x, y, width, height, hParent, (HMENU)id) )
    {
        s32 lineNumberTextWidth = 50;
        s32 lineNumberMarginWidth = 20;
        s32 lineNumberControlWidth = lineNumberTextWidth + lineNumberMarginWidth;
        s32 textLeft = lineNumberControlWidth+3;
        s32 textWidth = cliWidth()-textLeft;
        s32 textHeight = cliHeight();
        richText.CreateThis(getHandle(), textLeft-1, 0, textWidth+1, textHeight, readOnly, true, Id::RichText);
        lineNumbers.CreateThis(getHandle(), -1, 0, lineNumberControlWidth+1, textHeight, true, false, Id::LineNumbers);
        PARAFORMAT paraFormat = {};
        paraFormat.cbSize = sizeof(PARAFORMAT);
        paraFormat.dwMask = PFM_ALIGNMENT;
        paraFormat.wAlignment = PFA_RIGHT;
        SendMessage(lineNumbers.getHandle(), EM_SETPARAFORMAT, 0, (LPARAM)&paraFormat);
        SendMessage(lineNumbers.getHandle(), EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, lineNumberMarginWidth));

        richText.SetScrollMatcher(&lineNumbers);
        ClassWindow::ShowNormal();
        return true;
    }
    return false;
}

void LoggerWindow::Refresh()
{
    std::string lineNumbers = "";
    size_t totalLines = lineStart.size();
    size_t firstLine = size_t(lineNumber-totalLines);
    for ( size_t i=0; i<totalLines; i++ )
    {
        if ( i > 0 )
            lineNumbers += '\n';

        lineNumbers += std::to_string(firstLine+i);
    }
    this->lineNumbers.SetUiText(icux::toUistring(lineNumbers));
    richText.SetUiText(prevLines);
    richText.ScrollBottom();
}
 
bool LoggerWindow::IsVisible()
{
    return isVisible;
}

void LoggerWindow::ToggleVisible()
{
    if ( isVisible )
    {
        isVisible = false;
        ClassWindow::Hide();
        chkd.mainMenu.SetText(ID_LOGGER_TOGGLELOGGER, "Show Logger");
        chkd.SizeSubWindows();
    }
    else
    {
        isVisible = true;
        ClassWindow::Show();
        chkd.mainMenu.SetText(ID_LOGGER_TOGGLELOGGER, "Hide Logger");
        chkd.SizeSubWindows();
    }
}

void LoggerWindow::ToggleLineNumbers()
{
    if ( showLineNumbers )
    {
        lineNumbers.SetRedraw(false);
        chkd.mainMenu.SetText(ID_LOGGER_TOGGLELINENUMBERS, "Show Line Numbers");
        lineNumbers.Hide();
    }
    else
    {
        lineNumbers.SetRedraw(true);
        chkd.mainMenu.SetText(ID_LOGGER_TOGGLELINENUMBERS, "Hide Line Numbers");
        lineNumbers.Show();
    }

    showLineNumbers = !showLineNumbers;
    SizeSubWindows();
    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE|RDW_ERASE);
}

int LoggerWindow::sync()
{
    return 0;
}

int LoggerWindow::overflow(int c)
{
    if ( c == '\n' )
    {
        lineStart.push_back(prevLines.size()+1);
        prevLines += icux::toUistring(currLine);
        lineNumber++;
        std::string lineNumbers = "";
            
        if ( lineStart.size() > 100 )
        {
            size_t lengthRemoved = lineStart[1]-lineStart[0];
            prevLines = prevLines.substr(lineStart[1]);
            for ( size_t i=1; i<lineStart.size(); i++ )
                lineStart[i] -= lengthRemoved;

            lineStart.pop_front();
        }

        size_t totalLines = lineStart.size();
        size_t firstLine = size_t(lineNumber-totalLines);
        for ( size_t i=0; i<totalLines; i++ )
        {
            if ( i > 0 )
                lineNumbers += '\n';

            lineNumbers += std::to_string(firstLine+i);
        }
        this->lineNumbers.SetUiText(icux::toUistring(lineNumbers));

        currLine.clear();
        richText.SetUiText(prevLines);
        richText.ScrollBottom();
        currLine.push_back(char(c));
    }
    else
        currLine.push_back(char(c));

    return 0;
}

void LoggerWindow::SizeSubWindows()
{
    s32 lineNumberTextWidth = 50;
    s32 lineNumberMarginWidth = 20;
    s32 lineNumberControlWidth = lineNumberTextWidth + lineNumberMarginWidth;
    s32 textLeft = showLineNumbers ? lineNumberControlWidth+3 : 0;
    s32 textWidth = cliWidth()-textLeft;
    s32 textHeight = cliHeight();
        
    richText.SetRedraw(false);
    lineNumbers.SetRedraw(false);
    richText.SetPos(textLeft, 0, textWidth, textHeight);
    lineNumbers.SetPos(0, 0, lineNumberControlWidth, textHeight);
    richText.ScrollBottom();
    richText.AlignMatcher();
    richText.SetRedraw(true);
    if ( showLineNumbers )
    {
        lineNumbers.SetRedraw(true);
        lineNumbers.RedrawThis();
    }
    richText.RedrawThis();
}
    
void LoggerWindow::ContextMenu(int x, int y)
{
    HMENU hMenu = ::CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ContextMenuItem::HideLogger, icux::toUistring("Hide Logger").c_str());
    auto displayLineNumbers = icux::toUistring(showLineNumbers ? "Hide Line Numbers" : "Show Line Numbers");
    AppendMenu(hMenu, MF_STRING, ContextMenuItem::ToggleLineNumbers, displayLineNumbers.c_str());
    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(hMenu, MF_STRING, ContextMenuItem::OpenLogFile, icux::toUistring("Open Log File").c_str());
    AppendMenu(hMenu, MF_STRING, ContextMenuItem::OpenLogFileDirectory, icux::toUistring("Open Log File Directory").c_str());
    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
    HMENU hLogLevel = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT_PTR)hLogLevel, icux::toUistring("Log Level").c_str());

    LogLevel logLevel = logger.getLogLevel();
    AppendMenu(hLogLevel, logLevel == LogLevel::Off ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelOff, icux::toUistring("[0] Off").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Fatal ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelFatal, icux::toUistring("[100] Fatal").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Error ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelError, icux::toUistring("[200] Error").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Warn ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelWarn, icux::toUistring("[300] Warn").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Info ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelInfo, icux::toUistring("[400] Info").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Debug ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelDebug, icux::toUistring("[500] Debug").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::Trace ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelTrace, icux::toUistring("[600] Trace").c_str());
    AppendMenu(hLogLevel, logLevel == LogLevel::All ? MF_STRING|MF_CHECKED : MF_STRING, ContextMenuItem::LogLevelAll, icux::toUistring("[-1] All").c_str());

    BOOL result = TrackPopupMenu(hMenu, TPM_RETURNCMD, x, y, 0, getHandle(), NULL);
    switch ( result )
    {
        case ContextMenuItem::HideLogger: ToggleVisible(); break;
        case ContextMenuItem::ToggleLineNumbers: ToggleLineNumbers(); break;
        case ContextMenuItem::OpenLogFile: chkd.OpenLogFile(); break;
        case ContextMenuItem::OpenLogFileDirectory: chkd.OpenLogFileDirectory(); break;

        case ContextMenuItem::LogLevelOff: chkd.SetLogLevel(LogLevel::Off); break;
        case ContextMenuItem::LogLevelFatal: chkd.SetLogLevel(LogLevel::Fatal); break;
        case ContextMenuItem::LogLevelError: chkd.SetLogLevel(LogLevel::Error); break;
        case ContextMenuItem::LogLevelWarn: chkd.SetLogLevel(LogLevel::Warn); break;
        case ContextMenuItem::LogLevelInfo: chkd.SetLogLevel(LogLevel::Info); break;
        case ContextMenuItem::LogLevelDebug: chkd.SetLogLevel(LogLevel::Debug); break;
        case ContextMenuItem::LogLevelTrace: chkd.SetLogLevel(LogLevel::Trace); break;
        case ContextMenuItem::LogLevelAll: chkd.SetLogLevel(LogLevel::All); break;
    }
}

LRESULT LoggerWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_CONTEXTMENU:
            ContextMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_SIZE:
            {
                // Get the size of the client area, toolbar, status bar, and left bar
                RECT rcMain, rcTool, rcStatus, rcLeftBar;
                GetClientRect(chkd.getHandle(), &rcMain);
                GetWindowRect(chkd.mainToolbar.getHandle(), &rcTool);
                GetWindowRect(chkd.statusBar.getHandle(), &rcStatus);
                GetWindowRect(chkd.mainPlot.leftBar.getHandle(), &rcLeftBar);

                int xBorder = GetSystemMetrics(SM_CXSIZEFRAME),
                    yBorder = GetSystemMetrics(SM_CYSIZEFRAME);

                // Fit logger to the area between the left bar and right edge without changing the height
                SetWindowPos(chkd.mainPlot.loggerWindow.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - 3*xBorder,
                    rcMain.bottom-rcMain.top+2*yBorder-1-chkd.mainPlot.loggerWindow.Height()-(rcStatus.bottom-rcStatus.top)-(rcTool.bottom-rcTool.top),
                    rcMain.right - rcMain.left - (rcLeftBar.right - rcLeftBar.left) + 4*xBorder + 5, ClassWindow::Height(), SWP_NOZORDER | SWP_NOACTIVATE);

                // Fit the map MDIClient to the area right of the left bar and between the toolbar and logger
                SetWindowPos(chkd.maps.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - xBorder + 1, rcTool.bottom - rcTool.top,
                    rcMain.right - rcMain.left - rcLeftBar.right + rcLeftBar.left + xBorder - 1, chkd.mainPlot.loggerWindow.IsVisible() ? chkd.mainPlot.loggerWindow.Top() : rcStatus.top - rcTool.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
                SetWindowPos(chkd.maps.getHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

                SizeSubWindows();
                return 0;
            }
            break;
        case WM_PAINT:
            LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            RedrawWindow(chkd.mainPlot.leftBar.getHandle(), NULL, NULL, RDW_INVALIDATE|RDW_FRAME|RDW_UPDATENOW);
            return result;
            break;
    }
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}
