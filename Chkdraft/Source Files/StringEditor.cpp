#include "StringEditor.h"
#include "Chkdraft.h"

#include <fstream>
#include <string>

enum ID {
	DELETE_STRING = ID_FIRST,
	CHECK_EXTENDEDSTRING,
	SAVE_TO,
	COMPRESS_STRINGS,
	REPAIR_STRINGS,
	LB_STRINGS,
	STRING_GUIDE,
	EDIT_STRING,
	PREVIEW_STRING,
	LB_STRINGUSE
};

StringEditorWindow::StringEditorWindow() : currSelString(0), numVisibleStrings(0), stringListDC(NULL)
{

}

bool StringEditorWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "StringEditor", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
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
	if ( chkd.maps.curr != nullptr )
	{
		listStrings.SetRedraw(false);
		listStrings.ClearItems();
		numVisibleStrings = 0;
		int toSelect = -1;
		StringUsageTable strUse;
		if ( strUse.populateTable(chkd.maps.curr->scenario(), false) )
		{
			std::string str;
			u32 lastUsed = strUse.lastUsedString();
			for ( u32 i=0; i<=lastUsed; i++ )
			{
				if ( strUse.isUsed(i) && chkd.maps.curr->getRawString(str, i) && str.size() > 0 )
				{
					int newListIndex = listStrings.AddItem(i);
					if ( newListIndex != -1 ) // Only consider the string if it could be added to the ListBox
					{
						numVisibleStrings ++;
						if ( currSelString == i ) // This string is the currSelString
							toSelect = newListIndex; // Mark position for selection
					}
				}
			}
		}
		listStrings.SetRedraw(true);
		if ( toSelect != -1 && listStrings.SetCurSel(toSelect) ) // Attempt selection
			chkd.mapSettingsWindow.SetTitle((std::string("Map Settings - [String #") + std::to_string(currSelString) + ']').c_str());
		else
		{
			currSelString = 0; // Clear currSelString if selection fails
			editString.SetText("");
			chkd.mapSettingsWindow.SetTitle("Map Settings");
		}
	}
}

void StringEditorWindow::CreateSubWindows(HWND hWnd)
{
	textAboutStrings.CreateThis(hWnd, 5, 5, 100, 20, "String Editor...", 0);

	listStrings.CreateThis(hWnd, 5, 25, 453, 262, true, false, LB_STRINGS);

	buttonDeleteString.CreateThis(hWnd, 130, 290, 200, 20, "Delete String", DELETE_STRING);
	checkExtendedString.CreateThis(hWnd, 20, 294, 100, 10, false, "Extended", CHECK_EXTENDEDSTRING);
	checkExtendedString.DisableThis();
	buttonSaveString.CreateThis(hWnd, 340, 290, 75, 20, "Save to...", SAVE_TO);
	editString.CreateThis(hWnd, 5, 310, 453, 140, true, EDIT_STRING);

	textStringUsage.CreateThis(hWnd, 480, 379, 125, 20, "String Usage:", 0);
	listUsage.CreateThis(hWnd, 463, 394, 125, 83, false, false, LB_STRINGUSE);

	stringGuide.CreateThis(hWnd);
	stringPreviewWindow.CreateThis(hWnd, PREVIEW_STRING);
}

