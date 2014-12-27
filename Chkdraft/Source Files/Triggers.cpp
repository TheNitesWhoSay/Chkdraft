#include "Triggers.h"
#include "Chkdraft.h"

#define TRIGGER_TOP_PADDING 1
#define TRIGGER_LEFT_PADDING 1
#define TRIGGER_RIGHT_PADDING 1
#define TRIGGER_BOTTOM_PADDING 1

#define STRING_TOP_PADDING 0
#define STRING_LEFT_PADDING 2
#define STRING_RIGHT_PADDING 1
#define STRING_BOTTOM_PADDING 2

#define NO_TRIGGER (u32(-1))

#define BUTTON_NEW 41001
#define BUTTON_MODIFY 41002
#define BUTTON_DELETE 41003
#define BUTTON_COPY 41004
#define BUTTON_MOVEUP 41005
#define BUTTON_MOVEDOWN 41006
#define BUTTON_MOVETO 41007
#define LIST_GROUPS 41008
#define ID_LB_TRIGGERS 41009

TriggersWindow::TriggersWindow() : currTrigger(NO_TRIGGER), currGroups(0)
{

}

bool TriggersWindow::CreateThis(HWND hParent)
{
	if ( getHandle() != NULL )
		return SetParent(getHandle(), hParent) != NULL;

	RECT rcCli;
	if ( GetWindowRect(hParent, &rcCli) == TRUE &&
		 ClassWindow::RegisterWindowClass(NULL, NULL, NULL, NULL, NULL, "Triggers", NULL, false) &&
		 ClassWindow::CreateClassWindow(NULL, "Triggers", WS_VISIBLE|WS_CHILD,
			5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
			hParent, (HMENU)ID_TRIGGERS) )
	{
		CreateSubWindows(getHandle());
		return true;
	}
	else
		return false;
}

bool TriggersWindow::DestroyThis()
{
	return false;
}

void TriggersWindow::RefreshWindow()
{
	HWND hTriggerList = GetDlgItem(getHandle(), ID_LB_TRIGGERS);
	Scenario* chk = chkd.maps.curr;
	Trigger* trigger;
	if ( hTriggerList != NULL && chk != nullptr && chk->TRIG().exists() )
	{
		SendMessage(hTriggerList, LB_RESETCONTENT, NULL, NULL);
		for ( u32 i=0; i<chk->numTriggers(); i++ )
		{
			if ( chk->getTrigger(trigger, i) && ShowTrigger(trigger) )
				SendMessage(hTriggerList, LB_ADDSTRING, NULL, (LPARAM)i);
		}
	}
}

void TriggersWindow::DoSize()
{
	RECT rcCli;
	if ( GetClientRect(getHandle(), &rcCli) )
	{
		long left = rcCli.left,
			 top = rcCli.top,
			 right = rcCli.right,
			 bottom = rcCli.bottom,
			 width = right - left,
			 height = bottom - top,

			 leftPadding = 5,
			 topPadding = 5,
			 rightPadding = 5,
			 bottomPadding = 5,
			 buttonPadding = 2,
			 listPadding = 4,

			 buttonX = right-buttonNew.Width()-rightPadding,
			 leftWidth = buttonX-buttonPadding-leftPadding,
			 listGroupsBottom = topPadding+listGroups.Height(),

			 moveButtonsHeight = buttonMoveUp.Height()+buttonMoveDown.Height()+buttonMoveTo.Height()+buttonPadding*3+bottomPadding,
			 buttonsHeight = buttonNew.Height()+buttonModify.Height()+buttonDelete.Height()+buttonCopy.Height()+buttonPadding*4+
							 moveButtonsHeight,

			 triggerListHeight = height-listGroupsBottom-bottomPadding-listPadding;

		if ( buttonsHeight+listGroupsBottom > height )
		{
			listGroups.SetPos(leftPadding, topPadding, leftWidth, listGroups.Height());
			listTriggers.SetPos(leftPadding, 117, listGroups.Width(), triggerListHeight);
			buttonNew.MoveTo(buttonX, topPadding);
		}
		else
		{
			buttonNew.MoveTo(buttonX, listGroups.Bottom()+buttonPadding);
			listGroups.SetPos(leftPadding, topPadding, buttonNew.Right()-leftPadding, listGroups.Height());
			listTriggers.SetPos(leftPadding, 117, buttonNew.Left()-buttonPadding-leftPadding, triggerListHeight);
		}

		buttonModify.MoveTo(buttonX, buttonNew.Bottom()+buttonPadding );
		buttonDelete.MoveTo(buttonX, buttonModify.Bottom()+buttonPadding );
		buttonCopy.MoveTo(buttonX, buttonDelete.Bottom()+buttonPadding );
		
		long remainingButtonSpace = height-buttonCopy.Bottom();

		if ( moveButtonsHeight > remainingButtonSpace )
		{
			buttonMoveUp.MoveTo(buttonX, buttonCopy.Bottom()+buttonPadding);
			buttonMoveDown.MoveTo(buttonX, buttonMoveUp.Bottom()+buttonPadding);
			buttonMoveTo.MoveTo(buttonX, buttonMoveDown.Bottom()+buttonPadding);
		}
		else
		{
			buttonMoveTo.MoveTo(buttonX, bottom-bottomPadding-buttonMoveTo.Height());
			buttonMoveDown.MoveTo(buttonX, buttonMoveTo.Top()-buttonPadding-buttonMoveDown.Height());
			buttonMoveUp.MoveTo(buttonX, buttonMoveDown.Top()-buttonPadding-buttonMoveUp.Height());
		}
	}
}

