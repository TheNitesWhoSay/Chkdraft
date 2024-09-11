#include "logger_window.h"
#include "chkdraft/chkdraft.h"
#include <WindowsX.h>

extern Logger logger;

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
    if ( ClassWindow::RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), WinLib::ResourceManager::getSolidBrush(RGB(240, 240, 240)), NULL, "NumberedEdit", NULL, false) &&
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
        lineStart.push_back(lineStart.empty() ? 0 : prevLines.size()+1);
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
    WinLib::ContextMenu menu {};
    menu.append("Hide Logger", [&](){ ToggleVisible(); })
        .append(showLineNumbers ? "Hide Line Numbers" : "Show Line Numbers", [&](){ ToggleLineNumbers(); })
        .appendSeparator()
        .append("Open Log File", [](){ chkd.OpenLogFile(); })
        .append("Open Log File Directory", [](){ chkd.OpenLogFileDirectory(); })
        .appendSeparator();

    if ( auto & logLevelMenu = menu.appendSubmenu("Log Level") )
    {
        LogLevel logLevel = logger.getLogLevel();
        logLevelMenu.append("[0] Off"    , [](){ chkd.SetLogLevel(LogLevel::Off);   }, logLevel == LogLevel::Off  )
                    .append("[100] Fatal", [](){ chkd.SetLogLevel(LogLevel::Fatal); }, logLevel == LogLevel::Fatal)
                    .append("[200] Error", [](){ chkd.SetLogLevel(LogLevel::Error); }, logLevel == LogLevel::Error)
                    .append("[300] Warn" , [](){ chkd.SetLogLevel(LogLevel::Warn);  }, logLevel == LogLevel::Warn )
                    .append("[400] Info" , [](){ chkd.SetLogLevel(LogLevel::Info);  }, logLevel == LogLevel::Info )
                    .append("[500] Debug", [](){ chkd.SetLogLevel(LogLevel::Debug); }, logLevel == LogLevel::Debug)
                    .append("[600] Trace", [](){ chkd.SetLogLevel(LogLevel::Trace); }, logLevel == LogLevel::Trace)
                    .append("[-1] All"   , [](){ chkd.SetLogLevel(LogLevel::All);   }, logLevel == LogLevel::All  );
    }

    menu.select(getHandle(), x, y);
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

                RECT curr {};
                this->getClientRect(curr);
                auto currHeight = curr.bottom - curr.top;
                auto currWidth = curr.right - curr.left;

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
