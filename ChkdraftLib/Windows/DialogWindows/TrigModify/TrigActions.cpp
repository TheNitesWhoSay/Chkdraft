#include "TrigActions.h"
#include "../../../Chkdraft.h"
#include "../../ChkdControls/ChkdStringInput.h"
#include "../../ChkdControls/CuwpInput.h"
#include "../../../Mapping/Settings.h"

#define TOP_ACTION_PADDING 50
#define BOTTOM_ACTION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum_t(Id, u32, {
    GRID_ACTIONS = ID_FIRST,
    BUTTON_UNITPROPERTIES,
    BUTTON_EDITSTRING,
    BUTTON_EDITWAV
});

TrigActionsWindow::TrigActionsWindow() : hBlack(NULL), trigIndex(0), gridActions(*this, 64),
    suggestions(gridActions.GetSuggestions()), stringEditEnabled(false), wavEditEnabled(false), unitPropertiesEditEnabled(false), isPasting(false)
{

}

TrigActionsWindow::~TrigActionsWindow()
{

}

bool TrigActionsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "TrigActions", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "TrigActions", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool TrigActionsWindow::DestroyThis()
{
    suggestions.Hide();
    return false;
}

void TrigActionsWindow::RefreshWindow(u32 trigIndex)
{
    if ( scriptTable.size() == 0 )
        InitializeScriptTable();

    gridActions.ClearItems();
    this->trigIndex = trigIndex;
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart);
    if ( trig != nullptr && ttg.loadScenario(CM) )
    {
        for ( u8 y = 0; y<Chk::Trigger::MaxActions; y++ )
        {
            Chk::Action & action = trig->action(y);
            if ( action.actionType > Chk::Action::Type::NoAction && action.actionType <= Chk::Action::Type::LastAction )
            {
                gridActions.item(1, y).SetText(ttg.getActionName(action.actionType));
                for ( u8 x = 0; x<Chk::Action::MaxArguments; x++ )
                {
                    Chk::Action::Argument argument = Chk::Action::getClassicArg(action.actionType, x);
                    if ( argument.type != Chk::Action::ArgType::NoType )
                    {
                        gridActions.item(x + 2, y).SetDisabled(false);
                        gridActions.item(x + 2, y).SetText(ttg.getActionArgument(action, argument));
                    }
                    else
                    {
                        gridActions.item(x + 2, y).SetText("");
                        gridActions.item(x + 2, y).SetDisabled(true);
                    }
                }

                gridActions.SetEnabledCheck(y, !action.isDisabled());
            }
            else if ( action.actionType == Chk::Action::Type::NoAction )
            {
                for ( u8 x = 0; x < 10; x++ )
                {
                    gridActions.item(x, y).SetText("");
                    if ( x > 1 )
                        gridActions.item(x, y).SetDisabled(true);
                }

                gridActions.SetEnabledCheck(y, false);
            }
        }

        gridActions.AutoSizeColumns(DEFAULT_COLUMN_WIDTH, DEFAULT_COLUMN_WIDTH * 2);
    }
    gridActions.RedrawHeader();
}

void TrigActionsWindow::DoSize()
{
    if ( stringEditEnabled || wavEditEnabled || unitPropertiesEditEnabled )
    {
        gridActions.SetPos(2, TOP_ACTION_PADDING, cliWidth() - 2,
            cliHeight() - TOP_ACTION_PADDING - BOTTOM_ACTION_PADDING - buttonEditString.Height() - 5);
    }
    else
        gridActions.SetPos(2, TOP_ACTION_PADDING, cliWidth() - 2, cliHeight() - TOP_ACTION_PADDING - BOTTOM_ACTION_PADDING - 5);

    if ( stringEditEnabled )
    {
        buttonEditString.MoveTo(cliWidth() - 2 - buttonEditString.Width(),
            cliHeight() - buttonEditString.Height() - 3);
    }

    if ( wavEditEnabled )
    {
        if ( stringEditEnabled )
        {
            buttonEditWav.MoveTo(cliWidth() - 4 - buttonEditWav.Width() - buttonEditString.Width(),
                cliHeight() - buttonEditWav.Height() - 3);
        }
        else
        {
            buttonEditWav.MoveTo(cliWidth() - 2 - buttonEditString.Width(),
                cliHeight() - buttonEditString.Height() - 3);
        }
    }

    if ( unitPropertiesEditEnabled )
    {
        buttonUnitProperties.MoveTo(cliWidth() - 2 - buttonUnitProperties.Width(),
            cliHeight() - buttonEditString.Height() - 3);
    }
}

void TrigActionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
    if ( wParam == VK_TAB || wParam == VK_RETURN )
        SendMessage(gridActions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void TrigActionsWindow::HideSuggestions()
{
    suggestions.Hide();
}

void TrigActionsWindow::CndActEnableToggled(u8 actionNum)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( actionNum >= 0 && actionNum < 64 && trig != nullptr )
    {
        Chk::Action & action = trig->action(actionNum);
        if ( action.actionType != Chk::Action::Type::NoAction )
        {
            action.toggleDisabled();

            CM->notifyChange(false);
            RefreshWindow(trigIndex);
            chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);

            gridActions.SetEnabledCheck(actionNum, !action.isDisabled());
        }
    }
}

void TrigActionsWindow::CutSelection()
{
    if ( gridActions.isSelectionRectangular() )
    {
        std::string str;
        if ( gridActions.BuildSelectionString(str) )
            StringToWindowsClipboard(str);
        else
            Error("Problem building clipboard string");

        gridActions.DeleteSelection();
    }
    else
        Error("Operation only supported on rectangular selections.");
}

void TrigActionsWindow::CopySelection()
{
    if ( gridActions.isSelectionRectangular() )
    {
        std::string str;
        if ( gridActions.BuildSelectionString(str) )
            StringToWindowsClipboard(str);
        else
            Error("Problem building clipboard string");
    }
    else
        Error("Operation only supported on rectangular selections.");
}

