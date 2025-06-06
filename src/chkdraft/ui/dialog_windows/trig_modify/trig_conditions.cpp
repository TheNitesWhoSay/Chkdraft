#include "trig_conditions.h"
#include "mapping/clipboard.h"
#include "chkdraft/chkdraft.h"
#include "mapping/settings.h"
#include <string>
#include <map>
#include <numeric>
#include <CommCtrl.h>
#include <Windows.h>

#define TOP_CONDITION_PADDING 50
#define BOTTOM_CONDITION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum_t(Id, u32, {
    GRID_CONDITIONS = ID_FIRST
});

TrigConditionsWindow::TrigConditionsWindow() : trigIndex(0), gridConditions(*this, 16),
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
    ClassWindow::DestroyThis();
    this->trigIndex = 0;
    this->isPasting = false;
    return true;
}

void TrigConditionsWindow::RefreshWindow(u32 trigIndex)
{
    gridConditions.ClearItems();
    this->trigIndex = trigIndex;
    const Chk::Trigger & trig = CM->getTrigger(trigIndex);
    TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart, true);
    if ( ttg.loadScenario((Scenario &)*CM) )
    {
        for ( u8 y=0; y<Chk::Trigger::MaxConditions; y++ )
        {
            const Chk::Condition & condition = trig.condition(y);
            if ( condition.conditionType > Chk::Condition::Type::NoCondition && condition.conditionType < Chk::Condition::NumConditionTypes )
            {
                gridConditions.item(1, y).SetDisabled(false);
                gridConditions.item(1, y).SetText(ttg.getConditionName(condition.conditionType));
                for ( u8 x=0; x<Chk::Condition::MaxArguments; x++ )
                {
                    Chk::Condition::Argument argument = Chk::Condition::getClassicArg(condition.conditionType, x);
                    if ( argument.type != Chk::Condition::ArgType::NoType )
                    {
                        gridConditions.item(x + 2, y).SetDisabled(false);
                        gridConditions.item(x+2, y).SetText(ttg.getConditionArgument(condition, argument));
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

        gridConditions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH, DEFAULT_COLUMN_WIDTH * 4);
    }
    gridConditions.RedrawHeader();
}

void TrigConditionsWindow::DoSize()
{
    gridConditions.SetPos(5, TOP_CONDITION_PADDING, cliWidth() - 10, cliHeight() - TOP_CONDITION_PADDING - BOTTOM_CONDITION_PADDING - 5);
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
    gridConditions.EndEditing();
}

void TrigConditionsWindow::CndActEnableToggled(u8 conditionNum)
{
    if ( conditionNum >= 0 && conditionNum < 16 )
    {
        auto editCondition = CM->editTrigger(trigIndex).editCondition(conditionNum);
        if ( editCondition->conditionType != Chk::Condition::Type::NoCondition )
        {
            editCondition.toggleDisabled();

            RefreshWindow(trigIndex);
            chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);

            gridConditions.SetEnabledCheck(conditionNum, !editCondition->isDisabled());
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

void TrigConditionsWindow::RedrawThis()
{
    auto dc = this->getDeviceContext();
    EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)dc.getDcHandle(), 0);
    dc.release();
    ClassWindow::RedrawThis();
    gridConditions.RedrawThis();
}

bool TrigConditionsWindow::IsSuggestionsWindow(HWND hWnd)
{
    return hWnd == suggestions.getHandle();
}

void TrigConditionsWindow::FocusGrid()
{
    gridConditions.FocusThis();
}

void TrigConditionsWindow::CreateSubWindows(HWND hWnd)
{
    gridConditions.CreateThis(hWnd, 2, 40, 100, 100, Id::GRID_CONDITIONS);
    suggestions.CreateThis(hWnd, 0, 0, 200, 95);
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

void TrigConditionsWindow::ChangeConditionType(std::size_t triggerIndex, std::size_t conditionIndex, Chk::Condition::Type conditionType)
{
    if ( triggerIndex < CM->numTriggers() && conditionIndex < Chk::Trigger::MaxConditions )
    {
        const auto & condition = CM->read.triggers[triggerIndex].conditions[conditionIndex];
        if ( condition.conditionType != conditionType )
        {
            CM->operator()(ActionDescriptor::ChangeTriggerConditionType)->triggers[triggerIndex].conditions[conditionIndex] = Chk::Condition {
                .locationId = u32((conditionType == Chk::Condition::Type::Command || conditionType ==  Chk::Condition::Type::Bring ||
                    conditionType ==  Chk::Condition::Type::CommandTheMostAt || conditionType ==  Chk::Condition::Type::CommandTheLeastAt)
                    ? 64 : 0),
                .player = 0,
                .amount = 0,
                .unitType = Sc::Unit::Type::TerranMarine,
                .comparison = conditionType == Chk::Condition::Type::Switch ? Chk::Condition::Comparison::NotSet : Chk::Condition::Comparison::AtLeast,
                .conditionType = conditionType,
                .typeIndex = 0,
                .flags = Chk::Condition::getDefaultFlags(conditionType),
                .maskFlag = Chk::Condition::MaskFlag::Disabled
            };
        }
    }
    DrawSelectedCondition();
}

bool TrigConditionsWindow::TransformCondition(std::size_t triggerIndex, std::size_t conditionIndex, Chk::Condition::Type conditionType, bool refreshImmediately)
{
    if ( CM->read.triggers[triggerIndex].conditions[conditionIndex].conditionType != conditionType )
    {
        ChangeConditionType(triggerIndex, conditionIndex, conditionType);
        if ( refreshImmediately )
            RefreshConditionAreas();

        return true;
    }
    return false;
}

void TrigConditionsWindow::RefreshConditionAreas()
{
    RefreshWindow(trigIndex);
    chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
}

void TrigConditionsWindow::UpdateConditionName(u8 conditionNum, const std::string & newText, bool refreshImmediately)
{
    const Chk::Trigger & trig = CM->getTrigger(trigIndex);
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::Condition::Type conditionType = Chk::Condition::Type::NoCondition;
    SuggestionItem suggestion = suggestions.Take();
    if ( suggestion.data )
        TransformCondition(trigIndex, conditionNum, Chk::Condition::Type(suggestion.data.value()), refreshImmediately);
    else if ( ttc.parseConditionName(newText, conditionType) || ttc.parseConditionName(suggestion.str, conditionType) )
        TransformCondition(trigIndex, conditionNum, conditionType, refreshImmediately);
    else if ( newText.length() == 0 )
    {
        if ( trig.condition(conditionNum).conditionType != conditionType )
        {
            CM->editTrigger(trigIndex).deleteCondition(conditionNum);
            if ( refreshImmediately )
                RefreshConditionAreas();
        }
    }
}

void TrigConditionsWindow::UpdateConditionArg(u8 conditionNum, u8 argNum, const std::string & newText, bool refreshImmediately)
{
    if ( trigIndex < CM->numTriggers() && conditionNum < Chk::Trigger::MaxConditions )
    {
        RawString rawUpdateText, rawSuggestText;
        SuggestionItem suggestion = suggestions.Take();
        TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);

        const Chk::Trigger & trig = CM->getTrigger(trigIndex);
        Chk::Condition::Argument argument = Chk::Condition::getClassicArg(trig.condition(conditionNum).conditionType, argNum);

        auto editedCondition = CM->editTrigger(trigIndex).editCondition(conditionNum);
        auto editCondition = editedCondition.edit;

        if ( suggestion.data )
        {
            switch ( argument.field )
            {
                case Chk::Condition::ArgField::LocationId: editCondition.locationId = suggestion.data.value(); break;
                case Chk::Condition::ArgField::Player: editCondition.player = suggestion.data.value(); break;
                case Chk::Condition::ArgField::Amount: editCondition.amount = suggestion.data.value(); break;
                case Chk::Condition::ArgField::UnitType: editCondition.unitType = Sc::Unit::Type(suggestion.data.value()); break;
                case Chk::Condition::ArgField::Comparison: editCondition.comparison = Chk::Condition::Comparison(suggestion.data.value()); break;
                case Chk::Condition::ArgField::ConditionType: editCondition.conditionType = Chk::Condition::Type(suggestion.data.value()); break;
                case Chk::Condition::ArgField::TypeIndex: editCondition.typeIndex = u8(suggestion.data.value()); break;
                case Chk::Condition::ArgField::Flags: editCondition.flags = u8(suggestion.data.value()); break;
                case Chk::Condition::ArgField::MaskFlag: editCondition.maskFlag = Chk::Condition::MaskFlag(suggestion.data.value()); break;
                default: logger.error() << "Unknown condition arg field encountered" << std::endl; break;
            }

            if ( refreshImmediately )
                RefreshConditionAreas();
        }
        else
        {
            auto copyArg = [&](Chk::Condition::ArgField argField, const Chk::Condition & srcCondition) {
                switch ( argField )
                {
                    case Chk::Condition::ArgField::LocationId: editCondition.locationId = srcCondition.locationId; break;
                    case Chk::Condition::ArgField::Player: editCondition.player = srcCondition.player; break;
                    case Chk::Condition::ArgField::Amount: editCondition.amount = srcCondition.amount; break;
                    case Chk::Condition::ArgField::UnitType: editCondition.unitType = srcCondition.unitType; break;
                    case Chk::Condition::ArgField::Comparison: editCondition.comparison = srcCondition.comparison; break;
                    case Chk::Condition::ArgField::ConditionType: editCondition.conditionType = srcCondition.conditionType; break;
                    case Chk::Condition::ArgField::TypeIndex: editCondition.typeIndex = srcCondition.typeIndex; break;
                    case Chk::Condition::ArgField::Flags: editCondition.flags = srcCondition.flags; break;
                    case Chk::Condition::ArgField::MaskFlag: editCondition.maskFlag = srcCondition.maskFlag; break;
                }
            };

            bool madeChanges = false;
            if ( parseChkdStr(ChkdString(newText), rawUpdateText) )
            {
                Chk::Condition tempCondition {};
                if ( auto result = ttc.parseConditionArg(rawUpdateText, argument, tempCondition, (Scenario &)*CM, chkd.scData, trigIndex, !suggestion.str.empty()) )
                {
                    madeChanges = true;
                    copyArg(*result, tempCondition);
                }
            }
            if ( !madeChanges && !suggestion.str.empty() && parseChkdStr(ChkdString(suggestion.str), rawSuggestText) )
            {
                Chk::Condition tempCondition {};
                if ( auto result = ttc.parseConditionArg(rawSuggestText, argument, tempCondition, (Scenario &)*CM, chkd.scData, trigIndex, false) )
                {
                    madeChanges = true;
                    copyArg(*result, tempCondition);
                }
            }
            if ( madeChanges && refreshImmediately )
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
    const Chk::Trigger & trig = CM->getTrigger(trigIndex);
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxConditions )
    {
        u8 conditionNum = (u8)gridItemY;

        if ( gridItemX == 1 && // Condition Name
             trig.condition(conditionNum).conditionType != Chk::Condition::Type::NoCondition )
        {
            ChangeConditionType(trigIndex, conditionNum, Chk::Condition::Type::NoCondition);
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
    if ( auto dc = this->getDeviceContext() )
    {
        const Chk::Trigger & trig = CM->getTrigger(trigIndex);
        int focusedX = -1,
            focusedY = -1;

        if ( gridConditions.GetFocusedItem(focusedX, focusedY) )
        {
            u8 conditionNum = (u8)focusedY;
            TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart, true);
            std::string str;
            ttg.loadScenario((Scenario &)*CM);
            str = chkd.trigEditorWindow.triggersWindow.GetConditionString(conditionNum, trig, ttg);
            ttg.clearScenario();

            UINT width = 0, height = 0;
            GetStringDrawSize(dc, width, height, str);
            RECT rect;
            rect.left = gridConditions.Left()+5;
            rect.right = gridConditions.Right()-5;
            rect.top = gridConditions.Top()-height-1;
            rect.bottom = gridConditions.Top()-1;
            dc.fillSysRect(rect, COLOR_MENU);

            dc.setBkMode(TRANSPARENT);
            DrawString(dc, gridConditions.Left()+6, gridConditions.Top()-height-1, 500, RGB(0, 0, 0), str);
        }
    }
}

int TrigConditionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
    std::string text;
    if ( gridConditions.item(gridItemX, gridItemY).getText(text) )
    {
        if ( auto dc = this->getDeviceContext() )
        {
            dc.setDefaultFont();
            UINT width = 0, height = 0;
            if ( GetStringDrawSize(dc, width, height, text) )
                return width + 4;
        }
    }
    return 0;
}

void TrigConditionsWindow::PreDrawItems()
{

}

void TrigConditionsWindow::SysColorRect(const WinLib::DeviceContext & dc, RECT & rect, DWORD color)
{
    dc.setBkColor(GetSysColor(color));
    dc.fillSysRect(rect, color);
}

void TrigConditionsWindow::DrawItemBackground(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top;
    rcFill.bottom = rcItem.bottom - 1;
    rcFill.left = xStart;
    rcFill.right = xStart + width - 1;

    if ( gridConditions.isFocused(gridItemX, gridItemY) )
        SysColorRect(dc, rcFill, COLOR_ACTIVEBORDER);
    else if ( gridConditions.item(gridItemX, gridItemY).isSelected() )
        SysColorRect(dc, rcFill, COLOR_HIGHLIGHT);
    else
        SysColorRect(dc, rcFill, COLOR_WINDOW);
}

void TrigConditionsWindow::DrawItemFrame(const WinLib::DeviceContext & dc, RECT & rcItem, int width, int & xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top - 1;
    rcFill.bottom = rcItem.bottom;
    rcFill.left = xStart - 1;
    rcFill.right = xStart + width;

    dc.frameRect(rcFill, RGB(0, 0, 0));
}

void TrigConditionsWindow::DrawGridViewItem(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int & xStart)
{
    if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < Chk::Trigger::MaxConditions )
        gridConditions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

    int width = ListView_GetColumnWidth(gridConditions.getHandle(), gridItemX);
    DrawItemBackground(dc, gridItemX, gridItemY, rcItem, width, xStart);

    std::string text;
    RECT rcClip{xStart, rcItem.top, xStart+width-2, rcItem.bottom};
    if ( gridConditions.item(gridItemX, gridItemY).getText(text) && text.length() > 0 )
        dc.drawText(text, xStart + 1, rcItem.top, rcClip, true, false);

    if ( !gridConditions.item(gridItemX, gridItemY).isDisabled() )
        DrawItemFrame(dc, rcItem, width, xStart);

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
        WinLib::DeviceContext dc {hDC};
        rect.left -= 1;
        rect.top -= 1;
        dc.frameRect(rect, RGB(0, 0, 200));
        rect.left -= 1;
        rect.top -= 1;
        rect.right += 1;
        rect.bottom += 1;
        dc.frameRect(rect, RGB(0, 0, 200));
    }
    gridConditions.RedrawHeader();
}

void TrigConditionsWindow::PostDrawItems()
{

}

void TrigConditionsWindow::SuggestNothing()
{
    suggestions.ClearItems();
    suggestions.Hide();
}

void TrigConditionsWindow::SuggestUnit(u16 currUnit)
{
    if ( CM != nullptr )
    {
        u16 numUnitStrings = (u16)Sc::Unit::defaultDisplayNames.size();
        for ( u16 i = 0; i < numUnitStrings; i++ )
        {
            auto str = CM->getUnitName<SingleLineChkdString>((Sc::Unit::Type)i, true);
            suggestions.AddItem(SuggestionItem{i, *str});
            if ( str->compare(std::string(Sc::Unit::defaultDisplayNames[i])) != 0 )
                suggestions.AddItem(SuggestionItem{i, std::string(Sc::Unit::defaultDisplayNames[i])});
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestLocation(u32 currLocationId)
{
    if ( CM != nullptr )
    {
        suggestions.AddItem(SuggestionItem{Chk::LocationId::NoLocation, std::string("No Location")});
        size_t numLocations = CM->numLocations();
        for ( size_t i = 1; i <= numLocations; i++ )
        {
            const Chk::Location & loc = CM->getLocation(i);
            if ( auto locationName = loc.stringId > 0 ? CM->getLocationName<SingleLineChkdString>(i) : std::nullopt )
                suggestions.AddItem(SuggestionItem{uint32_t(i), *locationName});
            else if ( !loc.isBlank() )
            {
                std::stringstream ssLoc;
                ssLoc << i;
                suggestions.AddItem(SuggestionItem{uint32_t(i), ssLoc.str()});
            }
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestPlayer(u32 currPlayer)
{
    if ( CM != nullptr )
    {
        static auto trigPlayers = [&]() {
            std::vector<SuggestionItem> trigPlayers {};
            for ( size_t i=0; i<triggerPlayers.size(); ++i )
                trigPlayers.push_back(SuggestionItem{uint32_t(i), triggerPlayers[i]});

            return trigPlayers;
        }();
        suggestions.AddItems(trigPlayers);
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestAmount(u32 currAmount)
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestNumericComparison(Chk::Condition::Comparison currNumericComparison)
{
    suggestions.AddItem(SuggestionItem{Chk::Condition::Comparison::AtLeast, "At Least"});
    suggestions.AddItem(SuggestionItem{Chk::Condition::Comparison::AtMost, "At Most"});
    suggestions.AddItem(SuggestionItem{Chk::Condition::Comparison::Exactly, "Exactly"});
    suggestions.Show();
}

void TrigConditionsWindow::SuggestResourceType(u8 currType)
{
    suggestions.AddItem(SuggestionItem{Chk::Trigger::ResourceType::Ore, "Ore"});
    suggestions.AddItem(SuggestionItem{Chk::Trigger::ResourceType::OreAndGas, "Ore and Gas"});
    suggestions.AddItem(SuggestionItem{Chk::Trigger::ResourceType::Gas, "Gas"});
    suggestions.Show();
}

void TrigConditionsWindow::SuggestScoreType(u8 currType)
{
    static auto trigScores = [&]() {
        std::vector<SuggestionItem> trigScores {};
        for ( size_t i=0; i<triggerScores.size(); ++i )
            trigScores.emplace_back(SuggestionItem{uint32_t(i), triggerScores[i]});

        return trigScores;
    }();
    suggestions.AddItems(trigScores);
    suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitch(u8 currSwitch)
{
    if ( CM != nullptr )
    {
        for ( u16 i = 0; i < 256; i++ )
        {
            if ( auto str = CM->getSwitchName<SingleLineChkdString>(i) )
                suggestions.AddItem(SuggestionItem{i, *str});
            else
            {
                std::stringstream ss;
                ss << "Switch " << i + 1 << std::endl;
                suggestions.AddItem(SuggestionItem{i, ss.str()});
            }
        }
    }
    suggestions.Show();
}

void TrigConditionsWindow::SuggestSwitchState(Chk::Condition::Comparison currSwitchState)
{
    suggestions.AddItem(SuggestionItem{3, std::string("Cleared")});
    suggestions.AddItem(SuggestionItem{2, std::string("Set")});
    suggestions.Show();
}

void TrigConditionsWindow::SuggestComparison(Chk::Condition::Comparison currComparison)
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestConditionType(Chk::Condition::Type currConditionType)
{
    static auto trigConditions = [&]() {
        std::vector<SuggestionItem> trigConditions {};
        for ( size_t i=0; i<triggerConditions.size(); ++i )
            trigConditions.emplace_back(SuggestionItem{uint32_t(i), triggerConditions[i]});

        return trigConditions;
    }();
    suggestions.AddItems(trigConditions);
    suggestions.Show();
}

void TrigConditionsWindow::SuggestTypeIndex(u8 currTypeIndex)
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestFlags(u8 currFlags)
{
    //suggestions.Show();
}

void TrigConditionsWindow::SuggestMaskFlag(Chk::Condition::MaskFlag maskFlag)
{
    //suggestions.Show();
}

void TrigConditionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
    const Chk::Trigger & trig = CM->getTrigger(trigIndex);
    const Chk::Condition & condition = trig.condition((u8)gridItemY);
    Chk::Condition::Argument arg = Chk::Condition::noArg;
    if ( gridItemX == 1 ) // Condition Name
    {
        arg.type = Chk::Condition::ArgType::ConditionType;
        arg.field = Chk::Condition::ArgField::NoField;
    }
    else if ( gridItemX > 1 ) // Condition Arg
    {
        u8 conditionArgNum = (u8)gridItemX - 2;
        if ( condition.conditionType < Chk::Condition::NumConditionTypes )
            arg = condition.getClassicArg(condition.conditionType, conditionArgNum);
    }

    if ( arg.type != Chk::Condition::ArgType::NoType )
    {
        POINT pt = gridConditions.GetFocusedBottomRightScreenPt();
        if ( pt.x != -1 || pt.y != -1 )
            suggestions.MoveTo(pt.x, pt.y);
    }

    u32 argData = 0;
    switch ( arg.field )
    {
        case Chk::Condition::ArgField::LocationId: argData = condition.locationId; break;
        case Chk::Condition::ArgField::Player: argData = condition.player; break;
        case Chk::Condition::ArgField::Amount: argData = condition.amount; break;
        case Chk::Condition::ArgField::UnitType: argData = condition.unitType; break;
        case Chk::Condition::ArgField::Comparison: argData = condition.comparison; break;
        case Chk::Condition::ArgField::ConditionType: argData = condition.conditionType; break;
        case Chk::Condition::ArgField::TypeIndex: argData = condition.typeIndex; break;
        case Chk::Condition::ArgField::Flags: argData = condition.flags; break;
        case Chk::Condition::ArgField::MaskFlag: argData = condition.maskFlag; break;
    }

    suggestions.ClearItems();
    switch ( arg.type )
    {
        case Chk::Condition::ArgType::NoType: SuggestNothing(); break;
        case Chk::Condition::ArgType::Unit: SuggestUnit(u16(argData)); break;
        case Chk::Condition::ArgType::Location: SuggestLocation(argData); break;
        case Chk::Condition::ArgType::Player: SuggestPlayer(argData); break;
        case Chk::Condition::ArgType::Amount: SuggestAmount(argData); break;
        case Chk::Condition::ArgType::NumericComparison: SuggestNumericComparison(Chk::Condition::Comparison(argData)); break;
        case Chk::Condition::ArgType::ResourceType: SuggestResourceType(u8(argData)); break;
        case Chk::Condition::ArgType::ScoreType: SuggestScoreType(u8(argData)); break;
        case Chk::Condition::ArgType::Switch: SuggestSwitch(u8(argData)); break;
        case Chk::Condition::ArgType::SwitchState: SuggestSwitchState(Chk::Condition::Comparison(argData)); break;
        case Chk::Condition::ArgType::Comparison: SuggestComparison(Chk::Condition::Comparison(argData)); break;
        case Chk::Condition::ArgType::ConditionType: SuggestConditionType(Chk::Condition::Type(argData)); break;
        case Chk::Condition::ArgType::TypeIndex: SuggestTypeIndex(u8(argData)); break;
        case Chk::Condition::ArgType::Flags: SuggestFlags(u8(argData)); break;
        case Chk::Condition::ArgType::MaskFlag: SuggestMaskFlag(Chk::Condition::MaskFlag(argData)); break;
    }

    if ( suggestions.HasItems() && !suggestions.HasSelection() )
        suggestions.SelectFirst();
}

void TrigConditionsWindow::SelConfirmed(WPARAM wParam)
{
    gridConditions.EndEditing();
    gridConditions.FocusThis();
    SendMessage(gridConditions.getHandle(), WM_KEYDOWN, wParam, NULL);
}

void TrigConditionsWindow::NewSelection(u16 gridItemX, u16 gridItemY)
{
    const Chk::Trigger & trig = CM->getTrigger(trigIndex);
    const Chk::Condition & condition = trig.condition((u8)gridItemY);
    Chk::Condition::Type conditionType = condition.conditionType;

    if ( gridItemX == 1 ) // Condition Name
    {

    }
    else if ( gridItemX > 1 ) // Condition Arg
    {

    }
    DoSize();
    chkd.trigEditorWindow.triggersWindow.trigModifyWindow.RedrawThis(); // TODO: This fixes some drawing issues but intensifies flashing & should be replaced
}

void TrigConditionsWindow::NewSuggestion(std::string & str)
{
    gridConditions.SetEditText(str, !suggestions.IsShown());
}

LRESULT TrigConditionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( (BOOL)wParam == FALSE )
    {
        suggestions.Hide();
        gridConditions.EndEditing();
        gridConditions.KillFocus();
    }
    else
    {
        int x = 0;
        int y = 0;
        if ( !gridConditions.GetFocusedItem(x, y) )
            gridConditions.FocusItem(1, 0);

        gridConditions.FocusThis();
    }

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
        case WM_ACTIVATE:
            if ( wParam != WA_INACTIVE )
                FocusGrid();
            else
                gridConditions.KillFocus();
            break;
        case WM_NCHITTEST: break;
        case WM_MEASUREITEM: return MeasureItem(hWnd, msg, wParam, lParam); break;
        case WM_ERASEBKGND: return EraseBackground(hWnd, msg, wParam, lParam); break;
        case WM_SHOWWINDOW: return ShowWindow(hWnd, msg, wParam, lParam); break;
        case WinLib::GV::WM_GRIDSELCHANGED: NewSelection(LOWORD(wParam), HIWORD(wParam)); break;
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
        case WinLib::LB::WM_SELCONFIRMED: SelConfirmed(wParam); break;
        case WinLib::LB::WM_DISMISSED: gridConditions.EndEditing(); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
