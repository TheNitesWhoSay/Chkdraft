#include "Suggestions.h"
#include "../../CommonFiles/CommonFiles.h"
#include "../../../WindowsLib/WindowsUi.h"
#include <string>
#include <vector>
#include "../../../CrossCutLib/Logger.h"
#include "../../Chkdraft.h"

extern Logger logger;

bool alphabetize(const SuggestionItem & first, const SuggestionItem & second)
{
    // true = first before second, false = second before first
    size_t firstLen = first.str.length(),
        secondLen = second.str.length();

    u32 i = 0;
    while ( i < firstLen && i < secondLen )
    {
        if ( tolower(first.str[i]) < tolower(second.str[i]) )
            return true;
        else if ( tolower(first.str[i]) > tolower(second.str[i]) )
            return false;

        i = i+1;
    }
    return firstLen < secondLen;
}

bool firstStartsWithSecond(const std::string & first, const std::string & second)
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
        if ( ClassWindow::CreateClassWindow(WS_EX_TOPMOST, "SuggestionsWindow", WS_POPUP, x, y, width, height, NULL, NULL) )
        {
            suggestParent = hParent;
            SetWindowPos(getHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
            listSuggestions.CreateThis(getHandle(), 0, 0, width, height + 12, false, false, true, false, true, 0);
            listSuggestions.setDefaultFont(false);
            DoSize();
            return true;
        }
    }
    return false;
}

void Suggestions::ClearItems()
{
    items.clear();
    listSuggestions.ClearSel();
    listSuggestions.ClearItems();
}

void Suggestions::AddItems(const std::vector<SuggestionItem> & items)
{
    for ( auto item : items )
        this->items.push_back(item);
}

int Suggestions::AddItem(const SuggestionItem & item)
{
    this->items.push_back(item);
    return int(items.size())-1;
}

void Suggestions::SetItems()
{
    listSuggestions.ClearSel();
    listSuggestions.ClearItems();

    items.sort(alphabetize);
    for ( auto & item : items )
        listSuggestions.AddString(item.str);
}

void Suggestions::SetItems(const std::vector<SuggestionItem> & items)
{
    this->items.clear();
    for ( auto item : items )
        this->items.push_back(item);

    SetItems();
}

void Suggestions::Show()
{
    SetItems();
    ShowWindow(getHandle(), SW_SHOWNA);
    isShown = true;
}

void Suggestions::Hide()
{
    isShown = false;
    ShowWindow(getHandle(), SW_HIDE);
}

bool Suggestions::SuggestIndex(int index)
{
    return listSuggestions.SetCurSel(index);
}

void Suggestions::SuggestNear(const std::string & str)
{
    SuggestFirstStartingWith(str);
}

bool Suggestions::HasSelection()
{
    int sel = 0;
    return listSuggestions.GetCurSel(sel);
}

void Suggestions::SelectFirst()
{
    listSuggestions.SetCurSel(0);
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

bool Suggestions::HasItems()
{
    return listSuggestions.GetNumItems() > 0;
}

bool Suggestions::IsShown()
{
    return isShown;
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
        HBRUSH hbr = WinLib::ResourceManager::getBrush(RGB(255, 0, 0));
        FillRect(hDC, &rect, hbr);
    }*/
}

void Suggestions::SuggestFirstStartingWith(const std::string & str)
{
    int i = 0;
    for ( auto & check : items )
    {
        if ( firstStartsWithSecond(check.str, str) )
        {
            listSuggestions.SetCurSel(i);
            return;
        }
        i++;
    }
}

int Suggestions::KeyDown(WPARAM wParam)
{
    int currSel = -1;
    switch ( wParam )
    {
        case VK_UP:
            listSuggestions.GetCurSel(currSel);
            if ( currSel > 0 )
            {
                listSuggestions.SetCurSel(currSel-1);
                Command(getHandle(), MAKEWPARAM(0, LBN_SELCHANGE), (LPARAM)listSuggestions.getHandle());
            }
            break;
        case VK_DOWN:
            listSuggestions.GetCurSel(currSel);
            listSuggestions.SetCurSel(currSel + 1);
            Command(getHandle(), MAKEWPARAM(0, LBN_SELCHANGE), (LPARAM)listSuggestions.getHandle());
            break;
        case VK_TAB:
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
            SendMessage(suggestParent, (UINT)WinLib::LB::WM_SELCONFIRMED, wParam, (LPARAM)listSuggestions.getHandle());
            break;
        default: return -1;
    }
    return -2;
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
                    SendMessage(suggestParent, (UINT)WinLib::LB::WM_NEWSELTEXT, 0, (LPARAM)&str);
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
        case WinLib::LB::WM_DBLCLKITEM: SendMessage(suggestParent, (UINT)WinLib::LB::WM_SELCONFIRMED, 0, (LPARAM)listSuggestions.getHandle()); break;
        case WinLib::GV::WM_NEWGRIDTEXT: SuggestFirstStartingWith(*(std::string*)lParam); break;
        case WM_KEYDOWN: KeyDown(wParam); break;
        case WM_ERASEBKGND: EraseBackground((HDC)wParam); break;
        case WM_MOUSEWHEEL: if ( isShown ) SendMessage(listSuggestions.getHandle(), WM_MOUSEWHEEL, wParam, lParam); break;
        case WM_ACTIVATE:
            if ( LOWORD(wParam) == WA_INACTIVE && isShown )
                Hide();

            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
        case WM_NCACTIVATE:
            if ( (BOOL)wParam == FALSE && isShown ) 
                Hide();

            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
        case WM_VKEYTOITEM:
            return KeyDown(LOWORD(wParam));
            break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}

