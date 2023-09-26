#include "StringEditor.h"
#include "../../../../../MappingCoreLib/FileBrowser.h"
#include "../../../../Chkdraft.h"
#include <fstream>
#include <string>

enum_t(Id, u32, {
    DELETE_STRING = ID_FIRST,
    SAVE_TO,
    COMPRESS_STRINGS,
    REPAIR_STRINGS,
    LB_STRINGS,
    STRING_GUIDE,
    EDIT_STRING,
    PREVIEW_STRING,
    LB_STRINGUSE,
    BUTTON_SWITCH,
});

StringEditorWindow::StringEditorWindow() : extended(false), currSelString(0), numVisibleStrings(0), stringListDc(std::nullopt), fileBrowser(getSaveTextFilters(), "Save Text", false, true)
{

}

StringEditorWindow::~StringEditorWindow()
{

}

bool StringEditorWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "StringEditor", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        RefreshWindow();
        return true;
    }
    else
        return false;
}

bool StringEditorWindow::DestroyThis()
{
    ClassWindow::Hide();
    stringGuide.DestroyThis();
    stringPreviewWindow.DestroyThis();
    ClassWindow::DestroyThis();
    this->extended = false;
    this->currSelString = 0;
    this->numVisibleStrings = 0;
    this->stringListDc = std::nullopt;
    return true;
}

void StringEditorWindow::RefreshWindow()
{
    if ( CM != nullptr )
    {
        buttonSwap.SetWinText(extended ? "Switch to Standard" : "Switch to Extended");
        size_t bytesUsed = CM->getBytesUsed(extended ? Chk::StrScope::Editor : Chk::StrScope::Game);
        size_t maxBytes = extended ? Chk::MaxChkSectionSize : 65536;
        textAboutStrings.SetText(std::to_string(bytesUsed) + " / " + std::to_string(maxBytes) + " Bytes Used");
        
        listStrings.SetRedraw(false);
        listStrings.ClearItems();
        numVisibleStrings = 0;
        int toSelect = -1;
        std::bitset<Chk::MaxStrings> strUsed;
        CM->markUsedStrings(strUsed, Chk::StrScope::Either, extended ? Chk::StrScope::Editor : Chk::StrScope::Game);

        size_t lastIndex = CM->getCapacity(extended ? Chk::StrScope::Editor : Chk::StrScope::Game);

        for ( size_t i = 0; i <= lastIndex; i++ )
        {
            if ( strUsed[i] )
            {
                if ( auto str = CM->getString<RawString>(i, extended ? Chk::StrScope::Editor : Chk::StrScope::Game) )
                {
                    int newListIndex = listStrings.AddItem((u32)i);
                    if ( newListIndex >= 0 ) // Only consider the string if it could be added to the ListBox
                    {
                        numVisibleStrings++;
                        if ( currSelString == i ) // This string is the currSelString
                            toSelect = newListIndex; // Mark position for selection
                    }
                }
            }
        }
        listStrings.SetRedraw(true);
        if ( toSelect != -1 && listStrings.SetCurSel(toSelect) ) // Attempt selection
        {
            if ( extended )
                chkd.mapSettingsWindow.SetWinText("Map Settings - [Extended String #" + std::to_string(currSelString) + "]");
            else
                chkd.mapSettingsWindow.SetWinText("Map Settings - [String #" + std::to_string(currSelString) + "]");
        }
        else
        {
            currSelString = 0; // Clear currSelString if selection fails
            editString.SetText("");
            chkd.mapSettingsWindow.SetWinText("Map Settings");
        }
    }
}

void StringEditorWindow::CreateSubWindows(HWND hWnd)
{
    textAboutStrings.CreateThis(hWnd, 5, 5, 150, 20, "String Editor...", 0);
    buttonSwap.CreateThis(hWnd, 310, 5, 150, 20, "", Id::BUTTON_SWITCH);

    listStrings.CreateThis(hWnd, 5, 25, 453, 262, true, false, false, false, Id::LB_STRINGS);

    buttonDeleteString.CreateThis(hWnd, 130, 290, 200, 20, "Delete String", Id::DELETE_STRING);
    buttonSaveString.CreateThis(hWnd, 340, 290, 75, 20, "Save to...", Id::SAVE_TO);
    editString.CreateThis(hWnd, 5, 310, 453, 140, true, Id::EDIT_STRING);

    textStringUsage.CreateThis(hWnd, 480, 379, 125, 20, "String Usage:", 0);
    listUsage.CreateThis(hWnd, 463, 394, 125, 83, false, false, false, false, Id::LB_STRINGUSE);

    stringGuide.CreateThis(hWnd);
    stringPreviewWindow.CreateThis(hWnd, Id::PREVIEW_STRING);
}