void TrigActionsWindow::Paste()
{
    isPasting = true;
    if ( gridActions.isSelectionRectangular() )
    {
        std::string pasteString;
        if ( WindowsClipboardToString(pasteString) )
        {
            int topLeftX = 0, topLeftY = 0;
            if ( gridActions.GetSelTopLeft(topLeftX, topLeftY) )
            {
                int xc = topLeftX, yc = topLeftY;

                ArgumentEnd argEndsBy;
                bool argEndsByLine = false;
                size_t argStart = 0, argEnd = 0, argEndSize = 0;
                size_t pasteStringLength = pasteString.length();
                while ( argStart < pasteStringLength )
                {
                    argEnd = gridActions.FindArgEnd(pasteString, argStart, argEndsBy);

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
    RefreshActionAreas();
    isPasting = false;
}

void TrigActionsWindow::RedrawThis()
{
    HDC hDC = GetDC(getHandle());
    EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, 0);
    ReleaseDC(hDC);
    ClassWindow::RedrawThis();
    gridActions.RedrawThis();
}

void TrigActionsWindow::InitializeScriptTable()
{
    size_t numScripts = chkd.scData.ai.numEntries();
    for ( size_t i = 0; i < numScripts; i++ )
    {
        std::string scriptName;
        const Sc::Ai::Entry & entry = chkd.scData.ai.getEntry(i);
        if ( chkd.scData.ai.getName(i, scriptName) )
        {
            char* scriptStringPtr = (char*)&entry.identifier;
            char scriptIdString[5] = {scriptStringPtr[0], scriptStringPtr[1], scriptStringPtr[2], scriptStringPtr[3], '\0'};
            std::string displayString = scriptName + " (" + std::string(scriptIdString) + ")";
            size_t hash = strHash(displayString);
            scriptTable.insert(std::pair<size_t, std::pair<u32, std::string>>(hash, std::pair<u32, std::string>(entry.identifier, displayString)));
        }
    }
}

void TrigActionsWindow::CreateSubWindows(HWND hWnd)
{
    gridActions.CreateThis(hWnd, 2, 40, 100, 100, Id::GRID_ACTIONS);
    suggestions.CreateThis(hWnd, 0, 0, 262, 100);
    buttonEditString.CreateThis(hWnd, 0, 0, 100, 22, "Edit String", Id::BUTTON_EDITSTRING);
    buttonEditString.Hide();
    buttonEditWav.CreateThis(hWnd, 0, 0, 100, 22, "Edit WAV", Id::BUTTON_EDITWAV);
    buttonEditWav.Hide();
    buttonUnitProperties.CreateThis(hWnd, 0, 0, 125, 22, "Edit Unit Properties", Id::BUTTON_UNITPROPERTIES);
    buttonUnitProperties.Hide();
    RefreshWindow(trigIndex);
}

LRESULT TrigActionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( wParam == Id::GRID_ACTIONS )
    {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        mis->itemWidth = DEFAULT_COLUMN_WIDTH;
        mis->itemHeight = 15;
        return TRUE;
    }
    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigActionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
    DrawSelectedAction();
    return result;
}

void TrigActionsWindow::ChangeActionType(Chk::Action & action, Chk::Action::Type newType)
{
    if ( action.actionType != newType )
    {
        action.locationId = 0;
        action.stringId = 0;
        action.soundStringId = 0;
        action.time = 0;
        action.group = 0;
        action.number = 0;
        action.type = 0;
        action.actionType = newType;

        if ( newType == Chk::Action::Type::SetSwitch || newType == Chk::Action::Type::LeaderboardCompPlayers ||
            newType == Chk::Action::Type::SetDoodadState || newType == Chk::Action::Type::SetInvincibility )
        {
            action.type2 = 5;
        }
        else if ( newType == Chk::Action::Type::SetCountdownTimer || newType == Chk::Action::Type::SetDeaths || newType == Chk::Action::Type::SetResources ||
            newType == Chk::Action::Type::SetScore || newType == Chk::Action::Type::Transmission )
        {
            action.type2 = 7;
        }
        else
            action.type2 = 0;

        action.flags = Chk::Action::getDefaultFlags(newType);
        action.padding = 0;
        action.maskFlag = Chk::Action::MaskFlag::Disabled;
    }
}

bool TrigActionsWindow::TransformAction(Chk::Action & action, Chk::Action::Type newType, bool refreshImmediately)
{
    if ( action.actionType != newType )
    {
        ChangeActionType(action, newType);
        if ( refreshImmediately )
            RefreshActionAreas();

        return true;
    }
    return false;
}

void TrigActionsWindow::RefreshActionAreas()
{
    CM->notifyChange(false);
    RefreshWindow(trigIndex);
    chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
}

void TrigActionsWindow::UpdateActionName(u8 actionNum, const std::string & newText, bool refreshImmediately)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::Action::Type newType = Chk::Action::Type::NoAction;
    if ( ttc.parseActionName(newText, newType) || ttc.parseActionName(suggestions.Take(), newType) )
    {
        if ( trig != nullptr )
        {
            Chk::Action & action = trig->action(actionNum);
            TransformAction(action, newType, refreshImmediately);
        }
    }
    else if ( newText.length() == 0 )
    {
        if ( trig != nullptr && trig->action(actionNum).actionType != newType )
        {
            trig->deleteAction((u8)actionNum);
            if ( refreshImmediately )
                RefreshActionAreas();
        }
    }
}

void TrigActionsWindow::UpdateActionArg(u8 actionNum, u8 argNum, const std::string & newText, bool refreshImmediately)
{
    RawString rawUpdateText, rawSuggestText;
    std::string suggestionString = suggestions.Take();
    bool hasSuggestion = !suggestionString.empty();
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Action & action = trig->action(actionNum);
        if ( action.actionType < Chk::Action::NumActionTypes )
        {
            Chk::Action::ArgType argType = Chk::Action::getClassicArgType(action.actionType, argNum);
            SingleLineChkdString chkdSuggestText = ChkdString(suggestionString);
            SingleLineChkdString chkdNewText = ChkdString(newText);

            bool madeChange = false;
            if ( argType == Chk::Action::ArgType::String || argType == Chk::Action::ArgType::Sound )
            {
                size_t newStringId = CM->strings.findString<SingleLineChkdString>(chkdSuggestText);
                if ( newStringId == Chk::StringId::NoString )
                    newStringId = CM->strings.findString<SingleLineChkdString>(chkdSuggestText);

                if ( newStringId != Chk::StringId::NoString )
                {
                    if ( argType == Chk::Action::ArgType::String )
                        action.stringId = (u32)newStringId;
                    else if ( argType == Chk::Action::ArgType::Sound )
                        action.soundStringId = (u32)newStringId;

                    madeChange = true;
                }
            }
            else if ( argType == Chk::Action::ArgType::Script )
            {
                u32 newScriptNum = 0;
                size_t hash = strHash(suggestionString);
                size_t numMatching = scriptTable.count(hash);
                if ( numMatching == 1 )
                {
                    std::string & scriptDisplayString = scriptTable.find(hash)->second.second;
                    if ( scriptDisplayString.compare(suggestionString) == 0 )
                        newScriptNum = scriptTable.find(hash)->second.first;
                }
                else if ( numMatching > 1 )
                {
                    auto range = scriptTable.equal_range(hash);
                    foreachin(pair, range)
                    {
                        std::string & scriptDisplayString = pair->second.second;
                        if ( scriptDisplayString.compare(suggestionString) == 0 )
                        {
                            newScriptNum = scriptTable.find(hash)->second.first;
                            break;
                        }
                    }
                }
                else
                {
                    Chk::Action::Argument argument = Chk::Action::getClassicArg(trig->action(actionNum).actionType, argNum);
                    madeChange = (parseChkdStr(chkdNewText, rawUpdateText) &&
                        ttc.parseActionArg(rawUpdateText, argument, action, CM, chkd.scData, trigIndex, hasSuggestion)) ||
                        (hasSuggestion && parseChkdStr(chkdSuggestText, rawSuggestText) &&
                            ttc.parseActionArg(rawSuggestText, argument, action, CM, chkd.scData, trigIndex, false));
                }

                if ( newScriptNum != 0 )
                {
                    action.number = newScriptNum;
                    madeChange = true;
                }
            }
            else
            {
                Chk::Action::Argument argument = Chk::Action::getClassicArg(trig->action(actionNum).actionType, argNum);
                madeChange = (parseChkdStr(chkdNewText, rawUpdateText) &&
                    ttc.parseActionArg(rawUpdateText, argument, action, CM, chkd.scData, trigIndex, hasSuggestion)) ||
                    (hasSuggestion && parseChkdStr(chkdSuggestText, rawSuggestText) &&
                        ttc.parseActionArg(rawSuggestText, argument, action, CM, chkd.scData, trigIndex, false));
            }

            if ( madeChange )
            {
                if ( refreshImmediately )
                    RefreshActionAreas();
            }
        }
    }
}

