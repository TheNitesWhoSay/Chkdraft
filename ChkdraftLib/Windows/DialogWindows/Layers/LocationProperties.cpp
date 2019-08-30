#include "LocationProperties.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Undos/ChkdUndos/LocationChange.h"
#include <exception>
#include <string>

enum class Id
{
    EditLocationName = IDC_LOCATION_NAME,
    EditLocationLeft = IDC_LOCLEFT,
    EditLocationTop = IDC_LOCTOP,
    EditLocationRight = IDC_LOCRIGHT,
    EditLocationBottom = IDC_LOCBOTTOM,
    EditRawFlags = IDC_RAWFLAGS,

    CheckUseExtended = IDC_EXTLOCNAMESTR,
    CheckLowGround = IDC_LOWGROUND,
    CheckMedGround = IDC_MEDGROUND,
    CheckHighGround = IDC_HIGHGROUND,
    CheckLowAir = IDC_LOWAIR,
    CheckMedAir = IDC_MEDAIR,
    CheckHighAir = IDC_HIGHAIR,

    ButtonOk = IDOK,
    ButtonInvertX = IDC_INVERTX,
    ButtonInvertY = IDC_INVERTY,
    ButtonInvertXY = IDC_INVERTXY,
};

LocationWindow::LocationWindow() : refreshing(true), preservedStat(0), currentLocationIndex(0)
{

}

LocationWindow::~LocationWindow()
{

}

bool LocationWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_LOCPROP), hParent) )
    {
        editLocName.FindThis(getHandle(), (u32)Id::EditLocationName);
        editLocLeft.FindThis(getHandle(), (u32)Id::EditLocationLeft);
        editLocTop.FindThis(getHandle(), (u32)Id::EditLocationTop);
        editLocRight.FindThis(getHandle(), (u32)Id::EditLocationRight);
        editLocBottom.FindThis(getHandle(), (u32)Id::EditLocationBottom);
        editRawFlags.FindThis(getHandle(), (u32)Id::EditRawFlags);

        checkUseExtended.FindThis(getHandle(), (u32)Id::CheckUseExtended);
        checkLowGround.FindThis(getHandle(), (u32)Id::CheckLowGround);
        checkMedGround.FindThis(getHandle(), (u32)Id::CheckMedGround);
        checkHighGround.FindThis(getHandle(), (u32)Id::CheckHighGround);
        checkLowAir.FindThis(getHandle(), (u32)Id::CheckLowAir);
        checkMedAir.FindThis(getHandle(), (u32)Id::CheckMedAir);
        checkHighAir.FindThis(getHandle(), (u32)Id::CheckHighAir);

        buttonInvertX.FindThis(getHandle(), (u32)Id::ButtonInvertX);
        buttonInvertY.FindThis(getHandle(), (u32)Id::ButtonInvertY);
        buttonInvertXY.FindThis(getHandle(), (u32)Id::ButtonInvertXY);

        RefreshLocationInfo();

        return true;
    }
    else
        return false;
}

bool LocationWindow::DestroyThis()
{
    if ( ClassDialog::DestroyDialog() )
    {
        currentLocationIndex = NO_LOCATION;
        refreshing = true;
        return true;
    }
    else
        return false;
}

void LocationWindow::RefreshLocationElevationFlags()
{
    bool prevRefreshingValue = refreshing;
    refreshing = true;

    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        checkLowGround.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::LowElevation) == 0);
        checkMedGround.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::MediumElevation) == 0);
        checkHighGround.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::HighElevation) == 0);
        checkLowAir.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::LowAir) == 0);
        checkMedAir.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::MediumAir) == 0);
        checkHighAir.SetCheck(((u16)locRef->elevationFlags & (u16)Chk::Location::Elevation::HighAir) == 0);
    }

    refreshing = prevRefreshingValue;
}

void LocationWindow::RefreshLocationInfo()
{
    refreshing = true;

    if ( CM == nullptr )
        DestroyThis();

    currentLocationIndex = CM->GetSelectedLocation();
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        editLocLeft.SetText(std::to_string(locRef->left));
        editLocTop.SetText(std::to_string(locRef->top));
        editLocRight.SetText(std::to_string(locRef->right));
        editLocBottom.SetText(std::to_string(locRef->bottom));

        char text[20];
        _itoa_s(locRef->elevationFlags, text, 20, 2);
        size_t len = std::strlen(text);
        if ( len > 0 && len < 16 )
        {
            std::memmove(&text[16-len], text, len);
            std::memset(text, '0', 16-len);
            text[16] = '\0';
        }
        editRawFlags.SetText(text);

        RefreshLocationElevationFlags();
        checkUseExtended.SetCheck(CM->isExtendedString(locRef->stringId));

        ChkdString locName;
        if ( CM->GetString(locName, locRef->stringId) )
            editLocName.SetText(locName);
        else
            editLocName.SetText("ERROR");
    }
    else
        DestroyThis();
    
    refreshing = false;
}

