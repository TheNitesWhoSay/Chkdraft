#include "MiniMap.h"
#include "Chkdraft.h"

bool MiniMap::CreateThis(HWND hParent)
{
	return ClassWindow::RegisterWindowClass( NULL, NULL, LoadCursor(NULL, IDC_ARROW),
											 CreateSolidBrush(RGB(166, 156, 132)), NULL, "MiniMap", NULL, false) &&
		   ClassWindow::CreateClassWindow(WS_EX_CLIENTEDGE, NULL, WS_VISIBLE|WS_CHILD, 6, 3, 132, 132, hParent, (HMENU)IDR_MINIMAP);
}

void MiniMap::MiniMapClick(LPARAM ClickPoints)
{
	if ( chkd.maps.curr != nullptr )
	{
		POINTS MiniClick = MAKEPOINTS(ClickPoints);

		RECT rect;
		GetClientRect(chkd.maps.curr->getHandle(), &rect);

		u16 xSize = chkd.maps.curr->XSize(),
			ySize = chkd.maps.curr->YSize();

		if ( xSize == 0 || ySize == 0 )
			return;

		HWND hMap = chkd.maps.curr->getHandle();
		RECT rcMap = { };
		GetClientRect(hMap, &rcMap);

		int screenWidth = rcMap.right-rcMap.left,
			screenHeight = rcMap.bottom-rcMap.top;

		float scale = 1.0f;

		if ( xSize >= ySize )
			if ( 128.0/xSize >= 1 )
				scale = (float)(128/xSize);
			else
				scale = 128.0f/xSize;
		else
			if ( 128.0/ySize >= 1 )
				scale = (float)(128/ySize);
			else
				scale = 128.0f/ySize;

		u16 xOffset = (u16)((128-xSize*scale)/2),
			yOffset = (u16)((128-ySize*scale)/2);

		chkd.maps.curr->display().x = (s32)((MiniClick.x-xOffset)*(32/scale)-screenWidth/2);
		chkd.maps.curr->display().y = (s32)((MiniClick.y-yOffset)*(32/scale)-screenHeight/2);

		chkd.maps.curr->Scroll(SCROLL_X|SCROLL_Y|VALIDATE_BORDER);

		RedrawWindow(chkd.maps.curr->getHandle(), NULL, NULL, RDW_INVALIDATE);
	}
}

LRESULT MiniMap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_LBUTTONDOWN:
			MiniMapClick(lParam);
			LockCursor(hWnd);
			break;

		case WM_LBUTTONUP:
			ClipCursor(NULL);
			break;

		case WM_MOUSEMOVE:
			if ( wParam == MK_LBUTTON )
				MiniMapClick(lParam);
			break;

		case WM_PAINT:
			{
				if ( chkd.maps.curr )
					chkd.maps.curr->PaintMiniMap(hWnd);
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