void StringEditorWindow::SwitchButtonPressed()
{
    extended = !extended;
    RefreshWindow();
}

LRESULT StringEditorWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case LBN_SELCHANGE:
        if ( LOWORD(wParam) == Id::LB_STRINGS ) // Change selection, update info boxes and so fourth
        {
            if ( currSelString != 0 )
                updateString(currSelString);

            currSelString = 0;
            listUsage.ClearItems();
            int lbIndex;
            if ( listStrings.GetCurSel(lbIndex) )
            {
                if ( listStrings.GetItemData(lbIndex, currSelString) && CM != nullptr )
                {
                    if ( auto str = CM->getString<ChkdString>(currSelString, this->extended ? Chk::StrScope::Editor : Chk::StrScope::Game) )
                    {
                        editString.SetText(*str);

                        std::vector<Chk::StringUser> stringUsers;
                        CM->appendUsage(currSelString, stringUsers, extended ? Chk::StrScope::Editor : Chk::StrScope::Game);
                        std::unordered_map<Chk::StringUserFlag, size_t> stringUserToUsageCount = {
                            { Chk::StringUserFlag::Location, 0 },
                            { Chk::StringUserFlag::AnyTrigger, 0 },
                            { Chk::StringUserFlag::AnyBriefingTrigger, 0 },
                            { Chk::StringUserFlag::ScenarioProperties, 0 },
                            { Chk::StringUserFlag::Force, 0 },
                            { Chk::StringUserFlag::Sound, 0 },
                            { Chk::StringUserFlag::BothUnitSettings, 0 },
                            { Chk::StringUserFlag::Switch, 0 }
                        };

                        for ( const Chk::StringUser & stringUser : stringUsers )
                        {
                            Chk::StringUserFlag userFlags = stringUser.userFlags;
                            if ( (userFlags & Chk::StringUserFlag::Location) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::Location)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::AnyTrigger)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::AnyBriefingTrigger) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::AnyBriefingTrigger)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::ScenarioProperties) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::ScenarioProperties)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::Force) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::Force)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::Sound) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::Sound)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::BothUnitSettings) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::BothUnitSettings)->second ++;
                            else if ( (userFlags & Chk::StringUserFlag::Switch) != Chk::StringUserFlag::None )
                                stringUserToUsageCount.find(Chk::StringUserFlag::Switch)->second ++;
                        }
                        
                        addUseItem("Locations", stringUserToUsageCount.find(Chk::StringUserFlag::Location)->second);
                        addUseItem("Triggers", stringUserToUsageCount.find(Chk::StringUserFlag::AnyTrigger)->second);
                        addUseItem("Briefing Triggers", stringUserToUsageCount.find(Chk::StringUserFlag::AnyBriefingTrigger)->second);
                        addUseItem("Map Properties", stringUserToUsageCount.find(Chk::StringUserFlag::ScenarioProperties)->second);
                        addUseItem("Forces", stringUserToUsageCount.find(Chk::StringUserFlag::Force)->second);
                        addUseItem("Sounds", stringUserToUsageCount.find(Chk::StringUserFlag::Sound)->second);
                        addUseItem("Units", stringUserToUsageCount.find(Chk::StringUserFlag::BothUnitSettings)->second);
                        addUseItem("Switches", stringUserToUsageCount.find(Chk::StringUserFlag::Switch)->second);

                        if ( extended )
                            chkd.mapSettingsWindow.SetWinText("Map Settings - [Extended String #" + std::to_string(currSelString) + "]");
                        else
                            chkd.mapSettingsWindow.SetWinText("Map Settings - [String #" + std::to_string(currSelString) + "]");
                    }
                    else
                        chkd.mapSettingsWindow.SetWinText("Map Settings");
                }
                else
                    chkd.mapSettingsWindow.SetWinText("Map Settings");
                return 0;
            }
            else
                chkd.mapSettingsWindow.SetWinText("Map Settings");
            editString.SetText("");
        }
        break;
    case LBN_KILLFOCUS: // String list box item may have lost focus, check if string should be updated
        if ( LOWORD(wParam) == Id::LB_STRINGS && currSelString != 0 && updateString(currSelString) )
            CM->refreshScenario();
        break;
    case EN_KILLFOCUS: // String edit box may have lost focus, check if string should be updated
        if ( LOWORD(wParam) == Id::EDIT_STRING && currSelString != 0 && updateString(currSelString) )
            CM->refreshScenario();
        break;
    case BN_CLICKED:
        if ( LOWORD(wParam) == Id::DELETE_STRING &&
            WinLib::GetYesNo("Forcefully deleting a string could cause problems, continue?", "Warning") == WinLib::PromptResult::Yes &&
            CM != nullptr && currSelString != 0 && CM->stringStored(currSelString) )
        {
            CM->deleteString(currSelString, Chk::StrScope::Both, false);
            CM->refreshScenario();
        }
        else if ( LOWORD(wParam) == Id::SAVE_TO && CM != nullptr )
        {
            saveStrings();
        }
        break;
    }
    return 0;
}

