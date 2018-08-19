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

    ChkLocation* locRef = nullptr;
    if ( currentLocationIndex != NO_LOCATION && CM->getLocation(locRef, currentLocationIndex) )
    {
        checkLowGround.SetCheck((locRef->elevation&LOC_ELEVATION_LOWGROUND) == 0);
        checkMedGround.SetCheck((locRef->elevation&LOC_ELEVATION_MEDGROUND) == 0);
        checkHighGround.SetCheck((locRef->elevation&LOC_ELEVATION_HIGHGROUND) == 0);
        checkLowAir.SetCheck((locRef->elevation&LOC_ELEVATION_LOWAIR) == 0);
        checkMedAir.SetCheck((locRef->elevation&LOC_ELEVATION_MEDAIR) == 0);
        checkHighAir.SetCheck((locRef->elevation&LOC_ELEVATION_HIGHAIR) == 0);
    }

    refreshing = prevRefreshingValue;
}

void LocationWindow::RefreshLocationInfo()
{
    refreshing = true;

    if ( CM == nullptr )
        DestroyThis();

    currentLocationIndex = CM->GetSelectedLocation();
    ChkLocation* locRef = nullptr;
    if ( currentLocationIndex != NO_LOCATION && CM->getLocation(locRef, currentLocationIndex) )
    {
        editLocLeft.SetText(std::to_string(locRef->xc1).c_str());
        editLocTop.SetText(std::to_string(locRef->yc1).c_str());
        editLocRight.SetText(std::to_string(locRef->xc2).c_str());
        editLocBottom.SetText(std::to_string(locRef->yc2).c_str());

        char text[20];
        _itoa_s(locRef->elevation, text, 20, 2);
        size_t len = std::strlen(text);
        if ( len > 0 && len < 16 )
        {
            std::memmove(&text[16-len], text, len);
            std::memset(text, '0', 16-len);
            text[16] = '\0';
        }
        editRawFlags.SetText(text);

        RefreshLocationElevationFlags();
        checkUseExtended.SetCheck(CM->isExtendedString(locRef->stringNum));

        ChkdString locName;
        if ( CM->GetString(locName, locRef->stringNum) )
            editLocName.SetText(locName.c_str());
        else
            editLocName.SetText("ERROR");
    }
    else
        DestroyThis();
    
    refreshing = false;
}

void LocationWindow::InvertXc()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC1, locRef->xc1));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC2, locRef->xc2));
        CM->AddUndo(locationChanges);
        std::swap(locRef->xc1, locRef->xc2);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertYc()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC1, locRef->yc1));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC2, locRef->yc2));
        CM->AddUndo(locationChanges);
        std::swap(locRef->yc1, locRef->yc2);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertXY()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC1, locRef->xc1));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC2, locRef->xc2));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC1, locRef->yc1));
        locationChanges->Insert(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC2, locRef->yc2));
        CM->AddUndo(locationChanges);
        std::swap(locRef->xc1, locRef->xc2);
        std::swap(locRef->yc1, locRef->yc2);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::NotifyLowGroundClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkLowGround.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_LOWGROUND);
        else
            locRef->elevation |= LOC_ELEVATION_LOWGROUND;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedGroundClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkMedGround.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_MEDGROUND);
        else
            locRef->elevation |= LOC_ELEVATION_MEDGROUND;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighGroundClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkHighGround.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_HIGHGROUND);
        else
            locRef->elevation |= LOC_ELEVATION_HIGHGROUND;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLowAirClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkLowAir.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_LOWAIR);
        else
            locRef->elevation |= LOC_ELEVATION_LOWAIR;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedAirClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkMedAir.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_MEDAIR);
        else
            locRef->elevation |= LOC_ELEVATION_MEDAIR;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighAirClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, locRef->elevation));

        if ( checkHighAir.isChecked() )
            locRef->elevation &= (~LOC_ELEVATION_HIGHAIR);
        else
            locRef->elevation |= LOC_ELEVATION_HIGHAIR;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyUseExtendedStringClicked()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        ChkdString str;
        if ( CM->GetString(str, locRef->stringNum) )
        {
            u32 newStrNum = 0;
            if ( CM->addString(str, newStrNum, checkUseExtended.isChecked()) )
            {
                u32 oldStrNum = locRef->stringNum;
                locRef->stringNum = u16(newStrNum);
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
    ChkLocation* locRef = nullptr;
    u16 newVal = 0;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) &&
        editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
    {
        locRef->elevation = newVal;
        RefreshLocationElevationFlags();
    }
}

void LocationWindow::LocationLeftUpdated()
{
    ChkLocation* locRef = nullptr;
    int newVal = 0;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) && editLocLeft.GetEditNum<int>(newVal) )
    {
        locRef->xc1 = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationTopUpdated()
{
    ChkLocation* locRef = nullptr;
    int newVal = 0;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) && editLocTop.GetEditNum<int>(newVal) )
    {
        locRef->yc1 = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationRightUpdated()
{
    ChkLocation* locRef = nullptr;
    int newVal = 0;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) && editLocRight.GetEditNum<int>(newVal) )
    {
        locRef->xc2 = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationBottomUpdated()
{
    ChkLocation* locRef = nullptr;
    int newVal = 0;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) && editLocBottom.GetEditNum<int>(newVal) )
    {
        locRef->yc2 = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationNameFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        ChkdString locationName;
        if ( editLocName.GetWinText(locationName) )
        {
            bool isExtended = checkUseExtended.isChecked();
            if ( CM->replaceString<u16>(locationName, locRef->stringNum, isExtended, true) )
            {
                CM->notifyChange(false);
                CM->refreshScenario();
            }
        }
    }
}

void LocationWindow::RawFlagsFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        u16 newVal = 0;
        if ( editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
        {
            locRef->elevation = newVal;
            CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_ELEVATION, preservedStat));
            RefreshLocationInfo();
        }
    }
}

void LocationWindow::LocationLeftFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        int newVal = 0;
        if ( editLocLeft.GetEditNum<int>(newVal) )
        {
            locRef->xc1 = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC1, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationTopFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        int newVal = 0;
        if ( editLocTop.GetEditNum<int>(newVal) )
        {
            locRef->yc1 = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC1, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationRightFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        int newVal = 0;
        if ( editLocRight.GetEditNum<int>(newVal) )
        {
            locRef->xc2 = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_XC2, preservedStat));
            
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationBottomFocusLost()
{
    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        int newVal = 0;
        if ( editLocBottom.GetEditNum<int>(newVal) )
        {
            locRef->yc2 = newVal;
            if ( newVal != preservedStat )
                CM->AddUndo(LocationChange::Make(currentLocationIndex, LOC_FIELD_YC2, preservedStat));
            
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

    ChkLocation* locRef = nullptr;
    if ( CM != nullptr && CM->getLocation(locRef, currentLocationIndex) )
    {
        switch ( (Id)idFrom )
        {
            case Id::EditLocationName: preservedStat = locRef->stringNum; break;
            case Id::EditRawFlags: preservedStat = locRef->elevation; break;
            case Id::EditLocationLeft: preservedStat = locRef->xc1; break;
            case Id::EditLocationTop: preservedStat = locRef->yc1; break;
            case Id::EditLocationRight:  preservedStat = locRef->xc2; break;
            case Id::EditLocationBottom:  preservedStat = locRef->yc2; break;
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
