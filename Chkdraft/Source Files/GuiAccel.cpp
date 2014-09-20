#include "GuiAccel.h"

BOOL CALLBACK SetFont(HWND hWnd, LPARAM hFont)
{
	SendMessage(hWnd, WM_SETFONT, hFont, TRUE);
	return TRUE;
}

void ReplaceChildFonts(HWND hWnd, HFONT hFont)
{
	EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
}

void LockCursor(HWND hWnd)
{
	if ( hWnd == NULL )
		ClipCursor(NULL);
	else
	{
		RECT rcMap, rcClip;
		POINT upperLeft, lowerRight;
		upperLeft.x = 0;
		upperLeft.y = 0;

		GetClientRect(hWnd, &rcMap);
		lowerRight.x = rcMap.right-1;
		lowerRight.y = rcMap.bottom-1;

		ClientToScreen(hWnd, &upperLeft);
		ClientToScreen(hWnd, &lowerRight);

		rcClip.left = upperLeft.x;
		rcClip.top = upperLeft.y;
		rcClip.bottom = lowerRight.y;
		rcClip.right = lowerRight.x;

		ClipCursor(&rcClip);
	}
}

void TrackMouse(HWND hWnd, DWORD hoverTime)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = hWnd;
	tme.dwHoverTime = hoverTime;
	TrackMouseEvent(&tme);
}

bool parseEscapedString(std::string& str)
{
	char currChar;
	std::string newStr = "";
	int strLength = str.length();
	const char* strPtr = str.c_str();
	try
	{
		u8 value;
		for ( int i=0; i<strLength; i++ )
		{
			currChar = strPtr[i];
			switch ( currChar )
			{
				case '\\':
					if ( i+1 < strLength )
					{
						i ++;
						currChar = strPtr[i];
						if ( currChar == 'X' || currChar == 'x' )
						{
							if ( i+3 < strLength && strPtr[i+1] >= '0' && strPtr[i+1] <= '9' && getTwoByteHexVal(&strPtr[i+2], value) )
							{
								newStr.push_back(value);
								i += 3;
							}
							else if ( i+2 < strLength && getTwoByteHexVal(&strPtr[i+1], value) )
							{
								newStr.push_back(value);
								i += 2;
							}
							else if ( i+1 < strLength && getOneByteHexVal(strPtr[i+1], value) )
							{
								newStr.push_back(value);
								i ++;
							}
							else
							{
								newStr.push_back('\\');
								newStr.push_back(currChar);
							}
						}
						else if ( currChar == 'n' )
							newStr.push_back('\n');
						else if ( currChar == 'r' )
							newStr.push_back('\r');
						else
							newStr.push_back(currChar);
					}
					else
						newStr.push_back(currChar);
					break;

				case '<':
					if ( i+2 < strLength )
					{
						if ( strPtr[i+2] == '>' && getOneByteHexVal(strPtr[i+1], value) )
						{
							newStr.push_back(value);
							i += 2;
						}
						else if ( i+3 < strLength && strPtr[i+3] == '>' && getTwoByteHexVal(&strPtr[i+1], value) )
						{
							newStr.push_back(value);
							i += 3;
						}
						else
							newStr.push_back(currChar);
					}
					else
						newStr.push_back(currChar);
					break;

				default:
					newStr.push_back(currChar);
					break;
			}
		}
		str = newStr;
		return true;
	}
	catch ( std::exception ) { return false; } // catches length_error and bad_alloc
}

// MDI-Map-Window(s) stuff

HWND AddMDIChild(HWND hClient, const char* name)
{
	return CreateMDIWindow("MdiChild", name, WS_MAXIMIZE,
						   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						   hClient, GetModuleHandle(NULL), 0);
}

BITMAPINFO GetBMI(s32 width, s32 height)
{
	BITMAPINFOHEADER bmiH = { };
	bmiH.biSize = sizeof(BITMAPINFOHEADER);
	bmiH.biWidth = width; bmiH.biHeight = -height;
	bmiH.biPlanes = 1;
	bmiH.biBitCount = 24;
	bmiH.biCompression = BI_RGB;
	bmiH.biXPelsPerMeter = 1; bmiH.biYPelsPerMeter = 1;
					
	BITMAPINFO bmi = { };
	bmi.bmiHeader = bmiH;
	return bmi;
}

