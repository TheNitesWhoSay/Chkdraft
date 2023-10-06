#include "BriefingTrigActions.h"
#include "../../../Chkdraft.h"
#include "../../ChkdControls/ChkdStringInput.h"
#include "../../ChkdControls/CuwpInput.h"
#include "../../../Mapping/Settings.h"
#include <CommCtrl.h>

#define TOP_ACTION_PADDING 50
#define BOTTOM_ACTION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum_t(Id, u32, {
    GRID_ACTIONS = ID_FIRST,
    BUTTON_EDITSTRING,
    BUTTON_EDITSOUND
});

BriefingTrigActionsWindow::BriefingTrigActionsWindow() : briefingTrigIndex(0), gridActions(*this, 64),
    suggestions(gridActions.GetSuggestions()), stringEditEnabled(false), soundEditEnabled(false), isPasting(false)
{

}

BriefingTrigActionsWindow::~BriefingTrigActionsWindow()
{

}

bool BriefingTrigActionsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "BriefingTrigActions", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "BriefingTrigActions", WS_CHILD,
            5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool BriefingTrigActionsWindow::DestroyThis()
{
    suggestions.Hide();
    ClassWindow::DestroyThis();
    this->briefingTrigIndex = 0;
    this->isPasting = false;
    this->stringEditEnabled = false;
    this->soundEditEnabled = false;
    return true;
}

void BriefingTrigActionsWindow::RefreshWindow(u32 briefingTrigIndex)
{
    gridActions.ClearItems();
    this->briefingTrigIndex = briefingTrigIndex;
    const auto & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    BriefingTextTrigGenerator ttg{};
    if ( ttg.loadScenario(*CM) )
    {
        for ( u8 y = 0; y<Chk::Trigger::MaxActions; y++ )
        {
            const Chk::Action & action = briefingTrig.action(y);
            if ( action.actionType > Chk::Action::Type::NoAction && action.actionType <= Chk::Action::Type::LastAction )
            {
                gridActions.item(1, y).SetText(ttg.getBriefingActionName(action.actionType));
                for ( u8 x = 0; x<Chk::Action::MaxArguments; x++ )
                {
                    Chk::Action::Argument argument = Chk::Action::getBriefingClassicArg(action.actionType, x);
                    if ( argument.type != Chk::Action::ArgType::NoType )
                    {
                        gridActions.item(x + 2, y).SetDisabled(false);
                        gridActions.item(x + 2, y).SetText(ttg.getBriefingActionArgument(action, argument));
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

void BriefingTrigActionsWindow::DoSize()
{
    if ( stringEditEnabled || soundEditEnabled )
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

    if ( soundEditEnabled )
    {
        if ( stringEditEnabled )
        {
            buttonEditSound.MoveTo(cliWidth() - 4 - buttonEditSound.Width() - buttonEditString.Width(),
                cliHeight() - buttonEditSound.Height() - 3);
        }
        else
        {
            buttonEditSound.MoveTo(cliWidth() - 2 - buttonEditString.Width(),
                cliHeight() - buttonEditString.Height() - 3);
        }
    }
}

void BriefingTrigActionsWindow::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
    if ( wParam == VK_TAB || wParam == VK_RETURN )
        SendMessage(gridActions.getHandle(), WM_KEYDOWN, wParam, lParam);
}

void BriefingTrigActionsWindow::HideSuggestions()
{
    suggestions.Hide();
}

void BriefingTrigActionsWindow::CndActEnableToggled(u8 actionNum)
{
    auto & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( actionNum >= 0 && actionNum < 64 )
    {
        Chk::Action & action = briefingTrig.action(actionNum);
        if ( action.actionType != Chk::Action::Type::NoAction )
        {
            action.toggleDisabled();

            CM->notifyChange(false);
            RefreshWindow(briefingTrigIndex);
            chkd.briefingTrigEditorWindow.briefingTriggersWindow.RefreshWindow(false);

            gridActions.SetEnabledCheck(actionNum, !action.isDisabled());
        }
    }
}

void BriefingTrigActionsWindow::CutSelection()
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

void BriefingTrigActionsWindow::CopySelection()
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

void BriefingTrigActionsWindow::Paste()
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

void BriefingTrigActionsWindow::RedrawThis()
{
    auto dc = this->getDeviceContext();
    EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)dc.getDcHandle(), 0);
    dc.release();
    ClassWindow::RedrawThis();
    gridActions.RedrawThis();
}

void BriefingTrigActionsWindow::CreateSubWindows(HWND hWnd)
{
    gridActions.CreateThis(hWnd, 2, 40, 100, 100, Id::GRID_ACTIONS);
    suggestions.CreateThis(hWnd, 0, 0, 262, 100);
    buttonEditString.CreateThis(hWnd, 0, 0, 100, 22, "Edit String", Id::BUTTON_EDITSTRING);
    buttonEditString.Hide();
    buttonEditSound.CreateThis(hWnd, 0, 0, 100, 22, "Edit Sound", Id::BUTTON_EDITSOUND);
    buttonEditSound.Hide();
    RefreshWindow(briefingTrigIndex);
}

LRESULT BriefingTrigActionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

LRESULT BriefingTrigActionsWindow::EraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = ClassWindow::WndProc(hWnd, msg, wParam, lParam);
    DrawSelectedAction();
    return result;
}

void BriefingTrigActionsWindow::ChangeActionType(Chk::Action & action, Chk::Action::Type newType)
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

        if ( newType == Chk::Action::Type::BriefingTransmission )
            action.type2 = Chk::Trigger::ValueModifier::SetTo;
        else
            action.type2 = 0;

        action.flags = Chk::Action::getBriefingDefaultFlags(newType);
        action.padding = 0;
        action.maskFlag = Chk::Action::MaskFlag::Disabled;
    }
}

