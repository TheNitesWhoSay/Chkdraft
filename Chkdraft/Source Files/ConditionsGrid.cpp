#include "ConditionsGrid.h"
#include <string>

// Copy -> escape tabs to \t
// Paste -> parse \t to tabs

enum ID
{
	CHECK_ENABLED_1 = ID_FIRST,
	CHECK_ENABLED_16 = (CHECK_ENABLED_1 + 15)
};

ConditionsGrid::ConditionsGrid(IConditionGridUser& user) : user(user), hasUpDownedThisEdit(false),
	startedByClick(false), ignoreChange(false)
{
	
}

bool ConditionsGrid::CreateThis(HWND hParent, int x, int y, int width, int height, u32 id)
{
	GridViewControl::CreateThis(hParent, 2, 40, 100, 100, true, id);
	GridViewControl::SetFont(defaultFont, false);
	for ( int i = 0; i<9; i++ )
		GridViewControl::AddColumn(0, " - ", 50, LVCFMT_CENTER);
	GridViewControl::AddColumn(0, "", 14, LVCFMT_CENTER);
	GridViewControl::Show();

	for ( int y = 0; y<16; y++ )
		GridViewControl::AddRow(9, (y + 1) * 100);

	CreateSubWindows(getHandle());
	return false;
}

void ConditionsGrid::SetEnabledCheck(u8 conditionId, bool enabled)
{
	if ( conditionId >= 0 && conditionId < 16 )
		checkEnabled[conditionId].SetCheck(enabled);
}

Suggestions & ConditionsGrid::GetSuggestions()
{
	return suggestions;
}

bool ConditionsGrid::HasUpDownedThisEdit()
{
	return hasUpDownedThisEdit;
}

bool ConditionsGrid::allowKeyNavDuringEdit(GVKey key)
{
	switch ( key )
	{
		case GVUp:
		case GVDown:
			return false;
			break;
		default:
			if ( startedByClick )
			{
				if ( key == GVLeft )
					return false;
				else if ( key == GVRight )
					return false;
			}
			break;
	}
	return true;
}

bool ConditionsGrid::allowSel(int x, int y)
{
	return x != 0;
}

bool ConditionsGrid::allowSel(int xStart, int xEnd, int yStart, int yEnd)
{
	return xStart != 0 || xEnd != 0;
}

void ConditionsGrid::adjustSel(int &xStart, int &xEnd, int &yStart, int &yEnd)
{
	if ( xStart == 0 )
		xStart = 1;
}

void ConditionsGrid::CellClicked(int x, int y)
{
	if ( x == 0 && y >= 0 && y < 16 )
		user.ConditionEnableToggled(y);
}

bool ConditionsGrid::CreateSubWindows(HWND hWnd)
{
	for ( int i = 0; i < 16; i++ )
	{
		POINT pt = GridViewControl::GetItemTopLeft(0, i);
		checkEnabled[i].CreateThis(getHandle(), 0, pt.y, 12, 13, true, false, "", CHECK_ENABLED_1 + i);
	}
	return true;
}

void ConditionsGrid::StartEditing(int xClick, int yClick, char initChar)
{
	if ( initChar == '\0' ) // Started via click
	{
		startedByClick = true;
		GridViewControl::EditBox().SetForwardArrowKeys(false);
		GridViewControl::EditBox().SetStopForwardOnClick(true);
	}
	else
	{
		startedByClick = false;
		GridViewControl::EditBox().SetForwardArrowKeys(true);
		GridViewControl::EditBox().SetStopForwardOnClick(true);
	}

	hasUpDownedThisEdit = false;
	return GridViewControl::StartEditing(xClick, yClick, initChar);
}

void ConditionsGrid::KeyDown(WPARAM wParam)
{
	if ( GridViewControl::isEditing() && (wParam == VK_UP || wParam == VK_DOWN) )
	{
		ignoreChange = true;

		if ( wParam == VK_UP )
			suggestions.ArrowUp();
		else if ( wParam == VK_DOWN )
			suggestions.ArrowDown();

		ignoreChange = false;
	}

	GridViewControl::KeyDown(wParam);
}

void ConditionsGrid::EditTextChanged(std::string &str)
{
	ignoreChange = true;
	suggestions.SuggestNear(str);
	ignoreChange = false;
}

BOOL ConditionsGrid::ProcessTrack(NMHEADER* nmHeader)
{
	if ( nmHeader->iItem == 0 )
		return TRUE; // Ignore first-column resizes
	else
		return FALSE;
}

BOOL ConditionsGrid::ProcessTrackW(NMHEADERW* nmHeader)
{
	if ( nmHeader->iItem == 0 )
		return TRUE; // Ignore first-column resizes
	else
		return FALSE;
}

LRESULT ConditionsGrid::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
	switch ( nmhdr->code )
	{
		case HDN_BEGINTRACK: return ProcessTrack((NMHEADER*)nmhdr); break;
		case HDN_BEGINTRACKW: return ProcessTrackW((NMHEADERW*)nmhdr); break;
	}
	return GridViewControl::Notify(hWnd, idFrom, nmhdr);
}

LRESULT ConditionsGrid::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( HIWORD(wParam) )
	{
		case EN_CHANGE:
			if ( !ignoreChange )
			{
				std::string str;
				GridViewControl::GetEditText(str);
				ignoreChange = true;
				suggestions.SuggestNear(str);
				ignoreChange = false;
			}
			break;
		case BN_CLICKED:
			if ( LOWORD(wParam) >= CHECK_ENABLED_1 && LOWORD(wParam) <= CHECK_ENABLED_16 )
			{
				u8 conditionNum = u8(LOWORD(wParam) - CHECK_ENABLED_1);
				if ( conditionNum >= 0 && conditionNum < 16 )
					user.ConditionEnableToggled(conditionNum);
			}
			break;
		default:
			return GridViewControl::Command(hWnd, wParam, lParam);
			break;
	}
	return GridViewControl::Command(hWnd, wParam, lParam);
}

LRESULT ConditionsGrid::ControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_KEYUP:
			if ( wParam == VK_UP )
				suggestions.ArrowUp();
			else if ( wParam == VK_DOWN )
				suggestions.ArrowDown();
			break;

		case WM_KEYDOWN:
			if ( GetKeyState(VK_CONTROL) & 0x8000 )
			{
				switch ( wParam )
				{
					case 'c': case 'C': user.CopySelection(); break;
					case 'p': case 'P': user.Paste(); break;
					case 'x': case 'X': user.CutSelection(); break;
					default: return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
				}
			}
			else
				return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
			break;

		default: return GridViewControl::ControlProc(hWnd, msg, wParam, lParam); break;
	}
	return 0;
}
