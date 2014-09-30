#ifndef GUIACCEL_H
#define GUIACCEL_H
#include "Common Files/CommonFiles.h"
#include "Windows UI/WindowsUI.h"

void ReplaceChildFonts(HWND hWnd, HFONT hFont);

void LockCursor(HWND hWnd);
void TrackMouse(HWND hWnd, DWORD hoverTime);

// String based stuff

/** Attempts to turn an escaped string (either via <XX>'s or \'s) into a raw string
	Returns true and modifies str if successful, str not modified otherwise */
bool parseEscapedString(std::string& str);

void StrToUpper(std::string str);

bool GetPlayerNum(DropdownControl control, u8 &dest);

// MDI-Map-Window(s) stuff

HWND AddMDIChild(HWND hClient, const char* Name);

BITMAPINFO GetBMI(s32 width, s32 height);
LPBITMAPINFO GetPalBMI(s32 width, s32 height);

#endif