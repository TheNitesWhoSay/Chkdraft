#include "ListViewControl.h"

bool ListViewControl::CreateThis(HWND hParent, int x, int y, int width, int height, u32 id)
{
	if ( WindowControl::CreateControl(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS,
									  9, 10, 549, 449, hParent, (HMENU)id, false) )
	{
		return true;
	}
	return false;
}

void ListViewControl::AddColumn(int insertAt, const char* title, int width, int alignmentFlags)
{
	LV_COLUMN lvCol = { };
	lvCol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM|LVCF_ORDER;
	lvCol.fmt = alignmentFlags;
	lvCol.cx = width;
	lvCol.iSubItem = insertAt+1;
	lvCol.pszText = (LPSTR)title;
	lvCol.cchTextMax = sizeof(title);
	lvCol.iOrder = insertAt;

	ListView_InsertColumn(getHandle(), insertAt, &lvCol);
}

void ListViewControl::AddRow(int numColumns, LPARAM lParam)
{
	LVITEM item = { };
	item.iItem = lParam;
	item.mask = LVIF_COLUMNS|LVIF_PARAM;
	item.cColumns = numColumns;
	item.lParam = lParam;

	ListView_InsertItem(getHandle(), &item);
}

void ListViewControl::RemoveRow(int rowNum)
{
	ListView_DeleteItem(getHandle(), rowNum);
}

void ListViewControl::SetItem(int row, int column, const char* text)
{
	LVITEM subItem = { };
	subItem.mask = LVIF_TEXT;
	subItem.iItem = row;
	subItem.pszText = LPSTR(text);
	subItem.cchTextMax = strlen(text);
	subItem.iSubItem = column;

	ListView_SetItem(getHandle(), &subItem);
}

void ListViewControl::SetItem(int row, int column, int value)
{
	char cVal[32] = { };
	_itoa_s(value, cVal, 10);
	SetItem(row, column, cVal);
}

int ListViewControl::GetItemRow(int lParam)
{
	LVFINDINFO lvFind = { };
	lvFind.flags = LVFI_PARAM;
	lvFind.lParam = lParam;
	return ListView_FindItem(getHandle(), -1, &lvFind);
}

void ListViewControl::EnableFullRowSelect()
{
	if ( getHandle() != NULL )
		SendMessage(getHandle(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
}
