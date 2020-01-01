#include "StringEditor.h"
#include "../../../../../MappingCoreLib/FileBrowser.h"
#include "../../../../Chkdraft.h"
#include <fstream>
#include <string>

enum_t(Id, u32, {
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
});

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
            textAboutStrings.SetText(std::string("TODO / 65536 Bytes Used"));
        
        listStrings.SetRedraw(false);
        listStrings.ClearItems();
        numVisibleStrings = 0;
        int toSelect = -1;
        std::bitset<Chk::MaxStrings> strUsed;
        CM->strings.markUsedStrings(strUsed, Chk::Scope::Either, Chk::Scope::Game);

        size_t lastIndex = CM->strings.getCapacity();

        for ( size_t i = 0; i <= lastIndex; i++ )
        {
            if ( strUsed[i] )
            {
                ChkdStringPtr str = CM->strings.getString<ChkdString>(i);
                if ( str != nullptr )
                {
                    int newListIndex = listStrings.AddItem((u32)i);
                    if ( newListIndex != -1 ) // Only consider the string if it could be added to the ListBox
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
    buttonSwap.CreateThis(hWnd, 310, 5, 150, 20, "", Id::BUTTON_SWITCH);

    listStrings.CreateThis(hWnd, 5, 25, 453, 262, true, false, false, false, Id::LB_STRINGS);

    buttonDeleteString.CreateThis(hWnd, 130, 290, 200, 20, "Delete String", Id::DELETE_STRING);
    checkExtendedString.CreateThis(hWnd, 20, 294, 100, 10, false, "Extended", Id::CHECK_EXTENDEDSTRING);
    checkExtendedString.DisableThis();
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
                    ChkdStringPtr str = CM->strings.getString<ChkdString>(currSelString);
                    if ( str != nullptr )
                    {
                        editString.SetText(*str);

                        /* TODO u32 locs, trigs, briefs, props, forces, wavs, units, switches;
                        CM->getStringUse(currSelString, locs, trigs, briefs, props, forces, wavs, units, switches);
                        addUseItem("Locations", locs);
                        addUseItem("Triggers", trigs);
                        addUseItem("Briefing Triggers", briefs);
                        addUseItem("Map Properties", props);
                        addUseItem("Forces", forces);
                        addUseItem("WAVs", wavs);
                        addUseItem("Units", units);
                        addUseItem("Switches", switches);*/
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
            CM != nullptr && currSelString != 0 && CM->strings.stringStored(currSelString) )
        {
            CM->strings.deleteString(currSelString, Chk::Scope::Both, false);
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
            stringListDC = listStrings.getDC();
            break;

        case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
                ChkdStringPtr str = CM->strings.getString<ChkdString>((size_t)mis->itemData);

                if ( str != nullptr &&
                     GetStringDrawSize(stringListDC, mis->itemWidth, mis->itemHeight, *str) )
                {
                    mis->itemWidth += 5;
                    mis->itemHeight += 2;
                }
                return TRUE;
            }
            break;

        case WinLib::LB::WM_POSTMEASUREITEMS: // Release items loaded for measurement
            listStrings.ReleaseDC(stringListDC);
            stringListDC = NULL;
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
                    ChkdStringPtr str = CM->strings.getString<ChkdString>((size_t)pdis->itemData);
                    if ( CM != nullptr && str != nullptr )
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
            filePath = MakeExtSystemFilePath(filePath, ".txt");

        RemoveFile(filePath);

        std::ofstream outFile(filePath, std::ofstream::out);
        if ( outFile.is_open() )
        {
            for ( u32 i=0; i<CM->strings.getCapacity(); i++ )
            {
                ChkdStringPtr str = CM->strings.getString<ChkdString>(i);
                if ( str != nullptr )
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
    ChkdStringPtr existingStr = CM->strings.getString<ChkdString>((size_t)stringNum);
    if ( CM != nullptr && editString.GetWinText(editStr) && existingStr != nullptr && existingStr->compare(editStr) != 0 )
    {
        CM->strings.replaceString<ChkdString>((size_t)stringNum, editStr);
        CM->notifyChange(false);
        if ( CM->layers.stringUsed(currSelString, Chk::Scope::EditorOverGame) )
            chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();

        editString.RedrawThis();
        return true;
    }
    else
        return false;
}
