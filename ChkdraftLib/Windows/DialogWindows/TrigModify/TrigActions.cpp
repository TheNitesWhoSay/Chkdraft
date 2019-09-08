#include "TrigActions.h"
#include "../../../Chkdraft.h"
#include "../../ChkdControls/ChkdStringInput.h"
#include "../../ChkdControls/CuwpInput.h"
#include "../../../Mapping/Settings.h"

#define TOP_ACTION_PADDING 50
#define BOTTOM_ACTION_PADDING 0
#define DEFAULT_COLUMN_WIDTH 50

enum ID
{
    GRID_ACTIONS = ID_FIRST,
    BUTTON_UNITPROPERTIES,
    BUTTON_EDITSTRING,
    BUTTON_EDITWAV
};

TrigActionsWindow::TrigActionsWindow() : hBlack(NULL), trigIndex(0), gridActions(*this, 64),
    suggestions(gridActions.GetSuggestions()), stringEditEnabled(false), wavEditEnabled(false), unitPropertiesEditEnabled(false), isPasting(false)
{
    InitializeArgMaps();
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
    if ( trig != nullptr && ttg.LoadScenario(CM) )
    {
        for ( u8 y = 0; y<Chk::Trigger::MaxActions; y++ )
        {
            Chk::Action& action = trig->action(y);
            if ( action.actionType > Chk::Action::Type::NoAction && action.actionType <= Chk::Action::Type::LastAction )
            {
                u8 numArgs = u8(actionArgMaps[(size_t)action.actionType].size());
                if ( numArgs > 8 )
                    numArgs = 8;

                gridActions.item(1, y).SetText(ttg.GetActionName((u8)action.actionType));
                for ( u8 x = 0; x<numArgs; x++ )
                {
                    gridActions.item(x + 2, y).SetDisabled(false);
                    gridActions.item(x + 2, y).SetText(ttg.GetActionArgument(action, x, actionArgMaps[(size_t)action.actionType]));
                }
                for ( u8 x = numArgs; x < 8; x++ )
                {
                    gridActions.item(x + 2, y).SetText("");
                    gridActions.item(x + 2, y).SetDisabled(true);
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
        Chk::Action &action = trig->action(actionNum);
        if ( action.actionType != Chk::Action::Type::NoAction )
        {
            action.ToggleDisabled();

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

void TrigActionsWindow::InitializeArgMaps()
{
    #define ADD_ARRAY(anArray, vector)                      \
        for ( u8 i=0; i<sizeof(anArray)/sizeof(u8); i++ )   \
            vector.push_back(anArray[i]);

    u8 centerView[] = { 0 };
    u8 comment[] = { 0 };
    u8 createUnits[] = { 2, 1, 3, 0 };
    u8 createUnitsWithProperties[] = { 2, 1, 3, 0, 4 };
    u8 displayTextMessage[] = { 1 };
    u8 giveUnitsToPlayer[] = { 3, 2, 0, 4, 1 };
    u8 killUnit[] = { 1, 0 };
    u8 killUnitsAtLocation[] = { 2, 1, 0, 3 };
    u8 leaderboardCtrlAtLoc[] = { 1, 2, 0 };
    u8 leaderboardCtrl[] = { 1, 0 };
    u8 leaderboardGreed[] = { 0 };
    u8 leaderboardKills[] = { 1, 0 };
    u8 leaderboardPoints[] = { 1, 0 };
    u8 leaderboardResources[] = { 1, 0 };
    u8 leaderboardGoalCtrlAtLoc[] = { 2, 1, 3, 0 };
    u8 leaderboardGoalCtrl[] = { 2, 1, 0 };
    u8 leaderboardGoalKills[] = { 2, 1, 0 };
    u8 leaderboardGoalPoints[] = { 2, 1, 0 };
    u8 leaderboardGoalResources[] = { 1, 2, 0 };
    u8 leaderboardCompPlayers[] = { 0 };
    u8 minimapPing[] = { 0 };
    u8 modifyUnitEnergy[] = { 3, 1, 0, 4, 2 };
    u8 modifyUnitHangerCount[] = { 2, 3, 1, 4, 0 };
    u8 modifyUnitHitPoints[] = { 3, 1, 0, 4, 2 };
    u8 modifyUnitResourceAmount[] = { 2, 0, 3, 1 };
    u8 modifyUnitShieldPoints[] = { 3, 1, 0, 4, 2 };
    u8 moveLocation[] = { 3, 1, 0, 2 };
    u8 moveUnits[] = { 2, 1, 0, 3, 4 };
    u8 order[] = { 1, 0, 2, 4, 3 };
    u8 playWav[] = { 0 };
    u8 removeUnit[] = { 1, 0 };
    u8 removeUnitsAtLocation[] = { 2, 1, 0, 3 };
    u8 runAiScript[] = { 0 };
    u8 runAiScriptAtLocation[] = { 0, 1 };
    u8 setAllianceStatus[] = { 0, 1 };
    u8 setCountdownTimer[] = { 0, 1 };
    u8 setDeaths[] = { 0, 2, 3, 1 };
    u8 setDoodadState[] = { 3, 1, 0, 2 };
    u8 setInvincibility[] = { 3, 1, 0, 2 };
    u8 setMissionObjectives[] = { 0 };
    u8 setNextScenario[] = { 0 };
    u8 setResources[] = { 0, 1, 2, 3 };
    u8 setScore[] = { 0, 1, 2, 3 };
    u8 setSwitch[] = { 1, 0 };
    u8 talkingPortrait[] = { 0, 1 };
    u8 transmission[] = { 2, 3, 6, 4, 5, 1 };
    u8 wait[] = { 0 };

    ADD_ARRAY(centerView, actionArgMaps[10]);
    ADD_ARRAY(comment, actionArgMaps[47]);
    ADD_ARRAY(createUnits, actionArgMaps[44]);
    ADD_ARRAY(createUnitsWithProperties, actionArgMaps[11]);
    ADD_ARRAY(displayTextMessage, actionArgMaps[9]);
    ADD_ARRAY(giveUnitsToPlayer, actionArgMaps[48]);
    ADD_ARRAY(killUnit, actionArgMaps[22]);
    ADD_ARRAY(killUnitsAtLocation, actionArgMaps[23]);
    ADD_ARRAY(leaderboardCtrlAtLoc, actionArgMaps[18]);
    ADD_ARRAY(leaderboardCtrl, actionArgMaps[17]);
    ADD_ARRAY(leaderboardGreed, actionArgMaps[40]);
    ADD_ARRAY(leaderboardKills, actionArgMaps[20]);
    ADD_ARRAY(leaderboardPoints, actionArgMaps[21]);
    ADD_ARRAY(leaderboardResources, actionArgMaps[19]);
    ADD_ARRAY(leaderboardGoalCtrlAtLoc, actionArgMaps[34]);
    ADD_ARRAY(leaderboardGoalCtrl, actionArgMaps[33]);
    ADD_ARRAY(leaderboardGoalKills, actionArgMaps[36]);
    ADD_ARRAY(leaderboardGoalPoints, actionArgMaps[37]);
    ADD_ARRAY(leaderboardGoalResources, actionArgMaps[35]);
    ADD_ARRAY(leaderboardCompPlayers, actionArgMaps[32]);
    ADD_ARRAY(minimapPing, actionArgMaps[28]);
    ADD_ARRAY(modifyUnitEnergy, actionArgMaps[50]);
    ADD_ARRAY(modifyUnitHangerCount, actionArgMaps[53]);
    ADD_ARRAY(modifyUnitHitPoints, actionArgMaps[49]);
    ADD_ARRAY(modifyUnitResourceAmount, actionArgMaps[52]);
    ADD_ARRAY(modifyUnitShieldPoints, actionArgMaps[51]);
    ADD_ARRAY(moveLocation, actionArgMaps[38]);
    ADD_ARRAY(moveUnits, actionArgMaps[39]);
    ADD_ARRAY(order, actionArgMaps[46]);
    ADD_ARRAY(playWav, actionArgMaps[8]);
    ADD_ARRAY(removeUnit, actionArgMaps[24]);
    ADD_ARRAY(removeUnitsAtLocation, actionArgMaps[25]);
    ADD_ARRAY(runAiScript, actionArgMaps[15]);
    ADD_ARRAY(runAiScriptAtLocation, actionArgMaps[16]);
    ADD_ARRAY(setAllianceStatus, actionArgMaps[57]);
    ADD_ARRAY(setCountdownTimer, actionArgMaps[14]);
    ADD_ARRAY(setDeaths, actionArgMaps[45]);
    ADD_ARRAY(setDoodadState, actionArgMaps[42]);
    ADD_ARRAY(setInvincibility, actionArgMaps[43]);
    ADD_ARRAY(setMissionObjectives, actionArgMaps[12]);
    ADD_ARRAY(setNextScenario, actionArgMaps[41]);
    ADD_ARRAY(setResources, actionArgMaps[26]);
    ADD_ARRAY(setScore, actionArgMaps[27]);
    ADD_ARRAY(setSwitch, actionArgMaps[13]);
    ADD_ARRAY(talkingPortrait, actionArgMaps[29]);
    ADD_ARRAY(transmission, actionArgMaps[7]);
    ADD_ARRAY(wait, actionArgMaps[4]);
}

void TrigActionsWindow::InitializeScriptTable()
{
    int numScripts = chkd.scData.aiScripts.GetNumAiScripts();
    for ( int i = 0; i < numScripts; i++ )
    {
        u32 scriptId = 0;
        std::string scriptName = "";
        if ( chkd.scData.aiScripts.GetAiIdAndName(i, scriptId, scriptName) )
        {
            char* scriptStringPtr = (char*)&scriptId;
            char scriptIdString[5] = {scriptStringPtr[0], scriptStringPtr[1], scriptStringPtr[2], scriptStringPtr[3], '\0'};
            std::string displayString = scriptName + " (" + std::string(scriptIdString) + ")";
            size_t hash = strHash(displayString);
            scriptTable.insert(std::pair<size_t, std::pair<u32, std::string>>(hash, std::pair<u32, std::string>(scriptId, displayString)));
        }
    }
}

void TrigActionsWindow::CreateSubWindows(HWND hWnd)
{
    gridActions.CreateThis(hWnd, 2, 40, 100, 100, GRID_ACTIONS);
    suggestions.CreateThis(hWnd, 0, 0, 262, 100);
    buttonEditString.CreateThis(hWnd, 0, 0, 100, 22, "Edit String", BUTTON_EDITSTRING);
    buttonEditString.Hide();
    buttonEditWav.CreateThis(hWnd, 0, 0, 100, 22, "Edit WAV", BUTTON_EDITWAV);
    buttonEditWav.Hide();
    buttonUnitProperties.CreateThis(hWnd, 0, 0, 125, 22, "Edit Unit Properties", BUTTON_UNITPROPERTIES);
    buttonUnitProperties.Hide();
    RefreshWindow(trigIndex);
}

LRESULT TrigActionsWindow::MeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( wParam == GRID_ACTIONS )
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

void TrigActionsWindow::ChangeActionType(Chk::Action &action, Chk::Action::Type newType)
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

        action.flags = TextTrigCompiler::defaultActionFlags(newType);
        action.padding = 0;
        action.maskFlag = Chk::Action::MaskFlag::Disabled;
    }
}

bool TrigActionsWindow::TransformAction(Chk::Action &action, Chk::Action::Type newType, bool refreshImmediately)
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

void TrigActionsWindow::UpdateActionName(u8 actionNum, const std::string &newText, bool refreshImmediately)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::Action::Type newType = Chk::Action::Type::NoAction;
    if ( ttc.ParseActionName(newText, newType) || ttc.ParseActionName(suggestions.Take(), newType) )
    {
        if ( trig != nullptr )
        {
            Chk::Action &action = trig->action(actionNum);
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

void TrigActionsWindow::UpdateActionArg(u8 actionNum, u8 argNum, const std::string &newText, bool refreshImmediately)
{
    RawString rawUpdateText, rawSuggestText;
    std::string suggestionString = suggestions.Take();
    TextTrigCompiler ttc(Settings::useAddressesForMemory, Settings::deathTableStart);
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Action &action = trig->action(actionNum);
        if ( (size_t)action.actionType < Chk::Action::NumActionTypes && argNum < actionArgMaps[(size_t)action.actionType].size() )
        {
            u8 textTrigArgNum = actionArgMaps[(size_t)action.actionType][argNum];
            Chk::Action::ArgType argType = action.getClassicArgType(action.actionType, textTrigArgNum);
            SingleLineChkdString chkdSuggestText = ChkdString(suggestionString);
            SingleLineChkdString chkdNewText = ChkdString(newText);

            bool madeChange = false;
            if ( argType == Chk::Action::ArgType::String || argType == Chk::Action::ArgType::Sound )
            {
                u32 newStringNum = 0;
                if ( CM->stringExists(chkdSuggestText, newStringNum) ||
                    CM->stringExists(chkdNewText, newStringNum) )
                {
                    if ( argType == Chk::Action::ArgType::String )
                        action.stringId = newStringNum;
                    else if ( argType == Chk::Action::ArgType::Sound )
                        action.soundStringId = newStringNum;

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
                    std::string &scriptDisplayString = scriptTable.find(hash)->second.second;
                    if ( scriptDisplayString.compare(suggestionString) == 0 )
                        newScriptNum = scriptTable.find(hash)->second.first;
                }
                else if ( numMatching > 1 )
                {
                    auto range = scriptTable.equal_range(hash);
                    foreachin(pair, range)
                    {
                        std::string &scriptDisplayString = pair->second.second;
                        if ( scriptDisplayString.compare(suggestionString) == 0 )
                        {
                            newScriptNum = scriptTable.find(hash)->second.first;
                            break;
                        }
                    }
                }
                else
                {
                    std::vector<u8> &argMap = actionArgMaps[(size_t)trig->action(actionNum).actionType];
                    madeChange = (ParseChkdStr(chkdNewText, rawUpdateText) &&
                        ttc.ParseActionArg(rawUpdateText, argNum, argMap, action, CM, chkd.scData)) ||
                        (ParseChkdStr(chkdSuggestText, rawSuggestText) &&
                            ttc.ParseActionArg(rawSuggestText, argNum, argMap, action, CM, chkd.scData));
                }

                if ( newScriptNum != 0 )
                {
                    action.number = newScriptNum;
                    madeChange = true;
                }
            }
            else
            {
                std::vector<u8> &argMap = actionArgMaps[(size_t)trig->action(actionNum).actionType];
                madeChange = (ParseChkdStr(chkdNewText, rawUpdateText) &&
                    ttc.ParseActionArg(rawUpdateText, argNum, argMap, action, CM, chkd.scData)) ||
                    (ParseChkdStr(chkdSuggestText, rawSuggestText) &&
                        ttc.ParseActionArg(rawSuggestText, argNum, argMap, action, CM, chkd.scData));
            }

            if ( madeChange )
            {
                if ( refreshImmediately )
                    RefreshActionAreas();
            }
        }
    }
}

BOOL TrigActionsWindow::GridItemChanging(u16 gridItemX, u16 gridItemY, const std::string& str)
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
                ttg.LoadScenario(CM);
                ChkdString str = chkd.trigEditorWindow.triggersWindow.GetActionString(actionNum, &(*trig), ttg);
                ttg.ClearScenario();

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

void TrigActionsWindow::SysColorRect(HDC hDC, RECT &rect, DWORD color)
{
    SetBkColor(hDC, GetSysColor(color));
    FillRect(hDC, &rect, GetSysColorBrush(color));
}

void TrigActionsWindow::DrawItemBackground(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int width, int xStart)
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

void TrigActionsWindow::DrawItemFrame(HDC hDC, RECT &rcItem, int width, int &xStart)
{
    RECT rcFill = {};
    rcFill.top = rcItem.top - 1;
    rcFill.bottom = rcItem.bottom;
    rcFill.left = xStart - 1;
    rcFill.right = xStart + width;

    ::FrameRect(hDC, &rcFill, hBlack);
}

void TrigActionsWindow::DrawGridViewItem(HDC hDC, int gridItemX, int gridItemY, RECT &rcItem, int &xStart)
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
    if ( gridId == GRID_ACTIONS )
    {
        bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
            drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
            drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

        if ( pdis->itemID != -1 && (drawSelection || drawEntire) )
        {
            RECT &rcItem = pdis->rcItem;
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
        u16 locationCapacity = (u16)CM->locationCapacity();
        for ( u16 i = 0; i < locationCapacity; i++ )
        {
            if ( CM->locationIsUsed(i) )
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
    }
    suggestions.Show();
}

void TrigActionsWindow::SuggestString()
{
    SingleLineChkdString str;
    if ( CM != nullptr )
    {
        suggestions.AddString(std::string("No String"));
        u32 numRegularStrings = (u32)CM->strSectionCapacity();
        u32 numExtendedStrings = CM->kstrSectionCapacity();
        for ( u32 i = 1; i <= numRegularStrings; i++ )
        {
            if ( CM->GetString(str, i) && str.size() > 0 )
                suggestions.AddString(str);
        }
        for ( u32 i = 65535; i > (65535 - numExtendedStrings); i-- )
        {
            if ( CM->GetString(str, i) && str.size() > 0 )
                suggestions.AddString(str);
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
            SingleLineChkdString str;
            CM->getUnitName(str, i);
            suggestions.AddString(str);
            if ( str.compare(std::string(Sc::Unit::defaultDisplayNames[i])) != 0 )
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
    for ( u8 i = 0; i < 64; i++ )
    {
        if ( CM->IsCuwpUsed(i) )
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
    for ( u16 i = 0; i < 512; i++ )
    {
        if ( CM->WavHasString(i) )
        {
            ChkdString wavStr;
            u32 stringId = 0;
            if ( CM->GetWav(i, stringId) && stringId > 0 && CM->GetString(wavStr, stringId) )
                suggestions.AddString(wavStr);
            else
                suggestions.AddString(std::to_string(stringId));
        }
    }
}

void TrigActionsWindow::SuggestDuration()
{

}

void TrigActionsWindow::SuggestScript()
{
    suggestions.AddString(std::string("No Script"));
    int numScripts = chkd.scData.aiScripts.GetNumAiScripts();
    for ( int i = 0; i < numScripts; i++ )
    {
        u32 scriptId = 0;
        std::string scriptName = "";
        if ( chkd.scData.aiScripts.GetAiIdAndName(i, scriptId, scriptName) )
        {
            char* scriptStringPtr = (char*)&scriptId;
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
            SingleLineChkdString str;
            if ( CM->getSwitchName(str, (u8)i) )
                suggestions.AddString(str);
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

void TrigActionsWindow::SuggestType()
{

}

void TrigActionsWindow::SuggestActionType()
{
    suggestions.AddStrings(triggerActions);
    suggestions.Show();
}

void TrigActionsWindow::SuggestSecondaryType()
{

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
            Chk::Action &action = trig->action((u8)focusedY);
            Chk::Action::Type actionType = action.actionType;

            std::vector<u8> &argMap = actionArgMaps[(size_t)actionType];
            u8 numArgs = (u8)actionArgMaps[(size_t)actionType].size();
            for ( u8 i = 0; i < numArgs; i++ )
            {
                Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
                u32 stringNum = 0;
                if ( argType == Chk::Action::ArgType::String && CM->addString(str, stringNum, false) )
                {
                    action.stringId = stringNum;
                    chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
                    break;
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
            Chk::Action &action = trig->action((u8)focusedY);
            Chk::Action::Type actionType = action.actionType;

            std::vector<u8> &argMap = actionArgMaps[(size_t)actionType];
            u8 numArgs = (u8)actionArgMaps[(size_t)actionType].size();
            for ( u8 i = 0; i < numArgs; i++ )
            {
                Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
                u32 stringNum = 0;

                if ( argType == Chk::Action::ArgType::Sound && CM->addString(wavStr, stringNum, false) )
                {
                    CM->AddWav(stringNum);
                    action.soundStringId = stringNum;
                    chkd.trigEditorWindow.triggersWindow.RefreshWindow(false);
                    break;
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
    Chk::Cuwp initialCuwp = {};
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    int focusedX = 0, focusedY = 0;
    if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action &action = trig->action((u8)focusedY);
        u32 cuwpIndex = action.number;
        if ( CM->GetCuwp((u8)cuwpIndex, initialCuwp) )
        {
            Chk::Cuwp newCuwp = {};
            if ( CuwpInputDialog::GetCuwp(newCuwp, initialCuwp, getHandle()) )
            {
                u8 newCuwpIndex = 0;
                if ( CM->FindCuwp(newCuwp, newCuwpIndex) || CM->AddCuwp(newCuwp, newCuwpIndex) )
                {
                    action.number = newCuwpIndex;
                    CM->notifyChange(false);
                }
                else
                    Error("Unable to add CUWP, all 64 slots may be in use.");
            }
        }
    }
}

void TrigActionsWindow::GridEditStart(u16 gridItemX, u16 gridItemY)
{
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr )
    {
        Chk::Action &action = trig->action((u8)gridItemY);
        Chk::Action::ArgType argType = Chk::Action::ArgType::NoType;
        if ( gridItemX == 1 ) // Action Name
            argType = Chk::Action::ArgType::ActionType;
        else if ( gridItemX > 1 ) // Action Arg
        {
            u8 actionArgNum = (u8)gridItemX - 2;
            if ( (size_t)action.actionType < Chk::Action::NumActionTypes && actionArgMaps[(size_t)action.actionType].size() > actionArgNum )
            {
                u8 textTrigArgNum = actionArgMaps[(size_t)action.actionType][actionArgNum];
                argType = action.getClassicArgType(action.actionType, textTrigArgNum);
            }
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
            case Chk::Action::ArgType::Type: SuggestType(); break;
            case Chk::Action::ArgType::ActionType: SuggestActionType(); break;
            case Chk::Action::ArgType::SecondaryType: SuggestSecondaryType(); break;
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
        Chk::Action &action = trig->action((u8)gridItemY);
        Chk::Action::Type actionType = action.actionType;
        if ( (size_t)actionType < Chk::Action::NumActionTypes )
        {
            bool includesString = false, includesWav = false;
            bool isCUWP = (actionType == Chk::Action::Type::CreateUnitWithProperties);
            std::vector<u8> &argMap = actionArgMaps[(size_t)actionType];
            u8 numArgs = (u8)actionArgMaps[(size_t)actionType].size();
            for ( u8 i = 0; i < numArgs; i++ )
            {
                Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
                if ( argType == Chk::Action::ArgType::String )
                    includesString = true;
            }

            for ( u8 i = 0; i < numArgs; i++ )
            {
                Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
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

void TrigActionsWindow::NewSuggestion(std::string &str)
{
    gridActions.SetEditText(str);
}

ChkdString TrigActionsWindow::GetCurrentActionsString()
{
    int focusedX, focusedY;
    Chk::TriggerPtr trig = CM->triggers.getTrigger(trigIndex);
    if ( trig != nullptr && gridActions.GetFocusedItem(focusedX, focusedY) )
    {
        Chk::Action &action = trig->action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;

        std::vector<u8> &argMap = actionArgMaps[(size_t)actionType];
        u8 numArgs = (u8)actionArgMaps[(size_t)actionType].size();
        for ( u8 i = 0; i < numArgs; i++ )
        {
            Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
            if ( argType == Chk::Action::ArgType::String )
            {
                ChkdString dest;
                if ( CM->GetString(dest, action.stringId) )
                    return dest;
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
        Chk::Action &action = trig->action((u8)focusedY);
        Chk::Action::Type actionType = action.actionType;

        std::vector<u8> &argMap = actionArgMaps[(size_t)actionType];
        u8 numArgs = (u8)actionArgMaps[(size_t)actionType].size();
        for ( u8 i = 0; i < numArgs; i++ )
        {
            Chk::Action::ArgType argType = action.getClassicArgType(actionType, argMap[i]);
            if ( argType == Chk::Action::ArgType::Sound )
            {
                ChkdString dest;
                if ( CM->GetString(dest, action.soundStringId) )
                    return dest;
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
                case BUTTON_EDITSTRING: ButtonEditString(); break;
                case BUTTON_EDITWAV: ButtonEditWav(); break;
                case BUTTON_UNITPROPERTIES: ButtonEditUnitProperties(); break;
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
