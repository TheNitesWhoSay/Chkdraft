#include "UnitSettings.h"
#include "../../../Chkdraft.h"
#include <sstream>
#include <string>

enum class Id
{
    TREE_UNITSETTINGS = ID_FIRST,
    BUTTON_RESETALLUNITDEFAULTS,
    CHECK_USEUNITDEFAULTS,
    CHECK_ENABLEDBYDEFAULT,
    GROUP_UNITPROPERTIES,
    GROUP_GROUNDWEAPON,
    GROUP_AIRWEAPON,
    GROUP_UNITNAME,
    GROUP_UNITAVAILABILITY,
    TEXT_UNITHITPOINTS,
    EDIT_UNITHITPOINTS,
    EDIT_UNITHITPOINTSBYTE,
    TEXT_UNITSHIELDPOINTS,
    EDIT_UNITSHIELDPOINTS,
    TEXT_UNITARMOR,
    EDIT_UNITARMOR,
    TEXT_UNITBUILDTIME,
    EDIT_UNITBUILDTIME,
    TEXT_UNITMINERALCOST,
    EDIT_UNITMINERALCOST,
    TEXT_UNITGASCOST,
    EDIT_UNITGASCOST,
    TEXT_UNITGROUNDDAMAGE,
    EDIT_UNITGROUNDDAMAGE,
    TEXT_UNITGROUNDBONUS,
    EDIT_UNITGROUNDBONUS,
    TEXT_UNITAIRDAMAGE,
    EDIT_UNITAIRDAMAGE,
    TEXT_UNITAIRBONUS,
    EDIT_UNITAIRBONUS,
    CHECK_USEDEFAULTUNITNAME,
    EDIT_UNITNAME,
    TEXT_P1UNITAVAILABILITY,
    TEXT_P12UNITAVAILABILITY = (TEXT_P1UNITAVAILABILITY + 11),
    DROP_P1UNITAVAILABILITY,
    DROP_P12UNITAVAILABILITY = (DROP_P1UNITAVAILABILITY + 11)
};

UnitSettingsWindow::UnitSettingsWindow() : selectedUnitType(Sc::Unit::Type::NoUnit), possibleUnitNameUpdate(false), isDisabled(true), refreshing(false)
{

}

UnitSettingsWindow::~UnitSettingsWindow()
{

}

