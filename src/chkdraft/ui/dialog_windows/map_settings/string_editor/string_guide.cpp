#include "string_guide.h"

enum_t(Id, u32, {
    TEXT_COLOR_FIRST = ID_FIRST,
    TEXT_COLOR_LAST = (TEXT_COLOR_FIRST+27)
});



const std::vector<std::tuple<COLORREF, std::string, std::string>> strColors =
{
    std::make_tuple(RGB(184, 184, 232), " - Use Default",           "<01><1>"),
    std::make_tuple(RGB(184, 184, 232), " - Cyan",                  "<02><2>"),
    std::make_tuple(RGB(220, 220, 60),  " - Yellow",                "<03><3>"),
    std::make_tuple(RGB(255, 255, 255), " - White",                 "<04><4>"),
    std::make_tuple(RGB(132, 116, 116), " - Grey",                  "<05><5>"),
    std::make_tuple(RGB(200, 24, 24),   " - Red",                   "<06><6>"),
    std::make_tuple(RGB(16, 252, 24),   " - Green",                 "<07><7>"),
    std::make_tuple(RGB(244, 4, 4),     " - Red (p1)",              "<08><8>"),
    std::make_tuple(RGB(16, 252,  24),  " - Invisible",             "<0B>"),
    std::make_tuple(RGB(16, 252,  24),  " - Remove beyond",         "<0C>"),
    std::make_tuple(RGB(12,  72, 204),  " - Blue (p2)",             "<0E>"),
    std::make_tuple(RGB(44, 180, 148),  " - Teal (p3)",             "<0F>"),
    std::make_tuple(RGB(132,  64, 156), " - Purple (p4)",           "<10>"),
    std::make_tuple(RGB(248, 140,  20), " - Orange (p5)",           "<11>"),
    std::make_tuple(RGB(16, 252,  24),  " - Right Align",           "<12><R>"),
    std::make_tuple(RGB(16, 252,  24),  " - Center Align",          "<13><C>"),
    std::make_tuple(RGB(16, 252,  24),  " - Invisible",             "<14>"),
    std::make_tuple(RGB(112,  48,  20), " - Brown (p6)",            "<15>"),
    std::make_tuple(RGB(204, 224, 208), " - White (p7)",            "<16>"),
    std::make_tuple(RGB(252, 252,  56), " - Yellow (p8)",           "<17>"),
    std::make_tuple(RGB(8, 128,   8),   " - Green (p9)",            "<18>"),
    std::make_tuple(RGB(252, 252, 124), " - Bright Yellow (p10)",   "<19>"),
    std::make_tuple(RGB(184, 184, 232), " - Cyan",                  "<1A>"),
    std::make_tuple(RGB(236, 196, 176), " - Pinkish (p11)",         "<1B>"),
    std::make_tuple(RGB(64, 104, 212),  " - Dark Cyan (p12)",       "<1C>"),
    std::make_tuple(RGB(116, 164, 124), " - Greygreen",             "<1D>"),
    std::make_tuple(RGB(144, 144, 184), " - Bluegrey",              "<1E>"),
    std::make_tuple(RGB(0, 228, 252),   " - Turquoise",             "<1F>")
};

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