bool BriefingTrigActionsWindow::TransformAction(Chk::Action & action, Chk::Action::Type newType, bool refreshImmediately)
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

void BriefingTrigActionsWindow::RefreshActionAreas()
{
    CM->notifyChange(false);
    RefreshWindow(briefingTrigIndex);
    chkd.briefingTrigEditorWindow.briefingTriggersWindow.RefreshWindow(false);
}

void BriefingTrigActionsWindow::UpdateActionName(u8 actionNum, const std::string & newText, bool refreshImmediately)
{
    auto & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    BriefingTextTrigCompiler ttc{};
    Chk::Action::Type newType = Chk::Action::Type::NoAction;
    if ( ttc.parseBriefingActionName(newText, newType) || ttc.parseBriefingActionName(suggestions.Take(), newType) )
    {
        Chk::Action & action = briefingTrig.action(actionNum);
        TransformAction(action, newType, refreshImmediately);
    }
    else if ( newText.length() == 0 )
    {
        if ( briefingTrig.action(actionNum).actionType != newType )
        {
            briefingTrig.deleteAction((u8)actionNum);
            if ( refreshImmediately )
                RefreshActionAreas();
        }
    }
}

void BriefingTrigActionsWindow::UpdateActionArg(u8 actionNum, u8 argNum, const std::string & newText, bool refreshImmediately)
{
    RawString rawUpdateText, rawSuggestText;
    std::string suggestionString = suggestions.Take();
    bool hasSuggestion = !suggestionString.empty();
    BriefingTextTrigCompiler ttc {};
    auto & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    Chk::Action & action = briefingTrig.action(actionNum);
    if ( action.actionType < Chk::Action::NumBriefingActionTypes )
    {
        Chk::Action::ArgType argType = Chk::Action::getBriefingClassicArgType(action.actionType, argNum);
        SingleLineChkdString chkdSuggestText = ChkdString(suggestionString);
        SingleLineChkdString chkdNewText = ChkdString(newText);

        bool madeChange = false;
        if ( argType == Chk::Action::ArgType::String || argType == Chk::Action::ArgType::Sound )
        {
            size_t newStringId = CM->findString<SingleLineChkdString>(chkdSuggestText);
            if ( newStringId == Chk::StringId::NoString )
                newStringId = CM->findString<SingleLineChkdString>(chkdSuggestText);

            if ( newStringId != Chk::StringId::NoString )
            {
                if ( argType == Chk::Action::ArgType::String )
                    action.stringId = (u32)newStringId;
                else if ( argType == Chk::Action::ArgType::Sound )
                    action.soundStringId = (u32)newStringId;
                    
                CM->deleteUnusedStrings(Chk::StrScope::Both);
                madeChange = true;
            }
        }
        else
        {
            Chk::Action::Argument argument = Chk::Action::getBriefingClassicArg(briefingTrig.action(actionNum).actionType, argNum);
            madeChange = (parseChkdStr(chkdNewText, rawUpdateText) &&
                ttc.parseBriefingActionArg(rawUpdateText, argument, action, *CM, chkd.scData, briefingTrigIndex, hasSuggestion)) ||
                (hasSuggestion && parseChkdStr(chkdSuggestText, rawSuggestText) &&
                    ttc.parseBriefingActionArg(rawSuggestText, argument, action, *CM, chkd.scData, briefingTrigIndex, false));
        }

        if ( madeChange )
        {
            if ( refreshImmediately )
                RefreshActionAreas();
        }
    }
}

