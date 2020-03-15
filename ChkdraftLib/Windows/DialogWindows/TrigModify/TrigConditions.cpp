#include "TrigConditions.h"
#include "../../../Mapping/Clipboard.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Settings.h"
#include <string>
#include <map>
#include <numeric>

#define TOP_CONDITION_PADDING 50
#define BOTTOM_CONDITION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum_t(Id, u32, {
    GRID_CONDITIONS = ID_FIRST
});

TrigConditionsWindow::TrigConditionsWindow() : hBlack(NULL), trigIndex(0), gridConditions(*this, 16),
    suggestions(gridConditions.GetSuggestions()), isPasting(false)
{

}

TrigConditionsWindow::~TrigConditionsWindow()
{

}

bool TrigConditionsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TrigConditions", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "TrigConditions", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TrigConditionsWindow::DestroyThis()
{
    suggestions.Hide();
    return false;
}

void TrigConditionsWindow::RefreshWindow(u32 trigIndex)
{
    gridConditions.ClearItems();
    this->trigIndex = trigIndex;
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart);
    if ( trig != nullptr && ttg.LoadScenario(CM) )
    {
        for ( u8 y=0; y<Chk::Trigger::MaxConditions; y++ )
        {
            Chk::Condition & condition = trig->condition(y);
            if ( condition.conditionType > Chk::Condition::Type::NoCondition && condition.conditionType < Chk::Condition::NumConditionTypes )
            {
                gridConditions.item(1, y).SetDisabled(false);
                gridConditions.item(1, y).SetText(ttg.GetConditionName(condition.conditionType));
                for ( u8 x=0; x<Chk::Condition::MaxArguments; x++ )
                {
                    Chk::Condition::Argument argument = Chk::Condition::getClassicArg(condition.conditionType, x);
                    if ( argument.type != Chk::Condition::ArgType::NoType )
                    {
                        gridConditions.item(x + 2, y).SetDisabled(false);
                        gridConditions.item(x+2, y).SetText(ttg.GetConditionArgument(condition, argument));
                    }
                    else
                    {
                        gridConditions.item(x + 2, y).SetDisabled(true);
                        gridConditions.item(x + 2, y).SetText("");
                    }
                }
                
                gridConditions.SetEnabledCheck(y, !condition.isDisabled());
            }
            else if ( condition.conditionType == Chk::Condition::Type::NoCondition )
            {
                for ( u8 x = 0; x < 10; x++ )
                {
                    if ( x > 1 )
                        gridConditions.item(x, y).SetDisabled(true);
                    gridConditions.item(x, y).SetText("");
                }

                gridConditions.SetEnabledCheck(y, false);
            }
        }

        gridConditions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH, DEFAULT_COLUMN_WIDTH*2);
    }
    gridConditions.RedrawHeader();
}

void TrigConditionsWindow::DoSize()
{
    gridConditions.SetPos(2, TOP_CONDITION_PADDING, cliWidth()-2, cliHeight()-TOP_CONDITION_PADDING-BOTTOM_CONDITION_PADDING);
}

void TrigConditionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
    if ( wParam == VK_TAB )
        SendMessage(gridConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
    else if ( wParam == VK_RETURN )
        SendMessage(gridConditions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void TrigConditionsWindow::HideSuggestions()
{
    suggestions.Hide();
}

void TrigConditionsWindow::CndActEnableToggled(u8 conditionNum)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( conditionNum >= 0 && conditionNum < 16 && trig != nullptr )
    {
        Chk::Condition & condition = trig->condition(conditionNum);
        if ( condition.conditionType != Chk::Condition::Type::NoCondition )
        {
            condition.ToggleDisabled();

            CM->notifyChange(false);
            RefreshWindow(trigIndex);
            chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);

            gridConditions.SetEnabledCheck(conditionNum, !condition.isDisabled());
        }
    }
}