LPBITMAPINFO GetPalBMI(s32 width, s32 height)
{
	BITMAPINFOHEADER bmiH = { };
	bmiH.biSize = sizeof(BITMAPINFOHEADER);
	bmiH.biWidth = width; bmiH.biHeight = -height;
	bmiH.biPlanes = 1;
	bmiH.biBitCount = 32;
	bmiH.biClrUsed = 256;
	bmiH.biClrImportant = 256;
	bmiH.biCompression = BI_RGB;
	bmiH.biXPelsPerMeter = 1; bmiH.biYPelsPerMeter = 1;
					
	char* cBmi = new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256];
	LPBITMAPINFO bmi = (LPBITMAPINFO)cBmi;
	bmi->bmiHeader = bmiH;
	return bmi;
}


// Edit control stuff

bool GetEditText(HWND hEdit, char* &dest)
{
	bool success = false;
	int length = GetWindowTextLength(hEdit)+1;
	if ( length > 1 )
	{
		char* text;
		try {
			text = new char[length];
		} catch ( std::bad_alloc ) {
			return false;
		}
		if ( GetWindowText(hEdit, text, length) )
		{
			text[length-1] = '\0';
			dest = new char[length];
			strcpy_s(dest, length, text);
			success = true;
		}
		delete[] text;
	}
	return success;
}

bool GetEditText(HWND hEdit, std::string& dest)
{
	char* temp;
	if ( GetEditText(hEdit, temp) )
	{
		dest = temp;
		delete[] temp;
		return true;
	}
	else
		return false;
}

bool GetEditText(HWND hDlg, int editID, char* &dest)
{
	return GetEditText(GetDlgItem(hDlg, editID), dest);
}

template <typename numType>
bool GetEditNum(HWND hEdit, numType &dest)
{
	bool success = false;
	char* text;
	if ( GetEditText(hEdit, text) )
	{
		int temp;
		if ( temp = atoi(text) )
		{
			dest = temp;
			success = true;
		}
		else if ( strlen(text) > 0 && text[0] == '0' )
		{
			dest = 0;
			success = true;
		}
		delete[] text;
	}
	return success;
}
template bool GetEditNum<u8>(HWND hEdit, u8 &dest);
template bool GetEditNum<u16>(HWND hEdit, u16 &dest);
template bool GetEditNum<s32>(HWND hEdit, s32 &dest);
template bool GetEditNum<u32>(HWND hEdit, u32 &dest);
template bool GetEditNum<int>(HWND hEdit, int &dest);

template <typename numType>
bool GetEditNum(HWND hDlg, int editID, numType &dest)
{
	return GetEditNum<numType>(GetDlgItem(hDlg, editID), dest);
}
template bool GetEditNum<u8>(HWND hDlg, int editID, u8 &dest);
template bool GetEditNum<u16>(HWND hDlg, int editID, u16 &dest);
template bool GetEditNum<s32>(HWND hDlg, int editID, s32 &dest);
template bool GetEditNum<u32>(HWND hDlg, int editID, u32 &dest);
template bool GetEditNum<int>(HWND hDlg, int editID, int &dest);