BOOL BriefingTrigActionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string & str)
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

BOOL BriefingTrigActionsWindow::GridItemDeleting(u16 gridItemX, u16 gridItemY)
{
    auto & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( gridItemY >= 0 && gridItemY < Chk::Trigger::MaxActions )
    {
        u8 actionNum = (u8)gridItemY;

        if ( gridItemX == 1 && // Action Name
            briefingTrig.action(actionNum).actionType != Chk::Action::Type::NoAction )
        {
            ChangeActionType(briefingTrig.action(actionNum), Chk::Action::Type::NoAction);
        }
        else if ( gridItemX > 1 ) // Action Arg
        {
            // Nothing planned yet
        }
    }
    return FALSE;
}

void BriefingTrigActionsWindow::DrawSelectedAction()
{
    if ( auto dc = this->getDeviceContext() )
    {
        const Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
        int focusedX = -1,
            focusedY = -1;

        if ( gridActions.GetFocusedItem(focusedX, focusedY) )
        {
            u8 actionNum = (u8)focusedY;
            BriefingTextTrigGenerator ttg {};
            ttg.loadScenario(*CM);
            ChkdString str = chkd.briefingTrigEditorWindow.briefingTriggersWindow.GetActionString(actionNum, briefingTrig, ttg);
            ttg.clearScenario();

            UINT width = 0, height = 0;
            GetStringDrawSize(dc, width, height, str);
            RECT rect {};
            rect.left = gridActions.Left() + 5;
            rect.right = gridActions.Right() - 5;
            rect.top = gridActions.Top() - height - 1;
            rect.bottom = gridActions.Top() - 1;
            dc.fillSysRect(rect, COLOR_MENU);
            dc.setBkMode(TRANSPARENT);
            DrawString(dc, gridActions.Left() + 6, gridActions.Top() - height - 1, 500, RGB(0, 0, 0), str);
        }
    }
}

int BriefingTrigActionsWindow::GetGridItemWidth(int gridItemX, int gridItemY)
{
    std::string text;
    if ( gridActions.item(gridItemX, gridItemY).getText(text) )
    {
        if ( auto dc = this->getDeviceContext() )
        {
            UINT width = 0, height = 0;
            if ( GetStringDrawSize(dc, width, height, text) )
                return width + 2;
        }
    }
    return 0;
}

void BriefingTrigActionsWindow::PreDrawItems()
{
    if ( auto dc = this->getDeviceContext() )
        EraseBackground(getHandle(), WM_ERASEBKGND, (WPARAM)dc.getDcHandle(), 0);
}

void BriefingTrigActionsWindow::SysColorRect(const WinLib::DeviceContext & dc, RECT & rect, DWORD color)
{
    dc.setBkColor(GetSysColor(color));
    dc.fillSysRect(rect, color);
}

void BriefingTrigActionsWindow::DrawItemBackground(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int width, int xStart)
{
    RECT rcFill;
    rcFill.top = rcItem.top;
    rcFill.bottom = rcItem.bottom - 1;
    rcFill.left = xStart;
    rcFill.right = xStart + width - 1;

    if ( gridActions.isFocused(gridItemX, gridItemY) )
        SysColorRect(dc, rcFill, COLOR_ACTIVEBORDER);
    else if ( gridActions.item(gridItemX, gridItemY).isSelected() )
        SysColorRect(dc, rcFill, COLOR_HIGHLIGHT);
    else
        SysColorRect(dc, rcFill, COLOR_WINDOW);
}

void BriefingTrigActionsWindow::DrawItemFrame(const WinLib::DeviceContext & dc, RECT & rcItem, int width, int & xStart)
{
    RECT rcFill = {};
    rcFill.top = rcItem.top - 1;
    rcFill.bottom = rcItem.bottom;
    rcFill.left = xStart - 1;
    rcFill.right = xStart + width;

    dc.frameRect(rcFill, RGB(0, 0, 0));
}

