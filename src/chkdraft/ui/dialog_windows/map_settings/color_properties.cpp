#include "color_properties.h"
#include <common_files/common_files.h>

COLORREF ColorPropertiesDialog::customColors[16]{};

std::optional<Chk::Rgb> ColorPropertiesDialog::GetCrgbColor(HWND hParent, Chk::Rgb previousColor)
{
    CHOOSECOLOR chooseColor {};
    chooseColor.lStructSize = sizeof(CHOOSECOLOR);
    chooseColor.hwndOwner = hParent;
    chooseColor.hInstance = NULL;
    chooseColor.rgbResult = RGB(previousColor.red, previousColor.green, previousColor.blue);
    chooseColor.lpCustColors = &customColors[0];
    chooseColor.Flags = CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;
    chooseColor.lCustData = NULL;
    chooseColor.lpfnHook = NULL;
    chooseColor.lpTemplateName = NULL;
    if ( ChooseColor(&chooseColor) != 0 )
        return std::optional<Chk::Rgb>({GetRValue(chooseColor.rgbResult), GetGValue(chooseColor.rgbResult), GetBValue(chooseColor.rgbResult)});

    return std::nullopt;
}

std::optional<u32> ColorPropertiesDialog::GetArgbColor(HWND hParent, u32 previousColor)
{
    CHOOSECOLOR chooseColor {};
    chooseColor.lStructSize = sizeof(CHOOSECOLOR);
    chooseColor.hwndOwner = hParent;
    chooseColor.hInstance = NULL;
    chooseColor.rgbResult = previousColor;
    chooseColor.lpCustColors = &customColors[0];
    chooseColor.Flags = CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;
    chooseColor.lCustData = NULL;
    chooseColor.lpfnHook = NULL;
    chooseColor.lpTemplateName = NULL;
    if ( ChooseColor(&chooseColor) != 0 )
        return std::optional<u32>(chooseColor.rgbResult);

    return std::nullopt;
}
