#include "CUWPs.h"
#include "Chkdraft.h"

enum ID
{
    CuwpTree = ID_FIRST,
    CheckUsed,
    EditHitpoints,
    EditMana,
    EditShields,
    EditResources,
    EditHanger,
    CheckInvincible,
    CheckBurrowed,
    CheckLifted,
    CheckHallucinated,
    CheckCloaked
};

CUWPsWindow::CUWPsWindow() : selectedCuwp(-1)
{

}

bool CUWPsWindow::CreateThis(HWND hParent, u32 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    RECT rcCli;
    if ( GetWindowRect(hParent, &rcCli) &&
         ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "CUWPs", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "CUWPs", WS_CHILD,
            5, 22, rcCli.right-rcCli.left, rcCli.bottom-rcCli.top,
            hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool CUWPsWindow::DestroyThis()
{
    return false;
}

void CUWPsWindow::RefreshWindow(bool includeTree)
{
    ChkCuwp cuwp = {};
    if ( includeTree )
    {
        cuwpTree.EmptySubTree(NULL);
        int cuwpCapacity = CM->CuwpCapacity();
        for ( int i = 0; i < cuwpCapacity; i++ )
        {
            if ( CM->IsCuwpUsed(i) )
                cuwpTree.InsertTreeItem(NULL, std::string("CUWP #" + std::to_string(i)).c_str(), i);
            else
                cuwpTree.InsertTreeItem(NULL, std::string("(#" + std::to_string(i) + ")").c_str(), i);
        }
    }

    if ( selectedCuwp != -1 && CM->GetCuwp(selectedCuwp, cuwp) )
    {
        checkUsed.SetCheck(CM->IsCuwpUsed(selectedCuwp));
        editHitpointPercent.SetEditNum<u8>(cuwp.percentHitpoints);
        editManaPercent.SetEditNum<u8>(cuwp.percentEnergyPoints);
        editShieldPercent.SetEditNum<u8>(cuwp.percentShieldPoints);
        editResources.SetEditNum<u32>(cuwp.resourceAmount);
        editHanger.SetEditNum<u16>(cuwp.numInHanger);

        checkInvincible.SetCheck(cuwp.IsUnitInvincible());
        checkBurrowed.SetCheck(cuwp.IsUnitBurrowed());
        checkLifted.SetCheck(cuwp.IsBuildingInTransit());
        checkHallucinated.SetCheck(cuwp.IsUnitHallucinated());
        checkCloaked.SetCheck(cuwp.IsUnitCloaked());
    }
    else
        DisableEditing();
}

void CUWPsWindow::DoSize()
{
    cuwpTree.SetPos(cuwpTree.Left(), cuwpTree.Top(), cuwpTree.Width(), cliHeight()-cuwpTree.Top()-5);
}

void CUWPsWindow::CreateSubWindows(HWND hWnd)
{
    cuwpTree.CreateThis(hWnd, 5, 5, 175, 100, false, CuwpTree);
    checkUsed.CreateThis(hWnd, 185, 5, 100, 20, false, "Is Used", CheckUsed);
    checkUsed.DisableThis();

    healthPercent.CreateThis(hWnd, 190, 40, 65, 20, "Life (%): ", 0);
    manaPercent.CreateThis(hWnd, 190, 65, 65, 20, "Mana (%): ", 0);
    shieldPercent.CreateThis(hWnd, 190, 90, 65, 20, "Shield (%): ", 0);
    resourceAmount.CreateThis(hWnd, 190, 115, 65, 20, "Resources: ", 0);
    hangerAmount.CreateThis(hWnd, 190, 140, 65, 20, "Hanger: ", 0);

    editHitpointPercent.CreateThis(hWnd, 260, 38, 80, 20, false, EditHitpoints);
    editManaPercent.CreateThis(hWnd, 260, 63, 80, 20, false, EditMana);
    editShieldPercent.CreateThis(hWnd, 260, 88, 80, 20, false, EditShields);
    editResources.CreateThis(hWnd, 260, 113, 80, 20, false, EditResources);
    editHanger.CreateThis(hWnd, 260, 138, 80, 20, false, EditHanger);

    checkInvincible.CreateThis(hWnd, 190, 170, 100, 20, false, "Invincible", CheckInvincible);
    checkBurrowed.CreateThis(hWnd, 190, 190, 100, 20, false, "Burrowed", CheckBurrowed);
    checkLifted.CreateThis(hWnd, 190, 210, 100, 20, false, "Lifted", CheckLifted);
    checkHallucinated.CreateThis(hWnd, 190, 230, 100, 20, false, "Hallucinated", CheckHallucinated);
    checkCloaked.CreateThis(hWnd, 190, 250, 100, 20, false, "Cloaked", CheckCloaked);

    DisableEditing();
    RefreshWindow(true);
}

void CUWPsWindow::DisableEditing()
{
    healthPercent.DisableThis();
    manaPercent.DisableThis();
    shieldPercent.DisableThis();
    resourceAmount.DisableThis();
    hangerAmount.DisableThis();

    editHitpointPercent.DisableThis();
    editManaPercent.DisableThis();
    editShieldPercent.DisableThis();
    editResources.DisableThis();
    editHanger.DisableThis();

    checkInvincible.DisableThis();
    checkBurrowed.DisableThis();
    checkLifted.DisableThis();
    checkHallucinated.DisableThis();
    checkCloaked.DisableThis();
}

void CUWPsWindow::EnableEditing()
{
    healthPercent.EnableThis();
    manaPercent.EnableThis();
    shieldPercent.EnableThis();
    resourceAmount.EnableThis();
    hangerAmount.EnableThis();

    editHitpointPercent.EnableThis();
    editManaPercent.EnableThis();
    editShieldPercent.EnableThis();
    editResources.EnableThis();
    editHanger.EnableThis();

    checkInvincible.EnableThis();
    checkBurrowed.EnableThis();
    checkLifted.EnableThis();
    checkHallucinated.EnableThis();
    checkCloaked.EnableThis();
}

LRESULT CUWPsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        s32 cuwpId = ((NMTREEVIEW*)nmhdr)->itemNew.lParam;
        if ( cuwpId >= 0 && cuwpId < 64 )
        {
            if ( selectedCuwp == -1 )
                EnableEditing();

            selectedCuwp = cuwpId;
        }
        else
        {
            if ( selectedCuwp != -1 )
                DisableEditing();

            selectedCuwp = -1;
        }
        RefreshWindow(false);
    }
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

void CUWPsWindow::NotifyTreeSelChanged(LPARAM newValue)
{

}

void CUWPsWindow::NotifyButtonClicked(int idFrom, HWND hWndFrom)
{
    ChkCuwp cuwp = {};
    if ( CM->GetCuwp(selectedCuwp, cuwp) )
    {
        switch ( idFrom )
        {
            case CheckInvincible: cuwp.SetUnitIsInvincible(checkInvincible.isChecked()); break;
            case CheckBurrowed: cuwp.SetUnitIsBurrowed(checkBurrowed.isChecked()); break;
            case CheckLifted: cuwp.SetBuildingInTransit(checkLifted.isChecked()); break;
            case CheckHallucinated: cuwp.SetUnitIsHallucinated(checkHallucinated.isChecked()); break;
            case CheckCloaked: cuwp.SetUnitIsCloaked(checkCloaked.isChecked()); break;
        }
        CM->ReplaceCuwp(cuwp, selectedCuwp);
    }
}

void CUWPsWindow::NotifyEditUpdated(int idFrom, HWND hWndFrom)
{
    ChkCuwp cuwp = {};
    if ( CM->GetCuwp(selectedCuwp, cuwp) )
    {
        int editNum = 0;
        switch ( idFrom )
        {
            case EditHitpoints: if ( editHitpointPercent.GetEditNum(editNum) ) cuwp.percentHitpoints = editNum; break;
            case EditMana: if ( editManaPercent.GetEditNum(editNum) ) cuwp.percentEnergyPoints = editNum; break;
            case EditShields: if ( editShieldPercent.GetEditNum(editNum) ) cuwp.percentShieldPoints = editNum; break;
            case EditResources: if ( editResources.GetEditNum(editNum) ) cuwp.resourceAmount = editNum; break;
            case EditHanger: if ( editHanger.GetEditNum(editNum) ) cuwp.numInHanger = editNum; break;
        }
        CM->ReplaceCuwp(cuwp, selectedCuwp);
    }
}

LRESULT CUWPsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return ClassWindow::Command(hWnd, wParam, lParam);
}
