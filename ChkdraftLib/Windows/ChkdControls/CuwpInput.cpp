#include "CuwpInput.h"
#include "../../CommonFiles/CommonFiles.h"

CuwpInputDialog::~CuwpInputDialog()
{

}

bool CuwpInputDialog::GetCuwp(Chk::Cuwp &cuwp, const Chk::Cuwp &initialCuwp, HWND hParent)
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
    newCuwp.validUnitStateFlags = 0;
    newCuwp.validUnitFieldFlags = 0;
    newCuwp.owner = 0;
    newCuwp.hitpointPercent = 0;
    newCuwp.shieldPercent = 0;
    newCuwp.energyPercent = 0;
    newCuwp.resourceAmount = 0;
    newCuwp.hangerAmount = 0;
    newCuwp.unitStateFlags = 0;

    initialCuwp.validUnitStateFlags = 0;
    initialCuwp.validUnitFieldFlags = 0;
    initialCuwp.owner = 0;
    initialCuwp.hitpointPercent = 0;
    initialCuwp.shieldPercent = 0;
    initialCuwp.energyPercent = 0;
    initialCuwp.resourceAmount = 0;
    initialCuwp.hangerAmount = 0;
    initialCuwp.unitStateFlags = 0;
}

bool CuwpInputDialog::InternalGetCuwp(Chk::Cuwp &cuwp, const Chk::Cuwp &initialCuwp, HWND hParent)
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
            editHitpointPercent.GetEditNum<u8>(newCuwp.hitpointPercent) &&
            editManaPercent.GetEditNum<u8>(newCuwp.energyPercent) &&
            editShieldPercent.GetEditNum<u8>(newCuwp.shieldPercent) &&
            editResources.GetEditNum<u32>(newCuwp.resourceAmount) &&
            editHanger.GetEditNum<u16>(newCuwp.hangerAmount);

        newCuwp.setInvincible(checkInvincible.isChecked());
        newCuwp.setBurrowed(checkBurrowed.isChecked());
        newCuwp.setInTransit(checkLifted.isChecked());
        newCuwp.setHallucinated(checkHallucinated.isChecked());
        newCuwp.setCloaked(checkCloaked.isChecked());

        if ( gotCuwp )
            EndDialog(hWnd, IDOK);
        else
            WinLib::Message("Invalid Entry", "Error");

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

        editHitpointPercent.SetEditNum<u8>(initialCuwp.hitpointPercent);
        editManaPercent.SetEditNum<u8>(initialCuwp.energyPercent);
        editShieldPercent.SetEditNum<u8>(initialCuwp.shieldPercent);
        editResources.SetEditNum<u32>(initialCuwp.resourceAmount);
        editHanger.SetEditNum<u16>(initialCuwp.hangerAmount);

        checkInvincible.FindThis(hWnd, IDC_CHECKCUWPINVINCIBLE);
        checkBurrowed.FindThis(hWnd, IDC_CHECKCUWPBURROWED);
        checkLifted.FindThis(hWnd, IDC_CHECKCUWPLIFTED);
        checkHallucinated.FindThis(hWnd, IDC_CHECKCUWPHALLUCINATED);
        checkCloaked.FindThis(hWnd, IDC_CHECKCUWPCLOAKED);

        checkInvincible.SetCheck(initialCuwp.isInvincible());
        checkBurrowed.SetCheck(initialCuwp.isBurrowed());
        checkLifted.SetCheck(initialCuwp.isInTransit());
        checkHallucinated.SetCheck(initialCuwp.isHallucinated());
        checkCloaked.SetCheck(initialCuwp.isCloaked());
    }

    return ClassDialog::DlgProc(hWnd, msg, wParam, lParam);
}
