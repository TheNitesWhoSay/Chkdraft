#ifndef COLORPROPERTIES_H
#define COLORPROPERTIES_H
#include "../../../../MappingCoreLib/MappingCore.h"
#include "../../../../WindowsLib/WindowsUi.h"

class ColorPropertiesDialog
{
public:
    static std::optional<Chk::Rgb> GetCrgbColor(HWND hParent, Chk::Rgb previousColor);

private:
    static COLORREF customColors[16];
};

#endif