bool UnitSettingsWindow::CreateThis(HWND hParent, u64 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "UnitSettings", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "UnitSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool UnitSettingsWindow::DestroyThis()
{
    return true;
}

void UnitSettingsWindow::RefreshWindow()
{
    refreshing = true;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit && (u16)selectedUnitType < Sc::Unit::TotalTypes && CM != nullptr )
    {
        if ( isDisabled )
            EnableUnitEditing();

        bool unitUsesDefaultSettings = CM->properties.unitUsesDefaultSettings(selectedUnitType);
        checkUseUnitDefaults.SetCheck(unitUsesDefaultSettings);
        if ( unitUsesDefaultSettings )
            DisableUnitProperties();
        else
            EnableUnitProperties();

        UNITDAT* unitDat = chkd.scData.UnitDat((u16)selectedUnitType);
        u32 groundWeapon = (u32)unitDat->GroundWeapon,
            airWeapon    = (u32)unitDat->AirWeapon;
        u16 subUnitId = unitDat->Subunit1;
        
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
        }

        if ( groundWeapon == 130 )
        {
            groupGroundWeapon.SetWinText("No Ground Weapon");
            groupGroundWeapon.DisableThis();
            editGroundDamage.SetWinText("");
            editGroundDamage.DisableThis();
            editGroundBonus.SetWinText("");
            editGroundBonus.DisableThis();
            textGroundDamage.DisableThis();
            textGroundBonus.DisableThis();
        }
        else
        {
            groupGroundWeapon.SetWinText("Ground Weapon [" + weaponNames.at(groundWeapon) + "]");
        }

        if ( airWeapon == 130 || airWeapon == groundWeapon )
        {
            if ( airWeapon != 130 && airWeapon == groundWeapon )
            {
                groupAirWeapon.SetWinText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
            }
            else
                groupAirWeapon.SetWinText("No Air Weapon");

            groupAirWeapon.DisableThis();
            editAirDamage.SetText("");
            editAirDamage.DisableThis();
            editAirBonus.SetText("");
            editAirBonus.DisableThis();
            textAirDamage.DisableThis();
            textAirBonus.DisableThis();
        }
        else
        {
            groupAirWeapon.SetWinText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
        }

        if ( unitUsesDefaultSettings )
        {
            editHitPoints.SetEditNum<u32>(unitDat->HitPoints/256);
            editHitPointsByte.SetEditNum<u32>(unitDat->HitPoints%256);
            editShieldPoints.SetEditNum<u16>(unitDat->ShieldAmount);
            editArmor.SetEditNum<u8>(unitDat->Armor);
            editBuildTime.SetEditNum<u16>(unitDat->BuildTime);
            editMineralCost.SetEditNum<u16>(unitDat->MineralCost);
            editGasCost.SetEditNum<u16>(unitDat->VespeneCost);
            if ( groundWeapon != 130 )
            {
                editGroundDamage.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageAmount);
                editGroundBonus.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageBonus);
            }
            if ( airWeapon != 130 && airWeapon != groundWeapon )
            {
                editAirDamage.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageAmount);
                editAirBonus.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageBonus);
            }
        }
        else // Not default settings
        {
            u32 unitHitpoints = CM->properties.getUnitHitpoints(selectedUnitType);
            editHitPoints.SetEditNum<u32>(unitHitpoints/256);
            editHitPointsByte.SetEditNum<u8>(unitHitpoints%256);
            editShieldPoints.SetEditNum<u16>(CM->properties.getUnitShieldPoints(selectedUnitType));
            editArmor.SetEditNum<u8>(CM->properties.getUnitArmorLevel(selectedUnitType));
            editBuildTime.SetEditNum<u16>(CM->properties.getUnitBuildTime(selectedUnitType));
            editMineralCost.SetEditNum<u16>(CM->properties.getUnitMineralCost(selectedUnitType));
            editGasCost.SetEditNum<u16>(CM->properties.getUnitGasCost(selectedUnitType));
            editGroundDamage.SetEditNum<u16>(CM->properties.getWeaponBaseDamage((Sc::Weapon::Type)unitDat->GroundWeapon));
            editGroundBonus.SetEditNum<u16>(CM->properties.getUnitArmorLevel(selectedUnitType));
            editAirDamage.SetEditNum<u16>(CM->properties.getUnitArmorLevel(selectedUnitType));
            editAirBonus.SetEditNum<u16>(CM->properties.getUnitArmorLevel(selectedUnitType));
        }

        checkEnabledByDefault.SetCheck(CM->properties.isUnitDefaultBuildable(selectedUnitType));

        for ( size_t i=0; i<12; i++ )
        {
            if ( CM->properties.playerUsesDefaultUnitBuildability(selectedUnitType, i) )
                dropPlayerAvailability[i].SetSel(0);
            else if ( CM->properties.isUnitBuildable(selectedUnitType, i) )
                dropPlayerAvailability[i].SetSel(1);
            else // playerUsesDefault == false && isUnitBuildable == false
                dropPlayerAvailability[i].SetSel(2);
        }

        if ( CM->properties.getUnitNameStringId(selectedUnitType) != Chk::StringId::NoString )
        {
            editUnitName.DisableThis();
            checkUseDefaultName.SetCheck(true);
        }
        else
        {
            if ( !unitUsesDefaultSettings )
                editUnitName.EnableThis();

            checkUseDefaultName.SetCheck(false);
        }
            
        ChkdStringPtr unitName = CM->strings.getUnitName<ChkdString>(selectedUnitType, true);
        editUnitName.SetText(*unitName);
        chkd.mapSettingsWindow.SetWinText("Map Settings - [" + Sc::Unit::defaultDisplayNames[(u16)selectedUnitType] + ']');
    }
    else
        DisableUnitEditing();

    refreshing = false;
}

