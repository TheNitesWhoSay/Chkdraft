#include "LocationProperties.h"
#include "../../../Chkdraft.h"
#include "../../../Mapping/Undos/ChkdUndos/LocationChange.h"
#include "../../ChkdControls/ChkdStringInput.h"
#include <exception>
#include <string>

enum_t(Id, u32, {
    EditLocationName = IDC_LOCATION_NAME,
    EditLocationLeft = IDC_LOCLEFT,
    EditLocationTop = IDC_LOCTOP,
    EditLocationRight = IDC_LOCRIGHT,
    EditLocationBottom = IDC_LOCBOTTOM,
    EditRawFlags = IDC_RAWFLAGS,

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
    ButtonLocNameProperties = IDC_LOCNAME_PROPERTIES
});

LocationWindow::LocationWindow() : refreshing(true), preservedStat(0), currentLocationId(0)
{

}

LocationWindow::~LocationWindow()
{

}

bool LocationWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_LOCPROP), hParent) )
    {
        editLocName.FindThis(getHandle(), Id::EditLocationName);
        buttonLocNameProperties.FindThis(getHandle(), Id::ButtonLocNameProperties);
        buttonLocNameProperties.SetImageFromResourceId(IDB_PROPERTIES);

        editLocLeft.FindThis(getHandle(), Id::EditLocationLeft);
        editLocTop.FindThis(getHandle(), Id::EditLocationTop);
        editLocRight.FindThis(getHandle(), Id::EditLocationRight);
        editLocBottom.FindThis(getHandle(), Id::EditLocationBottom);
        editRawFlags.FindThis(getHandle(), Id::EditRawFlags);

        checkLowGround.FindThis(getHandle(), Id::CheckLowGround);
        checkMedGround.FindThis(getHandle(), Id::CheckMedGround);
        checkHighGround.FindThis(getHandle(), Id::CheckHighGround);
        checkLowAir.FindThis(getHandle(), Id::CheckLowAir);
        checkMedAir.FindThis(getHandle(), Id::CheckMedAir);
        checkHighAir.FindThis(getHandle(), Id::CheckHighAir);

        buttonInvertX.FindThis(getHandle(), Id::ButtonInvertX);
        buttonInvertY.FindThis(getHandle(), Id::ButtonInvertY);
        buttonInvertXY.FindThis(getHandle(), Id::ButtonInvertXY);

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
        currentLocationId = NO_LOCATION;
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

    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        checkLowGround.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::LowElevation) == 0);
        checkMedGround.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::MediumElevation) == 0);
        checkHighGround.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::HighElevation) == 0);
        checkLowAir.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::LowAir) == 0);
        checkMedAir.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::MediumAir) == 0);
        checkHighAir.SetCheck((locRef.elevationFlags & Chk::Location::Elevation::HighAir) == 0);
    }

    refreshing = prevRefreshingValue;
}

void LocationWindow::RefreshLocationInfo()
{
    refreshing = true;

    if ( CM == nullptr )
        DestroyThis();
    
    currentLocationId = CM->GetSelectedLocation();
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        editLocLeft.SetText(std::to_string(locRef.left));
        editLocTop.SetText(std::to_string(locRef.top));
        editLocRight.SetText(std::to_string(locRef.right));
        editLocBottom.SetText(std::to_string(locRef.bottom));

        char text[20];
        _itoa_s(locRef.elevationFlags, text, 20, 2);
        size_t len = std::strlen(text);
        if ( len > 0 && len < 16 )
        {
            std::memmove(&text[16-len], text, len);
            std::memset(text, '0', 16-len);
            text[16] = '\0';
        }
        editRawFlags.SetText(text);

        RefreshLocationElevationFlags();

        if ( auto locName = CM->getString<ChkdString>(locRef.stringId) )
            editLocName.SetText(*locName);
        else
            editLocName.SetText("");
    }
    else
        DestroyThis();
    
    refreshing = false;
}