BOOL TrigActionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str)
{
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxActions )
    {
        u8 actionNum = (u8)gridItemY;
        if ( gridItemX == 1 ) // Action Name
        {
            UpdateActionName(actionNum, str, !isPasting);
        }
        else if ( gridItemX > 1 ) // Action Arg
        {
            u8 argNum = (u8)gridItemX - 2;
            UpdateActionArg(actionNum, argNum, str, !isPasting);
        }
    }
    return FALSE;
}

BOOL TrigActionsWindow::GridItemDeleting(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxActions )
    {
        u8 actionNum = (u8)gridItemY;

        if ( gridItemX == 1 && // Action Name
            trig != nullptr &&
            trig->action(actionNum).actionType != Chk::Action::Type::NoAction )
        {
            ChangeActionType(trig->action(actionNum), Chk::Action::Type::NoAction);
        }
        else if ( gridItemX > 1 ) // Action Arg
        {
            // Nothing planned yet
        }
    }
    return FALSE;
}

void TrigActionsWindow::DrawSelectedAction()
{
    HDC hDC = GetDC(getHandle());
    if ( hDC != NULL )
    {
        Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
        if ( trig != nullptr )
        {
            int focusedX = -1,
                focusedY = -1;

            if ( gridActions.GetFocusedItem(focusedX, focusedY) )
            {
                u8 actionNum = (u8)focusedY;
                TextTrigGenerator ttg(Settings::useAddressesForMemory, Settings::deathTableStart);
                ttg.loadScenario(CM);
                ChkdString str = chkd.trigEditorWindow.triggersWindow.GetActionString(actionNum, &(*trig), ttg);
                ttg.clearScenario();

                UINT width = 0, height = 0;
                GetStringDrawSize(hDC, width, height, str);
                HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_MENU));
                RECT rect;
                rect.left = gridActions.Left() + 5;
                rect.right = gridActions.Right() - 5;
                rect.top = gridActions.Top() - height - 1;
                rect.bottom = gridActions.Top() - 1;
                FillRect(hDC, &rect, hBackground);
                DeleteBrush(hBackground);

                SetBkMode(hDC, TRANSPARENT);
                DrawString(hDC, gridActions.Left() + 6, gridActions.Top() - height - 1, 500, RGB(0, 0, 0), str);
            }
        }
        ReleaseDC(hDC);
    }
}

int TrigActionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
    std::string text;
    if ( gridActions.item(gridItemX, gridItemY).getText(text) )
    {
        HDC hDC = GetDC(getHandle());
        UINT width = 0, height = 0;
        if ( GetStringDrawSize(hDC, width, height, text) )
            return width + 2;

        ReleaseDC(hDC);
    }
    return 0;
}

void TrigActionsWindow::PreDrawItems()
{
    HDC hDC = GetDC(getHandle());
    EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)hDC, 0);
    ReleaseDC(hDC);
    hBlack = CreateSolidBrush(RGB(0, 0, 0));
}

