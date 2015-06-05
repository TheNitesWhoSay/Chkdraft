#include "TrigConditions.h"
#include "Graphics.h"

#define TOP_CONDITION_PADDING 50
#define BOTTOM_CONDITION_PADDING 0

enum ID
{
	LIST_CONDITIONS = ID_FIRST
};

bool TrigConditionsWindow::CreateThis(HWND hParent, u32 windowId)
{
	if ( getHandle() != NULL )
		return SetParent(hParent);

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "TrigConditions", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "TrigConditions", WS_CHILD,
			5, 62, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)windowId) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TrigConditionsWindow::DestroyThis()
{
	return false;
}

void TrigConditionsWindow::RefreshWindow()
{

}

void TrigConditionsWindow::DoSize()
{
	listConditions.SetPos(0, TOP_CONDITION_PADDING, cliWidth()-2, cliHeight()-TOP_CONDITION_PADDING-BOTTOM_CONDITION_PADDING);
}

void TrigConditionsWindow::CreateSubWindows(HWND hWnd)
{
	TextControl text;
	listConditions.CreateThis(hWnd, 0, 40, 100, 100, true, LIST_CONDITIONS);
	for ( int i=0; i<9; i++ )
		listConditions.AddColumn(0, " - ", 50, LVCFMT_CENTER);

	listConditions.Show();

	for ( int y=0; y<9; y++ )
	{
		listConditions.AddRow(9, (y+1)*100);
		for ( int x=0; x<9; x++ )
		{
			stringstream text;
			text << x << ", " << y;
			listConditions.item(x, y).SetText(text.str().c_str());
		}
	}
}

LRESULT TrigConditionsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			break;

		case WM_NOTIFY:
			{
				NMHDR* hdr = (NMHDR*)lParam;
				if ( hdr->idFrom == LIST_CONDITIONS )
				{
					switch ( hdr->code )
					{
						case LVN_ODSTATECHANGED:
							{
								NMLVODSTATECHANGE* changeInfo = (NMLVODSTATECHANGE*)lParam;
							}
							break;
					}
				}
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		case WM_MEASUREITEM:
			if ( wParam == LIST_CONDITIONS )
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				mis->itemWidth = 120;
				mis->itemHeight = 15;
				return TRUE;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_DRAWITEM:
			if ( wParam == LIST_CONDITIONS )
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
				bool isSelected = ((pdis->itemState&ODS_SELECTED) == ODS_SELECTED),
					 drawSelection = ((pdis->itemAction&ODA_SELECT) == ODA_SELECT),
					 drawEntire = ((pdis->itemAction&ODA_DRAWENTIRE) == ODA_DRAWENTIRE);

				if ( pdis->itemID != -1 && ( drawSelection || drawEntire ) )
				{
					RECT &rcItem = pdis->rcItem;
					std::string text;
					int numColumns = listConditions.GetNumColumns();

					RECT rcFill;
					
					int itemStart = rcItem.left;
					for ( int i=0; i<numColumns; i++ )
					{
						int width = ListView_GetColumnWidth(listConditions.getHandle(), i);
						rcFill.top = rcItem.top;
						rcFill.bottom = rcItem.bottom;
						rcFill.left = itemStart;
						rcFill.right = itemStart+width;

						HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
						FrameRect(pdis->hDC, &rcFill, hBlack);
						DeleteBrush(hBlack);

						rcFill.bottom -= 1;
						rcFill.right -= 1;

						if ( listConditions.isFocused(i, pdis->itemID) )
							FillRect(pdis->hDC, &rcFill, GetSysColorBrush(COLOR_ACTIVEBORDER));
						else if ( listConditions.item(i, pdis->itemID).isSelected() )
							FillRect(pdis->hDC, &rcFill, GetSysColorBrush(COLOR_HIGHLIGHT));
						else
							FillRect(pdis->hDC, &rcFill, GetSysColorBrush(COLOR_WINDOW));

						listConditions.item(i, pdis->itemID).getText(text);
						//DrawString(pdis->hDC, itemStart+1, rcFill.top, width-2, RGB(255, 0, 0), text);
						itemStart += width;
					}
				}
				return TRUE;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
