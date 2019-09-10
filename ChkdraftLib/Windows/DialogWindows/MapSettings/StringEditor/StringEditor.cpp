#include "StringEditor.h"
#include "../../../../../MappingCoreLib/FileBrowser.h"
#include "../../../../Chkdraft.h"
#include <fstream>
#include <string>

enum class Id {
    DELETE_STRING = ID_FIRST,
    CHECK_EXTENDEDSTRING,
    SAVE_TO,
    COMPRESS_STRINGS,
    REPAIR_STRINGS,
    LB_STRINGS,
    STRING_GUIDE,
    EDIT_STRING,
    PREVIEW_STRING,
    LB_STRINGUSE,
    BUTTON_SWITCH,
};

StringEditorWindow::StringEditorWindow() : extended(false), currSelString(0), numVisibleStrings(0), stringListDC(NULL), fileBrowser(getSaveTextFilters(), "Save Text", false, true)
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

void StringEditorWindow::RefreshWindow()
{
    if ( CM != nullptr )
    {
        buttonSwap.SetWinText(extended ? "Switch to Standard" : "Switch to Extended");
        if ( extended )
            textAboutStrings.SetText("");
        else
            textAboutStrings.SetText(std::string(std::to_string(CM->strBytesUsed()) + " / 65536 Bytes Used"));
        
        listStrings.SetRedraw(false);
        listStrings.ClearItems();
        numVisibleStrings = 0;
        int toSelect = -1;
        std::bitset<Chk::MaxStrings> strUsed;
        CM->strings.markUsedStrings(strUsed, Chk::Scope::Either, Chk::Scope::Game);
        ChkdString str;
        u32 lastIndex = extended ? CM->kstrSectionCapacity() : (u32)CM->strSectionCapacity();
        for ( u32 i = 0; i <= lastIndex; i++ )
        {
            u32 strId = extended ? 65536 - i : i;
            if ( strUsed[i] && CM->GetString(str, strId) && str.size() > 0 )
            {
                int newListIndex = listStrings.AddItem(strId);
                if ( newListIndex != -1 ) // Only consider the string if it could be added to the ListBox
                {
                    numVisibleStrings++;
                    if ( currSelString == strId ) // This string is the currSelString
                        toSelect = newListIndex; // Mark position for selection
                }
            }
        }
        listStrings.SetRedraw(true);
        if ( toSelect != -1 && listStrings.SetCurSel(toSelect) ) // Attempt selection
            chkd.mapSettingsWindow.SetWinText("Map Settings - [String #" + std::to_string(currSelString) + "]");
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
    buttonSwap.CreateThis(hWnd, 310, 5, 150, 20, "", (u64)Id::BUTTON_SWITCH);

    listStrings.CreateThis(hWnd, 5, 25, 453, 262, true, false, false, false, (u64)Id::LB_STRINGS);

    buttonDeleteString.CreateThis(hWnd, 130, 290, 200, 20, "Delete String", (u64)Id::DELETE_STRING);
    checkExtendedString.CreateThis(hWnd, 20, 294, 100, 10, false, "Extended", (u64)Id::CHECK_EXTENDEDSTRING);
    checkExtendedString.DisableThis();
    buttonSaveString.CreateThis(hWnd, 340, 290, 75, 20, "Save to...", (u64)Id::SAVE_TO);
    editString.CreateThis(hWnd, 5, 310, 453, 140, true, (u64)Id::EDIT_STRING);

    textStringUsage.CreateThis(hWnd, 480, 379, 125, 20, "String Usage:", 0);
    listUsage.CreateThis(hWnd, 463, 394, 125, 83, false, false, false, false, (u64)Id::LB_STRINGUSE);

    stringGuide.CreateThis(hWnd);
    stringPreviewWindow.CreateThis(hWnd, (u64)Id::PREVIEW_STRING);
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
        if ( LOWORD(wParam) == (WORD)Id::LB_STRINGS ) // Change selection, update info boxes and so fourth
        {
            if ( currSelString != 0 )
                updateString(currSelString);

            currSelString = 0;
            listUsage.ClearItems();
            int lbIndex;
            if ( listStrings.GetCurSel(lbIndex) )
            {
                ChkdString str;
                if ( listStrings.GetItemData(lbIndex, currSelString) && CM != nullptr &&
                    CM->GetString(str, currSelString) && str.length() > 0 )
                {
                    editString.SetText(str);

                    u32 locs, trigs, briefs, props, forces, wavs, units, switches;
                    CM->getStringUse(currSelString, locs, trigs, briefs, props, forces, wavs, units, switches);
                    addUseItem("Locations", locs);
                    addUseItem("Triggers", trigs);
                    addUseItem("Briefing Triggers", briefs);
                    addUseItem("Map Properties", props);
                    addUseItem("Forces", forces);
                    addUseItem("WAVs", wavs);
                    addUseItem("Units", units);
                    addUseItem("Switches", switches);
                    chkd.mapSettingsWindow.SetWinText("Map Settings - [String #" + std::to_string(currSelString) + "]");
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
        if ( LOWORD(wParam) == (WORD)Id::LB_STRINGS && currSelString != 0 && updateString(currSelString) )
            CM->refreshScenario();
        break;
    case EN_KILLFOCUS: // String edit box may have lost focus, check if string should be updated
        if ( LOWORD(wParam) == (WORD)Id::EDIT_STRING && currSelString != 0 && updateString(currSelString) )
            CM->refreshScenario();
        break;
    case BN_CLICKED:
        if ( LOWORD(wParam) == (WORD)Id::DELETE_STRING &&
            WinLib::GetYesNo("Forcefully deleting a string could cause problems, continue?", "Warning") == WinLib::PromptResult::Yes &&
            CM != nullptr && currSelString != 0 && CM->stringExists(currSelString)
            )
        {
            CM->forceDeleteString(currSelString);
            CM->refreshScenario();
        }
        else if ( LOWORD(wParam) == (WORD)Id::SAVE_TO && CM != nullptr )
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

        case (UINT)WinLib::LB::WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
            stringListDC = listStrings.getDC();
            break;

        case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                ChkdString str;

                if ( CM->GetString(str, (u32)mis->itemData) && str.size() > 0 &&
                     GetStringDrawSize(stringListDC, mis->itemWidth, mis->itemHeight, str) )
                {
                    mis->itemWidth += 5;
                    mis->itemHeight += 2;
                }
                return TRUE;
            }
            break;

        case (UINT)WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listStrings.ReleaseDC(stringListDC);
            stringListDC = NULL;
            break;

        case (UINT)WinLib::LB::WM_PREDRAWITEMS:
            break;

        case WM_DRAWITEM:
            {
                PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
                bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
                     drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
                     drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

                if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
                {
                    ChkdString str;
                    if ( CM != nullptr && CM->GetString(str, (u32)pdis->itemData) && str.size() > 0 )
                    {
                        HBRUSH hBackground = CreateSolidBrush(RGB(0, 0, 0)); // Same color as in WM_CTLCOLORLISTBOX
                        if ( hBackground != NULL )
                        {
                            FillRect(pdis->hDC, &pdis->rcItem, hBackground); // Draw far background
                            DeleteObject(hBackground);
                            hBackground = NULL;
                        }
                        SetBkMode(pdis->hDC, TRANSPARENT);
                        DrawString(pdis->hDC, pdis->rcItem.left+3, pdis->rcItem.top+1, pdis->rcItem.right-pdis->rcItem.left,
                            RGB(16, 252, 24), str);
                    }
                    if ( isSelected )
                        DrawFocusRect(pdis->hDC, &pdis->rcItem);
                }
                return TRUE;
            }
            break;

        case (UINT)WinLib::LB::WM_POSTDRAWITEMS:
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}

void StringEditorWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    switch ( (Id)idFrom )
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
            filePath = MakeExtSystemFilePath(filePath, ".txt");

        RemoveFile(filePath);

        std::ofstream outFile(filePath, std::ofstream::out);
        if ( outFile.is_open() )
        {
            ChkdString str;
            for ( u32 i=0; i<CM->strSectionCapacity(); i++ )
            {
                if ( CM->GetString(str, i) && str.size() > 0 )
                    outFile << i << ": " << str << "\r\n";
            }
            outFile.close();
        }
    }
}

void StringEditorWindow::addUseItem(std::string str, u32 amount)
{
    if ( amount > 0 )
        listUsage.AddString(str + ": " + std::to_string(amount));
}

bool StringEditorWindow::updateString(u32 stringNum)
{
    ChkdString editStr;
    if ( CM != nullptr && editString.GetWinText(editStr) && CM->FindDifference(editStr, stringNum) )
    {
        if ( CM->editString<u32>(editStr, stringNum, CM->isExtendedString(stringNum), false) )
        {
            CM->notifyChange(false);
            if ( CM->stringUsedWithLocs(currSelString) )
                chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
            editString.RedrawThis();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
