#include "CndActGrid.h"
#include <string>

// Copy -> escape tabs to \t
// Paste -> parse \t to tabs

enum ID
{
    CHECK_ENABLED_1 = ID_FIRST,
    CHECK_ENABLED_16 = (CHECK_ENABLED_1 + 15),
    CHECK_ENABLED_64 = (CHECK_ENABLED_16 + 48)
};

ICndActGridUser::~ICndActGridUser()
{

}

CndActGrid::CndActGrid(ICndActGridUser& user, int numUsedRows) : user(user), hasUpDownedThisEdit(false),
    startedByClick(false), ignoreChange(false), numUsedRows(numUsedRows)
{
    
}

CndActGrid::~CndActGrid()
{

}

bool CndActGrid::CreateThis(HWND hParent, int x, int y, int width, int height, u32 id)
{
    GridViewControl::CreateThis(hParent, 2, 40, 100, 100, id);
    GridViewControl::SetFont(defaultFont, false);
    for ( int i = 0; i<9; i++ )
        GridViewControl::AddColumn(0, " - ", 50, LVCFMT_CENTER);
    GridViewControl::AddColumn(0, "", 14, LVCFMT_CENTER);
    GridViewControl::Show();

    for ( int y = 0; y<numUsedRows; y++ )
        GridViewControl::AddRow(9, (y + 1) * 100);

    for ( int y = 0; y < numUsedRows; y++ )
    {
        for ( int x = 2; x < 10; x++ )
        {
            GridViewControl::item(x, y).SetDisabled(true);
        }
    }

    CreateSubWindows(getHandle());
    return false;
}

void CndActGrid::SetEnabledCheck(u8 cndActId, bool enabled)
{
    if ( cndActId >= 0 && cndActId < numUsedRows )
        checkEnabled[cndActId].SetCheck(enabled);
}

Suggestions & CndActGrid::GetSuggestions()
{
    return suggestions;
}

bool CndActGrid::HasUpDownedThisEdit()
{
    return hasUpDownedThisEdit;
}

bool CndActGrid::allowKeyNavDuringEdit(WinLib::GVKey key)
{
    switch ( key )
    {
        case WinLib::GVKey::GVUp:
        case WinLib::GVKey::GVDown:
            return false;
            break;
        default:
            if ( startedByClick )
            {
                if ( key == WinLib::GVKey::GVLeft )
                    return false;
                else if ( key == WinLib::GVKey::GVRight )
                    return false;
            }
            break;
    }
    return true;
}

bool CndActGrid::allowSel(int x, int y)
{
    return x != 0 &&
        (x == 1 || !GridViewControl::item(x, y).isDisabled());
}

bool CndActGrid::allowSel(int xStart, int xEnd, int yStart, int yEnd)
{
    return xStart != 0 || xEnd != 0;
}

void CndActGrid::adjustSel(int &xStart, int &xEnd, int &yStart, int &yEnd)
{
    if ( xStart == 0 )
        xStart = 1;
}

void CndActGrid::CellClicked(int x, int y)
{
    if ( x == 0 && y >= 0 && y < numUsedRows )
        user.CndActEnableToggled(y);
}

size_t CndActGrid::FindArgEnd(std::string &str, size_t argStart, ArgumentEnd &outEndsBy)
{
    size_t nextCr = str.find('\r', argStart);
    size_t nextLf = str.find('\n', argStart);
    size_t nextCrLf = str.find("\r\n", argStart);
    size_t nextTab = str.find('\t', argStart);
    if ( nextCr == std::string::npos ) nextCr = std::numeric_limits<size_t>().max();
    if ( nextLf == std::string::npos ) nextLf = std::numeric_limits<size_t>().max();
    if ( nextCrLf == std::string::npos ) nextCrLf = std::numeric_limits<size_t>().max();
    if ( nextTab == std::string::npos ) nextTab = std::numeric_limits<size_t>().max();
    size_t argEnd = std::min({ nextCr, nextLf, nextCrLf, nextTab, str.size() });

    if ( argEnd == nextCrLf )
        outEndsBy = ArgumentEnd::TwoCharLineBreak;
    else if ( argEnd == nextCr || argEnd == nextLf )
        outEndsBy = ArgumentEnd::OneCharLineBreak;
    else if ( argEnd == nextTab )
        outEndsBy = ArgumentEnd::Tab;
    else
        outEndsBy = ArgumentEnd::StringEnd;

    return argEnd;
}

void CndActGrid::EscapeString(std::string &str)
{
    size_t length = str.length();
    for ( size_t i = 0; i < length; i++ )
    {
        if ( str[i] == '\t' )
        {
            str[i] = '\\';
            i++;
            str.insert(i, "t");
            length++;
        }
    }
}