void TriggersWindow::CreateSubWindows(HWND hWnd)
{
	buttonNew.CreateThis(hWnd, 0, 0, 75, 23, "New", BUTTON_NEW);
	buttonModify.CreateThis(hWnd, 0, 25, 75, 23, "Modify", BUTTON_MODIFY);
	buttonDelete.CreateThis(hWnd, 0, 50, 75, 23, "Delete", BUTTON_DELETE);
	buttonCopy.CreateThis(hWnd, 0, 75, 75, 23, "Copy", BUTTON_COPY);
	
	buttonMoveUp.CreateThis(hWnd, 0, 100, 75, 23, "Move Up", BUTTON_MOVEUP);
	buttonMoveDown.CreateThis(hWnd, 0, 125, 75, 23, "Move Down", BUTTON_MOVEDOWN);
	buttonMoveTo.CreateThis(hWnd, 0, 150, 75, 23, "Move To", BUTTON_MOVETO);

	listGroups.CreateThis(hWnd, 0, 0, 200, 116, false, true, LIST_GROUPS);
	listTriggers.CreateThis(hWnd, 0, 120, 200, 150, true, false, ID_LB_TRIGGERS);

	for ( int i=1; i<=8; i++ )
	{
		stringstream ssPlayer;
		ssPlayer << "Player " << i;
		listGroups.AddString(ssPlayer.str().c_str());
	}

	listGroups.AddString("Force 1");
	listGroups.AddString("Force 2");
	listGroups.AddString("Force 3");
	listGroups.AddString("Force 4");
	listGroups.AddString("All Players");
}

bool TriggersWindow::ShowTrigger(Trigger* trigger)
{
	return true;
}

string TriggersWindow::GetTriggerString(u32 trigNum)
{
	stringstream ssTrigger;
	ssTrigger << "#" << trigNum;
	return ssTrigger.str();
}

bool TriggersWindow::GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, Scenario* chk, u32 triggerNum, Trigger* trigger)
{
	string str;
	if ( chk->getActiveComment(trigger, str) )
	{
		if ( GetStringDrawSize(hDC, width, height, str) )
		{
			width += TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
			height += TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;
		}
	}
	else
	{
		str = GetTriggerString(triggerNum);
		HGDIOBJ sel = SelectObject(hDC, defaultFont);
		if ( sel != NULL && sel != HGDI_ERROR )
		{
			width = 0;
			height = TRIGGER_TOP_PADDING+TRIGGER_BOTTOM_PADDING+STRING_TOP_PADDING+STRING_BOTTOM_PADDING;

			UINT strWidth, strHeight;
			if ( GetStringDrawSize(hDC, strWidth, strHeight, str) )
			{
				UINT newWidth = strWidth+TRIGGER_LEFT_PADDING+TRIGGER_RIGHT_PADDING+STRING_LEFT_PADDING+STRING_RIGHT_PADDING;
				if ( newWidth > width )
					width = newWidth;

				height += strHeight;
				return true;
			}
		}
	}
	return false;
}

