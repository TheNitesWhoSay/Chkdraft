#include "suggestions.h"
#include <common_files/common_files.h>
#include <windows/windows_ui.h>
#include <string>
#include <vector>
#include "cross_cut/logger.h"
#include "chkdraft/chkdraft.h"

extern Logger logger;

enum_t(Id, u32, {
    TOOLBAR = ID_FIRST,
    ID_CONTEXTUAL_PROPERTIES,
    ID_CANCEL
});

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

Suggestions::Toolbar::~Toolbar()
{

}

bool Suggestions::Toolbar::CreateThis(HWND hParent, std::uint64_t windowId)
{
    if ( WindowControl::CreateControl(0, TOOLBARCLASSNAME, "", WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS|CCS_BOTTOM,
        0, 0, 0, 0, hParent, (HMENU)windowId, false ) )
    {
        HWND hTool = getHandle();
        SendMessage(hTool, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
        SendMessage(hTool, TB_SETMAXTEXTROWS, 0, 0);

        // Set toolbar to default button set
        TBADDBITMAP tbab { .hInst = HINST_COMMCTRL, .nID = IDB_STD_SMALL_COLOR };
        SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

        std::vector<TBBUTTON> buttons {
            TBBUTTON { .iBitmap = 8, .fsStyle = BTNS_SEP },
            TBBUTTON { .iBitmap = STD_PROPERTIES, .idCommand = Id::ID_CONTEXTUAL_PROPERTIES,
                       .fsState = 0, .fsStyle = TBSTYLE_BUTTON, .iString = (INT_PTR)contextualProperties.c_str() },
            TBBUTTON { .iBitmap = 8, .fsStyle = BTNS_SEP },
            TBBUTTON { .iBitmap = STD_DELETE, .idCommand = Id::ID_CANCEL,
                       .fsState = TBSTATE_ENABLED, .fsStyle = TBSTYLE_BUTTON, .iString = (INT_PTR)cancel.c_str() }
        };

        SendMessage(hTool, TB_ADDBUTTONS, buttons.size(), (LPARAM)&buttons[0]);

        return true;
    }
    else
        return false;
}

void Suggestions::Toolbar::AutoSize()
{

}

void Suggestions::Toolbar::EnableContextualPropertiesButton(const std::string & hoverText)
{
    SendMessage(getHandle(), TB_ENABLEBUTTON, Id::ID_CONTEXTUAL_PROPERTIES, 1);
    
    this->contextualProperties = icux::toUistring(hoverText);
    TBBUTTONINFO tbUpdate {
        .cbSize = sizeof(TBBUTTONINFO),
        .dwMask = TBIF_TEXT,
        .pszText = &this->contextualProperties[0]
    };
    SendMessage(getHandle(), TB_SETBUTTONINFO, Id::ID_CONTEXTUAL_PROPERTIES, (LPARAM)&tbUpdate);
}

void Suggestions::Toolbar::DisableContextualPropertiesButton()
{
    SendMessage(getHandle(), TB_ENABLEBUTTON, Id::ID_CONTEXTUAL_PROPERTIES, 0);
    
    this->contextualProperties = icux::toUistring("(Disabled)");
    TBBUTTONINFO tbUpdate {
        .cbSize = sizeof(TBBUTTONINFO),
        .dwMask = TBIF_TEXT,
        .pszText = &this->contextualProperties[0]
    };
    SendMessage(getHandle(), TB_SETBUTTONINFO, Id::ID_CONTEXTUAL_PROPERTIES, (LPARAM)&tbUpdate);
}

Suggestions::Suggestions() : isShown(false), currentTextItemIndex(-1), currentNumberItemIndex(-1), modifying(false),
    currTextItem{std::numeric_limits<uint32_t>::max(), "[]"}, currNumberItem{std::numeric_limits<uint32_t>::max(), "[]"},
    currentNumberDecorator{}, currentStringDecorator{}
{
    suggestParent = NULL;
}

Suggestions::~Suggestions()
{

}

bool Suggestions::CreateThis(HWND hParent, int x, int y, int width, int height)
{
    suggestParent = hParent;
    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Suggestions", NULL, false) )
    {
        if ( ClassWindow::CreateClassWindow(WS_EX_TOPMOST, "SuggestionsWindow", WS_POPUP, x, y, width, height, hParent, NULL) )
        {
            SetWindowPos(getHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
            toolbar.CreateThis(getHandle(), Id::TOOLBAR);
            toolbar.AutoSize();
            listSuggestions.CreateThis(getHandle(), -1, 0, width, height-toolbar.Height(), true, false, true, false, true, 0);
            this->defaultChildFonts();
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

    int newSel = -1;
    bool searchForSel = this->suggestionType == Type::FreeformText;
    currentTextItemIndex = -1;
    currentNumberItemIndex = -1;
    for ( auto & item : items )
    {
        if ( searchForSel )
        {
            int index = listSuggestions.AddItem((LPARAM)&item);
            if ( item.str == this->currTextItem.str )
            {
                newSel = index;
                searchForSel = false;
            }
        }
        else
            listSuggestions.AddItem((LPARAM)&item);
    }

    listSuggestions.SetRedraw(true);
    if ( newSel != -1 )
        listSuggestions.SetCurSel(newSel);
}

void Suggestions::SetItems(const std::vector<SuggestionItem> & items)
{
    this->items.clear();
    for ( auto item : items )
        this->items.push_back(item);

    SetItems();
}

void Suggestions::Show(Type suggestionType, std::function<std::string(std::uint32_t)> numberDecorator, std::function<std::string(const std::string &)> stringDecorator)
{
    this->suggestionType = suggestionType;
    this->currentNumberDecorator = numberDecorator;
    this->currentStringDecorator = stringDecorator;
    SetItems();
    ShowWindow(getHandle(), SW_SHOWNA);
    isShown = true;
}

void Suggestions::Hide()
{
    isShown = false;
    ShowWindow(getHandle(), SW_HIDE);
}

void Suggestions::EnableContextualPropertiesButton(const std::string & hoverText)
{
    toolbar.EnableContextualPropertiesButton(hoverText);
}

void Suggestions::DisableContextualPropertiesButton()
{
    toolbar.DisableContextualPropertiesButton();
}

bool Suggestions::SuggestIndex(int index)
{
    return listSuggestions.SetCurSel(index);
}

void Suggestions::SuggestNear(const std::string & str)
{
    SuggestFirstStartingWith(str);
}

void Suggestions::OpenContextualProperties()
{
    SendMessage(suggestParent, (UINT)WinLib::LB::WM_CONTEXTUALPROPERTIES, NULL, (LPARAM)listSuggestions.getHandle());
}

void Suggestions::CancelSuggestion()
{
    SendMessage(suggestParent, (UINT)WinLib::LB::WM_CANCELLED, NULL, (LPARAM)listSuggestions.getHandle());
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
    int itemIndex = 0;
    LPARAM itemData = 0;
    if ( !isShown )
        return SuggestionItem{std::nullopt, "0"};
    else if ( listSuggestions.GetCurSelItem(itemData, itemIndex) )
    {
        if ( itemIndex == currentTextItemIndex )
            return SuggestionItem{.data = std::nullopt, .str = this->undecoratedCurrentText};
        else
            return *((SuggestionItem*)itemData);
    }
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
    RECT rect = {};
    if ( getClientRect(rect) )
    {
        HBRUSH hbr = WinLib::ResourceManager::getSolidBrush(GetSysColor(COLOR_3DFACE));
        FillRect(hDC, &rect, hbr);
    }
}

void Suggestions::SuggestFirstStartingWith(const std::string & str)
{
    bool foundAnyMatches = false;
    bool exactMatch = false;
    int i = 0;
    if ( !str.empty() )
    {
        for ( auto & check : items )
        {
            if ( firstStartsWithSecond(check.str, str) )
            {
                foundAnyMatches = true;
                if ( check.str == str )
                    exactMatch = true;

                break;
            }
            i++;
        }
    }
    if ( i == items.size() )
        i = 0;

    std::optional<std::uint32_t> currentNumber = [](const std::string & text) -> std::optional<std::uint32_t> {
        try {
            std::size_t pos = 0;
            auto number = std::stoll(text, &pos);
            if ( pos == text.size() )
                return std::make_optional(static_cast<std::uint32_t>(number));
        } catch ( ... ) {}
        return std::nullopt;
    }(str);
    bool isNumber = currentNumber.has_value();

    this->currTextItem.str = this->currentStringDecorator ? this->currentStringDecorator(str) : str;
    this->undecoratedCurrentText = str;
    this->currNumberItem.str = isNumber ? (this->currentNumberDecorator ? this->currentNumberDecorator(*currentNumber) : str) : "";
    this->currNumberItem.data = currentNumber;

    bool numberExists = currentNumberItemIndex >= 0;
    bool numberAtWrongIndex = numberExists && currentNumberItemIndex != i;
    bool removeNumber = numberExists && (!isNumber || numberAtWrongIndex);
    bool addNumber = isNumber && (!numberExists || removeNumber);

    bool currTextPossible = suggestionType == Type::FreeformText;
    bool currTextExists = currTextPossible && currentTextItemIndex >= 0;
    bool currTextAtWrongIndex = currTextExists && (numberExists ? currentTextItemIndex != i+1 : currentTextItemIndex != i);
    bool removeCurrText = currTextExists && (exactMatch || currTextAtWrongIndex);
    bool addCurrText = currTextPossible && !exactMatch && (!currTextExists || removeCurrText);

    auto updateSelection = [&]()
    {
        bool hasCurrentNumber = currentNumberItemIndex >= 0;
        bool hasCurrentText = currTextPossible && currentTextItemIndex >= 0;
        if ( exactMatch ) // Select the exact existing-item match if any
        {
            if ( hasCurrentNumber && hasCurrentText )
                listSuggestions.SetCurSel(i+2);
            else if ( hasCurrentNumber || hasCurrentText )
                listSuggestions.SetCurSel(i+1);
            else
                listSuggestions.SetCurSel(i);
        }
        else if ( hasCurrentText ) // Select the current text
            listSuggestions.SetCurSel(currentTextItemIndex);
        else if ( hasCurrentNumber ) // && !hasCurrentText
        {
            if ( this->suggestionType == Type::Amount ) // An amount field: prefer the current number
                listSuggestions.SetCurSel(currentNumberItemIndex);
            else if ( foundAnyMatches ) // Else prefer the closest matching item if any matched
                listSuggestions.SetCurSel(currentNumberItemIndex+1);
            else
                listSuggestions.SetCurSel(i);
        }
        else
            listSuggestions.SetCurSel(i);
    };
    
    if ( removeNumber || addNumber || removeCurrText || addCurrText ) // Making modifications to the list
    {
        modifying = true;
        listSuggestions.SetRedraw(false);

        if ( removeCurrText )
        {
            listSuggestions.RemoveItem(currentTextItemIndex);
            this->currentTextItemIndex = -1;
        }

        if ( removeNumber )
        {
            listSuggestions.RemoveItem(currentNumberItemIndex);
            this->currentNumberItemIndex = -1;
            if ( currTextExists && !removeCurrText )
                --currentTextItemIndex;
        }

        if ( addNumber )
        {
            if ( currTextExists && !removeCurrText )
                ++currentTextItemIndex;

            this->currentNumberItemIndex = i;
            listSuggestions.InsertItem(this->currentNumberItemIndex, (LPARAM)&currNumberItem);
        }

        if ( addCurrText )
        {
            this->currentTextItemIndex = currentNumberItemIndex >= 0 ? i+1 : i;
            listSuggestions.InsertItem(this->currentTextItemIndex, (LPARAM)&currTextItem);
        }

        updateSelection();
        modifying = false;
        listSuggestions.SetRedraw(true);
        listSuggestions.RedrawThis();
    }
    else
    {
        updateSelection();
        listSuggestions.RedrawThis();
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
                int itemIndex = -1;
                LPARAM itemData = 0;
                if ( listSuggestions.GetCurSelItem(itemData, itemIndex) &&
                     itemIndex != this->currentTextItemIndex &&
                     itemIndex != this->currentNumberItemIndex )
                {
                    SuggestionItem & suggestionItem = *((SuggestionItem*)itemData);
                    SendMessage(suggestParent, (UINT)WinLib::LB::WM_NEWSELTEXT, 0, (LPARAM)&suggestionItem.str);
                }
            }
            break;
        default:
            switch ( LOWORD(wParam) )
            {
                case Id::ID_CONTEXTUAL_PROPERTIES: OpenContextualProperties(); break;
                case Id::ID_CANCEL: CancelSuggestion(); break;
                default:
                    return ClassWindow::Command(hWnd, wParam, lParam);
            }
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

namespace decorate
{
    std::string unitId(std::uint32_t number)
    {
        if ( number < chkd.scData->units.numReferenceUnitTypes() )
        {
            if ( auto str = CM->getUnitName<SingleLineChkdString>((Sc::Unit::Type)number, &chkd.scData.value(), false) )
                return "(Unit ID " + std::to_string(number) + "): " + *str;
        }
        return "(Unit ID): " + std::to_string(number);
    }

    std::string locationId(std::uint32_t number)
    {
        if ( auto locName = CM->getLocationName<SingleLineChkdString>(number) )
            return "(Location ID " + std::to_string(number) + "): " + *locName;
        else
            return "(Location ID): " + std::to_string(number);
    }

    std::string player(std::uint32_t number)
    {
        if ( number < triggerPlayers.size() )
            return "(Group Index " + std::to_string(number) + "): " + triggerPlayers[number];
        else
            return "(Group Index): " + std::to_string(number);
    }

    std::string numericComparison(std::uint32_t number)
    {
        switch ( Chk::Condition::Comparison(number) )
        {
            case Chk::Condition::Comparison::AtLeast: return "(Numeric Comparison " + std::to_string(number) + "): At Least";
            case Chk::Condition::Comparison::AtMost: return "(Numeric Comparison " + std::to_string(number) + "): At Most";
            case Chk::Condition::Comparison::Exactly: return "(Numeric Comparison " + std::to_string(number) + "): Exactly";
            default: return "Numeric Comparison Index): " + std::to_string(number);
        }
    }

    std::string resourceType(std::uint32_t number)
    {
        switch ( Chk::Trigger::ResourceType(number) )
        {
            case Chk::Trigger::ResourceType::Ore: return "(Resource Type " + std::to_string(number) + "): Ore";
            case Chk::Trigger::ResourceType::OreAndGas: return "(Resource Type " + std::to_string(number) + "): Ore and Gas";
            case Chk::Trigger::ResourceType::Gas: return "(Resource Type " + std::to_string(number) + "): Gas";
            default: return "(Resource Type Index): " + std::to_string(number);
        }
    }

    std::string scoreType(std::uint32_t number)
    {
        if ( number < triggerScores.size() )
            return "(Score Type " + std::to_string(number) + "): " + triggerScores[number];
        else
            return "(Score Type Index): " + std::to_string(number);
    }

    std::string switchIndex(std::uint32_t number)
    {
        if ( auto str = CM->getSwitchName<SingleLineChkdString>(number) )
            return "(Switch Index " + std::to_string(number) + "): " + *str;
        else
            return "(Switch Index): " + std::to_string(number);
    }

    std::string switchState(std::uint32_t number)
    {
        switch ( Chk::Condition::Comparison(number) )
        {
            case Chk::Condition::Comparison::Set: return "(Switch Index " + std::to_string(number) + "): Set";
            case Chk::Condition::Comparison::NotSet: return "(Switch Index " + std::to_string(number) + "): Cleared";
            default: return "(Switch State Index): " + std::to_string(number);
        }
    }

    std::string conditionType(std::uint32_t number)
    {
        if ( number < triggerConditions.size() )
            return "(Condition Type Index " + std::to_string(number) + "): " + triggerConditions[number];
        else
            return "(Condition Type Index): " + std::to_string(number);
    }

    std::string stringIndex(std::uint32_t number)
    {
        if ( auto str = CM->getString<SingleLineChkdString>(std::size_t(number), Chk::Scope::Game) )
            return "(String " + std::to_string(number) + "): " + *str;
        else
            return "(Unused String Index): " + std::to_string(number);
    }

    std::string string(const std::string & str)
    {
        return "(New String): " + str;
    }

    std::string numUnits(std::uint32_t number)
    {
        if ( Chk::Action::NumUnits(number) == Chk::Action::NumUnits::All )
            return "(Amount " + std::to_string(number) + "): All";
        else
            return "(Amount): " + std::to_string(number);
    }

    std::string cuwp(std::uint32_t number)
    {
        return "(CUWP Index): " + std::to_string(number);
    }

    std::string stateMod(std::uint32_t number)
    {
        switch ( Chk::Trigger::ValueModifier(number) )
        {
            case Chk::Trigger::ValueModifier::Disable: return "(State Modifier " + std::to_string(number) + "): Disable";
            case Chk::Trigger::ValueModifier::Enable: return "(State Modifier " + std::to_string(number) + "): Enable";
            case Chk::Trigger::ValueModifier::Toggle: return "(State Modifier " + std::to_string(number) + "): Toggle";
            default: return "(State Modifier Index): " + std::to_string(number);
        }
    }

    std::string percent(std::uint32_t number)
    {
        return std::to_string(number) + "%";
    }

    std::string order(std::uint32_t number)
    {
        switch ( Chk::Action::Order(number) )
        {
            case Chk::Action::Order::Attack: return "(Order " + std::to_string(number) + "): Attack";
            case Chk::Action::Order::Move: return "(Order " + std::to_string(number) + "): Move";
            case Chk::Action::Order::Patrol: return "(Order " + std::to_string(number) + "): Patrol";
            default: return "(Order Index): " + std::to_string(number);
        }
    }

    std::string soundStringIndex(std::uint32_t number)
    {
        if ( auto str = CM->getString<SingleLineChkdString>(std::size_t(number), Chk::Scope::Game) )
            return "(String " + std::to_string(number) + "): " + *str;
        else
            return "(Unused String Index): " + std::to_string(number);
    }

    std::string soundPathString(const std::string & str)
    {
        return "(New Sound Path String): " + str;
    }

    std::string script(std::uint32_t number)
    {
        std::string aiName {};
        if ( chkd.scData->ai.getNameById(number, aiName) )
            return "(Script Value " + std::to_string(number) + "): " + aiName;
        else
            return "(Script Value): " + std::to_string(number);
    }

    std::string allyState(std::uint32_t number)
    {
        switch ( Chk::Action::AllianceStatus(number) )
        {
            case Chk::Action::AllianceStatus::AlliedVictory: return "(Ally State " + std::to_string(number) + "): Allied Victory";
            case Chk::Action::AllianceStatus::Ally: return "(Ally State " + std::to_string(number) + "): Ally";
            case Chk::Action::AllianceStatus::Enemy: return "(Ally State " + std::to_string(number) + "): Enemy";
            default: return "(Ally State Index): " + std::to_string(number);
        }
    }

    std::string numericMod(std::uint32_t number)
    {
        switch ( Chk::Trigger::ValueModifier(number) )
        {
            case Chk::Trigger::ValueModifier::Add: return "(Value Modifier " + std::to_string(number) + "): Add";
            case Chk::Trigger::ValueModifier::SetTo: return "(Value Modifier " + std::to_string(number) + "): Set to";
            case Chk::Trigger::ValueModifier::Subtract: return "(Value Modifier " + std::to_string(number) + "): Subtract";
            default: return "(Numeric Modifier Index): " + std::to_string(number);
        }
    }

    std::string switchMod(std::uint32_t number)
    {
        switch ( Chk::Trigger::ValueModifier(number) )
        {
            case Chk::Trigger::ValueModifier::Clear: return "(Switch Modifier " + std::to_string(number) + "): Clear";
            case Chk::Trigger::ValueModifier::Randomize: return "(Switch Modifier " + std::to_string(number) + "): Randomize";
            case Chk::Trigger::ValueModifier::Set: return "(Switch Modifier " + std::to_string(number) + "): Set";
            case Chk::Trigger::ValueModifier::Toggle: return "(Switch Modifier " + std::to_string(number) + "): Toggle";
            default: return "(Switch Modifier Index): " + std::to_string(number);
        }
    }

    std::string actionType(std::uint32_t number)
    {
        if ( number < triggerActions.size() )
            return "(Action Type " + std::to_string(number) + "): " + triggerActions[number];
        else
            return "(Action Type Index): " + std::to_string(number);
    }

    std::string briefingSlot(std::uint32_t number)
    {
        if ( number < briefingTriggerSlots.size() )
            return "(Briefing Slot Index " + std::to_string(number) + "): " + briefingTriggerSlots[number];
        else
            return "(Briefing Slot Index): " + std::to_string(number);
    }

    std::string briefingActionType(std::uint32_t number)
    {
        if ( number < briefingTriggerActions.size() )
            return "(Briefing Action Type " + std::to_string(number) + "): " + briefingTriggerActions[number];
        else
            return "(Briefing Action Type Index): " + std::to_string(number);
    }

}
