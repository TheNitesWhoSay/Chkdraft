#include "TabControl.h"

u32 TabControl::GetCurSel()
{
	return u32(SendMessage(getHandle(), TCM_GETCURSEL, 0, 0));
}

bool TabControl::SetCurSel(u32 tabId)
{
	return SendMessage(getHandle(), TCM_SETCURSEL, (WPARAM)tabId, 0) != -1;
}

bool TabControl::InsertTab(u32 tabId, const char* label)
{
	TCITEM item = { };
	item.mask = TCIF_TEXT;
	item.iImage = -1;
	item.pszText = (LPSTR)label;
	item.cchTextMax = strlen(label);
	return SendMessage(getHandle(), TCM_INSERTITEM, tabId, LPARAM(&item)) != -1;
}

void TabControl::ShowTab(u32 windowId)
{
	ShowWindow(GetDlgItem(getHandle(), windowId), SW_SHOW);
}

void TabControl::HideTab(u32 windowId)
{
	ShowWindow(GetDlgItem(getHandle(), windowId), SW_HIDE);
}

void TabControl::SetImageList(HIMAGELIST hImageList)
{
	SendMessage(getHandle(), TCM_SETIMAGELIST, 0, (LPARAM)hImageList);
}

void TabControl::SetMinTabWidth(int minTabWidth)
{
	SendMessage(getHandle(), TCM_SETMINTABWIDTH, 0, minTabWidth);
}