void BriefingTrigActionsWindow::DrawGridViewItem(const WinLib::DeviceContext & dc, int gridItemX, int gridItemY, RECT & rcItem, int & xStart)
{
    if ( gridItemX == 0 && gridItemY >= 0 && gridItemY < Chk::Trigger::MaxActions )
        gridActions.checkEnabled[gridItemY].MoveTo(rcItem.left, rcItem.top);

    int width = ListView_GetColumnWidth(gridActions.getHandle(), gridItemX);
    DrawItemBackground(dc, gridItemX, gridItemY, rcItem, width, xStart);

    std::string text;
    if ( gridActions.item(gridItemX, gridItemY).getText(text) )
        DrawString(dc, xStart + 1, rcItem.top, width - 2, RGB(0, 0, 0), text);

    if ( !gridActions.item(gridItemX, gridItemY).isDisabled() )
        DrawItemFrame(dc, rcItem, width, xStart);

    xStart += width;
}

void BriefingTrigActionsWindow::DrawGridViewRow(UINT gridId, PDRAWITEMSTRUCT pdis)
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
                DrawGridViewItem(WinLib::DeviceContext{pdis->hDC}, x, pdis->itemID, rcItem, itemStart);
        }
    }
}

void BriefingTrigActionsWindow::DrawTouchups(HDC hDC)
{
    RECT rect = {};
    if ( gridActions.GetEditItemRect(rect) )
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
    gridActions.RedrawHeader();
}

void BriefingTrigActionsWindow::PostDrawItems()
{

}

void BriefingTrigActionsWindow::SuggestNothing()
{
    suggestions.ClearStrings();
    suggestions.Hide();
}

