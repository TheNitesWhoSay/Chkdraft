#include "LocationProperties.h"
#include "Chkdraft.h"
#include "LocationChange.h"
#include <exception>
#include <string>

LocationWindow::LocationWindow() : initializing(false), preservedStat(0), locProcLocIndex(0)
{

}

bool LocationWindow::CreateThis(HWND hParent)
{
    if ( ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_LOCPROP), hParent) )
    {
        editLocName.FindThis(getHandle(), IDC_LOCATION_NAME);
        editLocLeft.FindThis(getHandle(), IDC_LOCLEFT);
        editLocTop.FindThis(getHandle(), IDC_LOCTOP);
        editLocRight.FindThis(getHandle(), IDC_LOCRIGHT);
        editLocBottom.FindThis(getHandle(), IDC_LOCBOTTOM);
        editRawFlags.FindThis(getHandle(), IDC_RAWFLAGS);
        checkUseExtended.FindThis(getHandle(), IDC_EXTLOCNAMESTR);
        return true;
    }
    else
        return false;
}

bool LocationWindow::DestroyThis()
{
    if ( ClassDialog::DestroyDialog() )
    {
        locProcLocIndex = NO_LOCATION;
        return true;
    }
    else
        return false;
}

void LocationWindow::RefreshLocationElevationFlags(ChkLocation* locRef, HWND hWnd)
{
    if ( (locRef->elevation&LOC_ELEVATION_LOWGROUND) == 0)
        SendMessage(GetDlgItem(hWnd, IDC_LOWGROUND), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_LOWGROUND), BM_SETCHECK, BST_UNCHECKED, 0);
    if ( (locRef->elevation&LOC_ELEVATION_MEDGROUND) == 0 )
        SendMessage(GetDlgItem(hWnd, IDC_MEDGROUND), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_MEDGROUND), BM_SETCHECK, BST_UNCHECKED, 0);
    if ( (locRef->elevation&LOC_ELEVATION_HIGHGROUND) == 0 )
        SendMessage(GetDlgItem(hWnd, IDC_HIGHGROUND), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_HIGHGROUND), BM_SETCHECK, BST_UNCHECKED, 0);
    if ( (locRef->elevation&LOC_ELEVATION_LOWAIR) == 0 )
        SendMessage(GetDlgItem(hWnd, IDC_LOWAIR), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_LOWAIR), BM_SETCHECK, BST_UNCHECKED, 0);
    if ( (locRef->elevation&LOC_ELEVATION_MEDAIR) == 0 )
        SendMessage(GetDlgItem(hWnd, IDC_MEDAIR), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_MEDAIR), BM_SETCHECK, BST_UNCHECKED, 0);
    if ( (locRef->elevation&LOC_ELEVATION_HIGHAIR) == 0 )
        SendMessage(GetDlgItem(hWnd, IDC_HIGHAIR), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hWnd, IDC_HIGHAIR), BM_SETCHECK, BST_UNCHECKED, 0);
}

void LocationWindow::RefreshLocationInfo()
{
    initializing = true;

    HWND hWnd = getHandle();
    if ( !CM )
    {
        locProcLocIndex = NO_LOCATION;
        initializing = false;
        EndDialog(hWnd, IDCLOSE);
    }

    locProcLocIndex = CM->GetSelectedLocation();
    ChkLocation* locRef;
    if ( locProcLocIndex != NO_LOCATION && CM->getLocation(locRef, locProcLocIndex) )
    {
        SetWindowText(GetDlgItem(hWnd, IDC_LOCLEFT), std::to_string(locRef->xc1).c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_LOCTOP), std::to_string(locRef->yc1).c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_LOCRIGHT), std::to_string(locRef->xc2).c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_LOCBOTTOM), std::to_string(locRef->yc2).c_str());

        char text[20];
        _itoa_s(locRef->elevation, text, 20, 2);
        size_t len = std::strlen(text);
        if ( len > 0 && len < 16 )
        {
            std::memmove(&text[16-len], text, len);
            std::memset(text, '0', 16-len);
            text[16] = '\0';
        }
        SetWindowText(GetDlgItem(hWnd, IDC_RAWFLAGS), text);

        RefreshLocationElevationFlags(locRef, hWnd);

        if ( CM->isExtendedString(locRef->stringNum) )
            SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_CHECKED, 0);
        else
            SendMessage(GetDlgItem(hWnd, IDC_EXTLOCNAMESTR), BM_SETCHECK, BST_UNCHECKED, 0);

        ChkdString locName;
        if ( CM->GetString(locName, locRef->stringNum) )
            SetWindowText(GetDlgItem(hWnd, IDC_LOCATION_NAME), locName.c_str());
        else
            SetWindowText(GetDlgItem(hWnd, IDC_LOCATION_NAME), "ERROR");
    }
    else
    {
        locProcLocIndex = NO_LOCATION;
        initializing = false;
        EndDialog(hWnd, IDCLOSE);
    }
    initializing = false;
}