void UnitSettingsWindow::CreateSubWindows(HWND hParent)
{
    unitTree.UpdateUnitNames(Sc::Unit::defaultDisplayNames);
    unitTree.CreateThis(hParent, 5, 5, 200, 489, false, (u64)Id::TREE_UNITSETTINGS);
    buttonResetUnitDefaults.CreateThis(hParent, 5, 494, 200, 25, "Reset All Units To Default", (u64)Id::BUTTON_RESETALLUNITDEFAULTS);
    checkUseUnitDefaults.CreateThis(hParent, 210, 5, 100, 20, false, "Use Unit Defaults", (u64)Id::CHECK_USEUNITDEFAULTS);
    checkEnabledByDefault.CreateThis(hParent, 403, 5, 120, 20, false, "Enabled by Default", (u64)Id::CHECK_ENABLEDBYDEFAULT);

    groupUnitProperties.CreateThis(hParent, 210, 25, 377, 292, "Unit Properties", (u64)Id::GROUP_UNITPROPERTIES);
    textHitPoints.CreateThis(hParent, 215, 40, 75, 20, "Hit Points", (u64)Id::TEXT_UNITHITPOINTS);
    editHitPoints.CreateThis(hParent, 303, 40, 63, 20, false, (u64)Id::EDIT_UNITHITPOINTS);
    editHitPointsByte.CreateThis(hParent, 371, 40, 15, 20, false, (u64)Id::EDIT_UNITHITPOINTSBYTE);
    textShieldPoints.CreateThis(hParent, 215, 65, 83, 20, "Shield Points", (u64)Id::TEXT_UNITSHIELDPOINTS);
    editShieldPoints.CreateThis(hParent, 303, 65, 83, 20, false, (u64)Id::EDIT_UNITSHIELDPOINTS);
    textArmor.CreateThis(hParent, 215, 90, 75, 20, "Armor", (u64)Id::TEXT_UNITARMOR);
    editArmor.CreateThis(hParent, 303, 90, 83, 20, false, (u64)Id::EDIT_UNITARMOR);
    textBuildTime.CreateThis(hParent, 411, 40, 75, 20, "Build Time", (u64)Id::TEXT_UNITBUILDTIME);
    editBuildTime.CreateThis(hParent, 499, 40, 83, 20, false, (u64)Id::EDIT_UNITBUILDTIME);
    textMineralCost.CreateThis(hParent, 411, 65, 75, 20, "Mineral Cost", (u64)Id::TEXT_UNITMINERALCOST);
    editMineralCost.CreateThis(hParent, 499, 65, 83, 20, false, (u64)Id::EDIT_UNITMINERALCOST);
    textGasCost.CreateThis(hParent, 411, 90, 75, 20, "Gas Cost", (u64)Id::TEXT_UNITGASCOST);
    editGasCost.CreateThis(hParent, 499, 90, 83, 20, false, (u64)Id::EDIT_UNITGASCOST);

    groupGroundWeapon.CreateThis(hParent, 215, 115, 367, 62, "Ground Weapon [NAME]", (u64)Id::GROUP_GROUNDWEAPON);
    textGroundDamage.CreateThis(hParent, 220, 135, 75, 20, "Damage", (u64)Id::TEXT_UNITGROUNDDAMAGE);
    editGroundDamage.CreateThis(hParent, 308, 135, 83, 20, false, (u64)Id::EDIT_UNITGROUNDDAMAGE);
    textGroundBonus.CreateThis(hParent, 401, 135, 75, 20, "Bonus", (u64)Id::TEXT_UNITGROUNDBONUS);
    editGroundBonus.CreateThis(hParent, 489, 135, 83, 20, false, (u64)Id::EDIT_UNITGROUNDBONUS);

    groupAirWeapon.CreateThis(hParent, 215, 182, 367, 62, "Air Weapon [NAME]", (u64)Id::GROUP_AIRWEAPON);
    textAirDamage.CreateThis(hParent, 220, 202, 75, 20, "Damage", (u64)Id::TEXT_UNITAIRDAMAGE);
    editAirDamage.CreateThis(hParent, 308, 202, 83, 20, false, (u64)Id::EDIT_UNITAIRDAMAGE);
    textAirBonus.CreateThis(hParent, 401, 202, 75, 20, "Bonus", (u64)Id::TEXT_UNITAIRBONUS);
    editAirBonus.CreateThis(hParent, 489, 202, 83, 20, false, (u64)Id::EDIT_UNITAIRBONUS);

    groupUnitName.CreateThis(hParent, 215, 249, 367, 62, "Unit Name", (u64)Id::GROUP_UNITNAME);
    checkUseDefaultName.CreateThis(hParent, 220, 269, 75, 20, false, "Use Default", (u64)Id::CHECK_USEDEFAULTUNITNAME);
    editUnitName.CreateThis(hParent, 342, 269, 234, 20, false, (u64)Id::EDIT_UNITNAME);

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            std::stringstream ssPlayer;
            if ( player < 9 )
                ssPlayer << "Player 0" << player+1;
            else
                ssPlayer << "Player " << player+1;

            textPlayerAvailability[player].CreateThis(hParent, 215+188*x, 336+27*y, 75, 20, ssPlayer.str(), (u64)Id::TEXT_P1UNITAVAILABILITY+player);
        }
    }

    const std::vector<std::string> items = { "Default", "Yes", "No" };
    size_t numItems = items.size();

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            dropPlayerAvailability[player].CreateThis( hParent, 304+188*x, 336+27*y, 84, 100, false, false,
                (u64)Id::DROP_P1UNITAVAILABILITY+player, items, defaultFont );
        }
    }

    groupUnitAvailability.CreateThis(hParent, 210, 321, 372, 198, "Unit Availability", (u64)Id::GROUP_UNITAVAILABILITY);

    DisableUnitEditing();
}

