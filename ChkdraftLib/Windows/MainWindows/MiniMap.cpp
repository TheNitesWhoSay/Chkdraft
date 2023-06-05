#include "MiniMap.h"
#include "../../Chkdraft.h"

MiniMap::~MiniMap()
{

}

bool MiniMap::CreateThis(HWND hParent, u64 id)
{
    return ClassWindow::RegisterWindowClass( 0, NULL, LoadCursor(NULL, IDC_ARROW),
                                             CreateSolidBrush(RGB(166, 156, 132)), NULL, "MiniMap", NULL, false) &&
           ClassWindow::CreateClassWindow(WS_EX_CLIENTEDGE, "", WS_VISIBLE|WS_CHILD, 6, 3, 132, 132, hParent, (HMENU)id);
}

void MiniMap::MiniMapClick(LPARAM ClickPoints)
{
    if ( CM != nullptr )
    {
        POINTS MiniClick = MAKEPOINTS(ClickPoints);

        RECT rect;
        GetClientRect(CM->getHandle(), &rect);

        u16 xSize = (u16)CM->getTileWidth(),
            ySize = (u16)CM->getTileHeight();

        if ( xSize == 0 || ySize == 0 )
            return;

        HWND hMap = CM->getHandle();
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

        CM->SetScreenLeft((s32)((MiniClick.x-xOffset)*(32/scale)-screenWidth/2));
        CM->SetScreenTop((s32)((MiniClick.y-yOffset)*(32/scale)-screenHeight/2));
        CM->Scroll(true, true, true);
        RedrawWindow(CM->getHandle(), NULL, NULL, RDW_INVALIDATE);
    }
}

LRESULT MiniMap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_LBUTTONDOWN:
            MiniMapClick(lParam);
            LockCursor();
            break;

        case WM_LBUTTONUP:
            ClipCursor(NULL);
            break;

        case WM_MOUSEMOVE:
            if ( wParam == MK_LBUTTON )
                MiniMapClick(lParam);
            break;

        case WM_PAINT:
            if ( WindowsItem::StartBufferedPaint() )
            {
                CM->PaintMiniMap(WindowsItem::GetPaintDc(), WindowsItem::PaintWidth(), WindowsItem::PaintHeight());
                WindowsItem::EndPaint();
            }
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
