#include "LoggerWindow.h"
#include "../../Chkdraft.h"

enum_t(Id, u32, {
    LineNumbers = ID_FIRST,
    RichText
});

LoggerWindow::LoggerWindow() : std::ostream((std::streambuf*)this), lineNumber(1), showLineNumbers(true)
{

}

LoggerWindow::~LoggerWindow()
{

}

bool LoggerWindow::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, bool readOnly, u64 id)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, LoadCursor(NULL, IDC_ARROW), CreateSolidBrush(RGB(240, 240, 240)), NULL, "NumberedEdit", NULL, false) &&
        ClassWindow::CreateClassWindow(WS_EX_CLIENTEDGE, "", WS_CHILD|WS_THICKFRAME, x, y, width, height, hParent, (HMENU)id) )
    {
        s32 lineNumberTextWidth = 50;
        s32 lineNumberMarginWidth = 20;
        s32 lineNumberControlWidth = lineNumberTextWidth + lineNumberMarginWidth;
        s32 textLeft = lineNumberControlWidth+3;
        s32 textWidth = cliWidth()-textLeft;
        s32 textHeight = cliHeight();
        richText.CreateThis(getHandle(), textLeft, 0, textWidth, textHeight, readOnly, true, Id::RichText);
        lineNumbers.CreateThis(getHandle(), 0, 0, lineNumberControlWidth, textHeight, true, false, Id::LineNumbers);
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
        size_t firstLine = lineNumber-totalLines;
        for ( size_t i=0; i<totalLines; i++ )
        {
            if ( i > 0 )
                lineNumbers += '\n';

            lineNumbers += std::to_string(firstLine+i);
        }
        this->lineNumbers.SetText(icux::toUistring(lineNumbers));

        currLine.clear();
        richText.SetText(prevLines);
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
    
void LoggerWindow::ToggleLineNumbers()
{
    if ( showLineNumbers )
    {
        lineNumbers.SetRedraw(false);
        lineNumbers.Hide();
    }
    else
    {
        lineNumbers.SetRedraw(true);
        lineNumbers.Show();
    }

    showLineNumbers = !showLineNumbers;
    SizeSubWindows();
    RedrawWindow(getHandle(), NULL, NULL, RDW_INVALIDATE|RDW_ERASE);
}
    
void LoggerWindow::ContextMenu(int x, int y)
{
    HMENU hMenu = ::CreatePopupMenu();
    auto displayLineNumbers = icux::toUistring(showLineNumbers ? "Hide Line Numbers" : "Show Line Numbers");
    AppendMenu(hMenu, MF_STRING, ContextMenuItem::ToggleLineNumbers, displayLineNumbers.c_str());
    BOOL result = TrackPopupMenu(hMenu, TPM_RETURNCMD, x, y, 0, getHandle(), NULL);
    if ( result == ContextMenuItem::ToggleLineNumbers )
        ToggleLineNumbers();
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
                RECT rcMain, rcTool, rcStatus, rcLeftBar;

                // Get the size of the client area, toolbar, status bar, and left bar
                GetClientRect(chkd.getHandle(), &rcMain);
                GetWindowRect(chkd.mainToolbar.getHandle(), &rcTool);
                GetWindowRect(chkd.statusBar.getHandle(), &rcStatus);
                GetWindowRect(chkd.mainPlot.leftBar.getHandle(), &rcLeftBar);

                int xBorder = GetSystemMetrics(SM_CXSIZEFRAME) - 1,
                    yBorder = GetSystemMetrics(SM_CYSIZEFRAME) - 1;

                int x = rcLeftBar.right - rcLeftBar.left - 3*xBorder - 2;
                int y = rcMain.bottom-rcMain.top+2*yBorder+1-chkd.mainPlot.loggerWindow.Height()-(rcStatus.bottom-rcStatus.top)-(rcTool.bottom-rcTool.top);
                int width = rcMain.right - rcMain.left - (rcLeftBar.right - rcLeftBar.left - 3*xBorder - 2)+xBorder+4;
                int height = chkd.mainPlot.loggerWindow.Height();

                // Fit logger to the area between the left bar and right edge without changing the height
                SetWindowPos(chkd.mainPlot.loggerWindow.getHandle(), NULL, x, y,
                    width, height,
                    SWP_NOZORDER | SWP_NOACTIVATE);

                // Fit the map MDIClient to the area right of the left bar and between the toolbar and logger
                SetWindowPos(chkd.maps.getHandle(), HWND_TOP, 0, 0, 0, 0,
                    SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
                SetWindowPos(chkd.maps.getHandle(), NULL, rcLeftBar.right - rcLeftBar.left - xBorder - 2, rcTool.bottom - rcTool.top,
                    rcMain.right - rcMain.left - rcLeftBar.right + rcLeftBar.left + xBorder + 2, chkd.mainPlot.loggerWindow.Top(),
                    SWP_NOZORDER | SWP_NOACTIVATE);

                SizeSubWindows();
                return 0;
            }
            break;
    }
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}