void UnitSettingsWindow::DisableUnitEditing()
{
    isDisabled = true;
    chkd.mapSettingsWindow.SetWinText("Map Settings");
    DisableUnitProperties();
    checkUseUnitDefaults.DisableThis();
    checkEnabledByDefault.DisableThis();

    groupUnitAvailability.DisableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].DisableThis();
        dropPlayerAvailability[i].DisableThis();
    }
}

void UnitSettingsWindow::EnableUnitEditing()
{
    isDisabled = false;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit && CM->properties.unitUsesDefaultSettings(selectedUnitType) == false )
        EnableUnitProperties();

    checkUseUnitDefaults.EnableThis();
    checkEnabledByDefault.EnableThis();

    groupUnitAvailability.EnableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].EnableThis();
        dropPlayerAvailability[i].EnableThis();
    }
}

void UnitSettingsWindow::DisableUnitProperties()
{
    groupUnitProperties.DisableThis();
    textHitPoints.DisableThis();
    editHitPoints.DisableThis();
    editHitPointsByte.DisableThis();
    textShieldPoints.DisableThis();
    editShieldPoints.DisableThis();
    textArmor.DisableThis();
    editArmor.DisableThis();
    textBuildTime.DisableThis();
    editBuildTime.DisableThis();
    textMineralCost.DisableThis();
    editMineralCost.DisableThis();
    textGasCost.DisableThis();
    editGasCost.DisableThis();

    groupGroundWeapon.DisableThis();
    textGroundDamage.DisableThis();
    editGroundDamage.DisableThis();
    textGroundBonus.DisableThis();
    editGroundBonus.DisableThis();

    groupAirWeapon.DisableThis();
    textAirDamage.DisableThis();
    editAirDamage.DisableThis();
    textAirBonus.DisableThis();
    editAirBonus.DisableThis();

    groupUnitName.DisableThis();
    checkUseDefaultName.DisableThis();
    editUnitName.DisableThis();
}