void TrigConditionsWindow::CutSelection()
{
    if ( gridConditions.isSelectionRectangular() )
    {
        std::string str;
        if ( gridConditions.BuildSelectionString(str) )
            StringToWindowsClipboard(str);
        else
            Error("Problem building clipboard string");

        gridConditions.DeleteSelection();
    }
    else
        Error("Operation only supported on rectangular selections.");
}

void TrigConditionsWindow::CopySelection()
{
    if ( gridConditions.isSelectionRectangular() )
    {
        std::string str;
        if ( gridConditions.BuildSelectionString(str) )
            StringToWindowsClipboard(str);
        else
            Error("Problem building clipboard string");
    }
    else
        Error("Operation only supported on rectangular selections.");
}

void TrigConditionsWindow::Paste()
{
    isPasting = true;
    if ( gridConditions.isSelectionRectangular() )
    {
        std::string pasteString;
        if ( WindowsClipboardToString(pasteString) )
        {
            int topLeftX = 0, topLeftY = 0;
            if ( gridConditions.GetSelTopLeft(topLeftX, topLeftY) )
            {
                int xc = topLeftX, yc = topLeftY;

                ArgumentEnd argEndsBy;
                bool argEndsByLine = false;
                size_t argStart = 0, argEnd = 0, argEndSize = 0;
                size_t pasteStringLength = pasteString.length();
                while ( argStart < pasteStringLength )
                {
                    argEnd = gridConditions.FindArgEnd(pasteString, argStart, argEndsBy);

                    if ( argEnd - argStart > 0 )
                        GridItemChanging(xc, yc, pasteString.substr(argStart, argEnd - argStart));
                    else
                        GridItemChanging(xc, yc, std::string(""));

                    if ( argEndsBy == ArgumentEnd::OneCharLineBreak || argEndsBy == ArgumentEnd::TwoCharLineBreak )
                    {
                        xc = topLeftX;
                        yc = yc + 1;
                    }
                    else if ( argEndsBy == ArgumentEnd::Tab )
                        xc = xc + 1;

                    if ( argEndsBy == ArgumentEnd::TwoCharLineBreak )
                        argStart = argEnd + 2;
                    else
                        argStart = argEnd + 1;
                }
            }
        }
        else
            Error("Failed to get clipboard data.");
    }
    RefreshConditionAreas();
    isPasting = false;
}

void TrigConditionsWindow::CreateSubWindows(HWND hWnd)
{
    gridConditions.CreateThis(hWnd, 2, 40, 100, 100, Id::GRID_CONDITIONS);
    suggestions.CreateThis(hWnd, 0, 0, 200, 100);
    RefreshWindow(trigIndex);
}

LRESULT TrigConditionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( wParam == Id::GRID_CONDITIONS )
    {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        mis->itemWidth = DEFAULT_COLUMN_WIDTH;
        mis->itemHeight = 15;
        return TRUE;
    }
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
    DrawSelectedCondition();
    return result;
}

void TrigConditionsWindow::ChangeConditionType(Chk::Condition & condition, Chk::Condition::Type conditionType)
{
    if ( condition.conditionType != conditionType )
    {
        if ( conditionType == Chk::Condition::Type::Command || conditionType ==  Chk::Condition::Type::Bring ||
            conditionType ==  Chk::Condition::Type::CommandTheMostAt || conditionType ==  Chk::Condition::Type::CommandTheLeastAt )
        {
            condition.locationId = 64;
        }
        else
            condition.locationId = 0;

        condition.player = 0;
        condition.amount = 0;
        condition.unitType = Sc::Unit::Type::TerranMarine;

        if ( conditionType == Chk::Condition::Type::Switch )
            condition.comparison = Chk::Condition::Comparison::NotSet;
        else
            condition.comparison = Chk::Condition::Comparison::AtLeast;

        condition.conditionType = conditionType;
        condition.typeIndex = 0;
        condition.flags = Chk::Condition::getDefaultFlags(conditionType);
        condition.maskFlag = Chk::Condition::MaskFlag::Disabled;
    }
}

