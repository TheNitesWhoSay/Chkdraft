#ifndef COLORPROPERTIES_H
#define COLORPROPERTIES_H
#include <mapping_core/mapping_core.h>
#include <windows/windows_ui.h>

class ColorPropertiesDialog
{
public:
    static std::optional<Chk::Rgb> GetCrgbColor(HWND hParent, Chk::Rgb previousColor);

private:
    static COLORREF customColors[16];
};

#endif