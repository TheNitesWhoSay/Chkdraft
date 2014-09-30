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

// String based stuff

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

void StrToUpper(std::string str)
{
	for ( auto & c: str )
		c = toupper(c);
}

bool GetPlayerNum(DropdownControl control, u8 &dest)
{
	bool success = false;
	std::string editText;

	if ( control.GetEditNum<u8>(dest) )
	{
		dest --; // to 0 based
		success = true;
	}
	else if ( control.GetEditText(editText) )
	{
		StrToUpper(editText);

		int parsedPlayer = 0;

		if (	sscanf_s(editText.c_str(), "PLAYER%d", &parsedPlayer) > 0
			 || sscanf_s(editText.c_str(), "P%d"	 , &parsedPlayer) > 0
			 || sscanf_s(editText.c_str(), "ID:%d"   , &parsedPlayer) > 0 )
		{
			dest = parsedPlayer;
			dest --; // to 0 based
			success = true;
		}
	}
	return success;
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