bool TrigConditionsWindow::TransformCondition(Chk::Condition & condition, Chk::Condition::Type conditionType, bool refreshImmediately)
{
    if ( condition.conditionType != conditionType )
    {
        ChangeConditionType(condition, conditionType);
        if ( refreshImmediately )
            RefreshConditionAreas();

        return true;
    }
    return false;
}

void TrigConditionsWindow::RefreshConditionAreas()
{
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
    chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
}

void TrigConditionsWindow::UpdateConditionName(u8 conditionNum, const std::string & newText, bool refreshImmediately)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::Condition::Type conditionType = Chk::Condition::Type::NoCondition;
    if ( ttc.ParseConditionName(newText, conditionType) || ttc.ParseConditionName(suggestions.Take(), conditionType) )
    {
        if ( trig != nullptr )
        {
            Chk::Condition & condition = trig->condition(conditionNum);
            TransformCondition(condition, conditionType, refreshImmediately);
        }
    }
    else if ( newText.length() == 0 )
    {
        if ( trig != nullptr && trig->condition(conditionNum).conditionType != conditionType )
        {
            trig->deleteCondition(conditionNum);
            if ( refreshImmediately )
                RefreshConditionAreas();
        }
    }
}

void TrigConditionsWindow::UpdateConditionArg(u8 conditionNum, u8 argNum, const std::string & newText, bool refreshImmediately)
{
    RawString rawUpdateText, rawSuggestText;
    std::string suggestionString = suggestions.Take();
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    if ( trig != nullptr )
    {
        Chk::Condition::Argument argument = Chk::Condition::getClassicArg(trig->condition(conditionNum).conditionType, argNum);
        if ( ( ParseChkdStr(ChkdString(newText), rawUpdateText) &&
               ttc.ParseConditionArg(rawUpdateText, argument, trig->condition(conditionNum), CM, chkd.scData, trigIndex) ) ||
             ( ParseChkdStr(ChkdString(suggestionString), rawSuggestText) &&
               ttc.ParseConditionArg(rawSuggestText, argument, trig->condition(conditionNum), CM, chkd.scData, trigIndex) ) )
        {
            if ( refreshImmediately )
                RefreshConditionAreas();
        }
    }
}

BOOL TrigConditionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str)
{
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxConditions )
    {
        u8 conditionNum = (u8)gridItemY;
        if ( gridItemX == 1 ) // Condition Name
        {
            UpdateConditionName(conditionNum, str, !isPasting);
        }
        else if ( gridItemX > 1 ) // Condition Arg
        {
            u8 argNum = (u8)gridItemX - 2;
            UpdateConditionArg(conditionNum, argNum, str, !isPasting);
        }
    }
    return FALSE;
}

BOOL TrigConditionsWindow::GridItemDeleting(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxConditions )
    {
        u8 conditionNum = (u8)gridItemY;

        if ( gridItemX == 1 && // Condition Name
             trig != nullptr &&
             trig->condition(conditionNum).conditionType != Chk::Condition::Type::NoCondition )
        {
            ChangeConditionType(trig->condition(conditionNum), Chk::Condition::Type::NoCondition);
        }
        else if ( gridItemX > 1 ) // Condition Arg
        {
            // Nothing planned yet
        }
    }
    return FALSE;
}

