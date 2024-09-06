#include "suggestions.h"
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <string>
#include <vector>
#include <cross_cut/logger.h>
#include "chkdraft/chkdraft.h"

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

Suggestions::Suggestions() : isShown(false), suggestCurrentTextFirst(false), currentTextItemIndex(-1), modifying(false), currTextItem{std::numeric_limits<uint32_t>::max(), "[]"}
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
            listSuggestions.CreateThis(getHandle(), 0, 0, width, height, true, false, true, false, true, 0);
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
    listSuggestions.SetRedraw(false);
    listSuggestions.ClearSel();
    listSuggestions.ClearItems();

    items.sort(alphabetize);

    currentTextItemIndex = -1;
    for ( auto & item : items )
        listSuggestions.AddItem((LPARAM)&item);

    listSuggestions.SetRedraw(true);
}

void Suggestions::SetItems(const std::vector<SuggestionItem> & items)
{
    this->items.clear();
    for ( auto item : items )
        this->items.push_back(item);

    SetItems();
}

void Suggestions::Show(bool suggestCurrentTextFirst)
{
    this->suggestCurrentTextFirst = suggestCurrentTextFirst;
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

SuggestionItem Suggestions::Take()
{
    LPARAM itemData = 0;
    if ( !isShown )
        return SuggestionItem{std::nullopt, "0"};
    else if ( listSuggestions.GetCurSelItem(itemData) )
        return *((SuggestionItem*)itemData);
    else
        return SuggestionItem{std::nullopt, ""};
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
    bool exactMatch = false;
    int i = 0;
    if ( !str.empty() )
    {
        for ( auto & check : items )
        {
            if ( firstStartsWithSecond(check.str, str) )
            {
                if ( check.str == str )
                    exactMatch = true;

                break;
            }
            i++;
        }
    }
    if ( i == items.size() )
        i = 0;

    currTextItem.str = str;

    if ( suggestCurrentTextFirst )
    {
        modifying = true;
        listSuggestions.SetRedraw(false);
        if ( exactMatch )
        {
            if ( currentTextItemIndex >= 0 ) // Curr-text item currently in list, remove it
            {
                listSuggestions.RemoveItem(currentTextItemIndex);
                listSuggestions.SetCurSel(i);
                currentTextItemIndex = -1;
            }
            else // Curr-text item already not in list, just update selection
                listSuggestions.SetCurSel(i);
        }
        else if ( currentTextItemIndex < 0 ) // Curr-text item does not exist, create and select it
        {
            currentTextItemIndex = i;
            listSuggestions.InsertItem(i, (LPARAM)&currTextItem);
            listSuggestions.SetCurSel(currentTextItemIndex);
        }
        else if ( currentTextItemIndex >= 0 && currentTextItemIndex != i ) // Curr-text item exists at the wrong index, remove, recreate, and select it
        {
            listSuggestions.RemoveItem(currentTextItemIndex);
            currentTextItemIndex = i;
            listSuggestions.InsertItem(currentTextItemIndex, (LPARAM)&currTextItem);
            listSuggestions.SetCurSel(currentTextItemIndex);
        } // Else curr-text item exists at the correct index

        modifying = false;
        listSuggestions.SetRedraw(true);
        listSuggestions.RedrawThis();
    }
    else // Not suggesting current-text first
        listSuggestions.SetCurSel(i);
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
                LPARAM itemData = 0;
                if ( listSuggestions.GetCurSelItem(itemData) )
                {
                    SuggestionItem & suggestionItem = *((SuggestionItem*)itemData);
                    SendMessage(suggestParent, (UINT)WinLib::LB::WM_NEWSELTEXT, 0, (LPARAM)&suggestionItem.str);
                }
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
            {
                SendMessage(suggestParent, (UINT)WinLib::LB::WM_DISMISSED, NULL, NULL);
                Hide();
            }

            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
        case WM_NCACTIVATE:
            if ( (BOOL)wParam == FALSE && isShown ) 
            {
                SendMessage(suggestParent, (UINT)WinLib::LB::WM_DISMISSED, NULL, NULL);
                Hide();
            }

            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
        case WM_VKEYTOITEM:
            return KeyDown(LOWORD(wParam));
            break;

        case WinLib::LB::WM_PREMEASUREITEMS:
            listDc.emplace(listSuggestions.getHandle());
            break;

        case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                if ( !modifying )
                {
                    SuggestionItem & suggestionItem = *((SuggestionItem*)mis->itemData);

                    int32_t width = 0;
                    int32_t height = 0;
                    if ( listDc->getTabTextExtent(suggestionItem.str, width, height) )
                    {
                        mis->itemWidth = width;
                        mis->itemHeight = height-3;

                        return TRUE;
                    }
                }
            }
            break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listDc = std::nullopt;
            break;

        case WinLib::LB::WM_PREDRAWITEMS:
            break;

        case WM_DRAWITEM:
            {
                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                {
                    WinLib::DeviceContext dc { pdis->hDC };
                    SuggestionItem & suggestionItem = *((SuggestionItem*)pdis->itemData);
                    SetBkMode(pdis->hDC, TRANSPARENT);
                    if ( isSelected )
                    {
                        dc.fillRect(pdis->rcItem, RGB(0, 120, 215));
                        DrawString(dc, pdis->rcItem.left+3, pdis->rcItem.top, pdis->rcItem.right-pdis->rcItem.left,
                            RGB(255, 255, 255), suggestionItem.str);
                    }
                    else
                    {
                        dc.fillRect(pdis->rcItem, RGB(255, 255, 255));
                        DrawString(dc, pdis->rcItem.left+3, pdis->rcItem.top, pdis->rcItem.right-pdis->rcItem.left,
                            RGB(0, 0, 0), suggestionItem.str);
                    }
                }
                return TRUE;
            }
            break;

        case WinLib::LB::WM_POSTDRAWITEMS:
            break;

        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}