void TrigActionsWindow::SysColorRect(HDC hDC, RECT & rect, DWORD color)
{
    SetBkColor(hDC, GetSysColor(color));
    FillRect(hDC, &rect, GetSysColorBrush(color));
}

void TrigActionsWindow::DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top;
    rcFill.bottom = rcItem.bottom - 1;
    rcFill.left = xStart;
    rcFill.right = xStart + width - 1;

    if ( gridActions.isFocused(gridItemX, gridItemY) )
        SysColorRect(hDC, rcFill, COLOR_ACTIVEBORDER);
    else if ( gridActions.item(gridItemX, gridItemY).isSelected() )
        SysColorRect(hDC, rcFill, COLOR_HIGHLIGHT);
    else
        SysColorRect(hDC, rcFill, COLOR_WINDOW);
}

void TrigActionsWindow::DrawItemFrame(HDC hDC, RECT & rcItem, int width, int & xStart)
{
    RECT rcFill = {};
    rcFill.top = rcItem.top - 1;
    rcFill.bottom = rcItem.bottom;
    rcFill.left = xStart - 1;
    rcFill.right = xStart + width;

    ::FrameRect(hDC, &rcFill, hBlack);
}

void TrigActionsWindow::DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT & rcItem, int & xStart)
{
    if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < Chk::Trigger::MaxActions )
        gridActions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

    int width = ListView_GetColumnWidth(gridActions.getHandle(), gridItemX);
    DrawItemBackground(hDC, gridItemX, gridItemY, rcItem, width, xStart);

    std::string text;
    if ( gridActions.item(gridItemX, gridItemY).getText(text) )
        DrawString(hDC, xStart + 1, rcItem.top, width - 2, RGB(0, 0, 0), text);

    if ( !gridActions.item(gridItemX, gridItemY).isDisabled() )
        DrawItemFrame(hDC, rcItem, width, xStart);

    xStart += width;
}

void TrigActionsWindow::DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis)
{
    if ( gridId == Id::GRID_ACTIONS )
    {
        bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
            drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
            drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

        if ( pdis->itemID != -1 && (drawSelection || drawEntire) )
        {
            RECT & rcItem = pdis->rcItem;
            int itemStart = rcItem.left;

            int numColumns = gridActions.GetNumColumns();
            for ( int x = 0; x<numColumns; x++ )
                DrawGridViewItem(pdis->hDC, x, pdis->itemID, rcItem, itemStart);
        }
    }
}

void TrigActionsWindow::DrawTouchups(HDC hDC)
{
    RECT rect = {};
    if ( gridActions.GetEditItemRect(rect) )
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
    gridActions.RedrawHeader();
}

void TrigActionsWindow::PostDrawItems()
{
    DeleteBrush(hBlack);
    hBlack = NULL;
}

void TrigActionsWindow::SuggestNothing()
{
    suggestions.ClearStrings();
    suggestions.Hide();
}

void TrigActionsWindow::SuggestLocation()
{
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No Location"));
        size_t numLocations = (u16)CM->layers.numLocations();
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

void TrigActionsWindow::SuggestString()
{
    SingleLineChkdString str;
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No String"));
        size_t stringCapacity = CM->strings.getCapacity(Chk::Scope::Game);
        for ( size_t i = 1; i <= stringCapacity; i++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getString<SingleLineChkdString>(i, Chk::Scope::Game);
            if ( str != nullptr )
                suggestions.AddString(*str);
        }
        stringCapacity = CM->strings.getCapacity(Chk::Scope::Editor);
        for ( size_t i = 1; i <= stringCapacity; i++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getString<SingleLineChkdString>(i, Chk::Scope::Editor);
            if ( str != nullptr )
                suggestions.AddString(*str);
        }
    }
    suggestions.Show();
}

void TrigActionsWindow::SuggestPlayer()
{
    suggestions.AddStrings(triggerPlayers);
    suggestions.Show();
}

void TrigActionsWindow::SuggestUnit()
{
    if ( CM != nullptr )
    {
        u16 numUnitTypes = (u16)Sc::Unit::defaultDisplayNames.size();
        for ( u16 i = 0; i < numUnitTypes; i++ )
        {
            SingleLineChkdStringPtr str = CM->strings.getUnitName<SingleLineChkdString>((Sc::Unit::Type)i, true);
            suggestions.AddString(*str);
            if ( str->compare(std::string(Sc::Unit::defaultDisplayNames[i])) != 0 )
                suggestions.AddString(std::string(Sc::Unit::defaultDisplayNames[i]));
        }
    }
    suggestions.Show();
}

