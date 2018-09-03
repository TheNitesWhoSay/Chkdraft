#include "Suggestions.h"
#include "../../CommonFiles/CommonFiles.h"
#include "../../../WindowsLib/WindowsUi.h"
#include <string>
#include <vector>

bool alphabetize(const std::string &first, const std::string &second)
{
    // true = first before second, false = second before first
    size_t firstLen = first.length(),
        secondLen = second.length();

    u32 i = 0;
    while ( i < firstLen && i < secondLen )
    {
        if ( tolower(first[i]) < tolower(second[i]) )
            return true;
        else if ( tolower(first[i]) > tolower(second[i]) )
            return false;

        i = i+1;
    }
    return firstLen < secondLen;
}

bool firstStartsWithSecond(const std::string &first, const std::string &second)
{
    size_t firstLen = first.length();
    size_t secondLen = second.length();

    if ( firstLen < secondLen )
        return false;

    for ( size_t i = 0; i < secondLen; i++ )
    {
        if ( tolower(first[i]) != tolower(second[i]) )
            return false;
    }
    return true;
}

Suggestions::Suggestions() : isShown(false)
{
    suggestParent = NULL;
}

Suggestions::~Suggestions()
{

}

bool Suggestions::CreateThis(HWND hParent, int x, int y, int width, int height)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Suggestions", NULL, false) )
    {
        if ( ClassWindow::CreateClassWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE, "SuggestionsWindow", WS_POPUP, x, y, width, height, NULL, NULL) )
        {
            suggestParent = hParent;
            SetWindowPos(getHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
            listSuggestions.CreateThis(getHandle(), 0, 0, width, height + 12, false, false, true, false, 0);
            listSuggestions.SetFont(defaultFont, false);
            DoSize();
            return true;
        }
    }
    return false;
}

void Suggestions::ClearStrings()
{
    strList.clear();
    listSuggestions.ClearSel();
    listSuggestions.ClearItems();
}

void Suggestions::AddStrings(const std::vector<std::string> &strings)
{
    for (auto string : strings)
        strList.push_back(string);
}

void Suggestions::AddString(const std::string &string)
{
    strList.push_back(string);
}

void Suggestions::SetStrings()
{
    listSuggestions.ClearSel();
    listSuggestions.ClearItems();

    strList.sort(alphabetize);
    for ( auto &str : strList )
        listSuggestions.AddString(str);
}

void Suggestions::SetStrings(const std::vector<std::string> &strings)
{
    strList.clear();
    for ( auto str : strings )
        strList.push_back(str);

    SetStrings();
}

void Suggestions::Show()
{
    SetStrings();
    ShowWindow(getHandle(), SW_SHOWNA);
    isShown = true;
}

void Suggestions::Hide()
{
    isShown = false;
    ShowWindow(getHandle(), SW_HIDE);
}

void Suggestions::SuggestNear(const std::string &str)
{
    SuggestFirstStartingWith(str);
}

void Suggestions::ArrowUp()
{
    KeyDown(VK_UP);
}

void Suggestions::ArrowDown()
{
    KeyDown(VK_DOWN);
}

std::string Suggestions::Take()
{
    std::string str;
    if ( !isShown )
        return "0";
    else if ( listSuggestions.GetCurSelString(str) )
        return str;
    else
        return "";
}

void Suggestions::DoSize()
{
    /*RECT rc = {};
    if ( getWindowRect(rc) )
        ;// listSuggestions.SetPos(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);*/
}

void Suggestions::EraseBackground(HDC hDC)
{
    /*RECT rect = {};
    if ( getClientRect(rect) )
    {
        HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hDC, &rect, hbr);
        DeleteBrush(hbr);
    }*/
}

void Suggestions::SuggestFirstStartingWith(const std::string &str)
{
    int i = 0;
    for ( auto &check : strList )
    {
        if ( firstStartsWithSecond(check, str) ) // *it begins with str
        {
            listSuggestions.SetCurSel(i);
            return;
        }
        i++;
    }
}

void Suggestions::KeyDown(WPARAM wParam)
{
    int currSel = -1;
    switch ( wParam )
    {
        case VK_UP:
            listSuggestions.GetCurSel(currSel);
            listSuggestions.SetCurSel(currSel-1);
            Command(getHandle(), MAKEWPARAM(0, LBN_SELCHANGE), (LPARAM)listSuggestions.getHandle());
            break;
        case VK_DOWN:
            listSuggestions.GetCurSel(currSel);
            listSuggestions.SetCurSel(currSel + 1);
            Command(getHandle(), MAKEWPARAM(0, LBN_SELCHANGE), (LPARAM)listSuggestions.getHandle());
            break;
    }
}

LRESULT Suggestions::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT Suggestions::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
        case LBN_SELCHANGE:
            {
                std::string str;
                if ( listSuggestions.GetCurSelString(str) )
                    SendMessage(suggestParent, WinLib::LB::WM_NEWSELTEXT, 0, (LPARAM)&str);
            }
            break;
        default:
            return ClassWindow::Command(hWnd, wParam, lParam);
            break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT Suggestions::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        //case WM_SIZE: DoSize(); break;
        case WinLib::GV::WM_NEWGRIDTEXT: SuggestFirstStartingWith(*(std::string*)lParam); break;
        case WM_KEYDOWN: KeyDown(wParam); break;
        case WM_ERASEBKGND: EraseBackground((HDC)wParam); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}