void UnitSettingsWindow::EnableUnitProperties()
{
    groupUnitProperties.EnableThis();
    textHitPoints.EnableThis();
    editHitPoints.EnableThis();
    editHitPointsByte.EnableThis();
    textShieldPoints.EnableThis();
    editShieldPoints.EnableThis();
    textArmor.EnableThis();
    editArmor.EnableThis();
    textBuildTime.EnableThis();
    editBuildTime.EnableThis();
    textMineralCost.EnableThis();
    editMineralCost.EnableThis();
    textGasCost.EnableThis();
    editGasCost.EnableThis();

    groupGroundWeapon.EnableThis();
    textGroundDamage.EnableThis();
    editGroundDamage.EnableThis();
    textGroundBonus.EnableThis();
    editGroundBonus.EnableThis();

    groupAirWeapon.EnableThis();
    textAirDamage.EnableThis();
    editAirDamage.EnableThis();
    textAirBonus.EnableThis();
    editAirBonus.EnableThis();

    groupUnitName.EnableThis();
    checkUseDefaultName.EnableThis();

    if ( SendMessage((HWND)checkUseDefaultName.getHandle(), BM_GETCHECK, 0, 0) == BST_UNCHECKED )
        editUnitName.EnableThis();
}

void UnitSettingsWindow::CheckReplaceUnitName()
{
    if ( possibleUnitNameUpdate && checkUseDefaultName.isChecked() )
        possibleUnitNameUpdate = false;

    RawString rawUnitName;
    ChkdString newUnitName;
    if ( possibleUnitNameUpdate && selectedUnitType != Sc::Unit::Type::NoUnit && (u16)selectedUnitType < Sc::Unit::TotalTypes && editUnitName.GetWinText(newUnitName) )
    {
        CM->strings.setUnitName<ChkdString>((Sc::Unit::Type)selectedUnitType, newUnitName);
        CM->notifyChange(false);
        chkd.unitWindow.RepopulateList();
        RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

        possibleUnitNameUpdate = false;
    }
}

void UnitSettingsWindow::SetDefaultUnitProperties()
{
    refreshing = true;
    if ( selectedUnitType != Sc::Unit::Type::NoUnit )
    {
        // Remove Custom Unit Name
        u16 origName = (u16)CM->properties.getUnitNameStringId(selectedUnitType, Chk::UseExpSection::No);
        u16 expName = (u16)CM->properties.getUnitNameStringId(selectedUnitType, Chk::UseExpSection::Yes);
        CM->properties.setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::No);
        CM->properties.setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Yes);
        
        ChkdStringPtr unitName = CM->strings.getUnitName<ChkdString>(selectedUnitType, true);
        editUnitName.SetText(*unitName);
        checkUseDefaultName.DisableThis();
        editUnitName.DisableThis();
        CM->strings.deleteString(origName);
        CM->strings.deleteString(expName);
        chkd.unitWindow.RepopulateList();
        RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

        u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->GroundWeapon,
            airWeapon    = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->AirWeapon;

        u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnitType)->Subunit1;
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
        }
        
        CM->properties.setUnitHitpoints(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->HitPoints);
        CM->properties.setUnitShieldPoints(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->ShieldAmount);
        CM->properties.setUnitArmorLevel(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->Armor);
        CM->properties.setUnitBuildTime(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->BuildTime);
        CM->properties.setUnitMineralCost(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->MineralCost);
        CM->properties.setUnitGasCost(selectedUnitType, chkd.scData.UnitDat((u16)selectedUnitType)->VespeneCost);

        if ( groundWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.WeaponDat(groundWeapon)->DamageAmount,
                defaultBonusDamage = chkd.scData.WeaponDat(groundWeapon)->DamageBonus;
            
            CM->properties.setWeaponBaseDamage((Sc::Weapon::Type)groundWeapon, defaultBaseDamage);
            CM->properties.setWeaponUpgradeDamage((Sc::Weapon::Type)groundWeapon, defaultBonusDamage);
        }

        if ( airWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.WeaponDat(airWeapon)->DamageAmount,
                defaultBonusDamage = chkd.scData.WeaponDat(airWeapon)->DamageBonus;
            
            CM->properties.setWeaponBaseDamage((Sc::Weapon::Type)airWeapon, defaultBaseDamage);
            CM->properties.setWeaponUpgradeDamage((Sc::Weapon::Type)airWeapon, defaultBonusDamage);
        }

        CM->notifyChange(false);
    }
    refreshing = false;
}