void TrigActionsWindow::SuggestNumUnits()
{
    suggestions.AddString(std::string("All"));
    suggestions.Show();
}

void TrigActionsWindow::SuggestCUWP()
{
    bool hasCuwps = false;
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( CM->triggers.cuwpUsed(i) )
        {
            hasCuwps = true;
            suggestions.AddString(std::to_string((int)i));
        }
    }

    if ( hasCuwps )
        suggestions.Show();
}

void TrigActionsWindow::SuggestTextFlags()
{

}

void TrigActionsWindow::SuggestAmount()
{

}

void TrigActionsWindow::SuggestScoreType()
{
    suggestions.AddStrings(triggerScores);
    suggestions.Show();
}

void TrigActionsWindow::SuggestResourceType()
{
    suggestions.AddString(std::string("Ore"));
    suggestions.AddString(std::string("Ore and Gas"));
    suggestions.AddString(std::string("Gas"));
    suggestions.Show();
}

void TrigActionsWindow::SuggestStateMod()
{
    const std::vector<std::string> stateMods = { "Disable", "Enable", "Toggle" };
    suggestions.AddStrings(stateMods);
    suggestions.Show();
}

void TrigActionsWindow::SuggestPercent()
{
    for ( int i = 0; i <= 100; i++ )
        suggestions.AddString(std::to_string(i));

    suggestions.Show();
}

void TrigActionsWindow::SuggestOrder()
{
    const std::vector<std::string> orders = { "Attack", "Move", "Patrol" };
    suggestions.AddStrings(orders);
    suggestions.Show();
}

void TrigActionsWindow::SuggestWav()
{
    for ( size_t i = 0; i < 512; i++ )
    {
        size_t soundStringId = CM->triggers.getSoundStringId(i);
        if ( soundStringId != Chk::StringId::UnusedSound )
        {
            ChkdStringPtr soundStr = CM->strings.getString<ChkdString>(soundStringId);
            if ( soundStr != nullptr )
                suggestions.AddString(*soundStr);
            else
                suggestions.AddString(std::to_string(soundStringId));
        }
    }
}

void TrigActionsWindow::SuggestDuration()
{

}

void TrigActionsWindow::SuggestScript()
{
    suggestions.AddString(std::string("No Script"));
    size_t numScripts = chkd.scData.ai.numEntries();
    for ( size_t i = 0; i < numScripts; i++ )
    {
        std::string scriptName;
        const Sc::Ai::Entry & entry = chkd.scData.ai.getEntry(i);
        if ( chkd.scData.ai.getName(i, scriptName) )
        {
            char* scriptStringPtr = (char*)&entry.identifier;
            char scriptIdString[5] = {scriptStringPtr[0], scriptStringPtr[1], scriptStringPtr[2], scriptStringPtr[3], '\0'};
            suggestions.AddString(scriptName + " (" + std::string(scriptIdString) + ")");
        }
    }
    suggestions.Show();
}

void TrigActionsWindow::SuggestAllyState()
{
    const std::vector<std::string> allyStates = { "Allied Victory", "Ally", "Enemy" };
    suggestions.AddStrings(allyStates);
    suggestions.Show();
}

void TrigActionsWindow::SuggestNumericMod()
{
    const std::vector<std::string> numericMod = { "Add", "Set to", "Subtract" };
    suggestions.AddStrings(numericMod);
    suggestions.Show();
}

void TrigActionsWindow::SuggestSwitch()
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

void TrigActionsWindow::SuggestSwitchMod()
{
    const std::vector<std::string> switchMod = { "Clear", "Randomize", "Set", "Toggle" };
    suggestions.AddStrings(switchMod);
    suggestions.Show();
}

void TrigActionsWindow::SuggestActionType()
{
    suggestions.AddStrings(triggerActions);
    suggestions.Show();
}

void TrigActionsWindow::SuggestFlags()
{

}

void TrigActionsWindow::SuggestNumber() // Amount, Group2, LocDest, UnitPropNum, ScriptNum
{

}

void TrigActionsWindow::SuggestTypeIndex() // Unit, ScoreType, ResourceType, AllianceStatus
{

}

void TrigActionsWindow::SuggestSecondaryTypeIndex() // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
{

}

void TrigActionsWindow::SuggestInternalData()
{

}

void TrigActionsWindow::EnableStringEdit()
{
    stringEditEnabled = true;
    buttonEditString.Show();
}

void TrigActionsWindow::DisableStringEdit()
{
    buttonEditString.Hide();
    stringEditEnabled = false;
}