void BriefingTrigActionsWindow::SuggestLocation()
{
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No Location"));
        size_t numLocations = (u16)CM->numLocations();
        for ( size_t i = 1; i <= numLocations; i++ )
        {
            const Chk::Location & loc = CM->getLocation(i);
            if ( auto locationName = loc.stringId > 0 ? CM->getLocationName<SingleLineChkdString>(i) : std::nullopt )
                suggestions.AddString(*locationName);
            else if ( !loc.isBlank() )
            {
                std::stringstream ssLoc;
                ssLoc << i;
                suggestions.AddString(ssLoc.str());
            }
        }
    }
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestString()
{
    SingleLineChkdString str;
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No String"));
        size_t stringCapacity = CM->getCapacity(Chk::StrScope::Game);
        for ( size_t i = 1; i <= stringCapacity; i++ )
        {
            if ( auto str = CM->getString<SingleLineChkdString>(i, Chk::StrScope::Game) )
                suggestions.AddString(*str);
        }
    }
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestSlot()
{
    suggestions.AddStrings(briefingTriggerSlots);
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestUnit()
{
    if ( CM != nullptr )
    {
        u16 numUnitTypes = (u16)Sc::Unit::defaultDisplayNames.size();
        for ( u16 i = 0; i < numUnitTypes; i++ )
        {
            auto str = CM->getUnitName<SingleLineChkdString>((Sc::Unit::Type)i, true);
            suggestions.AddString(*str);
            if ( str->compare(std::string(Sc::Unit::defaultDisplayNames[i])) != 0 )
                suggestions.AddString(std::string(Sc::Unit::defaultDisplayNames[i]));
        }
    }
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestAmount()
{

}

void BriefingTrigActionsWindow::SuggestSound()
{
    for ( size_t i = 0; i < 512; i++ )
    {
        size_t soundStringId = CM->getSoundStringId(i);
        if ( soundStringId != Chk::StringId::UnusedSound )
        {
            if ( auto soundStr = CM->getString<ChkdString>(soundStringId) )
                suggestions.AddString(*soundStr);
            else
                suggestions.AddString(std::to_string(soundStringId));
        }
    }
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestDuration()
{

}

void BriefingTrigActionsWindow::SuggestNumericMod()
{
    const std::vector<std::string> numericMod = { "Add", "Set to", "Subtract" };
    suggestions.AddStrings(numericMod);
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestActionType()
{
    suggestions.AddStrings(briefingTriggerActions);
    suggestions.Show();
}

void BriefingTrigActionsWindow::SuggestFlags()
{

}

void BriefingTrigActionsWindow::SuggestNumber() // Amount, Group2, LocDest, UnitPropNum, ScriptNum
{

}

void BriefingTrigActionsWindow::SuggestTypeIndex() // Unit, ScoreType, ResourceType, AllianceStatus
{

}

void BriefingTrigActionsWindow::SuggestSecondaryTypeIndex() // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
{

}

void BriefingTrigActionsWindow::SuggestInternalData()
{

}

void BriefingTrigActionsWindow::EnableStringEdit()
{
    stringEditEnabled = true;
    buttonEditString.Show();
}

void BriefingTrigActionsWindow::DisableStringEdit()
{
    buttonEditString.Hide();
    stringEditEnabled = false;
}

void BriefingTrigActionsWindow::ButtonEditString()
{
    int focusedX, focusedY;
    Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action & action = briefingTrig.action((u8)focusedY);
        if ( action.hasBriefingStringArgument() )
        {
            std::optional<ChkdString> gameString, editorString;
            GetCurrentActionString(gameString, editorString);
            ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), gameString, editorString, Chk::StringUserFlag::TriggerAction, briefingTrigIndex, focusedY);

            if ( (result & ChkdStringInputDialog::Result::GameStringChanged) == ChkdStringInputDialog::Result::GameStringChanged )
            {
                if ( gameString )
                {
                    size_t stringId = CM->addString<ChkdString>(*gameString);
                    if ( stringId != Chk::StringId::NoString )
                    {
                        action.stringId = (u32)stringId;
                    }
                }
                else
                    action.stringId = Chk::StringId::NoString;

                CM->deleteUnusedStrings(Chk::StrScope::Game);
            }

            if ( result > 0 )
                chkd.briefingTrigEditorWindow.briefingTriggersWindow.RefreshWindow(false);

            SetFocus(gridActions.getHandle());
        }
    }
}

void BriefingTrigActionsWindow::EnableSoundEdit()
{
    soundEditEnabled = true;
    buttonEditSound.Show();
}

void BriefingTrigActionsWindow::DisableSoundEdit()
{
    buttonEditSound.Hide();
    soundEditEnabled = false;
}

void BriefingTrigActionsWindow::ButtonEditSound()
{
    int focusedX, focusedY;
    Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action & action = briefingTrig.action((u8)focusedY);
        if ( action.hasBriefingSoundArgument() )
        {
            std::optional<ChkdString> gameString, editorString;
            GetCurrentActionSound(gameString, editorString);
            ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), gameString, editorString, Chk::StringUserFlag::TriggerActionSound, briefingTrigIndex, focusedY);

            if ( (result & ChkdStringInputDialog::Result::GameStringChanged) == ChkdStringInputDialog::Result::GameStringChanged )
            {
                if ( gameString )
                {
                    size_t stringId = CM->addString<ChkdString>(*gameString);
                    if ( stringId != Chk::StringId::NoString )
                    {
                        action.soundStringId = (u32)stringId;
                    }
                }
                else
                    action.soundStringId = Chk::StringId::NoString;

                CM->deleteUnusedStrings(Chk::StrScope::Game);
            }

            if ( result > 0 )
                chkd.briefingTrigEditorWindow.briefingTriggersWindow.RefreshWindow(false);

            SetFocus(gridActions.getHandle());
        }
    }
}

void BriefingTrigActionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
    Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    Chk::Action & action = briefingTrig.action((u8)gridItemY);
    Chk::Action::ArgType argType = Chk::Action::ArgType::NoType;
    if ( gridItemX == 1 ) // Action Name
        argType = Chk::Action::ArgType::ActionType;
    else if ( gridItemX > 1 ) // Action Arg
    {
        u8 actionArgNum = (u8)gridItemX - 2;
        if ( action.actionType < Chk::Action::NumBriefingActionTypes  )
            argType = Chk::Action::getBriefingClassicArgType(action.actionType, actionArgNum);
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
        case Chk::Action::ArgType::BriefingSlot: SuggestSlot(); break;
        case Chk::Action::ArgType::Unit: SuggestUnit(); break;
        case Chk::Action::ArgType::Amount: SuggestAmount(); break;
        case Chk::Action::ArgType::Sound: SuggestSound(); break;
        case Chk::Action::ArgType::Duration: SuggestDuration(); break;
        case Chk::Action::ArgType::NumericMod: SuggestNumericMod(); break;
        case Chk::Action::ArgType::ActionType: SuggestActionType(); break;
        case Chk::Action::ArgType::Flags: SuggestFlags(); break;
        case Chk::Action::ArgType::Number: SuggestNumber(); break; // Amount, Group2, LocDest, UnitPropNum, ScriptNum
        case Chk::Action::ArgType::TypeIndex: SuggestTypeIndex(); break; // Unit, ScoreType, ResourceType, AllianceStatus
        case Chk::Action::ArgType::SecondaryTypeIndex: SuggestSecondaryTypeIndex(); break; // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
        case Chk::Action::ArgType::Padding: SuggestInternalData(); break;
        case Chk::Action::ArgType::MaskFlag: SuggestInternalData(); break;
    }
}