LRESULT StringEditorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
                RefreshWindow();
            else if ( wParam == FALSE )
                updateString(currSelString);
            break;

        case WM_MOUSEWHEEL:
            listStrings.SetTopIndex(listStrings.GetTopIndex()-(int((s16(HIWORD(wParam)))/WHEEL_DELTA)));
            break;

        case WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
            stringListDc.emplace(listStrings.getHandle());
            break;

        case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                auto str = CM->getString<RawString>((size_t)mis->itemData, extended ? Chk::StrScope::Editor : Chk::StrScope::Game);

                if ( str && GetStringDrawSize(*stringListDc, mis->itemWidth, mis->itemHeight, *str) )
                {
                    mis->itemWidth += 5;
                    mis->itemHeight += 2;

                    if ( mis->itemHeight > 255 )
                        mis->itemHeight = 255;

                    return TRUE;
                }
            }
            break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            stringListDc = std::nullopt;
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
                    auto str = CM->getString<RawString>((size_t)pdis->itemData, this->extended ? Chk::StrScope::Editor : Chk::StrScope::Game);
                    if ( CM != nullptr && str )
                    {
                        dc.fillRect(pdis->rcItem, RGB(0, 0, 0)); // Draw far background, same color as in WM_CTLCOLORLISTBOX
                        SetBkMode(pdis->hDC, TRANSPARENT);
                        DrawString(dc, pdis->rcItem.left+3, pdis->rcItem.top+1, pdis->rcItem.right-pdis->rcItem.left,
                            RGB(16, 252, 24), *str);
                    }
                    if ( isSelected )
                        DrawFocusRect(pdis->hDC, &pdis->rcItem);
                }
                return TRUE;
            }
            break;

        case WinLib::LB::WM_POSTDRAWITEMS:
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void StringEditorWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( idFrom )
    {
        case Id::BUTTON_SWITCH: SwitchButtonPressed(); break;
    }
}

void StringEditorWindow::saveStrings()
{
    u32 filterIndex = 0;
    std::string filePath = "";

    if ( fileBrowser.browseForSavePath(filePath, filterIndex) )
    {
        if ( filterIndex == 1 && !hasExtension(filePath, ".txt") )
            filePath = makeExtSystemFilePath(filePath, ".txt");

        removeFile(filePath);

        std::ofstream outFile(filePath, std::ofstream::out);
        if ( outFile.is_open() )
        {
            for ( u32 i=0; i<CM->getCapacity(extended ? Chk::StrScope::Editor : Chk::StrScope::Game); i++ )
            {
                if ( auto str = CM->getString<ChkdString>(i, extended ? Chk::StrScope::Editor : Chk::StrScope::Game) )
                    outFile << i << ": " << *str << "\r\n";
            }
            outFile.close();
        }
    }
}

void StringEditorWindow::addUseItem(std::string str, size_t amount)
{
    if ( amount > 0 )
        listUsage.AddString(str + ": " + std::to_string(amount));
}

bool StringEditorWindow::updateString(u32 stringNum)
{
    auto existingStr = CM->getString<ChkdString>((size_t)stringNum);
    auto editStr = editString.GetWinText();
    if ( CM != nullptr && editStr && existingStr && existingStr->compare(*editStr) != 0 )
    {
        if ( auto editStr = editString.GetWinText() )
        {
            CM->replaceString<ChkdString>((size_t)stringNum, *editStr);
            CM->notifyChange(false);
            if ( CM->locationStringUsed(currSelString, Chk::StrScope::EditorOverGame) )
                chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

            editString.RedrawThis();
            return true;
        }
    }
    return false;
}
