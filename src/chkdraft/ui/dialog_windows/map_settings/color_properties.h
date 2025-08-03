#ifndef COLORPROPERTIES_H
#define COLORPROPERTIES_H
#include <mapping_core/mapping_core.h>
#include <windows/windows_ui.h>

class ColorPropertiesDialog
{
public:
    static std::optional<Chk::Rgb> GetCrgbColor(HWND hParent, Chk::Rgb previousColor);
    static std::optional<u32> GetArgbColor(HWND hParent, u32 previousColor);

private:
    static COLORREF customColors[16];
};

#endif