LRESULT StringEditorWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( HIWORD(wParam) )
	{
	case LBN_SELCHANGE:
		if ( LOWORD(wParam) == LB_STRINGS ) // Change selection, update info boxes and so fourth
		{
			if ( currSelString != 0 )
				updateString(currSelString);

			currSelString = 0;
			listUsage.ClearItems();
			int lbIndex;
			if ( listStrings.GetCurSel(lbIndex) )
			{
				std::string str = "";
				if ( listStrings.GetItemData(lbIndex, currSelString) && chkd.maps.curr != nullptr &&
					chkd.maps.curr->getString(str, currSelString) && str.length() > 0 )
				{
					editString.SetText(str.c_str());

					u32 locs, trigs, briefs, props, forces, wavs, units, switches;
					chkd.maps.curr->getStringUse(currSelString, locs, trigs, briefs, props, forces, wavs, units, switches);
					addUseItem("Locations", locs);
					addUseItem("Triggers", trigs);
					addUseItem("Briefing Triggers", briefs);
					addUseItem("Map Properties", props);
					addUseItem("Forces", forces);
					addUseItem("WAVs", wavs);
					addUseItem("Units", units);
					addUseItem("Switches", switches);
					chkd.mapSettingsWindow.SetTitle((std::string("Map Settings - [String #") +
						std::to_string(currSelString) + ']').c_str());
				}
				else
					chkd.mapSettingsWindow.SetTitle("Map Settings");
				return 0;
			}
			else
				chkd.mapSettingsWindow.SetTitle("Map Settings");
			editString.SetText("");
		}
		break;
	case LBN_KILLFOCUS: // String list box item may have lost focus, check if string should be updated
		if ( LOWORD(wParam) == LB_STRINGS && currSelString != 0 && updateString(currSelString) )
			chkd.maps.curr->refreshScenario();
		break;
	case EN_KILLFOCUS: // String edit box may have lost focus, check if string should be updated
		if ( LOWORD(wParam) == EDIT_STRING && currSelString != 0 && updateString(currSelString) )
			chkd.maps.curr->refreshScenario();
		break;
	case BN_CLICKED:
		if ( LOWORD(wParam) == DELETE_STRING  &&
			MessageBox(hWnd, "Forcefully deleting a string could cause problems, continue?", "Warning", MB_ICONEXCLAMATION | MB_YESNO) == IDYES &&
			chkd.maps.curr != nullptr && currSelString != 0 && chkd.maps.curr->stringExists(currSelString)
			)
		{
			chkd.maps.curr->forceDeleteString(currSelString);
			chkd.maps.curr->refreshScenario();
		}
		else if ( LOWORD(wParam) == SAVE_TO && chkd.maps.curr != nullptr )
			saveStrings();
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

		case WM_PREMEASUREITEMS: // Measuring is time sensative, load necessary items for measuring all strings once
			stringListDC = listStrings.getDC();
			break;

		case WM_MEASUREITEM:
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				std::string str;

				if ( chkd.maps.curr->getRawString(str, mis->itemData) && str.size() > 0 &&
					 GetStringDrawSize(stringListDC, mis->itemWidth, mis->itemHeight, str) )
				{
					mis->itemWidth += 5;
					mis->itemHeight += 2;
				}
				return TRUE;
			}
			break;

		case WM_POSTMEASUREITEMS: // Release items loaded for measurement
			listStrings.ReleaseDC(stringListDC);
			stringListDC = NULL;
			break;

		case WM_PREDRAWITEMS:
			break;

		case WM_DRAWITEM:
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
				bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
					 drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
					 drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

				if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
				{	
					std::string str;
					if ( chkd.maps.curr != nullptr && chkd.maps.curr->getRawString(str, pdis->itemData) && str.size() > 0 )
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

		case WM_POSTDRAWITEMS:
			break;

		default:
			return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void StringEditorWindow::saveStrings()
{
	char filePath[MAX_PATH] = { };
	OPENFILENAME ofn = GetOfn(filePath, "Text Documents(*.txt)\0*.txt\0All Files\0*\0", 0);
	if ( GetSaveFileName(&ofn) )
	{
		if ( ofn.nFilterIndex == 1 && std::strstr(filePath, ".txt") == nullptr )
			std::strcat(filePath, ".txt");

		DeleteFileA(filePath);

		std::ofstream outFile(filePath, std::ofstream::out);
		if ( outFile.is_open() )
		{
			std::string str;
			for ( u32 i=0; i<chkd.maps.curr->numStrings(); i++ )
			{
				if ( chkd.maps.curr->getString(str, i) && str.size() > 0 )
					outFile << i << ": " << str << "\r\n";
			}
			outFile.close();
		}
	}
}

void StringEditorWindow::addUseItem(std::string str, u32 amount)
{
	if ( amount > 0 )
		listUsage.AddString((str + ": " + std::to_string(amount)).c_str());
}

bool StringEditorWindow::updateString(u32 stringNum)
{
	std::string editStr;
	if ( chkd.maps.curr != nullptr && editString.GetEditText(editStr) && chkd.maps.curr->escStringDifference(editStr, stringNum) )
	{
		if ( parseEscapedString(editStr) &&
			 chkd.maps.curr->editString<u32>(editStr, stringNum, chkd.maps.curr->isExtendedString(stringNum), false) )
		{
			chkd.maps.curr->notifyChange(false);
			if ( chkd.maps.curr->stringUsedWithLocs(currSelString) )
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
