#include "string_guide.h"

enum_t(Id, u32, {
    TEXT_COLOR_FIRST = ID_FIRST,
    TEXT_COLOR_LAST = (TEXT_COLOR_FIRST+27)
});

StringGuideWindow::~StringGuideWindow()
{

}

bool StringGuideWindow::CreateThis(HWND hParent)
{
    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, WinLib::ResourceManager::getSolidBrush(RGB(0, 0, 0)), NULL, "StringGuide", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "", WS_OVERLAPPED|WS_CHILD|WS_VISIBLE, 463, 7, 125, 365, hParent, NULL) )
    {
        auto dc = this->getDeviceContext();
        dc.setDefaultFont();
        for ( size_t i=0; i<strColors.size(); ++i )
        {
            auto color = strColors.at(i);
            if ( auto size = dc.getTextExtentPoint32(icux::toUistring(std::get<2>(color)).c_str()) )
            {
                colorPrefix[i].CreateThis(this->getHandle(), 2, (s32)i*13, size->cx, 13, std::get<2>(color), 0);
                this->color[i].CreateThis(this->getHandle(), size->cx+3, (s32)i*13, 100, 13, std::get<1>(color), Id::TEXT_COLOR_FIRST+i);
            }
        }
        return true;
    }
    else
        return false;
}

bool StringGuideWindow::DestroyThis()
{
    ClassWindow::DestroyThis();
    return true;
}

LRESULT StringGuideWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_CTLCOLORSTATIC:
            {
                SetBkMode((HDC)wParam, TRANSPARENT);
                int id = GetDlgCtrlID((HWND)lParam);
                if ( id >= Id::TEXT_COLOR_FIRST && id <= Id::TEXT_COLOR_LAST )
                    SetTextColor((HDC)wParam, std::get<0>(strColors.at(id-Id::TEXT_COLOR_FIRST)));
                else
                    SetTextColor((HDC)wParam, RGB(16, 252, 24));

                return (LRESULT)WinLib::ResourceManager::getSolidBrush(RGB(0, 0, 0));
            }
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