BOOL LocationWindow::DlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( LOWORD(wParam) )
    {
    case IDOK:
        locProcLocIndex = NO_LOCATION;
        EndDialog(hWnd, IDOK);
        break;
    case IDCANCEL:
        locProcLocIndex = NO_LOCATION;
        EndDialog(hWnd, IDCANCEL);
        break;
    case IDC_INVERTX:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            ChkLocation* locRef;
            if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
            {
                auto locationChanges = ReversibleActions::Make();
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1));
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2));
                CM->AddUndo(locationChanges);
                std::swap(locRef->xc1, locRef->xc2);
                RefreshLocationInfo();
                CM->Redraw(false);
            }
        }
        break;
    case IDC_INVERTY:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            ChkLocation* locRef;
            if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
            {
                auto locationChanges = ReversibleActions::Make();
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1));
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2));
                CM->AddUndo(locationChanges);
                std::swap(locRef->yc1, locRef->yc2);
                RefreshLocationInfo();
                CM->Redraw(false);
            }
        }
        break;
    case IDC_INVERTXY:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            ChkLocation* locRef;
            if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
            {
                auto locationChanges = ReversibleActions::Make();
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC1, locRef->xc1));
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC2, locRef->xc2));
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC1, locRef->yc1));
                locationChanges->Insert(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC2, locRef->yc2));
                CM->AddUndo(locationChanges);
                std::swap(locRef->xc1, locRef->xc2);
                std::swap(locRef->yc1, locRef->yc2);
                RefreshLocationInfo();
                CM->Redraw(false);
            }
        }
        break;
    default:
        if ( !initializing )
        {
            switch ( HIWORD(wParam) )
            {
            case BN_CLICKED:
            {
                ChkLocation* locRef;
                if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
                {
                    LRESULT result = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                    if ( result == BST_CHECKED || result == BST_UNCHECKED )
                    {
                        bool isChecked = (result == BST_CHECKED);
                        if ( isChecked )
                        {
                            switch ( LOWORD(wParam) )
                            {
                            case IDC_LOWGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_LOWGROUND);
                                break;
                            case IDC_MEDGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_MEDGROUND);
                                break;
                            case IDC_HIGHGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_HIGHGROUND);
                                break;
                            case IDC_LOWAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_LOWAIR);
                                break;
                            case IDC_MEDAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_MEDAIR);
                                break;
                            case IDC_HIGHAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation &= (~LOC_ELEVATION_HIGHAIR);
                                break;
                            case IDC_EXTLOCNAMESTR:
                            {
                                ChkdString str;
                                if ( CM->GetString(str, locRef->stringNum) )
                                {
                                    u32 newStrNum;
                                    if ( CM->addString(str, newStrNum, true) )
                                    {
                                        u32 oldStrNum = locRef->stringNum;
                                        locRef->stringNum = u16(newStrNum);
                                        CM->removeUnusedString(oldStrNum);
                                        CM->refreshScenario();
                                    }
                                }
                                CM->notifyChange(false);
                            }
                            break;
                            }
                            RefreshLocationInfo();
                        }
                        else // notChecked
                        {
                            switch ( LOWORD(wParam) )
                            {
                            case IDC_LOWGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_LOWGROUND;
                                break;
                            case IDC_MEDGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_MEDGROUND;
                                break;
                            case IDC_HIGHGROUND:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_HIGHGROUND;
                                break;
                            case IDC_LOWAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_LOWAIR;
                                break;
                            case IDC_MEDAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_MEDAIR;
                                break;
                            case IDC_HIGHAIR:
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, locRef->elevation));
                                locRef->elevation |= LOC_ELEVATION_HIGHAIR;
                                break;
                            case IDC_EXTLOCNAMESTR:
                            {
                                ChkdString str;
                                if ( CM->GetString(str, locRef->stringNum) )
                                {
                                    u32 newStrNum;
                                    if ( CM->addString(str, newStrNum, false) )
                                    {
                                        u32 oldStrNum = locRef->stringNum;
                                        locRef->stringNum = u16(newStrNum);
                                        CM->removeUnusedString(oldStrNum);
                                        CM->refreshScenario();
                                    }
                                }
                                CM->notifyChange(false);
                            }
                            break;
                            }
                            RefreshLocationInfo();
                        }
                    }
                }
            }
            break;
            case EN_SETFOCUS:
            {
                ChkLocation* locRef;
                if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
                {
                    switch ( LOWORD(wParam) )
                    {
                    case IDC_LOCATION_NAME: preservedStat = locRef->stringNum; break;
                    case IDC_RAWFLAGS: preservedStat = locRef->elevation; break;
                    case IDC_LOCLEFT: preservedStat = locRef->xc1; break;
                    case IDC_LOCTOP: preservedStat = locRef->yc1; break;
                    case IDC_LOCRIGHT:  preservedStat = locRef->xc2; break;
                    case IDC_LOCBOTTOM:  preservedStat = locRef->yc2; break;
                    }
                }
            }
            break;
            case EN_KILLFOCUS:
            {
                ChkLocation* locRef;
                if ( CM != nullptr && CM->getLocation(locRef, locProcLocIndex) )
                {
                    switch ( LOWORD(wParam) )
                    {
                    case IDC_RAWFLAGS:
                    {
                        u16 newVal;
                        if ( editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
                        {
                            locRef->elevation = newVal;
                            CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_ELEVATION, preservedStat));
                            RefreshLocationInfo();
                        }
                    }
                    break;
                    case IDC_LOCLEFT:
                    {
                        int newVal;
                        if ( editLocLeft.GetEditNum<int>(newVal) )
                        {
                            locRef->xc1 = newVal;
                            if ( newVal != preservedStat )
                            {
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC1, preservedStat));
                            }
                            CM->Redraw(false);
                        }
                    }
                    break;
                    case IDC_LOCTOP:
                    {
                        int newVal;
                        if ( editLocTop.GetEditNum<int>(newVal) )
                        {
                            locRef->yc1 = newVal;
                            if ( newVal != preservedStat )
                            {
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC1, preservedStat));
                            }
                            CM->Redraw(false);
                        }
                    }
                    break;
                    case IDC_LOCRIGHT:
                    {
                        int newVal;
                        if ( editLocRight.GetEditNum<int>(newVal) )
                        {
                            locRef->xc2 = newVal;
                            if ( newVal != preservedStat )
                            {
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_XC2, preservedStat));
                            }
                            CM->Redraw(false);
                        }
                    }
                    break;
                    case IDC_LOCBOTTOM:
                    {
                        int newVal;
                        if ( editLocBottom.GetEditNum<int>(newVal) )
                        {
                            locRef->yc2 = newVal;
                            if ( newVal != preservedStat )
                            {
                                CM->AddUndo(LocationChange::Make(locProcLocIndex, LOC_FIELD_YC2, preservedStat));
                            }
                            CM->Redraw(false);
                        }
                    }
                    break;
                    case IDC_LOCATION_NAME:
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
                    break;
                    }
                }
            }
            break;
            case EN_UPDATE:
                switch ( LOWORD(wParam) )
                {
                case IDC_RAWFLAGS:
                {
                    ChkLocation* locRef;
                    u16 newVal;
                    if ( CM && CM->getLocation(locRef, locProcLocIndex) &&
                        editRawFlags.GetEditBinaryNum(newVal) && preservedStat != newVal )
                    {
                        locRef->elevation = newVal;
                        initializing = true;
                        RefreshLocationElevationFlags(locRef, hWnd);
                        initializing = false;
                    }
                }
                break;
                case IDC_LOCLEFT:
                {
                    ChkLocation* locRef;
                    int newVal;
                    if ( CM && CM->getLocation(locRef, locProcLocIndex) && editLocLeft.GetEditNum<int>(newVal) )
                    {
                        locRef->xc1 = newVal;
                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_LOCTOP:
                {
                    ChkLocation* locRef;
                    int newVal;
                    if ( CM && CM->getLocation(locRef, locProcLocIndex) && editLocTop.GetEditNum<int>(newVal) )
                    {
                        locRef->yc1 = newVal;
                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_LOCRIGHT:
                {
                    ChkLocation* locRef;
                    int newVal;
                    if ( CM && CM->getLocation(locRef, locProcLocIndex) && editLocRight.GetEditNum<int>(newVal) )
                    {
                        locRef->xc2 = newVal;
                        CM->Redraw(false);
                    }
                }
                break;
                case IDC_LOCBOTTOM:
                {
                    ChkLocation* locRef;
                    int newVal;
                    if ( CM && CM->getLocation(locRef, locProcLocIndex) && editLocBottom.GetEditNum<int>(newVal) )
                    {
                        locRef->yc2 = newVal;
                        CM->Redraw(false);
                    }
                }
                break;
                }
                break;
            }
        }
        break;
    }
    return TRUE;
}

BOOL LocationWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_ACTIVATE:
            if ( LOWORD(wParam) != WA_INACTIVE )
                chkd.SetCurrDialog(hWnd);
            break;

        case WM_INITDIALOG:
            RefreshLocationInfo();
            return TRUE;
            break;

        default:
            return FALSE;
            break;
    }
    return FALSE;
}
