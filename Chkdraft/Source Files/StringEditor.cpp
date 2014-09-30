#include "StringEditor.h"
#include "Chkdraft.h"

StringEditorWindow::StringEditorWindow() : currSelString(0)
{

}

bool StringEditorWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	if ( ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "StringEditor", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)ID_STRINGEDITOR) )
	{
		HWND hStringEditor = getHandle();
		textAboutStrings.CreateThis(hStringEditor, 5, 5, 100, 20, "String Editor...", 0);

		listboxStrings.CreateThis(hStringEditor, 5, 25, 453, 262, true, ID_LB_STRINGS);

		buttonDeleteString.CreateThis(hStringEditor, 130, 290, 200, 20, "Delete String", ID_DELETE_STRING);
		checkExtendedString.CreateThis(hStringEditor, 20, 294, 100, 10, false, "Extended", ID_CHECK_EXTENDEDSTRING);
		editString.CreateThis(hStringEditor, 5, 310, 453, 140, true, ID_EDIT_STRING);

		textStringUsage.CreateThis(hStringEditor, 480, 379, 125, 20, "String Usage:", 0);
		listUsage.CreateThis(hStringEditor, 463, 394, 125, 83, false, ID_LB_STRINGUSE);

		stringGuide.CreateThis(hStringEditor);
		stringPreviewWindow.CreateThis(hStringEditor);

		RefreshWindow();
		return true;
	}
	else
		return false;
}