bool CndActGrid::BuildSelectionString(std::string &str)
{
    int numRows = GridViewControl::NumRows();
    int numColumns = GridViewControl::NumColumns();
    std::stringstream ssSelection;
    for ( int y = 0; y < numRows; y++ )
    {
        bool rowUsed = false;
        bool hasPreviousThisRow = false;
        for ( int x = 0; x < numColumns; x++ )
        {
            auto currItem = GridViewControl::item(x, y);
            RawString currItemText;
            if ( GridViewControl::isFocused(x, y) || currItem.isSelected() )
            {
                if ( hasPreviousThisRow )
                    ssSelection << '\t';
                else
                    hasPreviousThisRow = true;

                if ( currItem.getTextLength() > 0 && currItem.getText(currItemText) )
                {
                    SingleLineChkdString chkdString;
                    if ( MakeChkdStr(currItemText, chkdString) )
                        ssSelection << currItemText;
                }
            }
        }

        if ( hasPreviousThisRow )
            ssSelection << "\r\n";
    }
    str = ssSelection.str();
    return true;
}

void CndActGrid::RedrawThis()
{
    GridViewControl::RedrawThis();
    for ( int i = 0; i < numUsedRows; i++ )
        checkEnabled[i].RedrawThis();
}

bool CndActGrid::CreateSubWindows(HWND hWnd)
{
    for ( int i = 0; i < numUsedRows; i++ )
    {
        POINT pt = GridViewControl::GetItemTopLeft(0, i);
        checkEnabled[i].CreateThis(getHandle(), 0, pt.y, 12, 13, true, false, "", CHECK_ENABLED_1 + i);
    }
    return true;
}

void CndActGrid::StartEditing(int xClick, int yClick, char initChar)
{
    if ( initChar == '\0' ) // Started via click
    {
        startedByClick = true;
        GridViewControl::EditBox().SetForwardArrowKeys(false);
        GridViewControl::EditBox().SetStopForwardOnClick(true);
    }
    else
    {
        startedByClick = false;
        GridViewControl::EditBox().SetForwardArrowKeys(true);
        GridViewControl::EditBox().SetStopForwardOnClick(true);
    }

    hasUpDownedThisEdit = false;
    return GridViewControl::StartEditing(xClick, yClick, initChar);
}

void CndActGrid::KeyDown(WPARAM wParam)
{
    if ( GridViewControl::isEditing() && (wParam == VK_UP || wParam == VK_DOWN) )
    {
        ignoreChange = true;

        if ( wParam == VK_UP )
            suggestions.ArrowUp();
        else if ( wParam == VK_DOWN )
            suggestions.ArrowDown();

        ignoreChange = false;
    }

    GridViewControl::KeyDown(wParam);
}

void CndActGrid::EditTextChanged(const std::string &str)
{
    ignoreChange = true;
    suggestions.SuggestNear(str);
    ignoreChange = false;
}

BOOL CndActGrid::ProcessTrackA(NMHEADERA* nmHeader)
{
    if ( nmHeader->iItem == 0 )
        return TRUE; // Ignore first-column resizes
    else
        return FALSE;
}

BOOL CndActGrid::ProcessTrackW(NMHEADERW* nmHeader)
{
    if ( nmHeader->iItem == 0 )
        return TRUE; // Ignore first-column resizes
    else
        return FALSE;
}

LRESULT CndActGrid::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
        case HDN_BEGINTRACKA: return ProcessTrackA((NMHEADERA*)nmhdr); break;
        case HDN_BEGINTRACKW: return ProcessTrackW((NMHEADERW*)nmhdr); break;
    }
    return GridViewControl::Notify(hWnd, idFrom, nmhdr);
}

LRESULT CndActGrid::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
        case EN_CHANGE:
            if ( !ignoreChange )
            {
                std::string str;
                GridViewControl::GetEditText(str);
                ignoreChange = true;
                suggestions.SuggestNear(str);
                ignoreChange = false;
            }
            break;
        case BN_CLICKED:
            {
                if ( LOWORD(wParam) >= CHECK_ENABLED_1 && LOWORD(wParam) <= CHECK_ENABLED_64 )
                {
                    u8 cndActNum = u8(LOWORD(wParam) - CHECK_ENABLED_1);
                    if ( cndActNum >= 0 && cndActNum < numUsedRows )
                        user.CndActEnableToggled(cndActNum);
                }
            }
            break;
        default:
            return GridViewControl::Command(hWnd, wParam, lParam);
            break;
    }
    return GridViewControl::Command(hWnd, wParam, lParam);
}

LRESULT CndActGrid::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_KEYUP:
            if ( wParam == VK_UP )
                suggestions.ArrowUp();
            else if ( wParam == VK_DOWN )
                suggestions.ArrowDown();
            break;

        case WM_KEYDOWN:
            if ( GetKeyState(VK_CONTROL) & 0x8000 )
            {
                switch ( wParam )
                {
                    case 'c': case 'C': user.CopySelection(); break;
                    case 'v': case 'V': user.Paste(); break;
                    case 'x': case 'X': user.CutSelection(); break;
                    default: return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
                }
            }
            else
                return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
            break;

        default: return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