void UnitSettingsWindow::ClearDefaultUnitProperties()
{
    if ( selectedUnitType != Sc::Unit::Type::NoUnit )
    {
        u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->GroundWeapon,
            airWeapon    = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->AirWeapon;

        u16 origName = (u16)CM->properties.getUnitNameStringId(selectedUnitType, Chk::UseExpSection::No),
            expName = (u16)CM->properties.getUnitNameStringId(selectedUnitType, Chk::UseExpSection::Yes);
        CM->properties.setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->strings.deleteString(origName);
        CM->strings.deleteString(expName);
        
        CM->properties.setUnitHitpoints(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->properties.setUnitShieldPoints(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->properties.setUnitArmorLevel(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->properties.setUnitBuildTime(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->properties.setUnitMineralCost(selectedUnitType, 0, Chk::UseExpSection::Both);
        CM->properties.setUnitGasCost(selectedUnitType, 0, Chk::UseExpSection::Both);

        CM->notifyChange(false);
    }
}

LRESULT UnitSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeTypePortion,
            itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeDataPortion;

        Sc::Unit::Type unitType = (Sc::Unit::Type)itemData;
        if ( itemType == TreeTypeUnit && (u16)unitType < Sc::Unit::TotalTypes )
        {
            CheckReplaceUnitName();
            selectedUnitType = unitType;
            RefreshWindow();
        }
        else
        {
            CheckReplaceUnitName();
            selectedUnitType = Sc::Unit::Type::NoUnit;
            DisableUnitEditing();
        }
    }
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT UnitSettingsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if ( refreshing )
        return ClassWindow::Command(hWnd, wParam, lParam);

    switch ( (Id)LOWORD(wParam) )
    {
    case Id::BUTTON_RESETALLUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            if ( WinLib::GetYesNo("Are you sure you want to reset all unit settings?", "Confirm") == WinLib::PromptResult::Yes )
            {
                CM->properties.setUnitsToDefault();
                CM->strings.deleteUnusedStrings();
                DisableUnitEditing();
                RefreshWindow();
                CM->notifyChange(false);
            }
        }
        break;

    case Id::CHECK_USEUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
            {
                if ( state == BST_CHECKED )
                {
                    ClearDefaultUnitProperties();
                    DisableUnitProperties();
                    CM->properties.setUnitUsesDefaultSettings(selectedUnitType, true);
                }
                else
                {
                    SetDefaultUnitProperties();
                    EnableUnitProperties();
                    CM->properties.setUnitUsesDefaultSettings(selectedUnitType, false);
                }

                RefreshWindow();
                chkd.unitWindow.RepopulateList();
                RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::CHECK_USEDEFAULTUNITNAME:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
            {
                if ( state == BST_CHECKED )
                {
                    editUnitName.DisableThis();
                    CM->properties.setUnitNameStringId(selectedUnitType, 0, Chk::UseExpSection::Both);
                    ChkdStringPtr unitName = CM->strings.getUnitName<ChkdString>(selectedUnitType, true);
                    editUnitName.SetText(*unitName);
                    chkd.unitWindow.RepopulateList();
                    RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
                }
                else
                    editUnitName.EnableThis();

                CM->notifyChange(false);
            }
        }
        break;
    case Id::CHECK_ENABLEDBYDEFAULT:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnitType != Sc::Unit::Type::NoUnit )
            {
                CM->properties.setUnitDefaultBuildable(selectedUnitType, state == BST_CHECKED);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITNAME:
        if ( HIWORD(wParam) == EN_CHANGE )
            possibleUnitNameUpdate = true;
        else if ( HIWORD(wParam) == EN_KILLFOCUS )
            CheckReplaceUnitName();
        break;
    case Id::EDIT_UNITHITPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u32 newHitPoints;
            if ( editHitPoints.GetEditNum<u32>(newHitPoints) )
            {
                u32 oldHitpoints = CM->properties.getUnitHitpoints(selectedUnitType);
                CM->properties.setUnitHitpoints(selectedUnitType, newHitPoints*256+oldHitpoints%256);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITHITPOINTSBYTE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newHitPointByte;
            if ( editHitPointsByte.GetEditNum<u8>(newHitPointByte) )
            {
                u32 oldHitpoints = CM->properties.getUnitHitpoints(selectedUnitType);
                CM->properties.setUnitHitpoints(selectedUnitType, oldHitpoints-oldHitpoints%256+newHitPointByte);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITSHIELDPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newShieldPoints;
            if ( editShieldPoints.GetEditNum<u16>(newShieldPoints) )
            {
                CM->properties.setUnitShieldPoints(selectedUnitType, newShieldPoints);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITARMOR:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newArmorByte;
            if ( editArmor.GetEditNum<u8>(newArmorByte) )
            {
                CM->properties.setUnitArmorLevel(selectedUnitType, newArmorByte);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITBUILDTIME:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newBuildTime;
            if ( editBuildTime.GetEditNum<u16>(newBuildTime) )
            {
                CM->properties.setUnitBuildTime(selectedUnitType, newBuildTime);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITMINERALCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newMineralCost;
            if ( editMineralCost.GetEditNum<u16>(newMineralCost) )
            {
                CM->properties.setUnitMineralCost(selectedUnitType, newMineralCost);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITGASCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGasCost;
            if ( editGasCost.GetEditNum<u16>(newGasCost) )
            {
                CM->properties.setUnitGasCost(selectedUnitType, newGasCost);
                CM->notifyChange(false);
            }
        }
        break;
    case Id::EDIT_UNITGROUNDDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundDamage;
            if ( editGroundDamage.GetEditNum<u16>(newGroundDamage) )
            {
                u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->GroundWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnitType)->Subunit1;

                if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                {
                    CM->properties.setWeaponBaseDamage((Sc::Weapon::Type)groundWeapon, newGroundDamage);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case Id::EDIT_UNITGROUNDBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundBonus;
            if ( editGroundBonus.GetEditNum<u16>(newGroundBonus) )
            {
                u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->GroundWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnitType)->Subunit1;

                if ( subUnitId != (u16)Sc::Unit::Type::NoSubUnit && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                {
                    CM->properties.setWeaponUpgradeDamage((Sc::Weapon::Type)groundWeapon, newGroundBonus);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case Id::EDIT_UNITAIRDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirDamage;
            if ( editAirDamage.GetEditNum<u16>(newAirDamage) )
            {
                u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->AirWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnitType)->Subunit1;

                if ( subUnitId != (u16)Sc::Unit::Type::NoSubUnit && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack
                if ( airWeapon < 130 )
                {
                    CM->properties.setWeaponBaseDamage((Sc::Weapon::Type)airWeapon, newAirDamage);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case Id::EDIT_UNITAIRBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirBonus;
            if ( editAirBonus.GetEditNum<u16>(newAirBonus) )
            {
                u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnitType)->AirWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnitType)->Subunit1;

                if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack

                CM->properties.setWeaponUpgradeDamage((Sc::Weapon::Type)airWeapon, newAirBonus);
                CM->notifyChange(false);
            }
        }
        break;
    default:
        if ( LOWORD(wParam) >= (WORD)Id::DROP_P1UNITAVAILABILITY && LOWORD(wParam) <= (WORD)Id::DROP_P12UNITAVAILABILITY &&
            HIWORD(wParam) == CBN_SELCHANGE )
        {
            u32 player = LOWORD(wParam) - (WORD)Id::DROP_P1UNITAVAILABILITY;
            int sel = dropPlayerAvailability[player].GetSel();

            if ( sel == 0 ) // Default
                CM->properties.setPlayerUsesDefaultUnitBuildability(selectedUnitType, player, dropPlayerAvailability[player].GetSel());
            else if ( sel == 1 ) // Yes
                CM->properties.setUnitBuildable(selectedUnitType, player, true);
            else if ( sel == 2 ) // No
                CM->properties.setUnitBuildable(selectedUnitType, player, false);

            CM->notifyChange(false);
            RefreshWindow();
        }
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT UnitSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
            {
                RefreshWindow();
                if ( selectedUnitType != Sc::Unit::Type::NoUnit )
                    chkd.mapSettingsWindow.SetWinText(std::string("Map Settings - [") + Sc::Unit::defaultDisplayNames[(u16)selectedUnitType] + ']');
                else
                    chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            else
                CheckReplaceUnitName();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