void LocationWindow::InvertXc()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Left, locRef.left));
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Right, locRef.right));
        CM->AddUndo(locationChanges);
        std::swap(locRef.left, locRef.right);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertYc()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Top, locRef.top));
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Bottom, locRef.bottom));
        CM->AddUndo(locationChanges);
        std::swap(locRef.top, locRef.bottom);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertXY()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto locationChanges = ReversibleActions::Make();
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Left, locRef.left));
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Right, locRef.right));
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Top, locRef.top));
        locationChanges->Insert(LocationChange::Make(currentLocationId, Chk::Location::Field::Bottom, locRef.bottom));
        CM->AddUndo(locationChanges);
        std::swap(locRef.left, locRef.right);
        std::swap(locRef.top, locRef.bottom);
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::NotifyLowGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkLowGround.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::LowElevation;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::LowElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkMedGround.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::MediumElevation;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::MediumElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkHighGround.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::HighElevation;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::HighElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLowAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkLowAir.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::LowAir;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::LowAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkMedAir.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::MediumAir;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::MediumAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, locRef.elevationFlags));

        if ( checkHighAir.isChecked() )
            locRef.elevationFlags &= ~Chk::Location::Elevation::HighAir;
        else
            locRef.elevationFlags |= Chk::Location::Elevation::HighAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLocNamePropertiesClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto gameString = CM->getLocationName<ChkdString>(currentLocationId, Chk::StrScope::Game);
        auto editorString = CM->getLocationName<ChkdString>(currentLocationId, Chk::StrScope::Editor);
        ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), gameString, editorString, Chk::StringUserFlag::Location, currentLocationId);

        if ( (result & ChkdStringInputDialog::Result::GameStringChanged) == ChkdStringInputDialog::Result::GameStringChanged )
        {
            if ( gameString )
                CM->setLocationName<ChkdString>(currentLocationId, *gameString, Chk::StrScope::Game);
            else
                CM->setLocationNameStringId(currentLocationId, Chk::StringId::NoString, Chk::StrScope::Game);

            CM->deleteUnusedStrings(Chk::StrScope::Game);
        }

        if ( (result & ChkdStringInputDialog::Result::EditorStringChanged) == ChkdStringInputDialog::Result::EditorStringChanged )
        {
            if ( editorString )
                CM->setLocationName<ChkdString>(currentLocationId, *editorString, Chk::StrScope::Editor);
            else
                CM->setLocationNameStringId(currentLocationId, Chk::StringId::NoString, Chk::StrScope::Editor);

            CM->deleteUnusedStrings(Chk::StrScope::Editor);
        }

        if ( result > 0 )
            CM->refreshScenario();
    }
}

void LocationWindow::RawFlagsUpdated()
{
    u16 newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.elevationFlags = newVal;
        RefreshLocationElevationFlags();
    }
}

void LocationWindow::LocationLeftUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocLeft.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.left = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationTopUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocTop.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.top = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationRightUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocRight.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.right = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationBottomUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocBottom.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.bottom = newVal;
        CM->Redraw(false);
    }
}

void LocationWindow::LocationNameFocusLost()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        ChkdString locationName;
        if ( editLocName.GetWinText(locationName) )
        {
            CM->replaceString<ChkdString>(locRef.stringId, locationName);
            CM->deleteUnusedStrings(Chk::StrScope::Both);
            CM->notifyChange(false);
            CM->refreshScenario();
        }
    }
}

void LocationWindow::RawFlagsFocusLost()
{
    u16 newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.elevationFlags = newVal;
        CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::ElevationFlags, preservedStat));
        RefreshLocationInfo();
    }
}

void LocationWindow::LocationLeftFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocLeft.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.left = newVal;
        if ( newVal != preservedStat )
            CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::Left, preservedStat));
            
        CM->Redraw(false);
    }
}

void LocationWindow::LocationTopFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocTop.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.top = newVal;
        if ( newVal != preservedStat )
            CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::Top, preservedStat));
            
        CM->Redraw(false);
    }
}

void LocationWindow::LocationRightFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocRight.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.right = newVal;
        if ( newVal != preservedStat )
            CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::Right, preservedStat));
            
        CM->Redraw(false);
    }
}

void LocationWindow::LocationBottomFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocBottom.GetEditNum<int>(newVal) )
    {
        Chk::Location & locRef = CM->getLocation(currentLocationId);
        locRef.bottom = newVal;
        if ( newVal != preservedStat )
            CM->AddUndo(LocationChange::Make(currentLocationId, Chk::Location::Field::Bottom, preservedStat));
            
        CM->Redraw(false);
    }
}

void LocationWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( idFrom )
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
        case Id::ButtonLocNameProperties: NotifyLocNamePropertiesClicked(); break;
    }
}

void LocationWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( idFrom )
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
    
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        switch ( idFrom )
        {
            case Id::EditLocationName: preservedStat = locRef.stringId; break; // TODO: This is sketchy, replace this
            case Id::EditRawFlags: preservedStat = locRef.elevationFlags; break;
            case Id::EditLocationLeft: preservedStat = locRef.left; break;
            case Id::EditLocationTop: preservedStat = locRef.top; break;
            case Id::EditLocationRight:  preservedStat = locRef.right; break;
            case Id::EditLocationBottom:  preservedStat = locRef.bottom; break;
        }
    }
}

void LocationWindow::NotifyEditFocusLost(int idFrom, HWND hWndFrom)
{
    if ( refreshing )
        return;

    switch ( idFrom )
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
