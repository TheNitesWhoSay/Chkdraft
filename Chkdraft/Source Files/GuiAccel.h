#ifndef GUIACCEL_H
#define GUIACCEL_H
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include "GuiMap.h"

// Generally for trees...
//	- LPARAM First 4 bits: defines the item type
//  - LPARAM Last 28 bits: defines data specific to the type of item
// 'AND' with the following to retrieve either from the LPARAM
#define TREE_ITEM_DATA (0x0FFFFFFF)
#define TREE_ITEM_TYPE (0xF0000000)

#define TREE_TYPE_ROOT			(0x00000000) // Roots are AND'd with an associated layer (found in TREE_ITEM_DATA)
#define TREE_TYPE_CATEGORY		(0x10000000) // Categories are AND'd with an associated layer (found in TREE_ITEM_DATA)
#define TREE_TYPE_ISOM			(0x20000000)
#define TREE_TYPE_UNIT			(0x30000000)
#define TREE_TYPE_LOCATION		(0x40000000)
#define TREE_TYPE_SPRITE		(0x50000000)
#define TREE_TYPE_DOODAD		(0x60000000)

void LockCursor(HWND hWnd);

/** Attempts to turn an escaped string (either via <XX>'s or \'s) into a raw string
	Returns true and modifies str if successful, str not modified otherwise */
bool parseEscapedString(string& str);

// MDI-Map-Window(s) stuff

HWND AddMDIChild(HWND hClient, const char* Name);

BITMAPINFO GetBMI(s32 width, s32 height);
LPBITMAPINFO GetPalBMI(s32 width, s32 height);

// Edit control stuff

//bool GetEditText(HWND hEdit, char* &dest); // Be sure to delete "dest" if successful

bool GetEditText(HWND hEdit, string& dest);

template <typename numType>
	bool GetEditNum(HWND hEdit, numType &dest);

bool GetPlayerNum(HWND hEdit, u8 &dest);

template <typename numType>
	bool GetEditNum(HWND hDlg, int editID, numType &dest);

bool GetEditBinaryNum(HWND hDlg, int editNum, u16 &dest);

// List View stuff

void AddColumn(HWND hListView, int insertAt, const char* title, int width, int flags);
void AddRow(HWND hListView, int numColumns, LPARAM lParam);
void RemoveRow(HWND hListView, int rowNum);
void ChangeLParam(HWND hListView, LPARAM oldLParam, LPARAM newLParam);
void SetItem(HWND hListView, int row, int column, const char* text);
void SetItem(HWND hListView, int row, int column, int value);
int GetItemRow(HWND hListView, int lParam);

// Tree View stuff

struct TreeView
{
	int numItems;
	HTREEITEM TreePtr[228];
	int UnitID[228];
};

extern TV_INSERTSTRUCT doodads;

extern TreeView DefaultTree;

TV_INSERTSTRUCT InsertUnits(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char** items, int amount);

HTREEITEM InsertUnitGrouping(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, HTREEITEM LastGroup);

TV_INSERTSTRUCT InsertChild(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText);

TV_INSERTSTRUCT InsertParent(HWND hWnd, TV_INSERTSTRUCT tvinsert, const char* pszText, LPARAM lParam);

void BuildLocationTree(GuiMap* map);

void BuildMainTree(HWND hWnd);

void UpdateTreeItemText(HWND hWnd, int UnitID);

void changeDisplayName(HWND hWnd, int UnitID, const char* name);

HWND CreateTabWindow(HWND hParent, LPCSTR className, u32 id);
HWND CreateEditBox(HWND hParent, int x, int y, int width, int height, const char* initText, bool wordWrap, u32 editID);
HWND CreateGroupBox(HWND hParent, int x, int y, int width, int height, const char* text);
HWND CreateStaticText(HWND hParent, int x, int y, int width, int height, const char* text, u32 id);
HWND CreateStaticText(HWND hParent, int x, int y, int width, int height, const char* text);
HWND CreateButton(HWND hParent, int x, int y, int width, int height, const char* text, u32 buttonID);
HWND CreateCheckBox(HWND hParent, int x, int y, int width, int height, bool checked, const char* text, u32 checkID);
HWND CreateDropdownBox(HWND hParent, int x, int y, int width, int height, const char** strings, int numStrings, int curSel, bool editable, u32 dropdownID);
HWND CreateListBox(HWND hParent, int x, int y, int width, int height, u32 id, bool ownerDrawn);
HWND CreateDragListBox(HWND hParent, int x, int y, int width, int height, u32 dropdownID);

#endif