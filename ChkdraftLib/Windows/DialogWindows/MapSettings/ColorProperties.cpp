#include "ColorProperties.h"
#include "../../../CommonFiles/CommonFiles.h"

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
    if ( ChooseColorW(&chooseColor) != 0 )
        return std::optional<Chk::Rgb>({GetRValue(chooseColor.rgbResult), GetGValue(chooseColor.rgbResult), GetBValue(chooseColor.rgbResult)});

    return std::nullopt;
}