void LocationWindow::InvertXc()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Left, locRef->left));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Right, locRef->right));
        CM->AddUndo(locationChanges);
        std::swap(locRef->left, locRef->right);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertYc()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Top, locRef->top));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Bottom, locRef->bottom));
        CM->AddUndo(locationChanges);
        std::swap(locRef->top, locRef->bottom);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertXY()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Left, locRef->left));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Right, locRef->right));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Top, locRef->top));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Bottom, locRef->bottom));
        CM->AddUndo(locationChanges);
        std::swap(locRef->left, locRef->right);
        std::swap(locRef->top, locRef->bottom);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::NotifyLowGroundClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkLowGround.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::LowElevation);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::LowElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedGroundClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkMedGround.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::MediumElevation);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::MediumElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighGroundClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkHighGround.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::HighElevation);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::HighElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLowAirClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkLowAir.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::LowAir);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::LowAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedAirClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkMedAir.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::MediumAir);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::MediumAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighAirClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, locRef->elevationFlags));

        if ( checkHighAir.isChecked() )
            locRef->elevationFlags &= (~(u16)Chk::Location::Elevation::HighAir);
        else
            locRef->elevationFlags |= (u16)Chk::Location::Elevation::HighAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyUseExtendedStringClicked()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        ChkdString str;
        if ( CM->GetString(str, locRef->stringId) )
        {
            u32 newStrNum = 0;
            if ( CM->addString(str, newStrNum, checkUseExtended.isChecked()) )
            {
                u32 oldStrNum = locRef->stringId;
                locRef->stringId = u16(newStrNum);
                CM->removeUnusedString(oldStrNum);
                CM->refreshScenario();
                CM->notifyChange(false);
                RefreshLocationInfo();
            }
        }
    }
}

void LocationWindow::RawFlagsUpdated()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    u16 newVal = 0;
    if ( locRef != nullptr &&
        editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
    {
        locRef->elevationFlags = newVal;
        RefreshLocationElevationFlags();
    }
}

void LocationWindow::LocationLeftUpdated()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    int newVal = 0;
    if ( locRef != nullptr && editLocLeft.GetEditNum<int>(newVal) )
    {
        locRef->left = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationTopUpdated()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    int newVal = 0;
    if ( locRef != nullptr && editLocTop.GetEditNum<int>(newVal) )
    {
        locRef->top = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationRightUpdated()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    int newVal = 0;
    if ( locRef != nullptr && editLocRight.GetEditNum<int>(newVal) )
    {
        locRef->right = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationBottomUpdated()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    int newVal = 0;
    if ( locRef != nullptr && editLocBottom.GetEditNum<int>(newVal) )
    {
        locRef->bottom = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationNameFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        ChkdString locationName;
        if ( editLocName.GetWinText(locationName) )
        {
            bool isExtended = checkUseExtended.isChecked();
            if ( CM->replaceString<u16>(locationName, locRef->stringId, isExtended, true) )
            {
                CM->notifyChange(false);
                CM->refreshScenario();
            }
        }
    }
}

void LocationWindow::RawFlagsFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        u16 newVal = 0;
        if ( editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
        {
            locRef->elevationFlags = newVal;
            CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::ElevationFlags, preservedStat));
            RefreshLocationInfo();
        }
    }
}

void LocationWindow::LocationLeftFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        int newVal = 0;
        if ( editLocLeft.GetEditNum<int>(newVal) )
        {
            locRef->left = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Left, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationTopFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        int newVal = 0;
        if ( editLocTop.GetEditNum<int>(newVal) )
        {
            locRef->top = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Top, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationRightFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        int newVal = 0;
        if ( editLocRight.GetEditNum<int>(newVal) )
        {
            locRef->right = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Right, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationBottomFocusLost()
{
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        int newVal = 0;
        if ( editLocBottom.GetEditNum<int>(newVal) )
        {
            locRef->bottom = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, Chk::Location::Field::Bottom, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( (Id)idFrom )
    {
        case Id::ButtonOk: DestroyThis(); break;
        case Id::ButtonInvertX: InvertXc(); break;
        case Id::ButtonInvertY: InvertYc(); break;
        case Id::ButtonInvertXY: InvertXY(); break;
        case Id::CheckLowGround: NotifyLowGroundClicked(); break;
        case Id::CheckMedGround: NotifyMedGroundClicked(); break;
        case Id::CheckHighGround: NotifyHighGroundClicked(); break;
        case Id::CheckLowAir: NotifyLowAirClicked(); break;
        case Id::CheckMedAir: NotifyMedAirClicked(); break;
        case Id::CheckHighAir: NotifyHighAirClicked(); break;
        case Id::CheckUseExtended: NotifyUseExtendedStringClicked(); break;
    }
}

void LocationWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( (Id)idFrom )
    {
        case Id::EditRawFlags: RawFlagsUpdated(); break;
        case Id::EditLocationLeft: LocationLeftUpdated(); break;
        case Id::EditLocationTop: LocationTopUpdated(); break;
        case Id::EditLocationRight: LocationRightUpdated(); break;
        case Id::EditLocationBottom: LocationBottomUpdated(); break;
    }
}

void LocationWindow::NotifyEditFocused(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;
    
    Chk::LocationPtr locRef = currentLocationIndex != NO_LOCATION ? CM->layers.getLocation(currentLocationIndex) : nullptr;
    if ( locRef != nullptr )
    {
        switch ( (Id)idFrom )
        {
            case Id::EditLocationName: preservedStat = locRef->stringId; break;
            case Id::EditRawFlags: preservedStat = locRef->elevationFlags; break;
            case Id::EditLocationLeft: preservedStat = locRef->left; break;
            case Id::EditLocationTop: preservedStat = locRef->top; break;
            case Id::EditLocationRight:  preservedStat = locRef->right; break;
            case Id::EditLocationBottom:  preservedStat = locRef->bottom; break;
        }
    }
}

void LocationWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( (Id)idFrom )
    {
        case Id::EditRawFlags: RawFlagsFocusLost(); break;
        case Id::EditLocationLeft: LocationLeftFocusLost(); break;
        case Id::EditLocationTop: LocationTopFocusLost(); break;
        case Id::EditLocationRight: LocationRightFocusLost(); break;
        case Id::EditLocationBottom: LocationBottomFocusLost(); break;
        case Id::EditLocationName: LocationNameFocusLost(); break;
    }
}

BOOL LocationWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_CLOSE:
            DestroyThis();
            break;

        default:
            return FALSE;
            break;
    }
    return FALSE;
}