void TriggersWindow::DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, Scenario* chk, u32 triggerNum, Trigger* trigger)
{
	HBRUSH hBackground = NULL;
	if ( isSelected )
		hBackground = CreateSolidBrush(RGB(51, 153, 255));
	else
		hBackground = CreateSolidBrush(RGB(253, 246, 208));

	if ( hBackground != NULL )
	{
		RECT rcNote;
		rcNote.left = rcItem.left+TRIGGER_LEFT_PADDING;
		rcNote.top = rcItem.top+TRIGGER_TOP_PADDING;
		rcNote.right = rcItem.right-TRIGGER_RIGHT_PADDING;
		rcNote.bottom = rcItem.bottom-TRIGGER_BOTTOM_PADDING;

		FillRect(hDC, &rcNote, hBackground);
		if ( isSelected )
			DrawFocusRect(hDC, &rcNote);

		DeleteObject(hBackground);
	}


	string str;
	if ( chk->getActiveComment(trigger, str) )
	{
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(0, 0, 0));
		DrawString(hDC, rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING, str);
	}
	else
	{
		str = GetTriggerString(triggerNum);
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(0, 0, 0));
		DrawString(hDC, rcItem.left+TRIGGER_LEFT_PADDING+STRING_LEFT_PADDING, rcItem.top+TRIGGER_TOP_PADDING+STRING_TOP_PADDING, str);
	}
}

LRESULT TriggersWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				RefreshWindow();
			else if ( wParam == FALSE )
			{
				// Perhaps update details of the current trigger
			}
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

		case WM_CTLCOLORLISTBOX:
			if ( listTriggers.getHandle() == (HWND)lParam )
			{
				HBRUSH hBrush = CreateSolidBrush(RGB(171, 171, 171));
				if ( hBrush != NULL )
					return (LPARAM)hBrush;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case LBN_SELCHANGE:
					if ( LOWORD(wParam) == ID_LB_TRIGGERS ) // Change selection, update info boxes and so fourth
					{
						
					}
					else if ( LOWORD(wParam) == LIST_GROUPS )
					{
						int numSel = listGroups.GetNumSel();
						for ( int i=0; i<numSel; i++ )
						{
							string selString;
							if ( listGroups.GetSelString(i, selString) )
								cout << selString << ", ";
							else
								cout << "ERROR" << ", ";
						}
						cout << endl;
					}
				case LBN_KILLFOCUS: // List box item may have lost focus, check if anything should be updated
					if ( LOWORD(wParam) == ID_LB_TRIGGERS )
					{
					
					}
					else if ( LOWORD(wParam) == LIST_GROUPS )
					{

					}
				case BN_CLICKED:
					if ( LOWORD(wParam) == BUTTON_NEW )
						mb("new");
					else if ( LOWORD(wParam) == BUTTON_MODIFY )
						mb("modify");
					else if ( LOWORD(wParam) == BUTTON_DELETE )
						mb("delete");
					else if ( LOWORD(wParam) == BUTTON_COPY )
						mb("copy");
					else if ( LOWORD(wParam) == BUTTON_MOVEUP )
						mb("move up");
					else if ( LOWORD(wParam) == BUTTON_MOVEDOWN )
						mb("move down");
					else if ( LOWORD(wParam) == BUTTON_MOVETO )
						mb("move to");
					break;
			}
			break;

		case WM_MEASUREITEM:
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				HWND hTriggerList = GetDlgItem(hWnd, ID_LB_TRIGGERS);
				Trigger* trigger;
				u32 listIndex = (u32)mis->itemData;
				u32 triggerNum = listIndex; // Trigger number needs be determined by listIndex and selected players

				if ( hTriggerList != NULL && chkd.maps.curr->getTrigger(trigger, triggerNum) )
				{
					HDC hDC = GetDC(hTriggerList);
					if ( hDC != NULL )
					{
						if ( GetTriggerDrawSize(hDC, mis->itemWidth, mis->itemHeight, chkd.maps.curr, triggerNum, trigger) )
						{
							ReleaseDC(hTriggerList, hDC);
							return TRUE;
						}
						ReleaseDC(hTriggerList, hDC);
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
					Trigger* trigger;
					u32 listIndex = (u32)pdis->itemData;
					u32 triggerNum = listIndex; // Trigger number needs be determined by listIndex and selected players
					
					if ( chkd.maps.curr != nullptr && chkd.maps.curr->getTrigger(trigger, triggerNum) )
						DrawTrigger(pdis->hDC, pdis->rcItem, (pdis->itemState&ODS_SELECTED), chkd.maps.curr, triggerNum, trigger);
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
