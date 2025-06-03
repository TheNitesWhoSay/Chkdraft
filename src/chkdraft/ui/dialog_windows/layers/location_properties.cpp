#include "location_properties.h"
#include "chkdraft/chkdraft.h"
#include "ui/chkd_controls/chkd_string_input.h"
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
        editLocName.FocusThis();
        editLocName.SelectAll();

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
        this->SetWinText(std::string("Edit Location #") + std::to_string(currentLocationId));
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
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto temp = locRef.left;
        edit->locations[currentLocationId].left = locRef.right;
        edit->locations[currentLocationId].right = temp;
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertYc()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto temp = locRef.top;
        edit->locations[currentLocationId].top = locRef.bottom;
        edit->locations[currentLocationId].bottom = temp;
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::InvertXY()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        auto prevLeft = locRef.left;
        auto prevTop = locRef.top;
        edit->locations[currentLocationId].left = locRef.right;
        edit->locations[currentLocationId].top = locRef.bottom;
        edit->locations[currentLocationId].right = prevLeft;
        edit->locations[currentLocationId].bottom = prevTop;
        RefreshLocationInfo();
        CM->Redraw(false);
    }
}

void LocationWindow::NotifyLowGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkLowGround.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::LowElevation;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::LowElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkMedGround.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::MediumElevation;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::MediumElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighGroundClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkHighGround.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::HighElevation;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::HighElevation;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLowAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkLowAir.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::LowAir;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::LowAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyMedAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkMedAir.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::MediumAir;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::MediumAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyHighAirClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);

        if ( checkHighAir.isChecked() )
            edit->locations[currentLocationId].elevationFlags &= ~Chk::Location::Elevation::HighAir;
        else
            edit->locations[currentLocationId].elevationFlags |= Chk::Location::Elevation::HighAir;

        RefreshLocationInfo();
    }
}

void LocationWindow::NotifyLocNamePropertiesClicked()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        auto edit = CM->operator()();
        auto gameString = CM->getLocationName<ChkdString>(currentLocationId, Chk::Scope::Game);
        auto editorString = CM->getLocationName<ChkdString>(currentLocationId, Chk::Scope::Editor);
        ChkdStringInputDialog::Result result = ChkdStringInputDialog::GetChkdString(getHandle(), gameString, editorString, Chk::StringUserFlag::Location, currentLocationId);

        if ( (result & ChkdStringInputDialog::Result::GameStringChanged) == ChkdStringInputDialog::Result::GameStringChanged )
        {
            if ( gameString )
                CM->setLocationName<ChkdString>(currentLocationId, *gameString, Chk::Scope::Game);
            else
                CM->setLocationNameStringId(currentLocationId, Chk::StringId::NoString, Chk::Scope::Game);

            CM->deleteUnusedStrings(Chk::Scope::Game);
        }

        if ( (result & ChkdStringInputDialog::Result::EditorStringChanged) == ChkdStringInputDialog::Result::EditorStringChanged )
        {
            if ( editorString )
                CM->setLocationName<ChkdString>(currentLocationId, *editorString, Chk::Scope::Editor);
            else
                CM->setLocationNameStringId(currentLocationId, Chk::StringId::NoString, Chk::Scope::Editor);

            CM->deleteUnusedStrings(Chk::Scope::Editor);
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
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].elevationFlags != newVal )
        {
            edit->locations[currentLocationId].elevationFlags = newVal;
            RefreshLocationElevationFlags();
        }
    }
}

void LocationWindow::LocationLeftUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocLeft.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].left != newVal )
        {
            edit->locations[currentLocationId].left = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationTopUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocTop.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].top != newVal )
        {
            edit->locations[currentLocationId].top = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationRightUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocRight.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].right != newVal )
        {
            edit->locations[currentLocationId].right = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationBottomUpdated()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocBottom.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].bottom != newVal )
        {
            edit->locations[currentLocationId].bottom = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationNameFocusLost()
{
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() )
    {
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( auto locationName = editLocName.GetWinText() )
        {
            auto edit = CM->operator()();
            CM->replaceString<ChkdString>(locRef.stringId, *locationName);
            CM->deleteUnusedStrings(Chk::Scope::Both);
            CM->refreshScenario();
        }
    }
}

void LocationWindow::RawFlagsFocusLost()
{
    u16 newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].elevationFlags != newVal )
        {
            edit->locations[currentLocationId].elevationFlags = newVal;
            RefreshLocationInfo();
        }
    }
}

void LocationWindow::LocationLeftFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocLeft.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].left != newVal )
        {
            edit->locations[currentLocationId].left = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationTopFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocTop.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].top != newVal )
        {
            edit->locations[currentLocationId].top = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationRightFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocRight.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].right != newVal )
        {
            edit->locations[currentLocationId].right = newVal;
            CM->Redraw(false);
        }
    }
}

void LocationWindow::LocationBottomFocusLost()
{
    int newVal = 0;
    if ( currentLocationId != NO_LOCATION && currentLocationId < CM->numLocations() && editLocBottom.GetEditNum<int>(newVal) )
    {
        auto edit = CM->operator()();
        const Chk::Location & locRef = CM->getLocation(currentLocationId);
        if ( CM->read.locations[currentLocationId].bottom != newVal )
        {
            edit->locations[currentLocationId].bottom = newVal;
            CM->Redraw(false);
        }
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