bool GetEditBinaryNum(HWND hDlg, int editNum, u16 &dest)
{
	char* editText;
	if ( GetEditText(hDlg, editNum, editText) )
	{
		const u16 u16BitValues[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
									 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

		int length = strlen(editText);
		dest = 0;
		for ( int i=length-1; i>=0; i-- )
		{
			if ( editText[i] == '1' )
				dest |= u16BitValues[(length-1)-i];
			else if ( editText[i] != '0' )
				return false;
		}
		delete[] editText;
		return true;
	}
	else
		return false;
}

void sToUpper(char* str)
{
	for ( size_t i=0; i<strlen(str); i++ )
		str[i] = toupper(str[i]);
}

bool GetPlayerNum(HWND hEdit, u8 &dest)
{
	bool success = false;
	char* editText;

	if ( GetEditNum<u8>(hEdit, dest) )
	{
		dest --; // to 0 based
		success = true;
	}
	else if ( GetEditText(hEdit, editText) )
	{
		sToUpper(editText);

		int parsedPlayer = 0;

		if (	sscanf_s(editText, "PLAYER%d", &parsedPlayer) > 0
			 || sscanf_s(editText, "P%d"	   , &parsedPlayer) > 0
			 || sscanf_s(editText, "ID:%d"   , &parsedPlayer) > 0 )
		{
			dest = parsedPlayer;
			dest --; // to 0 based
			success = true;
		}

		delete[] editText;
	}

	return success;
}

// List View stuff

void AddColumn(HWND hListView, int insertAt, const char* title, int width, int flags)
{
	LV_COLUMN lvCol = { };
	lvCol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM|LVCF_ORDER;
	lvCol.fmt = flags;
	lvCol.cx = width;
	lvCol.iSubItem = insertAt+1;
	lvCol.pszText = (LPSTR)title;
	lvCol.cchTextMax = sizeof(title);
	lvCol.iOrder = insertAt;

	ListView_InsertColumn(hListView, insertAt, &lvCol);
}

void AddRow(HWND hListView, int numColumns, LPARAM lParam)
{
	LVITEM item = { };
	item.iItem = lParam;
	item.mask = LVIF_COLUMNS|LVIF_PARAM;
	item.cColumns = numColumns;
	item.lParam = lParam;

	ListView_InsertItem(hListView, &item);
}

void RemoveRow(HWND hListView, int rowNum)
{
	ListView_DeleteItem(hListView, rowNum);
}

void SetItem(HWND hListView, int row, int column, const char* text)
{
	LVITEM subItem = { };
	subItem.mask = LVIF_TEXT;
	subItem.iItem = row;
	subItem.pszText = LPSTR(text);
	subItem.cchTextMax = strlen(text);
	subItem.iSubItem = column;

	ListView_SetItem(hListView, &subItem);
}

void SetItem(HWND hListView, int row, int column, int value)
{
	char cVal[32] = { };
	_itoa_s(value, cVal, 10);
	SetItem(hListView, row, column, cVal);
}

int GetItemRow(HWND hListView, int lParam)
{
	LVFINDINFO lvFind = { };
	lvFind.flags = LVFI_PARAM;
	lvFind.lParam = lParam;
	return ListView_FindItem(hListView, -1, &lvFind);
}

// Tree View stuff

TV_INSERTSTRUCT InsertChild(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam)
{
	tvinsert.hInsertAfter = TVI_LAST;
	tvinsert.item.mask |= LVIF_PARAM;
	tvinsert.item.pszText = (LPSTR)pszText;
	tvinsert.item.lParam = lParam;
	tvinsert.hParent = TreeView_InsertItem(hWnd, &tvinsert);
	return tvinsert;
}

TV_INSERTSTRUCT InsertParent(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam)
{
	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter = TVI_ROOT;
	tvinsert.item.mask |= (TVIF_TEXT|TVIF_PARAM);
	tvinsert.item.pszText = (LPSTR)pszText;
	tvinsert.item.lParam = TREE_TYPE_ROOT|lParam;
	tvinsert.hParent = TreeView_InsertItem(hWnd, &tvinsert);
	return tvinsert;
}

HWND CreateTabWindow(HWND hParent, LPCSTR className, u32 id)
{
	return CreateWindow(className, "", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)id, GetModuleHandle(NULL), NULL);
}

HWND CreateEditBox(HWND hParent, int x, int y, int width, int height, const char* initText, bool wordWrap, u32 editID)
{
	DWORD style = WS_VISIBLE|WS_CHILD;

	if ( wordWrap )
		style |= ES_MULTILINE|ES_AUTOVSCROLL;
	else
		style |= ES_AUTOHSCROLL;

	HWND hEdit = CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", initText,
						   style, x, y, width, height,
						   hParent, (HMENU)editID, NULL, NULL );

	if ( hEdit != NULL )
		SendMessage(hEdit, WM_SETFONT, (WPARAM)defaultFont, FALSE);

	return hEdit;
}

HWND CreateGroupBox(HWND hParent, int x, int y, int width, int height, const char* text)
{
	HWND hGroupBox = CreateWindow("Button", text, WS_CHILD|WS_VISIBLE|BS_GROUPBOX, x, y, width, height, hParent, NULL, GetModuleHandle(NULL), NULL);

	if ( hGroupBox != NULL )
		SendMessage(hGroupBox, WM_SETFONT, (WPARAM)defaultFont, FALSE);

	return hGroupBox;
}