void BriefingTrigActionsWindow::NewSelection(u16 gridItemX, u16 gridItemY)
{
    const Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    const Chk::Action & action = briefingTrig.action((u8)gridItemY);
    Chk::Action::Type actionType = action.actionType;
    if ( actionType < Chk::Action::NumBriefingActionTypes )
    {
        bool includesString = false, includesSound = false;
        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getBriefingClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::String )
                includesString = true;
        }

        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getBriefingClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::Sound )
                includesSound = true;
        }

        if ( includesString )
            EnableStringEdit();
        else
            DisableStringEdit();

        if ( includesSound )
            EnableSoundEdit();
        else
            DisableSoundEdit();
    }

    if ( gridItemX == 1 ) // Action Name
    {

    }
    else if ( gridItemX > 1 ) // Action Arg
    {

    }
    DoSize();
    chkd.briefingTrigEditorWindow.briefingTriggersWindow.briefingTrigModifyWindow.RedrawThis();
}

void BriefingTrigActionsWindow::NewSuggestion(std::string & str)
{
    gridActions.SetEditText(str);
}

void BriefingTrigActionsWindow::GetCurrentActionString(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString)
{
    int focusedX, focusedY;
    const Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        const Chk::Action & action = briefingTrig.action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;
        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getBriefingClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::String )
            {
                gameString = CM->getString<ChkdString>(action.stringId, Chk::StrScope::Game);
                editorString = CM->getString<ChkdString>(action.stringId, Chk::StrScope::Editor);
            }
        }
    }
}

void BriefingTrigActionsWindow::GetCurrentActionSound(std::optional<ChkdString> & gameString, std::optional<ChkdString> & editorString)
{
    int focusedX, focusedY;
    const Chk::Trigger & briefingTrig = CM->getBriefingTrigger(briefingTrigIndex);
    if ( gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        const Chk::Action & action = briefingTrig.action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;
        for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
        {
            Chk::Action::ArgType argType = Chk::Action::getBriefingClassicArgType(actionType, i);
            if ( argType == Chk::Action::ArgType::Sound )
            {
                gameString = CM->getString<ChkdString>(action.stringId, Chk::StrScope::Game);
                editorString = CM->getString<ChkdString>(action.stringId, Chk::StrScope::Editor);
            }
        }
    }
}

LRESULT BriefingTrigActionsWindow::ShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( (BOOL)wParam == FALSE )
    {
        suggestions.Hide();
        gridActions.EndEditing();
    }

    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
}

LRESULT BriefingTrigActionsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT BriefingTrigActionsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
        case BN_CLICKED:
            switch ( LOWORD(wParam) )
            {
                case Id::BUTTON_EDITSTRING: ButtonEditString(); break;
                case Id::BUTTON_EDITSOUND: ButtonEditSound(); break;
                default: return ClassWindow::Command(hWnd, wParam, lParam); break;
            }
            break;
        default: return ClassWindow::Command(hWnd, wParam, lParam); break;
    }
    return 0;
}

LRESULT BriefingTrigActionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        case WinLib::GV::WM_GRIDDELETEFINISHED: RefreshWindow(briefingTrigIndex); break;
        case WinLib::GV::WM_GRIDEDITSTART: GridEditStart(LOWORD(wParam), HIWORD(wParam)); break;
        case WinLib::GV::WM_GRIDEDITEND: suggestions.Hide(); break;
        default: return ClassWindow::WndProc(hWnd, msg, wParam, lParam); break;
    }
    return 0;
}