void TrigConditionsWindow::DrawSelectedCondition()
{
    HDC hDC = GetDC(getHandle());
    if ( hDC != NULL )
    {
        Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
        if ( trig != nullptr )
        {
            int focusedX = -1,
                focusedY = -1;

            if ( gridConditions.GetFocusedItem(focusedX, focusedY) )
            {
                u8 conditionNum = (u8)focusedY;
                TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart);
                std::string str;
                ttg.LoadScenario(CM);
                str = chkd.trigEditorWindow.triggersWindow.GetConditionString(conditionNum, &(*trig), ttg);
                ttg.ClearScenario();

                UINT width = 0, height = 0;
                GetStringDrawSize(hDC, width, height, str);
                HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_MENU));
                RECT rect;
                rect.left = gridConditions.Left()+5;
                rect.right = gridConditions.Right()-5;
                rect.top = gridConditions.Top()-height-1;
                rect.bottom = gridConditions.Top()-1;
                FillRect(hDC, &rect, hBackground);
                DeleteBrush(hBackground);

                SetBkMode(hDC, TRANSPARENT);
                DrawString(hDC, gridConditions.Left()+6, gridConditions.Top()-height-1, 500, RGB(0, 0, 0), str);
            }
        }
        ReleaseDC(hDC);
    }
}

int TrigConditionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
    std::string text;
    if ( gridConditions.item(gridItemX, gridItemY).getText(text) )
    {
        HDC hDC = GetDC(getHandle());
        UINT width = 0, height = 0;
        if ( GetStringDrawSize(hDC, width, height, text) )
            return width+2;
        ReleaseDC(hDC);
    }
    return 0;
}

void TrigConditionsWindow::PreDrawItems()
{
    HDC hDC = GetDC(getHandle());
    EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, 0);
    ReleaseDC(hDC);
    hBlack = CreateSolidBrush(RGB(0, 0, 0));
}

void TrigConditionsWindow::SysColorRect(HDC hDC, RECT & rect, DWORD color)
{
    SetBkColor(hDC, GetSysColor(color));
    FillRect(hDC, &rect, GetSysColorBrush(color));
}

void TrigConditionsWindow::DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top;
    rcFill.bottom = rcItem.bottom - 1;
    rcFill.left = xStart;
    rcFill.right = xStart + width - 1;

    if ( gridConditions.isFocused(gridItemX, gridItemY) )
        SysColorRect(hDC, rcFill, COLOR_ACTIVEBORDER);
    else if ( gridConditions.item(gridItemX, gridItemY).isSelected() )
        SysColorRect(hDC, rcFill, COLOR_HIGHLIGHT);
    else
        SysColorRect(hDC, rcFill, COLOR_WINDOW);
}

void TrigConditionsWindow::DrawItemFrame(HDC hDC, RECT & rcItem, int width, int & xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top - 1;
    rcFill.bottom = rcItem.bottom;
    rcFill.left = xStart - 1;
    rcFill.right = xStart + width;

    ::FrameRect(hDC, &rcFill, hBlack);
}

void TrigConditionsWindow::DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int & xStart)
{
    if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < Chk::Trigger::MaxConditions )
        gridConditions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

    int width = ListView_GetColumnWidth(gridConditions.getHandle(), gridItemX);
    DrawItemBackground(hDC, gridItemX, gridItemY, rcItem, width, xStart);

    std::string text;
    if ( gridConditions.item(gridItemX, gridItemY).getText(text) && text.length() > 0 )
        DrawString(hDC, xStart+1, rcItem.top, width-2, RGB(0, 0, 0), text);

    if ( !gridConditions.item(gridItemX, gridItemY).isDisabled() )
        DrawItemFrame(hDC, rcItem, width, xStart);

    xStart += width;
}

void TrigConditionsWindow::DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis)
{
    if ( gridId == Id::GRID_CONDITIONS )
    {
        bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
             drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
             drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

        if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
        {
            RECT & rcItem = pdis->rcItem;
            int itemStart = rcItem.left;

            int numColumns = gridConditions.GetNumColumns();
            for ( int x=0; x<numColumns; x++ )
                DrawGridViewItem(pdis->hDC, x, pdis->itemID, rcItem, itemStart);
        }
    }
}