void TrigActionsWindow::ButtonEditString()
{
    ChkdString str = "";
    if ( ChkdStringInputDialog::GetChkdString(str, GetCurrentActionsString(), getHandle()) )
    {
        int focusedX, focusedY;
        Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
        if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
        {
            Chk::Action & action = trig->action((u8)focusedY);
            Chk::Action::Type actionType = action.actionType;

            for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
            {
                Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);
                if ( argType == Chk::Action::ArgType::String )
                {
                    size_t stringId = CM->strings.addString<ChkdString>(str);
                    if ( stringId != Chk::StringId::NoString )
                    {
                        action.stringId = (u32)stringId;
                        chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
                        break;
                    }
                }
            }
        }
        SetFocus(gridActions.getHandle());
    }
}

void TrigActionsWindow::EnableWavEdit()
{
    wavEditEnabled = true;
    buttonEditWav.Show();
}

void TrigActionsWindow::DisableWavEdit()
{
    buttonEditWav.Hide();
    wavEditEnabled = false;
}

void TrigActionsWindow::ButtonEditWav()
{
    ChkdString wavStr;
    if ( ChkdStringInputDialog::GetChkdString(wavStr, GetCurrentActionsWav(), getHandle()) )
    {
        int focusedX = 0, focusedY = 0;
        Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
        if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
        {
            Chk::Action & action = trig->action((u8)focusedY);
            Chk::Action::Type actionType = action.actionType;

            for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
            {
                Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);

                if ( argType == Chk::Action::ArgType::Sound )
                {
                    size_t stringId = CM->strings.addString<ChkdString>(wavStr);
                    if ( stringId != Chk::StringId::NoString )
                    {
                        CM->addSound(stringId);
                        action.soundStringId = (u32)stringId;
                        chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
                        break;
                    }
                }
            }
        }
    }
}

void TrigActionsWindow::EnableUnitPropertiesEdit()
{
    unitPropertiesEditEnabled = true;
    buttonUnitProperties.Show();
}

void TrigActionsWindow::DisableUnitPropertiesEdit()
{
    buttonUnitProperties.Hide();
    unitPropertiesEditEnabled = false;
}

void TrigActionsWindow::ButtonEditUnitProperties()
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    int focusedX = 0, focusedY = 0;
    if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action & action = trig->action((u8)focusedY);
        u32 cuwpIndex = action.number;
        Chk::Cuwp initialCuwp = CM->triggers.getCuwp(cuwpIndex);
        Chk::Cuwp newCuwp = {};
        if ( CuwpInputDialog::GetCuwp(newCuwp, initialCuwp, getHandle()) )
        {
            size_t newCuwpIndex = CM->triggers.addCuwp(newCuwp, trigIndex, (size_t)focusedY);
            if ( newCuwpIndex < Sc::Unit::MaxCuwps )
            {
                action.number = (u32)newCuwpIndex;
                CM->triggers.setCuwpUsed(newCuwpIndex, true);
                RefreshWindow(trigIndex);
            }
            else
                Error("Unable to add CUWP, all 64 slots may be in use.");
        }
    }
}

void TrigActionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Action & action = trig->action((u8)gridItemY);
        Chk::Action::ArgType argType = Chk::Action::ArgType::NoType;
        if ( gridItemX == 1 ) // Action Name
            argType = Chk::Action::ArgType::ActionType;
        else if ( gridItemX > 1 ) // Action Arg
        {
            u8 actionArgNum = (u8)gridItemX - 2;
            if ( action.actionType < Chk::Action::NumActionTypes  )
                argType = Chk::Action::getClassicArgType(action.actionType, actionArgNum);
        }

        if ( argType != Chk::Action::ArgType::NoType )
        {
            POINT pt = gridActions.GetFocusedBottomRightScreenPt();
            if ( pt.x != -1 || pt.y != -1 )
                suggestions.MoveTo(pt.x, pt.y);
        }

        suggestions.ClearStrings();
        switch ( argType )
        {
            case Chk::Action::ArgType::NoType: SuggestNothing(); break;
            case Chk::Action::ArgType::Location: SuggestLocation(); break;
            case Chk::Action::ArgType::String: SuggestString(); break;
            case Chk::Action::ArgType::Player: SuggestPlayer(); break;
            case Chk::Action::ArgType::Unit: SuggestUnit(); break;
            case Chk::Action::ArgType::NumUnits: SuggestNumUnits(); break;
            case Chk::Action::ArgType::CUWP: SuggestCUWP(); break;
            case Chk::Action::ArgType::TextFlags: SuggestTextFlags(); break;
            case Chk::Action::ArgType::Amount: SuggestAmount(); break;
            case Chk::Action::ArgType::ScoreType: SuggestScoreType(); break;
            case Chk::Action::ArgType::ResourceType: SuggestResourceType(); break;
            case Chk::Action::ArgType::StateMod: SuggestStateMod(); break;
            case Chk::Action::ArgType::Percent: SuggestPercent(); break;
            case Chk::Action::ArgType::Order: SuggestOrder(); break;
            case Chk::Action::ArgType::Sound: SuggestWav(); break;
            case Chk::Action::ArgType::Duration: SuggestDuration(); break;
            case Chk::Action::ArgType::Script: SuggestScript(); break;
            case Chk::Action::ArgType::AllyState: SuggestAllyState(); break;
            case Chk::Action::ArgType::NumericMod: SuggestNumericMod(); break;
            case Chk::Action::ArgType::Switch: SuggestSwitch(); break;
            case Chk::Action::ArgType::SwitchMod: SuggestSwitchMod(); break;
            case Chk::Action::ArgType::ActionType: SuggestActionType(); break;
            case Chk::Action::ArgType::Flags: SuggestFlags(); break;
            case Chk::Action::ArgType::Number: SuggestNumber(); break; // Amount, Group2, LocDest, UnitPropNum, ScriptNum
            case Chk::Action::ArgType::TypeIndex: SuggestTypeIndex(); break; // Unit, ScoreType, ResourceType, AllianceStatus
            case Chk::Action::ArgType::SecondaryTypeIndex: SuggestSecondaryTypeIndex(); break; // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
            case Chk::Action::ArgType::Padding: SuggestInternalData(); break;
            case Chk::Action::ArgType::MaskFlag: SuggestInternalData(); break;
        }
    }
}

void TrigActionsWindow::NewSelection(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Action & action = trig->action((u8)gridItemY);
        Chk::Action::Type actionType = action.actionType;
        if ( actionType < Chk::Action::NumActionTypes )
        {
            bool includesString = false, includesWav = false;
            bool isCUWP = (actionType == Chk::Action::Type::CreateUnitWithProperties);
            for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
            {
                Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);
                if ( argType == Chk::Action::ArgType::String )
                    includesString = true;
            }

            for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
            {
                Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);
                if ( argType == Chk::Action::ArgType::Sound )
                    includesWav = true;
            }

            if ( includesString )
                EnableStringEdit();
            else
                DisableStringEdit();

            if ( includesWav )
                EnableWavEdit();
            else
                DisableWavEdit();

            if ( isCUWP )
                EnableUnitPropertiesEdit();
            else
                DisableUnitPropertiesEdit();
        }

        if ( gridItemX == 1 ) // Action Name
        {

        }
        else if ( gridItemX > 1 ) // Action Arg
        {

        }
    }
    DoSize();
    chkd.trigEditorWindow.triggersWindow.trigModifyWindow.RedrawThis();
}

void TrigActionsWindow::NewSuggestion(std::string & str)
{
    gridActions.SetEditText(str);
}

ChkdString TrigActionsWindow::GetCurrentActionsString()
{
    int focusedX, focusedY;
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action & action = trig->action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;
        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::String )
            {
                ChkdStringPtr dest = CM->strings.getString<ChkdString>(action.stringId);
                if ( dest != nullptr )
                    return *dest;
            }
        }
    }
    return "";
}

ChkdString TrigActionsWindow::GetCurrentActionsWav()
{
    int focusedX, focusedY;
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action & action = trig->action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;
        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::Sound )
            {
                ChkdStringPtr dest = CM->strings.getString<ChkdString>(action.stringId);
                if ( dest != nullptr )
                    return *dest;
            }
        }
    }
    return "";
}

LRESULT TrigActionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( (BOOL)wParam == FALSE )
    {
        suggestions.Hide();
        gridActions.EndEditing();
    }

    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT TrigActionsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT TrigActionsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
        case BN_CLICKED:
            switch ( LOWORD(wParam) )
            {
                case Id::BUTTON_EDITSTRING: ButtonEditString(); break;
                case Id::BUTTON_EDITWAV: ButtonEditWav(); break;
                case Id::BUTTON_UNITPROPERTIES: ButtonEditUnitProperties(); break;
                default: return ClassWindow::Command(hWnd, wParam, lParam); break;
            }
            break;
        default: return ClassWindow::Command(hWnd, wParam, lParam); break;
    }
    return 0;
}

LRESULT TrigActionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
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
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