HWND CreateStaticText(HWND hParent, int x, int y, int width, int height, const char* text, u32 id)
{
	HWND hStatic = CreateWindowA("STATIC", text, WS_VISIBLE|WS_CHILD, x, y, width, height, hParent, (HMENU)id, NULL, NULL);

	if ( hStatic != NULL )
		SendMessage(hStatic, WM_SETFONT, (WPARAM)defaultFont, FALSE);

	return hStatic;
}

HWND CreateStaticText(HWND hParent, int x, int y, int width, int height, const char* text)
{
	return CreateStaticText(hParent, x, y, width, height, text, NULL);
}

HWND CreateButton(HWND hParent, int x, int y, int width, int height, const char* text, u32 buttonID)
{
	HWND hButton = CreateWindowEx( WS_EX_CLIENTEDGE, "Button", text,
								   WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
								   x, y, width, height, hParent, (HMENU)buttonID, NULL, NULL );

	if ( hButton != NULL )
		SendMessage(hButton, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

	return hButton;
}

HWND CreateCheckBox(HWND hParent, int x, int y, int width, int height, bool checked, const char* text, u32 checkID)
{
	HWND hCheckBox = CreateWindow( "Button", text,
								   WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_VCENTER,
								   x, y, width, height, hParent, (HMENU)checkID, NULL, NULL );

	if ( hCheckBox != NULL )
	{
		SendMessage(hCheckBox, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));

		if ( checked )
			SendMessage(hCheckBox, BM_SETCHECK, BST_CHECKED, NULL);
	}

	return hCheckBox;
}

HWND CreateDropdownBox(HWND hParent, int x, int y, int width, int height, const char** strings, int numStrings, int curSel, bool editable, u32 dropdownID)
{
	DWORD style = CBS_DROPDOWN|WS_CHILD|WS_VSCROLL|WS_VISIBLE|CBS_AUTOHSCROLL|CBS_HASSTRINGS;

	if ( editable )
		style |= CBS_DROPDOWN;
	else
		style |= CBS_SIMPLE;

	HWND hDropdown = CreateWindowExA(WS_EX_CLIENTEDGE, "ComboBox", NULL,
						style,
						x, y, width, height,
						hParent, (HMENU)dropdownID, GetModuleHandle(NULL), NULL);

	if ( hDropdown != nullptr )
	{
		for ( int i=0; i<numStrings; i++ )
			SendMessage(hDropdown, CB_ADDSTRING, NULL, (LPARAM)strings[i]);

		SendMessage(hDropdown, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
		SendMessage(hDropdown, CB_SETCURSEL, curSel, NULL);
		PostMessage(hDropdown, CB_SETEDITSEL, NULL, (-1, 0));
	}

	return hDropdown;
}

HWND CreateListBox(HWND hParent, int x, int y, int width, int height, u32 id, bool ownerDrawn)
{
	u32 flags = WS_CHILD|WS_TABSTOP|LBS_NOTIFY|LBS_SORT;
	if ( ownerDrawn )
	{
		flags |= LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS|WS_VSCROLL;
		flags &= (~LBS_SORT);
	}

	HWND hListBox = CreateWindowExA( WS_EX_CLIENTEDGE, "ListBox", "", flags,
									 x, y, width, height,
									 hParent, (HMENU)id, GetModuleHandle(NULL), NULL );

	if ( hListBox != nullptr )
	{
		SendMessage(hListBox, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
		ShowWindow(hListBox, SW_SHOWNORMAL);
	}

	return hListBox;
}

HWND CreateDragListBox(HWND hParent, int x, int y, int width, int height, u32 dropdownID)
{
	HWND hListBox = CreateWindowEx( WS_EX_CLIENTEDGE, "ListBox", "", WS_CHILD|WS_TABSTOP|LBS_NOTIFY|LBS_SORT,
									 x, y, width, height, hParent, (HMENU)dropdownID,
									 GetModuleHandle(NULL), NULL );

	if ( hListBox != nullptr )
	{
		SendMessage(hListBox, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
		MakeDragList(hListBox);
		ShowWindow(hListBox, SW_SHOWNORMAL);
	}

	return hListBox;
}
