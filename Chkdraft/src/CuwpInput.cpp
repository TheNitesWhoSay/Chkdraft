#include "CuwpInput.h"
#include "CommonFiles/CommonFiles.h"

bool CuwpInputDialog::GetCuwp(ChkCuwp &cuwp, const ChkCuwp &initialCuwp, HWND hParent)
{
    CuwpInputDialog inputDialog;
    return inputDialog.InternalGetCuwp(cuwp, initialCuwp, hParent);
}

CuwpInputDialog::CuwpInputDialog() : gotCuwp(false)
{
    ClearStoredCuwps();
}

void CuwpInputDialog::ClearStoredCuwps()
{
    newCuwp.validSpecProps = 0;
    newCuwp.validElements = 0;
    newCuwp.owner = 0;
    newCuwp.percentHitpoints = 0;
    newCuwp.percentShieldPoints = 0;
    newCuwp.percentEnergyPoints = 0;
    newCuwp.resourceAmount = 0;
    newCuwp.numInHanger = 0;
    newCuwp.flags = 0;
    newCuwp.unknown = 0;

    initialCuwp.validSpecProps = 0;
    initialCuwp.validElements = 0;
    initialCuwp.owner = 0;
    initialCuwp.percentHitpoints = 0;
    initialCuwp.percentShieldPoints = 0;
    initialCuwp.percentEnergyPoints = 0;
    initialCuwp.resourceAmount = 0;
    initialCuwp.numInHanger = 0;
    initialCuwp.flags = 0;
    initialCuwp.unknown = 0;
}

bool CuwpInputDialog::InternalGetCuwp(ChkCuwp &cuwp, const ChkCuwp &initialCuwp, HWND hParent)
{
    gotCuwp = false;

    ClearStoredCuwps();

    this->initialCuwp = initialCuwp;
    CreateDialogBox(MAKEINTRESOURCE(IDD_EDITCUWP), hParent);
    if ( gotCuwp )
        cuwp = newCuwp;

    return gotCuwp;
}

BOOL CuwpInputDialog::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case IDCANCEL:
        gotCuwp = false;
        EndDialog(hWnd, IDCANCEL);
        break;

    case IDOK:
        gotCuwp =
            editHitpointPercent.GetEditNum<u8>(newCuwp.percentHitpoints) &&
            editManaPercent.GetEditNum<u8>(newCuwp.percentEnergyPoints) &&
            editShieldPercent.GetEditNum<u8>(newCuwp.percentShieldPoints) &&
            editResources.GetEditNum<u32>(newCuwp.resourceAmount) &&
            editHanger.GetEditNum<u16>(newCuwp.numInHanger);

        newCuwp.SetUnitIsInvincible(checkInvincible.isChecked());
        newCuwp.SetUnitIsBurrowed(checkBurrowed.isChecked());
        newCuwp.SetBuildingInTransit(checkLifted.isChecked());
        newCuwp.SetUnitIsHallucinated(checkHallucinated.isChecked());
        newCuwp.SetUnitIsCloaked(checkCloaked.isChecked());

        if ( gotCuwp )
            EndDialog(hWnd, IDOK);
        else
            MessageBox(NULL, "Invalid Entry", "Error", MB_OK | MB_ICONEXCLAMATION);

        break;
    }

    return ClassDialog::DlgCommand(hWnd, wParam, lParam);
}

BOOL CuwpInputDialog::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_INITDIALOG )
    {
        editHitpointPercent.FindThis(hWnd, IDC_EDITCUWPLIFE);
        editManaPercent.FindThis(hWnd, IDC_EDITCUWPMANA);
        editShieldPercent.FindThis(hWnd, IDC_EDITCUWPSHIELD);
        editResources.FindThis(hWnd, IDC_EDITCUWPRESOURCES);
        editHanger.FindThis(hWnd, IDC_EDITCUWPHANGER);

        editHitpointPercent.SetEditNum<u8>(initialCuwp.percentHitpoints);
        editManaPercent.SetEditNum<u8>(initialCuwp.percentEnergyPoints);
        editShieldPercent.SetEditNum<u8>(initialCuwp.percentShieldPoints);
        editResources.SetEditNum<u32>(initialCuwp.resourceAmount);
        editHanger.SetEditNum<u16>(initialCuwp.numInHanger);

        checkInvincible.FindThis(hWnd, IDC_CHECKCUWPINVINCIBLE);
        checkBurrowed.FindThis(hWnd, IDC_CHECKCUWPBURROWED);
        checkLifted.FindThis(hWnd, IDC_CHECKCUWPLIFTED);
        checkHallucinated.FindThis(hWnd, IDC_CHECKCUWPHALLUCINATED);
        checkCloaked.FindThis(hWnd, IDC_CHECKCUWPCLOAKED);

        checkInvincible.SetCheck(initialCuwp.IsUnitInvincible());
        checkBurrowed.SetCheck(initialCuwp.IsUnitBurrowed());
        checkLifted.SetCheck(initialCuwp.IsBuildingInTransit());
        checkHallucinated.SetCheck(initialCuwp.IsUnitHallucinated());
        checkCloaked.SetCheck(initialCuwp.IsUnitCloaked());
    }

    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