void StringEditorWindow::RefreshWindow()
{
	HWND hStringList = GetDlgItem(getHandle(), ID_LB_STRINGS);
	if ( hStringList != NULL && chkd.maps.curr != nullptr )
	{
		SendMessage(hStringList, LB_RESETCONTENT, NULL, NULL);
		bool success = false;
		StringUsageTable strUse(chkd.maps.curr->scenario(), false, success);
		if ( success )
		{
			string str;
			u32 lastUsed = strUse.lastUsedString();
			for ( u32 i=0; i<=lastUsed; i++ )
			{
				if ( strUse.isUsed(i) && chkd.maps.curr->getString(str, i) && str.size() > 0 )
				{
					int lbIndex = SendMessage(hStringList, LB_ADDSTRING, NULL, (LPARAM)"");
					if ( lbIndex != LB_ERR && lbIndex != LB_ERRSPACE )
						SendMessage(hStringList, LB_SETITEMDATA, lbIndex, i);
				}
			}
		}
	}
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
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_MOUSEWHEEL:
			{
				int distanceScrolled = int((s16(HIWORD(wParam)))/WHEEL_DELTA);
				HWND hStringSel = GetDlgItem(hWnd, ID_LB_STRINGS);
				if ( hStringSel != NULL )
					ListBox_SetTopIndex(hStringSel, ListBox_GetTopIndex(hStringSel)-distanceScrolled);
			}
			break;

		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case LBN_SELCHANGE:
					if ( LOWORD(wParam) == ID_LB_STRINGS ) // Change selection, update info boxes and so fourth
					{
						HWND hEditString = GetDlgItem(hWnd, ID_EDIT_STRING);
						if ( hEditString != NULL && currSelString != 0 )
							updateString(currSelString);
						HWND hStringUse = GetDlgItem(hWnd, ID_LB_STRINGUSE);
						if ( hStringUse != NULL )
							SendMessage(hStringUse, LB_RESETCONTENT, NULL, NULL);

						int lbIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0);
						if ( lbIndex != LB_ERR )
						{
							string str = "";
							currSelString = SendMessage((HWND)lParam, LB_GETITEMDATA, lbIndex, NULL);
							if ( currSelString != 0 && chkd.maps.curr != nullptr && chkd.maps.curr->getString(str, currSelString) && str.length() > 0 )
							{
								if ( hEditString != NULL )
									SetWindowText(hEditString, str.c_str());
						
								if ( hStringUse != NULL )
								{
									u32 locs, trigs, briefs, props, forces, wavs, units, switches;
									chkd.maps.curr->getStringUse(currSelString, locs, trigs, briefs, props, forces, wavs, units, switches);
									addUseItem(hStringUse, "Locations", locs);
									addUseItem(hStringUse, "Triggers", trigs);
									addUseItem(hStringUse, "Briefing Triggers", briefs);
									addUseItem(hStringUse, "Map Properties", props);
									addUseItem(hStringUse, "Forces", forces);
									addUseItem(hStringUse, "WAVs", wavs);
									addUseItem(hStringUse, "Units", units);
									addUseItem(hStringUse, "Switches", switches);
								}
								return 0;
							}
						}
						if ( hEditString != NULL )
							SetWindowText(hEditString, "");
					}
					break;
				case LBN_KILLFOCUS: // String list box item may have lost focus, check if string should be updated
					if ( LOWORD(wParam) == ID_LB_STRINGS )
					{
						HWND hEditString = GetDlgItem(hWnd, ID_EDIT_STRING);
						if ( hEditString != NULL && currSelString != 0 )
							updateString(currSelString);
					}
					break;
				case EN_KILLFOCUS: // String edit box may have lost focus, check if string should be updated
					if ( LOWORD(wParam) == ID_EDIT_STRING && currSelString != 0 && updateString(currSelString) )
						chkd.maps.curr->refreshScenario();
					break;
				case BN_CLICKED:
					if ( LOWORD(wParam) == ID_DELETE_STRING  &&
						 MessageBox(hWnd, "Forcefully deleting a string could cause problems, continue?", "Warning", MB_ICONEXCLAMATION|MB_YESNO) == IDYES &&
						 chkd.maps.curr != nullptr && currSelString != 0 && chkd.maps.curr->stringExists(currSelString)
					   )
					{
						chkd.maps.curr->forceDeleteString(currSelString);
						chkd.maps.curr->refreshScenario();
					}
					break;
			}
			return 0;
			break;

		case WM_MEASUREITEM:
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				HWND hStringList = GetDlgItem(hWnd, ID_LB_STRINGS);
				SIZE strSize;
				string str;

				if ( hStringList != NULL && chkd.maps.curr->getString(str, mis->itemData) && str.size() > 0 )
				{
					HDC hDC = GetDC(hStringList);
					if ( hDC != NULL )
					{
						HGDIOBJ sel = SelectObject(hDC, defaultFont);
						if ( sel != NULL && sel != HGDI_ERROR &&
							 GetTextExtentPoint32(hDC, (LPCSTR)str.c_str(), str.size(), &strSize) != 0 )
						{
							mis->itemWidth = strSize.cx+5;
							mis->itemHeight = strSize.cy+2;
							ReleaseDC(hWnd, hDC);
							return TRUE;
						}
						ReleaseDC(hWnd, hDC);
					}
				}
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		case WM_DRAWITEM:
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;

				if ( pdis->itemID != -1 && pdis->itemAction == ODA_SELECT || pdis->itemAction == ODA_DRAWENTIRE )
				{
					string str;
					HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
					if ( hBlack != NULL )
					{
						FillRect(pdis->hDC, &pdis->rcItem, hBlack);
						DeleteObject(hBlack);
					}
					if ( pdis->itemState & ODS_SELECTED )
						DrawFocusRect(pdis->hDC, &pdis->rcItem);
					
					if ( chkd.maps.curr != nullptr && chkd.maps.curr->getString(str, pdis->itemData) && str.size() > 0 )
					{
						SetBkMode(pdis->hDC, TRANSPARENT);
						SetTextColor(pdis->hDC, RGB(16, 252, 24));
						ExtTextOut(pdis->hDC, pdis->rcItem.left+3, pdis->rcItem.top+1, ETO_CLIPPED, &pdis->rcItem, (LPCSTR)str.c_str(), str.size(), 0);
					}
				}
				return TRUE;
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void StringEditorWindow::addUseItem(HWND hStringUse, const char* title, u32 amount)
{
	stringstream ss;
	if ( amount > 0 )
	{
		ss << title << ": " << amount;
		SendMessage(hStringUse, LB_ADDSTRING, NULL, (LPARAM)ss.str().c_str());
	}
}

bool StringEditorWindow::updateString(u32 stringNum)
{
	string editStr;
	if ( chkd.maps.curr != nullptr && editString.GetEditText(editStr) && chkd.maps.curr->escStringDifference(editStr, stringNum) )
	{
		if ( parseEscapedString(editStr) &&
			 chkd.maps.curr->editString<u32>(editStr, stringNum, chkd.maps.curr->isExtendedString(stringNum), false) )
		{
			if ( chkd.maps.curr->stringUsedWithLocs(currSelString) )
				chkd.mainPlot.leftBar.mainTree.locTree.RebuildLocationTree();
			RedrawWindow((HWND)editString.getHandle(), NULL, NULL, RDW_INVALIDATE);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