void TrigConditionsWindow::DrawTouchups(HDC hDC)
{
    RECT rect = { };
    if ( gridConditions.GetEditItemRect(rect) )
    {
        rect.left -= 1;
        rect.top -= 1;
        HBRUSH hHighlight = CreateSolidBrush(RGB(0, 0, 200));
        ::FrameRect(hDC, &rect, hHighlight);
        rect.left -= 1;
        rect.top -= 1;
        rect.right += 1;
        rect.bottom += 1;
        ::FrameRect(hDC, &rect, hHighlight);
        DeleteBrush(hHighlight);
    }
    gridConditions.RedrawHeader();
}

void TrigConditionsWindow::PostDrawItems()
{
    DeleteBrush(hBlack);
    hBlack = NULL;
}

void TrigConditionsWindow::SuggestNothing()
{
    suggestions.ClearStrings();
    suggestions.Hide();
}

void TrigConditionsWindow::SuggestUnit()
{
    if ( CM != nullptr )
    {
        u16 numUnitStrings = (u16)Sc::Unit::defaultDisplayNames.size();
        for ( u16 i = 0; i < numUnitStrings; i++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getUnitName<SingleLineChkdString>((Sc::Unit::Type)i, true);
            suggestions.AddString(*str);
            if ( str->compare(std::string(Sc::Unit::defaultDisplayNames[i])) != 0 )
                suggestions.AddString(std::string(Sc::Unit::defaultDisplayNames[i]));
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestLocation()
{
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No Location"));
        size_t numLocations = CM->layers.numLocations();
        for ( size_t i = 1; i <= numLocations; i++ )
        {
            Chk::LocationPtr loc = CM->layers.getLocation(i);
            std::shared_ptr<SingleLineChkdString> locationName = loc != nullptr && loc->stringId > 0 ? CM->strings.getLocationName<SingleLineChkdString>(i) : nullptr;
            if ( locationName != nullptr )
                suggestions.AddString(*locationName);
            else
            {
                std::stringstream ssLoc;
                ssLoc << "Location " << i;
                suggestions.AddString(ssLoc.str());
            }
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestPlayer()
{
    if ( CM != nullptr )
    {
        for (auto player : triggerPlayers)
            suggestions.AddString(player);
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestAmount()
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestNumericComparison()
{
    suggestions.AddString(std::string("At Least"));
    suggestions.AddString(std::string("At Most"));
    suggestions.AddString(std::string("Exactly"));
    suggestions.Show();
}

void TrigConditionsWindow::SuggestResourceType()
{
    suggestions.AddString(std::string("Ore"));
    suggestions.AddString(std::string("Ore and Gas"));
    suggestions.AddString(std::string("Gas"));
    suggestions.Show();
}

void TrigConditionsWindow::SuggestScoreType()
{
    suggestions.AddStrings(triggerScores);
    suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitch()
{
    if ( CM != nullptr )
    {
        for ( u16 i = 0; i < 256; i++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getSwitchName<SingleLineChkdString>(i);
            if ( str != nullptr )
                suggestions.AddString(*str);
            else
            {
                std::stringstream ss;
                ss << "Switch " << i + 1 << std::endl;
                suggestions.AddString(ss.str());
            }
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitchState()
{
    suggestions.AddString(std::string("Cleared"));
    suggestions.AddString(std::string("Set"));
    suggestions.Show();
}

void TrigConditionsWindow::SuggestComparison()
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestConditionType()
{
    suggestions.AddStrings(triggerConditions);
    suggestions.Show();
}

void TrigConditionsWindow::SuggestTypeIndex()
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestFlags()
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestInternalData()
{
    //suggestions.Show();
}

void TrigConditionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Condition & condition = trig->condition((u8)gridItemY);
        Chk::Condition::ArgType argType = Chk::Condition::ArgType::NoType;
        if ( gridItemX == 1 ) // Condition Name
            argType = Chk::Condition::ArgType::ConditionType;
        else if ( gridItemX > 1 ) // Condition Arg
        {
            u8 conditionArgNum = (u8)gridItemX - 2;
            if ( condition.conditionType < Chk::Condition::NumConditionTypes )
                argType = condition.getClassicArgType(condition.conditionType, conditionArgNum);
        }

        if ( argType != Chk::Condition::ArgType::NoType )
        {
            POINT pt = gridConditions.GetFocusedBottomRightScreenPt();
            if ( pt.x != -1 || pt.y != -1 )
                suggestions.MoveTo(pt.x, pt.y);
        }

        suggestions.ClearStrings();
        switch ( argType )
        {
            case Chk::Condition::ArgType::NoType: SuggestNothing(); break;
            case Chk::Condition::ArgType::Unit: SuggestUnit(); break;
            case Chk::Condition::ArgType::Location: SuggestLocation(); break;
            case Chk::Condition::ArgType::Player: SuggestPlayer(); break;
            case Chk::Condition::ArgType::Amount: SuggestAmount(); break;
            case Chk::Condition::ArgType::NumericComparison: SuggestNumericComparison(); break;
            case Chk::Condition::ArgType::ResourceType: SuggestResourceType(); break;
            case Chk::Condition::ArgType::ScoreType: SuggestScoreType(); break;
            case Chk::Condition::ArgType::Switch: SuggestSwitch(); break;
            case Chk::Condition::ArgType::SwitchState: SuggestSwitchState(); break;
            case Chk::Condition::ArgType::Comparison: SuggestComparison(); break;
            case Chk::Condition::ArgType::ConditionType: SuggestConditionType(); break;
            case Chk::Condition::ArgType::TypeIndex: SuggestTypeIndex(); break;
            case Chk::Condition::ArgType::Flags: SuggestFlags(); break;
            case Chk::Condition::ArgType::MaskFlag: SuggestInternalData(); break;
        }
    }
}

void TrigConditionsWindow::NewSuggestion(std::string & str)
{
    gridConditions.SetEditText(str);
}

LRESULT TrigConditionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( (BOOL)wParam == FALSE )
        suggestions.Hide();

    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigConditionsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT TrigConditionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_MEASUREITEM: return MeasureItem(hWnd, msg, wParam, lParam); break;
        case WM_ERASEBKGND: return EraseBackground(hWnd, msg, wParam, lParam); break;
        case WM_SHOWWINDOW: return ShowWindow(hWnd, msg, wParam, lParam); break;
        case WinLib::LB::WM_NEWSELTEXT: NewSuggestion(*(std::string*)lParam); break;
        case WinLib::GV::WM_GETGRIDITEMWIDTH: return GetGridItemWidth(LOWORD(wParam), HIWORD(wParam)); break;
        case WinLib::LB::WM_PREDRAWITEMS: PreDrawItems(); break;
        case WinLib::GV::WM_DRAWGRIDVIEWITEM: DrawGridViewRow((UINT)wParam, (PDRAWITEMSTRUCT)lParam); break;
        case WinLib::GV::WM_DRAWTOUCHUPS: DrawTouchups((HDC)wParam); break;
        case WinLib::LB::WM_POSTDRAWITEMS: PostDrawItems(); break;
        case WinLib::GV::WM_GETGRIDITEMCARETPOS: return -1; break;
        case WinLib::GV::WM_GRIDITEMCHANGING: return GridItemChanging(LOWORD(wParam), HIWORD(wParam), *(std::string*)lParam); break;
        case WinLib::GV::WM_GRIDITEMDELETING: return GridItemDeleting(LOWORD(wParam), HIWORD(wParam)); break;
        case WinLib::GV::WM_GRIDDELETEFINISHED: RefreshWindow(trigIndex); break;
        case WinLib::GV::WM_GRIDEDITSTART: GridEditStart(LOWORD(wParam), HIWORD(wParam)); break;
        case WinLib::GV::WM_GRIDEDITEND: suggestions.Hide(); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
